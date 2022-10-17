
// see https://hammertux.github.io/libc-start 
int __libc_start_main(int *(main) (int, char * *),
					  int argc,
					  char * * ubp_av,
					  void (*init) (void),
					  void (*fini) (void),
					  void (*rtld_fini) (void),
					  void (* stack_end)) {

    main(1,0);
    return 0;
}
