#ifndef UTILS_INTERRUPT_INTERRUPT_H
#define UTILS_INTERRUPT_INTERRUPT_H

extern volatile bool g_interrupt_enabled;

#  define cpu_irq_enable()                     \
    do {                                       \
        g_interrupt_enabled = true;            \
        __asm__("DMB");                               \
        __asm__("cpsie i");            \
    } while (0)
#  define cpu_irq_disable()                    \
    do {                                       \
        __asm__("cpsid i");            \
        __asm__("DMB");                              \
        g_interrupt_enabled = false;           \
    } while (0)

static inline irqflags_t cpu_irq_save(void)
{
    irqflags_t flags ;
    int result;
    __asm__("MRS %0, primask" : "=r" (result) );
    flags = (0 == result);
    cpu_irq_disable();
    return flags;
}

static inline bool cpu_irq_is_enabled_flags(irqflags_t flags)
{
    return (flags);
}

static inline void cpu_irq_restore(irqflags_t flags)
{
    if (cpu_irq_is_enabled_flags(flags)) {
        cpu_irq_enable();
    }
}
#endif
