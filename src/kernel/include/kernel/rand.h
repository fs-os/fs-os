
#ifndef _KERNEL_RAND_H
#define _KERNEL_RAND_H

#include <stdbool.h>
#include <stdint.h>

/* TODO: Move to stdlib? */

/* check_rdseed: returns true if the CPU supports the "rdseed" instruction. For more
 * information, see:
 *   src/kernel/rand.asm
 *   https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits */
bool check_rdseed(void);

/* check_rdrand: returns true if the CPU supports the "rdrand" instruction. For more
 * information, see: src/kernel/rand.asm
 *   https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features */
bool check_rdrand(void);

/* cpu_rand: returns a random number using the "rdrand" assembly instruction. See:
 * src/kernel/rand.asm */
uint32_t cpu_rand(void);

#endif /* _KERNEL_RAND_H */

