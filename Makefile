
# See config for more info
include config.mk

# ----------------------------------------------------------------------------------

.PHONY: all qemu debug-flags qemu-debug clean

all: sysroot $(ISO)

# Alterative: qemu-system-i386 -kernel fs-os.bin
# Change "-audiodev pa" if not using pulseaudio (try replacing "pa" with "alsa")
qemu: all
	qemu-system-i386                   \
		-rtc base=localtime            \
		-audiodev pa,id=audio0         \
		-machine pcspk-audiodev=audio0 \
		-enable-kvm                    \
		-cpu host                      \
		-monitor stdio                 \
		-boot d                        \
		-cdrom $(ISO)

# Add -g for compiling stuff
debug_flags:
	$(eval CFLAGS += -g)
	$(eval ASM_FLAGS += -g)

# Connect with the patched gdb from (https://github.com/fs-os/cross-compiler):
#   (gdb) target remote :1234
# Change "-audiodev pa" if not using pulseaudio (try replacing "pa" with "alsa")
qemu-debug: debug_flags clean all
	qemu-system-i386                   \
		-s                             \
		-rtc base=localtime            \
		-audiodev pa,id=audio0         \
		-machine pcspk-audiodev=audio0 \
		-enable-kvm                    \
		-cpu host                      \
		-monitor stdio                 \
		-boot d                        \
		-cdrom $(ISO)

clean:
	rm -f $(LIBK_OBJS) $(LIBC_OBJS) $(LIBC)
	rm -f $(KERNEL_OBJS) $(ASM_OBJS)
	rm -f $(KERNEL_BIN) $(ISO)
	rm -f $(APP_OBJS)
	rm -rf iso sysroot

# ----------------------------------------------------------------------------------

.PHONY: sysroot sysroot_headers sysroot_lib sysroot_boot

# Would be the same as: "Copy the headers to the sysroot, compile libc into object
# files, make the static lib and copy it to the sysroot, build the kernel binary and
# copy it to the sysroot"
sysroot: sysroot_headers sysroot_lib sysroot_boot

# Create the sysroot, copy the headers into the destination (include folder)
sysroot_headers:
	@mkdir -p $(SYSROOT)/$(SYSROOT_INCLUDEDIR)
	cp -R --preserve=timestamps $(KERNEL_INCLUDES)/. $(SYSROOT)/$(SYSROOT_INCLUDEDIR)/.
	cp -R --preserve=timestamps $(LIBC_INCLUDES)/. $(SYSROOT)/$(SYSROOT_INCLUDEDIR)/.

# Create the sysroot, copy the libc static library to destination (lib folder).
sysroot_lib: $(LIBC)
	@mkdir -p $(SYSROOT)/$(SYSROOT_LIBDIR)
	cp --preserve=timestamps $(LIBC) $(SYSROOT)/$(SYSROOT_LIBDIR)/

# Create the sysroot, copy the kernel binary to destination (boot folder).
# Make a target for the sysroot kernel file so $(ISO) doesn't have phony targets as
# rules.
sysroot_boot: $(SYSROOT_KERNEL)
$(SYSROOT_KERNEL): $(KERNEL_BIN)
	@mkdir -p $(SYSROOT)/$(SYSROOT_BOOTDIR)
	cp --preserve=timestamps $(KERNEL_BIN) $(SYSROOT_KERNEL)

# ----------------------------------------------------------------------------------

# Use the sysroot kernel path as rule to make sure we have the sysroot ready. User
# should run "make sysroot" before "make all". Sysroot already has all the components
# (kernel, inlcudes, lib) compiled and copied into it.
$(ISO): $(SYSROOT_KERNEL) limine
	mkdir -p iso/boot/
	cp $(SYSROOT_KERNEL) iso/boot/$(KERNEL_BIN)
	cp limine/limine.sys limine/limine-cd.bin iso/
	cat cfg/limine.cfg | sed "s/(GITHASH)/$(COMMIT_SHA1)/" > iso/limine.cfg
	xorriso -as mkisofs -b limine-cd.bin                 \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--protective-msdos-label                         \
		iso -o $(ISO)
	limine/limine-deploy --quiet $(ISO)

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

# We will use the same compiler for linking. Use sysroot for including with <>, etc.
$(KERNEL_BIN): cfg/linker.ld $(ASM_OBJS) $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS)
	$(CC) --sysroot=sysroot -isystem=/usr/include -T cfg/linker.ld -o $@ -ffreestanding -nostdlib $(CFLAGS) $(ASM_OBJS) $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS) -lgcc

$(ASM_OBJS): obj/kernel/%.o: src/kernel/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASM_FLAGS) $< -o $@

# We need the sysroot with the includes and the static lib for building the kernel,
# framebuffer, etc. We called 'make sysroot' in the 'all' target so we should be
# fine.
$(KERNEL_OBJS): obj/kernel/%.o: src/kernel/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

$(APP_OBJS): obj/apps/%.o: src/apps/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

# Libk is a modified version of libc for building the kernel. We don't need a static
# lib for libk, we will just link the kernel with these objs.
# TODO: Doesn't have its own include folder, so it uses the same headers as libc. If
# some function parameters need to change, or we need to add a custom function for
# the kernel, make libk header folder in sysroot.
$(LIBK_OBJS): obj/libk/%.o : src/libk/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

$(LIBC_OBJS): obj/libc/%.o : src/libc/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=sysroot -isystem=/usr/include -c $< -o $@ -ffreestanding -std=gnu11 $(CFLAGS) -Iinclude

# Libc used for the userspace. Currently useless. Archive the library objects into a
# static library.
$(LIBC): $(LIBC_OBJS)
	$(AR) rcs $(LIBC) $(LIBC_OBJS)

