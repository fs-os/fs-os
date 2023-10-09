
#ifndef KERNEL_MULTIBOOT_H_
#define KERNEL_MULTIBOOT_H_ 1

#include <stdint.h>

/**
 * @struct Multiboot
 * @brief Multiboot information structure returned by the bootloader.
 * @details See:
 * https://www.gnu.org/software/grub/manual/multiboot/html_node/Boot-information-format.html
 * https://stackoverflow.com/questions/11770451/what-is-the-meaning-of-attribute-packed-aligned4
 */
typedef struct {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;

    uint32_t cmdline;

    uint32_t mods_count;
    uint32_t mods_addr;

    /* Syms */
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;

    /* Memory map */
    uint32_t mmap_length;
    uint32_t mmap_addr;

    uint32_t drives_length;
    uint32_t drives_addr;

    uint32_t config_table;

    uint32_t boot_loader_name;

    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    /* Framebuffer */
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch; /* Bytes per row: (w * bpp / 8) */
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;  /* Bits per pixel */
    uint8_t framebuffer_type; /* See fb_types enum in framebuffer.h */

    /* color_info depends on the fb type */
} Multiboot __attribute__((packed));

#endif /* KERNEL_MULTIBOOT_H_ */
