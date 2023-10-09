
#ifndef KERNEL_IO_H_
#define KERNEL_IO_H_ 1

#include <stdint.h>

/**
 * @brief Reads a byte from an I/O port.
 * @details C wrapper for the `inb` assembly instruction. Defined in
 * src/kernel/io.asm
 * @param[in] port I/O port to read from.
 * @return Byte from that port.
 */
uint8_t io_inb(uint16_t port);

/**
 * @brief Reads a dword from an I/O port.
 * @details C wrapper for the `in` assembly instruction. Defined in
 * src/kernel/io.asm
 * @param[in] port I/O port to read from.
 * @return Dword from that port.
 */
uint32_t io_inl(uint16_t port);

/**
 * @brief Writes a byte to an I/O port.
 * @details C wrapper for the `outb` assembly instruction. Defined in
 * src/kernel/io.asm
 * @param[out] port I/O port to write to.
 * @param[in] data Byte to be written.
 */
void io_outb(uint16_t port, uint8_t data);

/**
 * @brief Writes a dword to an I/O port.
 * @details C wrapper for the `out` assembly instruction. Defined in
 * src/kernel/io.asm
 * @param[out] port I/O port to write to.
 * @param[in] data Dword to be written.
 */
void io_outl(uint16_t port, uint32_t data);

#endif /* KERNEL_IO_H_ */
