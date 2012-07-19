#ifndef _LINUX_MODULE_H
#define _LINUX_MODULE_H
/*
 * Dynamic loading of modules into the kernel.
 *
 * Rewritten by Richard Henderson <rth@tamu.edu> Dec 1996
 * Rewritten again by Rusty Russell, 2002
 */
#include <linux/list.h>
//#include <linux/stat.h>
#include <linux/compiler.h>
#include <linux/elf.h>

/* Not Yet Implemented */
#define MODULE_SUPPORTED_DEVICE(name)

/* some toolchains uses a `_' prefix for all user symbols */
#ifndef MODULE_SYMBOL_PREFIX
#define MODULE_SYMBOL_PREFIX ""
#endif

#define MAX_PARAM_PREFIX_LEN 64
#define MODULE_NAME_LEN MAX_PARAM_PREFIX_LEN

struct kernel_symbol
{
	unsigned long value;
	const char *name;
};

struct modversion_info
{
	unsigned long crc;
	char name[MODULE_NAME_LEN];
};


/* These are either module local, or the kernel's dummy ones. */
extern int init_module(void);
extern void cleanup_module(void);

#define THIS_MODULE ((struct module *)0)

/* Generic info of form tag = "info" */
#define MODULE_INFO(tag, info) 

/* For userspace: you can also call me... */
#define MODULE_ALIAS(_alias) MODULE_INFO(alias, _alias)

/*
 * The following license idents are currently accepted as indicating free
 * software modules
 *
 *	"GPL"				[GNU Public License v2 or later]
 *	"GPL v2"			[GNU Public License v2]
 *	"GPL and additional rights"	[GNU Public License v2 rights and more]
 *	"Dual BSD/GPL"			[GNU Public License v2
 *					 or BSD license choice]
 *	"Dual MIT/GPL"			[GNU Public License v2
 *					 or MIT license choice]
 *	"Dual MPL/GPL"			[GNU Public License v2
 *					 or Mozilla license choice]
 *
 * The following other idents are available
 *
 *	"Proprietary"			[Non free products]
 *
 * There are dual licensed components, but when running with Linux it is the
 * GPL that is relevant so this is a non issue. Similarly LGPL linked with GPL
 * is a GPL combined work.
 *
 * This exists for several reasons
 * 1.	So modinfo can show license info for users wanting to vet their setup 
 *	is free
 * 2.	So the community can ignore bug reports including proprietary modules
 * 3.	So vendors can do likewise based on their own policies
 */
#define MODULE_LICENSE(_license) MODULE_INFO(license, _license)

/* Author, ideally of form NAME[, NAME]*[ and NAME] */
#define MODULE_AUTHOR(_author) MODULE_INFO(author, _author)
  
/* What your module does. */
#define MODULE_DESCRIPTION(_description) MODULE_INFO(description, _description)

/* One for each parameter, describing how to use it.  Some files do
   multiple of these per line, so can't just use MODULE_INFO. */
#define MODULE_PARM_DESC(_parm, desc) \
	__MODULE_INFO(parm, _parm, #_parm ":" desc)

#define MODULE_DEVICE_TABLE(type,name)		\
  MODULE_GENERIC_TABLE(type##_device,name)

/* Version of form [<epoch>:]<version>[-<extra-version>].
   Or for CVS/RCS ID version, everything but the number is stripped.
  <epoch>: A (small) unsigned integer which allows you to start versions
           anew. If not mentioned, it's zero.  eg. "2:1.0" is after
	   "1:2.0".
  <version>: The <version> may contain only alphanumerics and the
           character `.'.  Ordered by numeric sort for numeric parts,
	   ascii sort for ascii parts (as per RPM or DEB algorithm).
  <extraversion>: Like <version>, but inserted for local
           customizations, eg "rh3" or "rusty1".

  Using this automatically adds a checksum of the .c files and the
  local headers in "srcversion".
*/
#define MODULE_VERSION(_version) MODULE_INFO(version, _version)

/* Optional firmware file (or files) needed by the module
 * format is simply firmware file name.  Multiple firmware
 * files require multiple MODULE_FIRMWARE() specifiers */
#define MODULE_FIRMWARE(_firmware) MODULE_INFO(firmware, _firmware)


#define __CRC_SYMBOL(sym, sec)

/* For every exported symbol, place a struct in the __ksymtab section */
#define __EXPORT_SYMBOL(sym, sec)				\
	extern typeof(sym) sym;					\
	__CRC_SYMBOL(sym, sec)					\
	static const char __kstrtab_##sym[]			\
	__attribute__((section("__ksymtab_strings"), aligned(1))) \
	= MODULE_SYMBOL_PREFIX #sym;                    	\
	static const struct kernel_symbol __ksymtab_##sym	\
	__used							\
	__attribute__((section("__ksymtab" sec), unused))	\
	= { (unsigned long)&sym, __kstrtab_##sym }

#define EXPORT_SYMBOL(sym)					\
	__EXPORT_SYMBOL(sym, "")

#define EXPORT_SYMBOL_GPL(sym)					\
	__EXPORT_SYMBOL(sym, "_gpl")

#define EXPORT_SYMBOL_GPL_FUTURE(sym)				\
	__EXPORT_SYMBOL(sym, "_gpl_future")


enum module_state
{
	MODULE_STATE_LIVE,
	MODULE_STATE_COMING,
	MODULE_STATE_GOING,
};

#endif /* _LINUX_MODULE_H */
