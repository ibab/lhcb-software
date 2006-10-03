// db_dim_server.c : Defines the entry point for the console application.
//
// Dietrich Beck
//
// This is a DIM test server. It just publishes a servivce and updates
// that service very fast.

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <dis.h>

#define BUFFSIZE 10000000

char buffer[BUFFSIZE];
int service_id;
int size=10000;
int count;

void main()
{
	service_id = dis_add_service("testBuffer","C",buffer,size,0,0);
    dis_start_serving("DIS_TEST");
	count = 0;
	while(1)
	{
		sprintf(buffer,"%d", count);
		dis_update_service(service_id);
		count++;
	}
}