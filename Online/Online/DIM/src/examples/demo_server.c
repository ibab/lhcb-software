#include <stdio.h>
#include <signal.h>
#include <dis.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int count;
int serv_id;
int sigfound = 0;
int fd;
char message[4096];

void do_printmsg(long *tag, char *msg, int *size){
  count++;
  sprintf(message,"%s\n",msg);
  write(fd,message,strlen(message));
}

static void sig_quit(int signo){
  sigfound = 1;
}

int main(int argc,char **argv){
  char aux[80];
  count = 0;
  message[0] = 0;

  memset(message,0,sizeof message);

  signal(SIGQUIT, sig_quit);
  fd = open("/tmp/dimdump.log",O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
  if(argc < 2){
    printf("usage: demo_server <servicename>\n");
    exit(1);
  }

  printf("Outputing data to /tmp/dimdump.log\n");

  sprintf(aux,"DEVICE/%s/DATA",argv[1]);
  serv_id = dis_add_service(aux, "I", &count, sizeof(int), 0, 0);
  
  sprintf(aux,"DEVICE/%s/CMD",argv[1]);
  dis_add_cmnd(aux, "C", do_printmsg, 0);
  
  if(dis_start_serving(argv[1]) != 1){
    fprintf(stderr,"dis_start_serving() failed\n");
    exit(1);
  }
  while(sigfound == 0)
    pause();
  close(fd);
  dis_remove_service(serv_id);
  dis_stop_serving();
  fprintf(stderr,"messages received: %d\n",count+1);
  exit(0);
}
