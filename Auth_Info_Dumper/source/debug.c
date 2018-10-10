/**
 * (c) 2017-2018 Alexandro Sanchez Bach.
 * Released under MIT license. Read LICENSE for more details.
 */
 
// modified from the original code to dump log to usb and not use network functions
 
#include "debug.h"

/* debugging */
#define DEBUG 1
//#define DEBUG_ADDR  IP(0,0,0,0);
//#define DEBUG_PORT  9022

#define ku_dprintf(...) do { \
    if (kernel) kdprintf(__VA_ARGS__); else dprintf(__VA_ARGS__); \
} while(0)

int debug_sockfd;
char debug_tmp[512];
int logfile;
int foundusb = 0;
char *tmpdbgbuffer;
char usb_mount_path[256];
 
 
char* getusbpath()
{
    int usbdir;
    char tmppath[64];
    char ps4path[64];
    char tmpusb[64];
    tmpusb[0] = '\0';
    char *retval;
    for (int x = 0; x <= 7; x++)
    {
       sprintf(tmppath, "/mnt/usb%i/.dirtest", x);
       usbdir = open(tmppath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (usbdir != -1)
                {
                close(usbdir);
                unlink(tmppath);
                sprintf(tmpusb, "/mnt/usb%i", x);
                    sprintf(ps4path, "/mnt/usb%i/PS4/", x);
                    if (dir_exists(ps4path))
                    {
                    sprintf(ps4path, "/mnt/usb%i", x);
                    retval = malloc (sizeof (char) * 10);
                    strcpy(retval, ps4path);
                    return retval;
                    }
             }
    }
    if (tmpusb[0] != '\0')
     {
       retval = malloc (sizeof (char) * 10);
       strcpy(retval, tmpusb);
       return retval;
     }
     return NULL;
}
 
 
void debug_init()
{
    if (!DEBUG) return;

   // struct sockaddr_in server;
   // memset(&server, 0, sizeof(server));
   // server.sin_len = sizeof(server);
   // server.sin_family = AF_INET;
   // server.sin_addr.s_addr = DEBUG_ADDR;
   // server.sin_port = sceNetHtons(DEBUG_PORT);
   // debug_sockfd = sceNetSocket("debug", AF_INET, SOCK_STREAM, 0);
   // sceNetConnect(debug_sockfd, (struct sockaddr *)&server, sizeof(server));
   

   tmpdbgbuffer = malloc(65536);
   strcpy(tmpdbgbuffer,"\n");

    char* usb_mnt_path = getusbpath();
    if (usb_mnt_path != NULL)
    {
            sprintf(usb_mount_path, "%s/auth_info.log", usb_mnt_path);
            free(usb_mnt_path);
            systemMessage("Dumping auth info to usb.");
			logfile = open(usb_mount_path, O_WRONLY | O_CREAT | O_APPEND, 0777);
			foundusb = 1;
       }
    else
    {
        systemMessage("No Usb Found\nYou must insert a usb drive before running this payload");
		foundusb = 0;
		return;
    }
   
}



void debug_close()
{
    if (!DEBUG) return;
  //sceNetSocketClose(debug_sockfd);
  
  if (foundusb == 1)
  {
	  tmpdbgbuffer = replace_str(tmpdbgbuffer,"\n","\r\n");
	  write(logfile, tmpdbgbuffer, strlen(tmpdbgbuffer));
	  close(logfile);
	  systemMessage("Auth dump finished\nLog saved as auth_info.log");
  }
  else
  {
	  systemMessage("No dump performed because a usb drive was not found");
  }
}

void _dputs(const char *msg)
{
   //sceNetSend(debug_sockfd, msg, strlen(msg), 0);
   strcat(tmpdbgbuffer, msg);
}

void _kdputs(const char *msg)
{
   // uint64_t len = strlen(msg);
   // memcpy(debug_tmp, msg, len);
   // write_args uap;
   // uap.fd = debug_sockfd;
   // uap.buf = debug_tmp;
   // uap.nbyte = len;
   // sys_write(curthread(), &uap);

	strcat(tmpdbgbuffer, msg);
	
}

void _hexdump(char *desc, void *addr, int len, int kernel) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    if (desc != NULL)
        ku_dprintf ("%s:\n", desc);

    if (len == 0) {
        ku_dprintf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        ku_dprintf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0)
                ku_dprintf ("  %s\n", buff);
            ku_dprintf ("  %04x ", i);
        }
        ku_dprintf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        ku_dprintf ("   ");
        i++;
    }
    ku_dprintf ("  %s\n", buff);
}
