/* 
 * stress.c 
 * high rate service updating while accepting a command to cause dead-lock situation
 *
 */
#include <dim/dic.h>
#include <dim/dis.h>
#include <stdio.h>

int service_id;  
long infinity;

void svc_cb(long *tag, int *buf, int *len)
{
	if (buf[0] % 10000 == 0) printf("stress updated %d\n", buf[0]);
}
  
void stress_svc(long *tag, long **address, int *size)  
{    
    *address = &infinity;  
    *size = sizeof(infinity);  
}    

void stress_cmd(long *tag, char *buf, int *size)  
{  
	static int i = 0;

    if(*tag == 1) {  
		if (!(i % 10000)) { 
        	printf("stress_cmd: %d %s received\n", i, buf);
		}
		i++;  
        dis_update_service(service_id);  
    }  
}  
  
int main(int argc, char **argv)  
{  
    service_id = dis_add_service("STRESS_SVC", "L", 0, 0,  
        stress_svc, 0);  
    dis_add_cmnd("STRESS_CMD", 0, stress_cmd, 1);  
    dis_start_serving("STRESS_SERVER"); 
	dic_info_service("PYSTRESS_SVC", MONITORED, 0, 0, 0, svc_cb, 0, 0, 0); 
    for (infinity = 0;;) {
	dis_update_service(service_id);
	infinity++;
    }
}  

