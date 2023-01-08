
/* See src/kernel/io.asm */

#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

uint8_t io_inb(uint16_t port);
uint32_t io_inl(uint16_t port);
void io_outb(uint16_t port, uint8_t data);
void io_outl(uint16_t port, uint32_t data);

#endif /* _KERNEL_IO_H */

