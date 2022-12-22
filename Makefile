
# See config for more info
include config.mk

.PHONY: clean all qemu sysroot_headers sysroot_lib sysroot_boot sysroot

# -----------------------------------------------------------------------

# Sysroot already has all the components compiled and copied into it
$(ISO): sysroot
	mkdir -p iso/boot/grub/
	cp $(KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	cp cfg/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) iso

# We will use the same compiler for linking. We use `-lc` to link the compiler (now
# in the sysroot) with the static lib that we have in `/usr/lib/libc.a`. See $(LIBC)
# todo comment.
$(KERNEL_BIN): cfg/linker.ld obj/kernel/boot.o obj/kernel/kernel.o obj/kernel/tty.o $(LIBC)
	$(CC) --sysroot=sysroot -isystem=/usr/include -T cfg/linker.ld -o $@ -O2 -ffreestanding -nostdlib $(CFLAGS) obj/kernel/boot.o obj/kernel/tty.o obj/kernel/kernel.o -lgcc -lc

obj/kernel/boot.o: src/kernel/boot.asm
	@mkdir -p obj/kernel/
	$(ASM) $(ASM_FLAGS) $< -o $@

# We need the sysroot with the includes and the static lib for building the kernel,
# tty, etc.
obj/kernel/kernel.o: src/kernel/kernel.c
	@mkdir -p obj/kernel/
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -O2 -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

obj/kernel/tty.o: src/kernel/tty.c
	@mkdir -p obj/kernel/
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -O2 -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

$(LIBC_OBJS): obj/libc/%.o : src/libc/%.c
	@mkdir -p obj/libc/
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -O2 -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

# TODO: For now it's using the same static library for "libc" and "libk", if "libc"
# gets too big/complex, it might be a good idea to separate them.
$(LIBC): $(LIBC_OBJS)
	$(AR) rcs $(LIBC) $(LIBC_OBJS)

# -----------------------------------------------------------------------

# Create the sysroot, copy the headers into the destination (include folder)
sysroot_headers:
	@mkdir -p $(SYSROOT)/$(SYSROOT_INCLUDEDIR)
	cp -R --preserve=timestamps $(KERNEL_INCLUDES)/. $(SYSROOT)/$(SYSROOT_INCLUDEDIR)/.
	cp -R --preserve=timestamps $(LIBC_INCLUDES)/. $(SYSROOT)/$(SYSROOT_INCLUDEDIR)/.

# Create the sysroot, copy the libc static library to destination (lib folder)
sysroot_lib: $(LIBC)
	@mkdir -p $(SYSROOT)/$(SYSROOT_LIBDIR)
	cp --preserve=timestamps $(LIBC) $(SYSROOT)/$(SYSROOT_LIBDIR)/

# Create the sysroot, copy the kernel binary to destination (boot folder)
sysroot_boot: $(KERNEL_BIN)
	@mkdir -p $(SYSROOT)/$(SYSROOT_BOOTDIR)
	cp --preserve=timestamps $(KERNEL_BIN) $(SYSROOT)/$(SYSROOT_BOOTDIR)/

# Would be the same as: "Copy the headers to the sysroot, compile libc into object
# files, make the static lib and copy it to the sysroot, build the kernel binary and
# copy it to the sysroot"
sysroot: sysroot_headers sysroot_lib sysroot_boot

# -----------------------------------------------------------------------

all: $(ISO)

# Alterative: qemu-system-i386 -kernel fs-os.bin
qemu: $(ISO)
	qemu-system-i386 -cdrom $<

clean:
	rm -f obj/kernel/*.o
	rm -f $(LIBC_OBJS) $(LIBC)
	rm -f $(KERNEL_BIN) $(ISO)
	rm -rf iso sysroot

