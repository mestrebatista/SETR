/* auto-generated by gen_syscalls.py, don't edit */
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <syscalls/nrf_qspi_nor.h>

extern void z_vrfy_nrf_qspi_nor_base_clock_div_force(const struct device * dev, bool force);
uintptr_t z_mrsh_nrf_qspi_nor_base_clock_div_force(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2,
		uintptr_t arg3, uintptr_t arg4, uintptr_t arg5, void *ssf)
{
	_current->syscall_frame = ssf;
	(void) arg2;	/* unused */
	(void) arg3;	/* unused */
	(void) arg4;	/* unused */
	(void) arg5;	/* unused */
	z_vrfy_nrf_qspi_nor_base_clock_div_force(*(const struct device **)&arg0, *(bool*)&arg1)
;
	_current->syscall_frame = NULL;
	return 0;
}

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif