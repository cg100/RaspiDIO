#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>
#include <tk.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <lirc/lirc_client.h>
#include <fcntl.h>

typedef enum _button
{
	BUTTON_NONE = 0,
	BUTTON_PLAY = 1,
	BUTTON_MENU = 2,
	BUTTON_PLUS = 3,
	BUTTON_MINUS = 4,
	BUTTON_FFWD = 5,
	BUTTON_REV = 6,
} button;


struct lircdata {
  struct lirc_config *config;
  Tcl_Interp *interp;
};

void lirc_event_handler(ClientData clientData, int mask) {
  int ret;
  char *code;
  char *c;
  char *buffer;
  int len;
  struct lircdata *data=(struct lircdata *)clientData;

  // If we have no config file loaded, just read the strings from
  // socket but don't act upon them.
  if (data->config == NULL) {
    while(lirc_nextcode(&code)==0 && code!=NULL) {
      free(code);
    }
    return;
  }

  // There can be multiple string in the socket waiting to be read.
  while((ret=lirc_nextcode(&code))==0 && code!=NULL) {
    // Translate code to action string and pass the string to TCL
    // procedure.
    ret=lirc_code2char(data->config,code,&c);
    if(ret==0 && c!=NULL) {
      len=strlen(c)+23;
      buffer=malloc(len);
      strcpy(buffer, "handle_lirc_command \"");
      strcpy(buffer+21, c);
      strcpy(buffer+len-2, "\"");

      Tcl_EvalEx(data->interp, buffer, len-1, TCL_EVAL_GLOBAL);
      free(buffer);
    }
    free(code);
    if(ret == -1) break;
  }
}

char *lirc_conffiles_changed(ClientData clientData, Tcl_Interp *interp,
			     char *name1, char *name2, int flags) {
  Tcl_Obj *configfilelist;
  Tcl_Obj **filenames;
  int num, i;
  char *filename;
  struct lircdata *data = (struct lircdata *)clientData;
  Tcl_DString *dstring;

  if (clientData == NULL ) return NULL;

  // The value of the variable is list of file names.
  configfilelist=Tcl_GetVar2Ex(interp, "lirc_config_files",
			       NULL, TCL_GLOBAL_ONLY);

  if (configfilelist != NULL) {
    // First free old config data.
    lirc_freeconfig(data->config);
    data->config=NULL;

    // Try to load every file and print error message it loading
    // fails.
    if (Tcl_ListObjGetElements(interp, configfilelist,
			       &num, &filenames) !=TCL_OK) {
      return NULL;
    }

    for(i=0; i<num; i++) {
      filename = Tcl_GetString(filenames[i]);
      printf("Loading LIRC config file %s.\n", filename);

      // Perform tilde substitution with Tcl_TranslateFileName. It
      // needs a Tcl_DString to return the name.
      dstring=(Tcl_DString *)Tcl_Alloc(sizeof(Tcl_DString));
      if(Tcl_TranslateFileName(interp, filename, dstring) != NULL) {
	filename = Tcl_DStringValue(dstring);
	if(lirc_readconfig(filename,&(data->config),NULL)==-1)
	  fprintf(stderr, "Error reading LIRC config file %s\n", filename);

	Tcl_DStringFree(dstring);
      }

      Tcl_Free((char *)dstring);
    }
  }

  return NULL;
}

ClientData init_lirc(Tcl_Interp *interp) {
  int flags;
  int lirc_socket;
  struct lirc_config *config;
  struct lircdata *data;

  printf("Initializing LIRC.\n");

  lirc_socket=lirc_init("avview",1);
  if(lirc_socket == -1) {
    fprintf(stderr, "Failed to initialize LIRC, remote control support disabled.\n");
    return NULL;
  }

  if(lirc_readconfig(NULL,&config,NULL)==-1) {
    fprintf(stderr, "Can't read default LIRC config file ~/.lircrc.\n");
    config=NULL;
  }

  // Make socket non-blocking.
  fcntl(lirc_socket,F_SETOWN,getpid());
  flags=fcntl(lirc_socket,F_GETFL,0);
  if(flags == -1) {
    fprintf(stderr, "Failed initialize LIRC socket, "
	    "remote control support disabled.\n");
    lirc_deinit();
    return NULL;
  }
  fcntl(lirc_socket,F_SETFL,flags|O_NONBLOCK);

  data=malloc(sizeof(struct lircdata));
  data->interp=interp;
  data->config=config;

  // Trace this variable, so that we can load new config file when its
  // name is written to lirc_config_files
  Tcl_TraceVar(interp, "lirc_config_files", TCL_TRACE_WRITES,
	       (Tcl_VarTraceProc *)lirc_conffiles_changed, data);

  // Register the event handler, which reads the socket and executes
  // the LIRC commands.
  Tcl_CreateFileHandler(lirc_socket, TCL_READABLE,
			lirc_event_handler, data);
  return data;
}

void deinit_lirc(ClientData clientData) {
  struct lircdata *data=clientData;

  // If data == NULL, the initialization failed and there is nothing
  // to deinitialize.
  if(data == NULL) return;

  lirc_freeconfig(data->config);
  lirc_deinit();
  free(data);
}
