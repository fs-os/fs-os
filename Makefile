
# See config for more info
include config.mk

# ------------------------------------------------------------------------------

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
	rm -f $(KERNEL_OBJS) $(ASM_OBJS) $(LIBK_OBJS)
	rm -f $(BOOTLOADER_BIN) $(KERNEL_BIN) $(ISO)
	rm -f $(APP_OBJS)
	rm -rf iso $(SYSROOT)

# ------------------------------------------------------------------------------

# The sysroot target creates the sysroot folder, copies the kernel and libk
# headers to the sysroot, and copies the compiles kernel (after building it)
# into the sysroot's boot folder.
sysroot: $(SYSROOT_HEADERS) $(SYSROOT_KERNEL_BIN)

# Copy the libk headers into the sysroot's include folder
$(SYSROOT_INCLUDE_DIR)/%.h: $(LIBK_INCLUDE_DIR)/%.h
	@mkdir -p $(dir $@)
	@cp --preserve=timestamps $< $@

# Copy the kernel headers into the sysroot's include folder
$(SYSROOT_INCLUDE_DIR)/%.h: $(KERNEL_INCLUDE_DIR)/%.h
	@mkdir -p $(dir $@)
	@cp --preserve=timestamps $< $@

# Copy the kernel binary into the sysroot's boot folder
$(SYSROOT_KERNEL_BIN): $(KERNEL_BIN)
	@mkdir -p $(dir $@)
	@cp --preserve=timestamps $< $@

# ------------------------------------------------------------------------------

# Use the sysroot kernel path as rule to make sure we have the sysroot ready.
# User should run "make sysroot" before "make all". Sysroot already has all the
# components (kernel and includes) compiled and copied into it.
# TODO: Fix bootloader in ISO
$(ISO): $(BOOTLOADER_BIN) $(SYSROOT_KERNEL_BIN)
	@mkdir -p iso/boot/
	cp $(SYSROOT_KERNEL_BIN) iso/boot/$(KERNEL_BIN)
	cp $(BOOTLOADER_BIN) iso/
	xorriso -as mkisofs -b $(BOOTLOADER_BIN)             \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--protective-msdos-label                         \
		iso -o $(ISO)

# Only ran once
# TODO: Remove all limine stuff after we have our own bootloader.
limine/limine-deploy:
	make -C limine

# We use --sysroot so we can for example include with <lib.h>
$(KERNEL_BIN): cfg/linker.ld $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS)
	$(CC) -T cfg/linker.ld -o $@ $(KERNEL_OBJS) $(LIBK_OBJS) $(APP_OBJS) $(LDFLAGS)

# Bootloader uses `-f bin` instead of `-f elf32`
$(BOOTLOADER_BIN): src/bootloader/entry.asm
	$(ASM) -f bin -o $@ $<

obj/%.asm.o: src/%.asm
	@mkdir -p $(dir $@)
	$(ASM) $(ASM_FLAGS) -o $@ $<

obj/%.c.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) --sysroot=$(SYSROOT) -isystem=/usr/include $(CFLAGS) -c -o $@ $<
