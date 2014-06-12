/*
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lirc/lirc_client.h"

char *progname;

int main()
{
	struct lirc_config *config;

	lirc_init("irexec",1);
	lirc_readconfig(NULL,&config, NULL);

	char *code;
	char *c;
	int ret;
	
	while(lirc_nextcode(&code)==0)
	{
		if(code==NULL) continue;
		while((ret=lirc_code2char(config,code,&c))==0 && c!=NULL)
		{
			printf("Commando: \"%s\"\n",c);
		}
		free(code);
		if(ret==-1)
			break;
	}
	lirc_freeconfig(config);
	lirc_deinit();
	exit(EXIT_SUCCESS);
}

*/
