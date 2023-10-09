
#ifndef KERNEL_RAND_H_
#define KERNEL_RAND_H_ 1

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Returns true if the CPU supports the `rdseed` instruction.
 * @details For more information, see:
 *  - src/kernel/rand.asm
 *  - https://en.wikipedia.org/wiki/CPUID#EAX=1:_Processor_Info_and_Feature_Bits
 *
 * @return True if supported.
 */
bool check_rdseed(void);

/**
 * @brief Returns true if the CPU supports the `rdrand` instruction.
 * @details For more information, see:
 *  - src/kernel/rand.asm
 *  - https://en.wikipedia.org/wiki/CPUID#EAX=7,_ECX=0:_Extended_Features
 *
 * @return True if supported.
 */
bool check_rdrand(void);

/**
 * @brief Returns a random number using the "rdrand" assembly instruction.
 * @details Will check if its supported. See src/kernel/rand.asm
 * @return Description
 */
uint32_t cpu_rand(void);

#endif /* KERNEL_RAND_H_ */
