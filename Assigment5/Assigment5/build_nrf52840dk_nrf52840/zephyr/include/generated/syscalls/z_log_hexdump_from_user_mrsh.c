/* auto-generated by gen_syscalls.py, don't edit */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <syscalls/log_core.h>

extern void z_vrfy_z_log_hexdump_from_user(uint32_t src_level_val, const char * metadata, const uint8_t * data, uint32_t len);
uintptr_t z_mrsh_z_log_hexdump_from_user(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
		uintptr_t arg3, uintptr_t arg4, uintptr_t arg5, void *ssf)
{
	_current->syscall_frame = ssf;
	(void) arg4;	/* unused */
	(void) arg5;	/* unused */
	z_vrfy_z_log_hexdump_from_user(*(uint32_t*)&arg0, *(const char **)&arg1, *(const uint8_t **)&arg2, *(uint32_t*)&arg3)
;
	_current->syscall_frame = NULL;
	return 0;
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
