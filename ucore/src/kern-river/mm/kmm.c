#include <glue_intr.h>
#include <glue_mp.h>
#include <glue_pmm.h>
#include <libs/string.h>
#include <libs/x86.h>
#include <libs/x86/bitsearch.h>
#include <trap/trap.h>

#include <mm/kmm.h>

#define META_SIZE (sizeof(uintptr_t))
#define MIN_SHIFT 3
#define MIN_ALLOC (1 << MIN_SHIFT)
#define ALLOC_DELTA_SHIFT 10
#define MAX_SHIFT (MIN_SHIFT + ALLOC_DELTA_SHIFT)
#define MAX_ALLOC (MIN_ALLOC << ALLOC_DELTA_SHIFT)
#define MIN_BLOCK_PAGE 4
#define MAX_BLOCK_PAGE 256

typedef struct kmm_ctrl_s
{
	size_t    alloc_size;
	size_t    block_psize;
	uintptr_t head;
} kmm_ctrl_s;

PLS static kmm_ctrl_s ctrl[ALLOC_DELTA_SHIFT + 1];

PLS static pgd_t *last_pgd;
PLS static uintptr_t last_addr;
PLS static pud_t *temp_pud;
PLS static pmd_t *temp_pmd;
PLS static pte_t *temp_ptd;

int
kmm_init(void)
{
	int i;
	for (i = 0; i <= ALLOC_DELTA_SHIFT; ++ i)
	{
		ctrl[i].alloc_size = MIN_ALLOC << i;
		ctrl[i].head = (uintptr_t)-1;
		ctrl[i].block_psize = MIN_BLOCK_PAGE;
	}

	last_pgd = NULL;
	last_addr = 0;
	temp_pud = KADDR_DIRECT(kalloc_pages(1));
	temp_pmd = KADDR_DIRECT(kalloc_pages(1));
	temp_ptd = KADDR_DIRECT(kalloc_pages(1));
	
	return 0;
}

static void *
kmm_ialloc(kmm_ctrl_s *ctrl)
{
	if (ctrl->head == (uintptr_t)-1)
	{
		uintptr_t psize = ctrl->block_psize;
		if (ctrl->block_psize < MAX_BLOCK_PAGE)
			ctrl->block_psize <<= 1;

		uintptr_t pa = kalloc_pages(psize);
		if (pa == 0) return NULL;
		void *block = KADDR_DIRECT(pa);

		memset(block, 0, psize << PGSHIFT);

		uintptr_t *tail = (uintptr_t *)(
			(uintptr_t)block +
			(((psize << PGSHIFT) / ctrl->alloc_size - 1) * (ctrl->alloc_size)));
		*tail = (uintptr_t)-1;

		ctrl->head = (uintptr_t)block;
	}
	 
	uintptr_t *head = (uintptr_t *)ctrl->head;
	ctrl->head = (*head == 0) ? (ctrl->head + ctrl->alloc_size) : (*head);

	*head = (uintptr_t)ctrl;

	return head + 1;
}

void *
kalloc(size_t size)
{
	local_irq_save();
	int size_index = bsr(size + META_SIZE - 1) - MIN_SHIFT + 1;
	 
	void *result;
	if (size_index < 0)
		result = NULL;
	else if (size_index <= ALLOC_DELTA_SHIFT)
	{
		result = kmm_ialloc((kmm_ctrl_s *)ctrl + size_index);
	}
	else
	{
		uintptr_t psize = (size + 2 * PGSIZE - 1) >> PGSHIFT;
		result = KADDR_DIRECT(kalloc_pages(psize) + PGSIZE);
		*((uintptr_t *)result - 1) = psize;
	}

	local_irq_restore();
	return result;
}

void
kfree(void *ptr)
{
	if (ptr == NULL) return;
	local_irq_save();

	if ((uintptr_t)ptr & (PGSIZE - 1))
	{
		uintptr_t *head = ((uintptr_t *)ptr - 1);
		kmm_ctrl_s *ctrl = (kmm_ctrl_s *)*head;
		
		*head = ctrl->head;
		ctrl->head = (uintptr_t)head;
	}
	else
	{
		kfree_pages((uintptr_t)PADDR_DIRECT(ptr) - PGSIZE, *((uintptr_t *)ptr - 1));
	}
	
	local_irq_restore();
}

#include <libs/x86.h>
#include <mp/mp.h>

void
kmm_pgfault(struct trapframe *tf)
{
	// uint64_t  err  = tf->tf_err;
	uintptr_t addr = rcr2();

	if (addr >= PBASE && addr < PBASE + PSIZE)
	{
		pgd_t *pgd = KADDR_DIRECT(PTE_ADDR(rcr3()));
		pud_t *pud;
		pmd_t *pmd;
		pte_t *ptd;
		
		/* PHYSICAL ADDRRESS ACCESSING */
		if (last_pgd != NULL)
		{
			pud = KADDR_DIRECT(PGD_ADDR(last_pgd[PGX(last_addr)]));
			pmd = KADDR_DIRECT(PUD_ADDR(pud[PUX(last_addr)]));
			ptd = KADDR_DIRECT(PMD_ADDR(pmd[PMX(last_addr)]));

			ptd[PTX(last_addr)] = 0;
			if (ptd == temp_ptd)
			{
				pmd[PUX(last_addr)] = 0;
				if (pmd == temp_pmd)
				{
					pud[PUX(last_addr)] = 0;
					if (pud == temp_pud)
						last_pgd[PGX(last_addr)] = 0;
				}
				
				if (last_pgd == pgd)
				{
					invlpg((void *)last_addr);
				}
			}
		}

		if (pgd[PGX(last_addr)] == 0)
			pgd[PGX(last_addr)] = PADDR_DIRECT(temp_pud) | PTE_W | PTE_P;
		pud = KADDR_DIRECT(PGD_ADDR(pgd[PGX(last_addr)]));
		if (pud[PUX(last_addr)] == 0)
			pud[PUX(last_addr)] = PADDR_DIRECT(temp_pmd) | PTE_W | PTE_P;
		pmd = KADDR_DIRECT(PUD_ADDR(pud[PUX(last_addr)]));
		if (pmd[PMX(last_addr)] == 0)
			pmd[PMX(last_addr)] = PADDR_DIRECT(temp_ptd) | PTE_W | PTE_P;
		ptd = KADDR_DIRECT(PMD_ADDR(pmd[PMX(last_addr)]));

		ptd[PTX(last_addr)] = PADDR_DIRECT(addr) | PTE_W | PTE_P;

		last_pgd = pgd;
		last_addr = addr;

		/* XXX? */
		// invlpg((void *)addr);
	}
}
