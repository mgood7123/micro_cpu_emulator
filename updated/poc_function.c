#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "varg.h"
#include <ctype.h>
#include "../Shell/builtins/printfmacro.h"
#include "../Shell/builtins/env.h"
#include <ffi.h>

void eval(size_t argc, env_t argv) {
	if(argc > 0) {
		if(!strcmp(argv[0], "printf")) {
            ffi_cif cif;
            ffi_type *args[2];
            void *values[2];
            char *s;
            char *s1;
            ffi_arg rc;

            /* Initialize the argument info vectors */
            args[0] = &ffi_type_pointer;
            args[1] = &ffi_type_pointer;
            values[0] = &s;
            values[1] = &s1;

            /* Initialize the cif */
//          ffi_status ffi_prep_cif_var (ffi_cif *CIF, ffi_abi varabi, unsigned int NFIXEDARGS, unsigned int varntotalargs, ffi_type *RTYPE, ffi_type **ARGTYPES)
            if (ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, 1, 2,
                            &ffi_type_sint, args) == FFI_OK)
              {
                s = argv[1];
                s1 = argv[2];
                ffi_call(&cif, printf, &rc, values);
                /* rc now holds the result of the call to puts */

                /* values holds a pointer to the function's arg, so to
                  call puts() again all we need to do is change the
                  value of s */
                s = argv[2];
                ffi_call(&cif, printf, &rc, values);
              }
		}
		else if (!strcmp(argv[0], "puts")) {
            ffi_cif cif;
            ffi_type *args[1];
            void *values[1];
            char *s;
            ffi_arg rc;

            /* Initialize the argument info vectors */
            args[0] = &ffi_type_pointer;
            values[0] = &s;

            /* Initialize the cif */
            if (ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 1,
                            &ffi_type_sint, args) == FFI_OK)
              {
                s = argv[1];
                ffi_call(&cif, puts, &rc, values);
                /* rc now holds the result of the call to puts */

                /* values holds a pointer to the function's arg, so to
                  call puts() again all we need to do is change the
                  value of s */
                s = argv[2];
                ffi_call(&cif, puts, &rc, values);
              }
        }
	}
}

void main(void) {
	char * s="printf";
	char * r1="hello %s\n";
	char * r2="world!\n";
	env_t argv = env__new();
	argv = env__add2(argv, s);
	argv = env__add2(argv, r1);
	argv = env__add2(argv, r2);
	eval(env__size(argv), argv);
	env__free(argv);
	s="puts";
	r1="hello %s\n";
	r2="world!";
	argv = env__new();
	argv = env__add2(argv, s);
	argv = env__add2(argv, r1);
	argv = env__add2(argv, r2);
	eval(env__size(argv), argv);
	env__free(argv);
}
