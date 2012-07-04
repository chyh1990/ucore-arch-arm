#ifndef __KERN_DRIVER_PICIRQ_H__
#define __KERN_DRIVER_PICIRQ_H__

void pic_init(void);
void pic_enable(unsigned int irq);
void pic_disable(unsigned int irq);
void irq_clear(unsigned int irq);
void irq_handler();

typedef int irq_handler_t(void *);

void register_irq(int irq, irq_handler_t handler, void *opaque);


#endif /* !__KERN_DRIVER_PICIRQ_H__ */

