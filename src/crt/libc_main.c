#include <libc_hooks.h>
#include <stdio.h>
#include <cpuid.h>

// https://www.intel.com/content/dam/develop/external/us/en/documents/how-to-detect-new-instruction-support-in-the-4th-generation-intel-core-processor-family.pdf
static int check_xcr0_ymm()
{
uint32_t xcr0;
#if defined(_MSC_VER)
	xcr0 = (uint32_t)_xgetbv(0); /* min VS2010 SP1 compiler is required */
#else
	__asm__ ("xgetbv" : "=a" (xcr0) : "c" (0) : "%edx" );
#endif
	return ((xcr0 & 6) == 6); /* checking if xmm and ymm state are enabled in XCR0 */
}

// see https://hammertux.github.io/libc-start 
void __libc_start_main(int *(main) (int, char * *),
					  int argc,
					  char * * ubp_av,
					  void (*init) (void),
					  void (*fini) (void),
					  void (*rtld_fini) (void),
					  void (* stack_end)) {

	int exit_code = -1;

	// check if XMM/YMM state is enabled and AVX2 support is present
	if ( check_xcr0_ymm() ) {
		//TODO: check leaf 7 support
		unsigned int eax, ebx, ecx, edx;
		eax = 0x7;
		ecx = 0;
		__cpuid(0x7, eax, ebx, edx, ecx);
		if ( ebx & bit_AVX2 ) {
			//TODO: proper argc + argv
			main(1,0);
			exit_code = 0;
		}
		else {
			printf("[__libc_start_main] unsupported HW; AVX2 not enabled\n");
			exit_code = -2;
		}
	}
	else {
		printf("[__libc_start_main] unsupported HW; XMM/YMM not enabled\n");
		exit_code = -3;
	}
    __exit(exit_code);
}
