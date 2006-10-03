
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dic.h>
#include <errno.h>

int no_link = -1;

void got_data(long *tag,int *data,int *size){
  if(*data == -1)
    printf("Server is dead\n");
  else
    printf("got data: %d\n",*data);
}

/*
void myErrorHandler(int severity, int code, char *message){
  printf("code: %d: severity: %d Message: %s\n",code,severity,message);
}
*/

int main(int argc, char **argv){
  int count, status, id;
  char msg[4096], aux[80];
  struct timeval start, finish;

  /*dic_add_error_handler(myErrorHandler);*/

  if(argc < 3){
    printf("Usage: oneminute_dim <service name> <logmsg>\n");
    exit(1);
  }
  else if(strlen(argv[2]) > 4096){
    printf("Max message size 4096 bytes\n");
    exit(1);
  }

  memset(msg,0,sizeof msg);
  msg[0] = 0;
  snprintf(msg,strlen(argv[2])+1,"%s\n",argv[2]);

  sprintf(aux,"DEVICE/%s/DATA",argv[1]);
  id = dic_info_service(aux, MONITORED,10,0,0,got_data,0,&no_link,(long)4);
  
  sprintf(aux,"DEVICE/%s/CMD",argv[1]);

  dic_cmnd_service(aux,msg,strlen(msg)+1);
  sleep(2);
  status = gettimeofday(&start,NULL);
  for(count = 0;;count++){
    if((dic_cmnd_service(aux,msg,strlen(msg)+1)) != 1){
      /* Something went wrong!*/
      status = gettimeofday(&finish,NULL);
      fprintf(stderr,"Died after %d secs\nMessages sent: %d\n",
	      finish.tv_sec - start.tv_sec,count);
      dic_release_service(id);
      return -1;
    }
    status = gettimeofday(&finish,NULL);
    if(finish.tv_sec - start.tv_sec >= 60){
      printf("Messages sent in 60 seconds: %d\n",count);
      sleep(60);
      dic_release_service(id);
      return 0;
    }
  }
  
}
