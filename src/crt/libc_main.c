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

#ifdef _JOSRT_HOSTED_ELF
// *nux ELF __libc_start_main 
// see https://hammertux.github.io/libc-start 
void __libc_start_main(int *(main) (int, char * *),
					  int argc,
					  char** argv,
					  void (*init) (void),
					  void (*fini) (void),
					  void (*rtld_fini) (void),
					  void (* stack_end)) {

	int exit_code = -1;

	// using MUSL's implementation

	char **envp = argv+argc+1;
	int i;
	for (i=0; envp[i]; i++) ;

#define AUX_CNT 38

	size_t aux[AUX_CNT] = {0};
	size_t* auxv = (size_t*)(envp+i+1);
	for (i=0; auxv[i]; i+=2) if (auxv[i]<AUX_CNT) aux[auxv[i]] = auxv[i+1];
	char* pn;
	if (!pn) pn = (char*)aux[31];
	if (!pn) pn = "";
	
	// check if XMM/YMM state is enabled and AVX2 support is present
	if ( check_xcr0_ymm() ) {
		//TODO: check leaf 7 support
		unsigned int eax, ebx, ecx, edx;
		eax = 0x7;
		ecx = 0;
		__cpuid(0x7, eax, ebx, edx, ecx);
		if ( ebx & bit_AVX2 ) {
			main(argc,argv);
			exit_code = 0;
		}
		else {
			//printf("[__libc_start_main] unsupported HW; AVX2 not enabled\n");
			exit_code = -2;
		}
	}
	else {
		//printf("[__libc_start_main] unsupported HW; XMM/YMM not enabled\n");
		exit_code = -3;
	}
    __exit(exit_code);
}
#endif // _JOSRT_HOSTED_ELF