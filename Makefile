
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
debug-flags:
	$(eval CFLAGS += -g -DDEBUG)
	$(eval ASM_FLAGS += -g -D DEBUG)

# Connect with the patched gdb from (https://github.com/fs-os/cross-compiler):
#   (gdb) target remote :1234
# Change "-audiodev pa" if not using pulseaudio (try replacing "pa" with "alsa")
qemu-debug: debug-flags clean all
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

.PHONY: sysroot sysroot_headers sysroot_boot

# Description of this target: Copy the headers to the sysroot, compile libc into
# object files, make the static lib and copy it to the sysroot, build the kernel
# binary and copy it to the sysroot.
# TODO: Don't copy headers if they are updated.
# NOTE: Maybe it can be fixed using `$?`:
#   https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html
sysroot: sysroot_headers sysroot_boot

# Create the sysroot, copy the headers into the destination (include folder)
sysroot_headers: $(KERNEL_INCLUDES)/* $(LIBK_INCLUDES)/*
	@mkdir -p $(SYSROOT_INCLUDEDIR)
	cp -R --preserve=timestamps $(KERNEL_INCLUDES)/. $(SYSROOT_INCLUDEDIR)/.
	cp -R --preserve=timestamps $(LIBK_INCLUDES)/. $(SYSROOT_INCLUDEDIR)/.

# Create the sysroot, copy the kernel binary to destination (boot folder).
# Make a target for the sysroot kernel file so $(ISO) doesn't have phony targets
# as rules.
sysroot_boot: $(SYSROOT_KERNEL)
$(SYSROOT_KERNEL): $(KERNEL_BIN)
	@mkdir -p $(SYSROOT_BOOTDIR)
	cp --preserve=timestamps $(KERNEL_BIN) $(SYSROOT_KERNEL)

# ----------------------------------------------------------------------------------

# Use the sysroot kernel path as rule to make sure we have the sysroot ready.
# User should run "make sysroot" before "make all". Sysroot already has all the
# components (kernel and includes) compiled and copied into it.
$(ISO): $(SYSROOT_KERNEL) limine/limine-deploy
	mkdir -p iso/boot/
	cp $(SYSROOT_KERNEL) iso/boot/$(KERNEL_BIN)
	cp limine/limine.sys limine/limine-cd.bin iso/
	cat cfg/limine.cfg | sed "s/(GITHASH)/$(COMMIT_SHA1)/" > iso/limine.cfg
	xorriso -as mkisofs -b limine-cd.bin                 \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--protective-msdos-label                         \
		iso -o $(ISO)
	limine/limine-deploy --quiet $(ISO)

# Only ran once
limine/limine-deploy:
	make -C limine

# We use --sysroot so we can for example include with <lib.h>
$(KERNEL_BIN): cfg/linker.ld $(ASM_OBJS) $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS)
	$(CC) --sysroot=sysroot -isystem=/usr/include -ffreestanding -nostdlib -T cfg/linker.ld -o $@ $(CFLAGS) $(ASM_OBJS) $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS) -lgcc

obj/%.asm.o: src/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASM_FLAGS) -o $@ $<

obj/%.c.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=sysroot -isystem=/usr/include -ffreestanding -std=gnu11 -Iinclude $(CFLAGS) -c -o $@ $<

