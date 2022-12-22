
josrt_add_elf(test_printf 1 0 tests/test_printf.c tests/syscall_driver.asm)

josrt_add_elf(test_collections 1 0 tests/test_collections.c tests/syscall_driver.asm)

josrt_add_elf(test_core 0 0 tests/test_core.c tests/syscall_driver.asm)
