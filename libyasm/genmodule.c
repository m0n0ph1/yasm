/* $Id$
 *
 * Generate module.c from module.in and list of modules.
 *
 *  Copyright (C) 2004  Peter Johnson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND OTHER CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR OTHER CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUTPUT	"module.c"
#define MAXLINE	1024

int
main(int argc, char *argv[])
{
    FILE *in, *out;
    char *str;
    int i;
    int len;
    char *strp;

    if (argc < 2) {
	fprintf(stderr, "Usage: %s <file> [module_name ...]\n", argv[0]);
	return EXIT_FAILURE;
    }

    out = fopen(OUTPUT, "wt");

    if (!out) {
	fprintf(stderr, "Could not open `%s'.\n", OUTPUT);
	return EXIT_FAILURE;
    }

    str = malloc(MAXLINE);

    fprintf(out, "/* This file auto-generated by genmodule.c"
		 " - don't edit it */\n\n");

    in = fopen(argv[1], "rt");
    if (!in) {
	fprintf(stderr, "Could not open `%s'.\n", argv[1]);
	fclose(out);
	remove(OUTPUT);
	return EXIT_FAILURE;
    }

    while (fgets(str, MAXLINE, in)) {
	if (strncmp(str, "MODULES_", 8) == 0) {
	    len = 0;
	    strp = str+8;
	    while (*strp != '\0' && *strp != '_') {
		len++;
		strp++;
	    }
	    *strp = '\0';

	    for (i=2; i<argc; i++) {
		if (strncmp(argv[i], str+8, len) == 0) {
		    fprintf(out, "    {\"%s\", &yasm_%s_LTX_%s},\n",
			    argv[i]+len+1, argv[i]+len+1, str+8);
		}
	    }
	} else if (strncmp(str, "EXTERN_LIST", 11) == 0) {
	    for (i=2; i<argc; i++) {
		strcpy(str, argv[i]);
		strp = str;
		while (*strp != '\0' && *strp != '_') {
		    len++;
		    strp++;
		}
		*strp++ = '\0';

		fprintf(out, "extern yasm_%s_module yasm_%s_LTX_%s;\n",
			str, strp, str);
	    }
	} else
	    fputs(str, out);
    }

    fclose(in);
    fclose(out);

    free(str);

    return EXIT_SUCCESS;
}