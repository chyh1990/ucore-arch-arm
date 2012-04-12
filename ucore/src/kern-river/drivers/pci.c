#include <drivers/pcireg.h>
#include <libs/x86.h>
#include <libs/string.h>
#include <debug/io.h>

// PCI subsystem interface
enum { pci_res_bus, pci_res_mem, pci_res_io, pci_res_max };

struct pci_bus_t;

struct pci_dev_t
{
	uint32_t dev;
	uint32_t func;

	uint32_t dev_id;
	uint32_t dev_class;

	uint32_t reg_base[6];
	uint32_t reg_size[6];
	uint8_t irq_line;
};

struct pci_func_t
{
	struct pci_bus_t *bus;	// Primary bus for bridges
	struct pci_dev_t dev;	 
};

struct pci_bus_t
{
	struct pci_func_t *parent_bridge;
	uint32_t busno;
};

#define PCI_ATTACH_BY_CLASS  0
#define PCI_ATTACH_BY_VENDOR 1

#define assert(x)													\
	do { if (!(x)) kprintf("assertion failed: %s", #x); } while (0)

// Flag to do "lspci" at bootup
static int pci_show_devs = 1;
static int pci_show_addrs = 1;

// PCI "configuration mechanism one"
static uint32_t pci_conf1_addr_ioport = 0x0cf8;
static uint32_t pci_conf1_data_ioport = 0x0cfc;

// Forward declarations
static int pci_bridge_attach(struct pci_func_t *pcif);

// PCI driver table
struct pci_driver_t {
	uint32_t key1, key2;
	int (*attachfn) (struct pci_func_t *pcif);
};

static struct pci_driver_t pci_attach_class[] = {
	{ PCI_CLASS_BRIDGE, PCI_SUBCLASS_BRIDGE_PCI, &pci_bridge_attach },
	{ 0, 0, 0 },
};

static struct pci_driver_t pci_attach_vendor[] = {
	{ 0, 0, 0 },
};


struct pci_node_t
{
	struct pci_func_t func;
};

#define PCI_NODES_MAX 256
#define PCI_BUSES_MAX 256

static struct pci_bus_t root_bus;
static struct pci_func_t pci_nodes[PCI_NODES_MAX];
static struct pci_bus_t pci_buses[PCI_BUSES_MAX];
static int pci_buses_count;
static int pci_nodes_count;

static void
pci_conf1_set_addr(uint32_t bus,
				   uint32_t dev,
				   uint32_t func,
				   uint32_t offset)
{
	assert(bus < 256);
	assert(dev < 32);
	assert(func < 8);
	assert(offset < 256);
	assert((offset & 0x3) == 0);

	uint32_t v = (1 << 31) |		// config-space
		(bus << 16) | (dev << 11) | (func << 8) | (offset);
	outl(pci_conf1_addr_ioport, v);
}

static uint32_t
pci_conf_read(struct pci_func_t *f, uint32_t off)
{
	pci_conf1_set_addr(f->bus->busno, f->dev.dev, f->dev.func, off);
	return inl(pci_conf1_data_ioport);
}

static void
pci_conf_write(struct pci_func_t *f, uint32_t off, uint32_t v)
{
	pci_conf1_set_addr(f->bus->busno, f->dev.dev, f->dev.func, off);
	outl(pci_conf1_data_ioport, v);
}

static int __attribute__((warn_unused_result))
pci_attach_match(uint32_t key1, uint32_t key2,
				 struct pci_driver_t *list, struct pci_func_t *pcif)
{
	uint32_t i;

	for (i = 0; list[i].attachfn; i++) {
		if (list[i].key1 == key1 && list[i].key2 == key2) {
			int r = list[i].attachfn(pcif);
			if (r > 0)
				return r;
			if (r < 0)
				///kprintf("pci_attach_match: attaching %x.%x (%p): %s\n",
				//	key1, key2, list[i].attachfn, e2s(r));
				kprintf("pci_attach_match: attaching %x.%x (%p): SOME ERROR\n",
						key1, key2, list[i].attachfn);
		}
	}

	return 0;
}

static int
pci_attach(struct pci_func_t *f)
{
	return
		pci_attach_match(PCI_CLASS(f->dev.dev_class), PCI_SUBCLASS(f->dev.dev_class),
						 &pci_attach_class[0], f) ||
		pci_attach_match(PCI_VENDOR(f->dev.dev_id), PCI_PRODUCT(f->dev.dev_id),
						 &pci_attach_vendor[0], f);
}

static int 
pci_scan_bus(struct pci_bus_t *bus)
{
	int totaldev = 0;
	struct pci_func_t df;
	memset(&df, 0, sizeof(df));
	df.bus = bus;

	for (df.dev.dev = 0; df.dev.dev < 32; df.dev.dev++) {
		uint32_t bhlc = pci_conf_read(&df, PCI_BHLC_REG);
		if (PCI_HDRTYPE_TYPE(bhlc) > 1)	    // Unsupported or no device
			continue;

		totaldev++;

		struct pci_func_t f;
		memmove(&f, &df, sizeof(struct pci_func_t));
		  
		for (f.dev.func = 0; f.dev.func < (PCI_HDRTYPE_MULTIFN(bhlc) ? 8 : 1);
			 f.dev.func++) {
			struct pci_func_t af;
			memmove(&af, &f, sizeof(struct pci_func_t));

			af.dev.dev_id = pci_conf_read(&f, PCI_ID_REG);
			if (PCI_VENDOR(af.dev.dev_id) == 0xffff)
				continue;

			uint32_t intr = pci_conf_read(&af, PCI_INTERRUPT_REG);
			af.dev.irq_line = PCI_INTERRUPT_LINE(intr);

			af.dev.dev_class = pci_conf_read(&af, PCI_CLASS_REG);
			if (pci_show_devs)
				kprintf("PCI: %02x:%02x.%d: %04x:%04x: class %x.%x irq %d -- ",
						af.bus->busno, af.dev.dev, af.dev.func,
						PCI_VENDOR(af.dev.dev_id), PCI_PRODUCT(af.dev.dev_id),
						PCI_CLASS(af.dev.dev_class), PCI_SUBCLASS(af.dev.dev_class),
						af.dev.irq_line);

			if (pci_attach(&af) == 0)
			{
				if (pci_nodes_count == PCI_NODES_MAX)
					kprintf("nodes are full\n");
				else
				{
					memmove(&pci_nodes[pci_nodes_count], &af,
							sizeof(struct pci_func_t));
					pci_nodes_count ++;
					kprintf("saved\n");
				}
			}
			else kprintf("attached\n");
		}
	}

	return totaldev;
}

static int
pci_bridge_attach(struct pci_func_t *pcif)
{
	uint32_t ioreg  = pci_conf_read(pcif, PCI_BRIDGE_STATIO_REG);
	uint32_t busreg = pci_conf_read(pcif, PCI_BRIDGE_BUS_REG);

	if (PCI_BRIDGE_IO_32BITS(ioreg)) {
		kprintf("PCI: %02x:%02x.%d: 32-bit bridge IO not supported.\n",
				pcif->bus->busno, pcif->dev.dev, pcif->dev.func);
		return 0;
	}

	struct pci_bus_t *nbus;

	if (pci_buses_count == PCI_BUSES_MAX)
	{
		kprintf("reach the maximum number of buses.\n");
		return 0;
	}
	nbus = pci_buses + (pci_buses_count ++);
	 
	memset(nbus, 0, sizeof(struct pci_bus_t));
	 
	nbus->parent_bridge = pcif;
	nbus->busno = (busreg >> PCI_BRIDGE_BUS_SECONDARY_SHIFT) & 0xff;

	if (pci_show_devs)
		kprintf("PCI: %02x:%02x.%d: bridge to PCI bus %d--%d\n",
				pcif->bus->busno, pcif->dev.dev, pcif->dev.func,
				nbus->busno,
				(busreg >> PCI_BRIDGE_BUS_SUBORDINATE_SHIFT) & 0xff);

	pci_scan_bus(nbus);
	return 1;
}

// External PCI subsystem interface

static void
pci_func_enable(struct pci_func_t *f)
{
	pci_conf_write(f, PCI_COMMAND_STATUS_REG,
				   PCI_COMMAND_IO_ENABLE |
				   PCI_COMMAND_MEM_ENABLE |
				   PCI_COMMAND_MASTER_ENABLE);
	 
	uint32_t bar_width;
	uint32_t bar;
	for (bar = PCI_MAPREG_START; bar < PCI_MAPREG_END;
		 bar += bar_width)
	{
		uint32_t oldv = pci_conf_read(f, bar);
		  
		bar_width = 4;
		/* ??? */
		pci_conf_write(f, bar, 0xffffffff);
		uint32_t rv = pci_conf_read(f, bar);

		if (rv == 0)
			continue;
		  
		int regnum = PCI_MAPREG_NUM(bar);
		uint32_t base, size;
		if (PCI_MAPREG_TYPE(rv) == PCI_MAPREG_TYPE_MEM) {
			if (PCI_MAPREG_MEM_TYPE(rv) == PCI_MAPREG_MEM_TYPE_64BIT)
				bar_width = 8;
			   
			size = PCI_MAPREG_MEM_SIZE(rv);
			base = PCI_MAPREG_MEM_ADDR(oldv);
			if (pci_show_addrs)
				kprintf("  mem region %d: %d bytes at 0x%x\n",
						regnum, size, base);
		} else {
			size = PCI_MAPREG_IO_SIZE(rv);
			base = PCI_MAPREG_IO_ADDR(oldv);
			if (pci_show_addrs)
				kprintf("  io region %d: %d bytes at 0x%x\n",
						regnum, size, base);
		}
		  
		pci_conf_write(f, bar, oldv);
		f->dev.reg_base[regnum] = base;
		f->dev.reg_size[regnum] = size;
		  
		kprintf("  -> reg_base[%d] = %08x\n", regnum, base);
		kprintf("  -> reg_size[%d] = %08x\n", regnum, size);
		  
		if (size && !base)
			kprintf("PCI device %02x:%02x.%d (%04x:%04x) may be misconfigured: "
					"region %d: base 0x%x, size %d\n",
					f->bus->busno, f->dev.dev, f->dev.func,
					PCI_VENDOR(f->dev.dev_id), PCI_PRODUCT(f->dev.dev_id),
					regnum, base, size);
	}
}

int
pci_init(void)
{
	memset(&root_bus, 0, sizeof(root_bus));
	pci_nodes_count = 0;
	pci_buses_count = 0;

	pci_scan_bus(&root_bus);
	return 0;
}
