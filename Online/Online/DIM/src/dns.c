/*
 * DNS (Delphi Name Server) Package implements the name server for the DIM
 * (Delphi Information Management) system
 *
 * Started date      : 26-10-92
 * Last modification : 02-08-94
 * Written by        : C. Gaspar
 * Adjusted by       : G.C. Ballintijn
 *
 */

#define DNS
#include <stdio.h>
#include <dim.h>
#include <dis.h>

#ifndef WIN32
#include <netdb.h>
#endif
/*
#define MAX_HASH_ENTRIES 5000
*/
#define MAX_HASH_ENTRIES 25000
FILE	*foutptr;

typedef struct node {
	struct node *client_next;
	struct node *client_prev;
	struct node *next;
	struct node *prev;
	int conn_id;
	int service_id;
	struct serv *servp;
} NODE;

typedef struct red_node {
	struct red_node *next;
	struct red_node *prev;
	int conn_id;
	int service_id;
	struct serv *servp;
} RED_NODE;

typedef struct serv {
	struct serv *server_next;
	struct serv *server_prev;
	struct serv *next;
	struct serv *prev;
	char serv_name[MAX_NAME];
	char serv_def[MAX_NAME];
	int state;
	int conn_id;
	int server_format;
	int serv_id;
	RED_NODE *node_head;
} DNS_SERVICE;

typedef struct red_serv {
	struct red_serv *next;
	struct red_serv *prev;
	char serv_name[MAX_NAME];
	char serv_def[MAX_NAME];
	int state;
	int conn_id;
	int server_format;
	int serv_id;
	RED_NODE *node_head;
} RED_DNS_SERVICE;

static DNS_SERVICE **Service_info_list;
static RED_DNS_SERVICE *Service_hash_table[MAX_HASH_ENTRIES];
static int Curr_n_services = 0;
static int Curr_n_servers = 0;
static int Last_conn_id;
/*
static int Debug = 2;
*/
static int Debug = 0;

static int Timer_q;
static int Server_info_id, Server_new_info_id, 
		   Rpc_id, wake_up;

static char RPC_dummy = 0;
static char *Rpc_info = &RPC_dummy;
static int Rpc_info_size = 0;

static char DNS_accepted_domains[1024] = {0};
static char DNS_accepted_nodes[1024] = {0};

_DIM_PROTO( DNS_SERVICE *service_exists, (char *name) );
_DIM_PROTO( void check_validity,         (int conn_id) );
_DIM_PROTO( void send_dns_server_info,   (int conn_id, int **bufp, int *size) );
_DIM_PROTO( void print_stats,            (void) );
_DIM_PROTO( void set_debug_on,           (int level) );
_DIM_PROTO( void set_debug_off,          (void) );
_DIM_PROTO( void kill_servers,           (void) );
_DIM_PROTO( void print_hash_table,       (void) );
_DIM_PROTO( void get_rpc_info,       	 (int *tag, char **info, int *size) );
_DIM_PROTO( void set_rpc_info,       	 (int *tag, char *name, int *size) );
_DIM_PROTO( void print_hash_table,       (void) );
_DIM_PROTO( static void release_conn,    (int conn_id) );


static void recv_rout( int conn_id, DIC_DNS_PACKET *packet, int size, int status )
{
	int handle_registration();
	int handle_client_request();

	if(size){}
	switch(status)
	{
	case STA_DISC:     /* connection broken */

		if(Debug)
		{
			dim_print_date_time();
			printf(" Disconnect received - conn: %d to %s@%s\n", conn_id,
				Net_conns[conn_id].task,Net_conns[conn_id].node );
		}

		release_conn( conn_id );
		break;
	case STA_CONN:     /* connection received */
		if(Debug)
		{
			dim_print_date_time();
			printf(" Connection request received - conn: %d\n", conn_id);
		}
		/* handle_conn( conn_id ); */
		break;
	case STA_DATA:     /* normal packet */
		switch( vtohl(packet->src_type) )
		{
		case SRC_DIS :
			handle_registration(conn_id, (DIS_DNS_PACKET *)packet, 1);
			break;
		case SRC_DIC :
			handle_client_request(conn_id,(DIC_DNS_PACKET *)packet);
			break;
		default:
			dim_print_date_time();
			printf(" conn: %d to %s@%s, Bad packet\n", conn_id,
				Net_conns[conn_id].task,Net_conns[conn_id].node );
			printf("packet->size = %d\n", vtohl(packet->size));
			printf("packet->src_type = %d\n", vtohl(packet->src_type));
			printf( "closing the connection.\n" );
			fflush(stdout);
			release_conn( conn_id );
/*
			panic( "recv_rout(): Bad switch(1)" );
*/
		}
		break;
	default:	
		dim_print_date_time();
		printf( " - DIM panic: recv_rout(): Bad switch, exiting...\n");
		abort();
	}
}

static void error_handler(int conn_id, int severity, int errcode, char *reason)
{
	if(conn_id){}
	if(errcode){}
	dim_print_msg(reason, severity);
/*
	if(severity == 3)
	{
			printf("Exiting!\n");
			exit(2);
	}
*/
}

int handle_registration( int conn_id, DIS_DNS_PACKET *packet, int tmout_flag )
{
	DNS_SERVICE *servp;
	DNS_DIS_PACKET dis_packet;
	int i, service_id;
	int n_services;
	char *ptr, *ptr1, *ptrt;
	int found;
	void do_update_did();
	void do_inform_clients();
	void inform_clients();
	void service_init();
	void service_insert();
	void service_remove();
#ifdef WIN32
	extern int time();
#endif
#ifdef VMS
	int format;
#endif
	int update_did = 0;
	int name_too_long = 0;
	int rem_only = 0;

	Dns_conns[conn_id].validity = (int)time(NULL);
	if( !Dns_conns[conn_id].service_head ) 
	{

		if(vtohl(packet->n_services) > 0)
		{
			service_id = vtohl(packet->services[0].service_id);
			if((unsigned)service_id & 0x80000000)
				rem_only = 1;
		}
/*
    if( Debug )
	{
			dim_print_date_time();
			printf( " !!!! New Conn %3d : Server %s@%s (PID %d) registering %d services, to delete %d\n",
				conn_id, packet->task_name,
				packet->node_name, 
				vtohl(packet->pid),
				vtohl(packet->n_services), rem_only );
			fflush(stdout);
	}
*/
		if(rem_only)
			return 0;

		Dns_conns[conn_id].already = 0;
		Dns_conns[conn_id].service_head =
			(char *) malloc(sizeof(DNS_SERVICE));
		dll_init( (DLL *) Dns_conns[conn_id].service_head );
		Dns_conns[conn_id].n_services = 0;
		Dns_conns[conn_id].timr_ent = NULL;
		Curr_n_servers++;
		Dns_conns[conn_id].src_type = SRC_DIS;
		Dns_conns[conn_id].protocol = vtohl(packet->protocol);
		strncpy( Dns_conns[conn_id].node_name, packet->node_name,
			(size_t)MAX_NODE_NAME ); 
		strncpy( Dns_conns[conn_id].task_name, packet->task_name,
			(size_t)(MAX_TASK_NAME-4) );
		strcpy(Dns_conns[conn_id].long_task_name, packet->task_name);
		Dns_conns[conn_id].task_name[MAX_TASK_NAME-4-1] = '\0';
		for(i = 0; i < 4; i++)
			Dns_conns[conn_id].node_addr[i] =  packet->node_addr[i];
		Dns_conns[conn_id].pid = vtohl(packet->pid);
		Dns_conns[conn_id].port = vtohl(packet->port);
/*
    if( Debug )
	{
			dim_print_date_time();
			printf( " !!!! New Conn %3d : Server %s@%s (PID %d) registered %d services\n",
				conn_id, Dns_conns[conn_id].task_name,
				Dns_conns[conn_id].node_name, 
				Dns_conns[conn_id].pid,
				vtohl(packet->n_services) );
			fflush(stdout);
	}
*/


		if(strcmp(Dns_conns[conn_id].task_name,"DIS_DNS"))
		if(DNS_accepted_domains[0] == 0)
		{
			if(!get_dns_accepted_domains(DNS_accepted_domains))
				DNS_accepted_domains[0] = (char)0xFF;
		}
		if((DNS_accepted_domains[0] != (char)0xFF) && (strcmp(Dns_conns[conn_id].task_name,"DIS_DNS")))
		{
			ptr = DNS_accepted_domains;
			found = 0;
			while(*ptr)
			{
				ptr1 = strchr(ptr,',');
				if(ptr1)
				{
					*ptr1 = '\0';
					ptr1++;
				}
				else
				{
					ptr1 = ptr;
					ptr1 += (int)strlen(ptr);
				}
				if(strstr(Dns_conns[conn_id].node_name,ptr))
				{
					found = 1;
					break;
				}
				ptr = ptr1;
			}
			if(!found)
			{
				dis_packet.type = htovl(DNS_DIS_STOP);
				dis_packet.size = htovl(DNS_DIS_HEADER);
				if( !dna_write_nowait(conn_id, &dis_packet, DNS_DIS_HEADER) )
				{
					dim_print_date_time();
					printf(" Stop Server: Couldn't write, releasing %d\n",conn_id);
					fflush(stdout);
				}
				dim_print_date_time();
				printf(" Connection from %s refused, stopping server %s\n",
						Dns_conns[conn_id].node_name, 
						Dns_conns[conn_id].task_name);
				fflush(stdout);
				release_conn(conn_id);

				return 0;
			}
		}
		if(tmout_flag)
			Dns_conns[conn_id].timr_ent = dtq_add_entry( Timer_q,
				(int)(WATCHDOG_TMOUT_MAX * 1.3), check_validity, conn_id);
		if(strcmp(Dns_conns[conn_id].task_name,"DIS_DNS"))
		{
			dna_set_test_write(conn_id, dim_get_keepalive_timeout());
		}
		Dns_conns[conn_id].old_n_services = 0;
/*
		Dns_conns[conn_id].n_services = 1;
		do_update_did(conn_id);
*/
		update_did = 1;
/*
		Dns_conns[conn_id].old_n_services = 0;
*/
		Dns_conns[conn_id].n_services = 0;
	} 
	else 
	{
		if( (Dns_conns[conn_id].n_services == -1) &&
		    vtohl(packet->n_services) )
		{
			if(strcmp(Dns_conns[conn_id].task_name,"DIS_DNS"))
				dna_set_test_write(conn_id, dim_get_keepalive_timeout());
			dim_print_date_time();
			printf( " Server %s out of error\n",
				Dns_conns[conn_id].task_name );
			fflush(stdout);
			Dns_conns[conn_id].n_services = 0;
		}
	}
	n_services = vtohl(packet->n_services);
	if((int)strlen(Dns_conns[conn_id].task_name) == MAX_TASK_NAME-4-1)
		name_too_long = 1;
	for( i = 0; i < n_services; i++ ) 
	{
/*
    if( Debug )
	{
			dim_print_date_time();
			printf( " Conn %3d : Server %s@%s (PID %d) registered %s\n",
				conn_id, Dns_conns[conn_id].task_name,
				Dns_conns[conn_id].node_name, 
				Dns_conns[conn_id].pid,
				packet->services[i].service_name );
			fflush(stdout);
	}
*/
		if(n_services == 1)
		{
			if(!strcmp(packet->services[i].service_name, "DUMMY_UPDATE_PACKET"))
			{
				do_inform_clients(conn_id);
				break;
			}
		}
		if( (servp = service_exists(packet->services[i].service_name)) )
		{
			/* if service available on another server send kill signal */
			if((servp->conn_id) && (servp->conn_id != conn_id))
			{
				dis_packet.type = htovl(DNS_DIS_KILL);
				dis_packet.size = htovl(DNS_DIS_HEADER);
#ifdef VMS
				format = vtohl(packet->format);
				if((format & MY_OS9) || (servp->state == -1))
				{
                Dns_conns[servp->conn_id].already = 1;
					if( !dna_write(servp->conn_id, &dis_packet, DNS_DIS_HEADER) )
					{
						dim_print_date_time();
						printf(" Couldn't write, releasing %d\n",servp->conn_id);
						fflush(stdout);
					}
					dim_print_date_time();
					printf(" Service %s already declared, killing server %s\n",
						servp->serv_name, Dns_conns[servp->conn_id].task_name);
					fflush(stdout);
					release_client(servp->conn_id);
					release_conn(servp->conn_id);
				}
				else
				{
#endif
					if((Dns_conns[servp->conn_id].port == Dns_conns[conn_id].port) &&
					  (!strcmp(Dns_conns[servp->conn_id].node_name, Dns_conns[conn_id].node_name)))
					{
						dim_print_date_time();
printf(" Service %s already declared by conn %d - %s@%s:%d (PID %d), Redeclared by conn %d - %s@%s:%d (PID %d)(same server) - Closing both conns %d %d\n",
							servp->serv_name, servp->conn_id, 
							Dns_conns[servp->conn_id].task_name,
							Dns_conns[servp->conn_id].node_name,
							Dns_conns[servp->conn_id].port,
							Dns_conns[servp->conn_id].pid,
							conn_id,
							Dns_conns[conn_id].task_name,
							Dns_conns[conn_id].node_name,
							Dns_conns[conn_id].port,
							Dns_conns[conn_id].pid,
							servp->conn_id, conn_id);
						fflush(stdout);
						release_conn(servp->conn_id);
						release_conn(conn_id);
/*
						update_did = 0;
*/
						return(0);

					}
					else
					{
						Dns_conns[conn_id].already = 1;

						if( !dna_write_nowait(conn_id, &dis_packet, DNS_DIS_HEADER) )
						{
							dim_print_date_time();
							printf(" Kill Server: Couldn't write, releasing %d\n",conn_id);
							fflush(stdout);
						}
						dim_print_date_time();
printf(" Service %s already declared by conn %d - %s@%s:%d (PID %d), killing server conn %d - %s@%s:%d (PID %d) \n",
							servp->serv_name, servp->conn_id, 
							Dns_conns[servp->conn_id].task_name,
							Dns_conns[servp->conn_id].node_name,
							Dns_conns[servp->conn_id].port,
							Dns_conns[servp->conn_id].pid,
							conn_id,
							Dns_conns[conn_id].task_name,
							Dns_conns[conn_id].node_name,
							Dns_conns[conn_id].port,
							Dns_conns[conn_id].pid);
						fflush(stdout);

						release_conn(conn_id);

						return(0);
					}
#ifdef VMS
				}
#endif
			}
			else if( servp->state != -1 ) 
			{
				if( !dll_empty((DLL *) servp->node_head)) 
				{
					/*there are interested clients waiting*/
					strncpy( servp->serv_def,
						packet->services[i].service_def,(size_t)MAX_NAME );
					servp->conn_id = conn_id;
					servp->state = 1;
					servp->server_format = vtohl(packet->format);
					servp->serv_id = vtohl(packet->services[i].service_id);
					dll_insert_queue((DLL *)
						Dns_conns[conn_id].service_head,
						(DLL *) servp);
					Dns_conns[conn_id].n_services++;

/*
					if(n_services == 1)
*/
					if(n_services < MAX_REGISTRATION_UNIT)
					{
						inform_clients(servp);
					}
					continue;
				} 
				else 
				{
					/* test if Service is to be removed */
					service_id = vtohl(packet->services[i].service_id);
					if((unsigned)service_id & 0x80000000)
					{
						dll_remove((DLL *) servp);
						service_remove(&(servp->next));
						Curr_n_services--;
						free(servp);
						Dns_conns[conn_id].n_services--;
						if( dll_empty((DLL *) Dns_conns[conn_id].service_head))
						{ 
						    if( Debug )
							{
								dim_print_date_time();
								printf( " Conn %3d : Server %s@%s unregistered All services, releasing it.\n",
									conn_id, Dns_conns[conn_id].task_name,
									Dns_conns[conn_id].node_name );
								fflush(stdout);
							}
							release_conn(conn_id);
							return(0);
						}
						continue;
                    }
				}
			} 
			else 
			{
				servp->state = 1;
				Dns_conns[conn_id].n_services++;
/*
				if(n_services == 1)
*/
				if(n_services < MAX_REGISTRATION_UNIT)
				{
					if( !dll_empty((DLL *) servp->node_head) )
					{
						inform_clients( servp );
					}
				}
				continue;
			}

		}
		if(!(servp = service_exists(packet->services[i].service_name)))
		{
			servp = (DNS_SERVICE *)malloc(sizeof(DNS_SERVICE));
			if(name_too_long)
			{
				if(strstr(packet->services[i].service_name,"/CLIENT_LIST"))
				{
					strncpy(Dns_conns[conn_id].long_task_name, packet->services[i].service_name,
						(size_t)MAX_NAME);
					ptrt = strstr(Dns_conns[conn_id].long_task_name,"/CLIENT_LIST");
					*ptrt = '\0';
				}
			}
			strncpy( servp->serv_name,
				packet->services[i].service_name,
				(size_t)MAX_NAME );
			strncpy( servp->serv_def,
				packet->services[i].service_def,
				(size_t)MAX_NAME );
			servp->state = 1;
			servp->conn_id = conn_id;
			servp->server_format = vtohl(packet->format);
			servp->serv_id = vtohl(packet->services[i].service_id);
			dll_insert_queue( (DLL *)
					  Dns_conns[conn_id].service_head, 
					  (DLL *) servp );
			Dns_conns[conn_id].n_services++;
			service_insert( &(servp->next) );
			servp->node_head = (RED_NODE *) malloc(sizeof(NODE));
			dll_init( (DLL *) servp->node_head );
			Curr_n_services++;
		} 
	}
	if(update_did)
		do_update_did(conn_id);
    if( Debug )
	{
		if(vtohl(packet->n_services) != 0)
		{
			dim_print_date_time();
			printf( " Conn %3d : Server %s@%s (PID %d) registered %d services\n",
				conn_id, Dns_conns[conn_id].task_name,
				Dns_conns[conn_id].node_name, 
				Dns_conns[conn_id].pid,
				vtohl(packet->n_services) );
			fflush(stdout);
		}
	}

	return(1);
}	

void update_did()
{
	int i;
	void do_update_did();

	for(i = 0; i< Curr_N_Conns; i++)
	{
		if(Dns_conns[i].src_type == SRC_DIS)
		{
			do_update_did(i);
		}
	}
}

void do_update_did(int conn_id)
{
	int n_services, old_n_services;

	n_services = Dns_conns[conn_id].n_services;
/*
	if(Dns_conns[conn_id].n_services)
	{
*/
	old_n_services = Dns_conns[conn_id].old_n_services;
	if(old_n_services != n_services)
	{
		Last_conn_id = conn_id;
		if((old_n_services <= 0) || (n_services == 0) || (n_services == -1))
			dis_update_service(Server_new_info_id);
		dis_update_service(Server_info_id);
		Dns_conns[conn_id].old_n_services = Dns_conns[conn_id].n_services;
	}
/*
	}
*/
}

void check_validity(int conn_id)
{
	int time_diff;
	DNS_DIS_PACKET dis_packet;
	void set_in_error();

	if(Dns_conns[conn_id].validity < 0)
	{
		/* timeout reached kill all services and connection */
		if(Dns_conns[conn_id].n_services != -1)
		{
			dim_print_date_time();
			printf(" Server %s (%s@%s) has been set in error\n",
				Dns_conns[conn_id].task_name, Net_conns[conn_id].task, Net_conns[conn_id].node);
			fflush(stdout);
			set_in_error(conn_id);
			return;
		}
/*
		Dns_conns[conn_id].validity = -Dns_conns[conn_id].validity;
*/
	}
	time_diff = (int)time(NULL) - Dns_conns[conn_id].validity;
	if(time_diff > (int)(WATCHDOG_TMOUT_MAX*1.2))
	{
		/* send register signal */
		dis_packet.type = htovl(DNS_DIS_REGISTER);
		dis_packet.size = htovl(DNS_DIS_HEADER);
		if(Debug)
		{
			dim_print_date_time();
			printf(" Conn %3d : Server %s@%s Registration Requested\n",
				conn_id, Net_conns[conn_id].task, Net_conns[conn_id].node);
			fflush(stdout);
		}
/* moved from dna_write to dna_write_nowait in 14/10/2008 */
		if( !dna_write_nowait(conn_id, &dis_packet, DNS_DIS_HEADER) )
		{
			dim_print_date_time();
			printf(" Server Validity: Couldn't write, releasing Conn %3d : Server %s@%s\n",conn_id,
				Net_conns[conn_id].task, Net_conns[conn_id].node);
			fflush(stdout);
			release_conn(conn_id);
		}
		else
			Dns_conns[conn_id].validity = -Dns_conns[conn_id].validity;
	}
}		


int handle_client_request( int conn_id, DIC_DNS_PACKET *packet )
{
	DNS_SERVICE *servp;
	NODE *nodep;
	RED_NODE *red_nodep; 
	int i, service_id;
	DNS_DIC_PACKET dic_packet;
	SERVICE_REG *serv_regp; 
	void service_insert();
	void service_remove();
	void tcpip_get_addresses();
	char *ptr, *ptr1;
	int found;

	serv_regp = (SERVICE_REG *)(&(packet->service));
	if(Debug)
	{
		dim_print_date_time();
		printf(" Conn %3d : Client %s@%s requested %s\n",
			conn_id, Net_conns[conn_id].task, Net_conns[conn_id].node,
			serv_regp->service_name);
		fflush(stdout);
	}

	if(DNS_accepted_nodes[0] == 0)
	{
		if(!get_dns_accepted_nodes(DNS_accepted_nodes))
			DNS_accepted_nodes[0] = (char)0xFF;
	}
	if(DNS_accepted_nodes[0] != (char)0xFF)
	{
		ptr = DNS_accepted_nodes;
		found = 0;
		while(*ptr)
		{
			ptr1 = strchr(ptr,',');
			if(ptr1)
			{
				*ptr1 = '\0';
				ptr1++;
			}
			else
			{
				ptr1 = ptr;
				ptr1 += (int)strlen(ptr);
			}
			if(strstr(Net_conns[conn_id].node,ptr))
			{
				found = 1;
				break;
			}
			ptr = ptr1;
		}
		if(!found)
		{
			dic_packet.service_id = serv_regp->service_id;
			dic_packet.node_name[0] = (char)0xFF; 
			dic_packet.task_name[0] = 0;
			dic_packet.node_addr[0] = 0;
			dic_packet.pid = 0;
			dic_packet.size = htovl(DNS_DIC_HEADER);
			dim_print_date_time();
			printf(" Connection from %s refused, stopping client pid=%s\n",
					Net_conns[conn_id].node,
					Net_conns[conn_id].task);
			fflush(stdout);
			if( !dna_write_nowait(conn_id, &dic_packet, DNS_DIC_HEADER) )
			{
				dim_print_date_time();
				printf(" Stop Client: Couldn't write, releasing Conn %3d : Client %s@%s\n",conn_id,
					Net_conns[conn_id].task,
					Net_conns[conn_id].node);
				fflush(stdout);
			}
			release_conn(conn_id);

			return 0;
		}
	}
	
	service_id = vtohl(serv_regp->service_id);
	if( service_id == -1 )  /* remove service */
	{
		if(Debug)
		{
			printf("\tRemoving Request\n");
			fflush(stdout);
		}
		if( (servp = service_exists(serv_regp->service_name))  ) 
		{
			red_nodep = servp->node_head;
			while( (red_nodep =
				(RED_NODE *) dll_get_next(
						(DLL *) servp->node_head,
						(DLL *) red_nodep)) )
			{
				if( red_nodep->conn_id == conn_id ) 
				{
					dll_remove((DLL *) red_nodep);
					ptr = (char *)red_nodep - (2 * sizeof(void *));
					nodep = (NODE *)ptr;
					dll_remove((DLL *) nodep);
					red_nodep = red_nodep->prev;
					free(nodep);
					break;
				}
			}
			if(( dll_empty((DLL *) servp->node_head) ) && (servp->state == 0))
			{
				if(Debug)
				{
					printf("\tand Removing Service\n");
					fflush(stdout);
				}
				service_remove(&(servp->next));
				Curr_n_services--;
				free(servp);
			}
		}
		return(0);
	}
	if( (unsigned)service_id & 0x80000000 )  /* remove service */
	{
		service_id &= 0x7fffffff;
		if(Debug)
		{
			printf("\tRemoving Request\n");
			fflush(stdout);
		}
		if( (servp = service_exists(serv_regp->service_name)) ) 
		{
			red_nodep = servp->node_head;
			while( (red_nodep =
				(RED_NODE *) dll_get_next(
						(DLL *) servp->node_head,
						(DLL *) red_nodep)) )
			{
				if(( red_nodep->conn_id == conn_id ) &&
				   ( red_nodep->service_id == service_id ) )
				{
					dll_remove((DLL *) red_nodep);
					ptr = (char *)red_nodep - (2 * sizeof(void *));
					nodep = (NODE *)ptr;
					dll_remove((DLL *) nodep);
					red_nodep = red_nodep->prev;
					free(nodep);
					break;
				}
			}
			if(( dll_empty((DLL *) servp->node_head) ) && (servp->state == 0))
			{
				if(Debug)
				{
					printf("\tand Removing Service\n");
					fflush(stdout);
				}
				service_remove(&(servp->next));
				Curr_n_services--;
				free(servp);
			}
		}
		return(0);
	}
	/* Is already in v.format */
	dic_packet.service_id = serv_regp->service_id;
	dic_packet.node_name[0] = 0; 
	dic_packet.task_name[0] = 0;
	dic_packet.node_addr[0] = 0;
	dic_packet.pid = 0;
	dic_packet.size = htovl(DNS_DIC_HEADER);
	if( Dns_conns[conn_id].src_type == SRC_NONE )
		dna_set_test_write(conn_id, dim_get_keepalive_timeout());
	if( !(servp = service_exists(serv_regp->service_name)) ) 
	{
		if(Debug)
		{
			printf("\tService does not exist, queueing request\n");
			fflush(stdout);
		}
		if( !Dns_conns[conn_id].node_head ) 
		{
			Dns_conns[conn_id].src_type = SRC_DIC;
			Dns_conns[conn_id].node_head =
					malloc(sizeof(NODE));
			dll_init( (DLL *) Dns_conns[conn_id].node_head );
		}
		servp = (DNS_SERVICE *) malloc(sizeof(DNS_SERVICE));
		strncpy( servp->serv_name, serv_regp->service_name, (size_t)MAX_NAME );
		servp->serv_def[0] = '\0';
		servp->state = 0;
		servp->conn_id = 0;
		service_insert(&(servp->next));
		Curr_n_services++;
		servp->node_head = (RED_NODE *)malloc(sizeof(NODE));
		dll_init( (DLL *) servp->node_head );
		nodep = (NODE *)malloc(sizeof(NODE));
		nodep->conn_id = conn_id;
		nodep->service_id = service_id;
		nodep->servp = servp;
		dll_insert_queue((DLL *) Dns_conns[conn_id].node_head,
				 (DLL *) nodep);
		dll_insert_queue((DLL *) servp->node_head,
				 (DLL *) &(nodep->next));
	} 
	else 
	{
		if( servp->state == 1 ) 
		{
#ifdef VMS
			if(servp->server_format & MY_OS9)
			{
				dna_test_write(servp->conn_id);
			}
#endif
			Dns_conns[conn_id].src_type = SRC_DIC;
			strcpy( dic_packet.node_name,
				Dns_conns[servp->conn_id].node_name );
			strcpy( dic_packet.task_name,
				Dns_conns[servp->conn_id].task_name );
			for(i = 0; i < 4; i++)
				dic_packet.node_addr[i] =
					Dns_conns[servp->conn_id].node_addr[i];
			dic_packet.port = htovl(Dns_conns[servp->conn_id].port);
			dic_packet.pid = htovl(Dns_conns[servp->conn_id].pid);
			dic_packet.protocol = htovl(Dns_conns[servp->conn_id].protocol);
			dic_packet.format = htovl(servp->server_format);
			strcpy( dic_packet.service_def, servp->serv_def );
			if(Debug)
			{
				printf("\tService exists in %s@%s, port = %d\n",
					dic_packet.task_name, dic_packet.node_name, 
					dic_packet.port);
				fflush(stdout);
			}
		} 
		else 
		{
			if(Debug)
			{
				if(servp->state == -1)
				{
					printf("\tService exists in BAD state, queueing request\n");
					fflush(stdout);
				}
				else
				{
					printf("\tService does not exist (other client(s) waiting), queueing request\n");
					fflush(stdout);
				}
			}
			if(!(NODE *)Dns_conns[conn_id].node_head ) 
			{
				Dns_conns[conn_id].src_type = SRC_DIC;
				Dns_conns[conn_id].node_head = 
					(char *) malloc(sizeof(NODE));
				dll_init((DLL *)Dns_conns[conn_id].node_head);
			}
			nodep = (NODE *)malloc(sizeof(NODE));
			nodep->conn_id = conn_id;
			nodep->service_id = service_id;
			nodep->servp = servp;
			dll_insert_queue((DLL *) Dns_conns[conn_id].node_head,
					 (DLL *) nodep);
			dll_insert_queue((DLL *) servp->node_head,
					 (DLL *) &(nodep->next));
		}
	}
/* Should it be dna_write_nowait? 16/9/2008 */
/* moved from dna_write to dna_write_nowait in 14/10/2008 */
	if( !dna_write_nowait(conn_id, &dic_packet, DNS_DIC_HEADER) )
	{
		dim_print_date_time();
		printf(" Client Request: Couldn't write, releasing Conn %3d : Client %s@%s\n",conn_id,
					Net_conns[conn_id].task,
					Net_conns[conn_id].node);
		fflush(stdout);
		release_conn(conn_id);
	}

	return(1);
}

void do_inform_clients(int conn_id)
{
	DNS_SERVICE *servp;
	int n_informed = 0;
	static DNS_SERVICE *prev_servp = (DNS_SERVICE *)0;
	void inform_clients();

	DISABLE_AST
	if(!Dns_conns[conn_id].service_head)
	{
		prev_servp = (DNS_SERVICE *)0;
		ENABLE_AST
		return;
	}
	if(prev_servp)
		servp = prev_servp;
	else
		servp = (DNS_SERVICE *)Dns_conns[conn_id].service_head;
	while( (servp = (DNS_SERVICE *) dll_get_next(
				(DLL *) Dns_conns[conn_id].service_head,
				(DLL *) servp)) )
	{
		if( servp->state != -1 ) 
		{
			if( !dll_empty((DLL *) servp->node_head)) 
			{
				inform_clients(servp);
				n_informed++;
				if(n_informed == 1000)
				{
					dtq_start_timer(0, do_inform_clients, conn_id);
					ENABLE_AST
					return;
				}
			}
		}
	}
	prev_servp = (DNS_SERVICE *)0;
	ENABLE_AST
}


void inform_clients(DNS_SERVICE *servp)
{
	RED_NODE *nodep, *prevp; 
	NODE *full_nodep; 
	DNS_DIC_PACKET packet;
	char *ptr;
	int i, to_release = 0;

	nodep = servp->node_head;
	prevp = nodep;
	while( (nodep = (RED_NODE *) dll_get_next((DLL *) servp->node_head,
						 (DLL *) prevp)) )
	{
		packet.service_id = htovl(nodep->service_id);
		strcpy(packet.node_name, Dns_conns[servp->conn_id].node_name);
		strcpy(packet.task_name, Dns_conns[servp->conn_id].task_name);
		for(i = 0; i < 4; i++)
			packet.node_addr[i] = Dns_conns[servp->conn_id].node_addr[i];
		packet.port = htovl(Dns_conns[servp->conn_id].port);
		packet.pid = htovl(Dns_conns[servp->conn_id].pid);
		packet.protocol = htovl(Dns_conns[servp->conn_id].protocol);
		packet.size = htovl(DNS_DIC_HEADER);
		packet.format = htovl(servp->server_format);
		strcpy( packet.service_def, servp->serv_def );
/* Should it be dna_write_nowait? 16/9/2008 */
/* moved from dna_write to dna_write_nowait in 14/10/2008 */
/*
		dna_write_nowait(nodep->conn_id, &packet, DNS_DIC_HEADER);
*/
		if( !dna_write_nowait(nodep->conn_id, &packet, DNS_DIC_HEADER) )
		{
			dim_print_date_time();
			printf(" Inform Client: Couldn't write, releasing Conn %3d : Client %s@%s\n",nodep->conn_id,
					Net_conns[nodep->conn_id].task,
					Net_conns[nodep->conn_id].node);
			fflush(stdout);
			to_release = nodep->conn_id;
/*
release_conn(nodep->conn_id);
*/
		}
/*
		if(dna_write_nowait(nodep->conn_id, &packet, DNS_DIC_HEADER))
		{
*/
			dll_remove( (DLL *) nodep );
			ptr = (char *)nodep - (2 * sizeof(void *));
			full_nodep = (NODE *)ptr;
			dll_remove( (DLL *) full_nodep );
			nodep = nodep->prev;
			free( full_nodep );
			prevp = nodep;
/*
		}
*/
	}
	if(to_release)
		release_conn(to_release);
}

#ifdef VMS
static release_client(int conn_id)
{
char *ptr_task;
char *ptr_node;
int i;

	ptr_task = Net_conns[conn_id].task;
	ptr_node = Net_conns[conn_id].node;
	for( i = 0; i< Curr_N_Conns; i++ )
	{
		if( (!strcmp(Net_conns[i].task,ptr_task)) &&
		    (!strcmp(Net_conns[i].node,ptr_node)) )
		{
			if(i != conn_id)
			{
				if( Dns_conns[i].src_type == SRC_DIC ) 
				{
					if(Debug)
					{
						dim_print_date_time();
						printf(" Releasing client on conn %d - %s@%s\n",
							i, Net_conns[i].task, Net_conns[i].node);
						fflush(stdout);
					}
					release_conn(i);
				}
			}
		}
	}
}
#endif

static void release_conn(int conn_id)
{
	DNS_SERVICE *servp, *old_servp;
	NODE *nodep, *old_nodep;
	void service_remove();

	servp = (DNS_SERVICE *)Dns_conns[conn_id].service_head;
	nodep = (NODE *)Dns_conns[conn_id].node_head;
	if(( Dns_conns[conn_id].src_type == SRC_DIS ) || (servp))
	{
		if( Debug )
		{
			dim_print_date_time();
			printf( " Conn %3d : Server %s@%s died\n",
				conn_id, Dns_conns[conn_id].task_name,
				Dns_conns[conn_id].node_name);
			fflush(stdout);
		}
		else
		{
			if(Dns_conns[conn_id].n_services == -1)
			{
				dim_print_date_time();
				printf( " Conn %3d : Server %s@%s died\n",
					conn_id, Dns_conns[conn_id].task_name,
					Dns_conns[conn_id].node_name);
				fflush(stdout);
			}
		}
		Curr_n_servers--;
		if( Dns_conns[conn_id].timr_ent ) 
		{
			dtq_rem_entry( Timer_q, Dns_conns[conn_id].timr_ent );
			Dns_conns[conn_id].timr_ent = NULL;
		}
		servp = (DNS_SERVICE *)Dns_conns[conn_id].service_head;
		while( (servp = (DNS_SERVICE *) dll_get_next(
				(DLL *) Dns_conns[conn_id].service_head,
				(DLL *) servp)) )
		{
			dll_remove((DLL *) servp);
			if(dll_empty((DLL *) servp->node_head)) 
			{
				service_remove(&(servp->next));
				Curr_n_services--;
				old_servp = servp;
				servp = servp->server_prev;
				free(old_servp);
			} 
			else 
			{
				servp->state = 0;
				servp->conn_id = 0;
				servp = servp->server_prev;
			}
		}
		if(Dns_conns[conn_id].n_services)
		{
			Dns_conns[conn_id].n_services = 0;
			
			do_update_did(conn_id);
/*
			Last_conn_id = conn_id;
			dis_update_service(Server_new_info_id);
		    dis_update_service(Server_info_id);
*/			
		}
		free((DNS_SERVICE *)Dns_conns[conn_id].service_head);
		Dns_conns[conn_id].service_head = 0;
		Dns_conns[conn_id].src_type = SRC_NONE;
		dna_close(conn_id);
	}
	else if((Dns_conns[conn_id].src_type == SRC_DIC) || (nodep))
	{
		if(Debug)
		{
			dim_print_date_time();
			printf(" Conn %3d : Client %s@%s died\n",
				conn_id, Net_conns[conn_id].task, Net_conns[conn_id].node);
			fflush(stdout);
		}
		if( (nodep = (NODE *)Dns_conns[conn_id].node_head) ) 
		{
			while( (nodep = (NODE *) dll_get_next(
					(DLL *) Dns_conns[conn_id].node_head,
					(DLL *) nodep)) )
			{
				servp = nodep->servp;
				dll_remove( (DLL *) nodep );
				dll_remove( (DLL *) &(nodep->next) );
				old_nodep = nodep;
				nodep = nodep->client_prev;
				free(old_nodep);
				if( (dll_empty((DLL *) servp->node_head)) &&
				    (!servp->conn_id) )
				{
					service_remove(&(servp->next));
					Curr_n_services--;
					free( servp );
				}
			}
			free(Dns_conns[conn_id].node_head);
			Dns_conns[conn_id].node_head = 0;
		}
		Dns_conns[conn_id].src_type = SRC_NONE;
		dna_close(conn_id);
	} 
	else 
	{
		if(Debug)
		{
			dim_print_date_time();
			printf(" Conn %3d : Undefined Type %s@%s died\n",
				conn_id, Net_conns[conn_id].task,
				Net_conns[conn_id].node);
			fflush(stdout);
		}
		dna_close(conn_id);
	}
}


void set_in_error(int conn_id)
{
	DNS_SERVICE *servp;

	if(Dns_conns[conn_id].src_type == SRC_DIS)
	{
		if(strcmp(Dns_conns[conn_id].task_name,"DIS_DNS"))
			dna_rem_test_write(conn_id);
		servp = (DNS_SERVICE *)Dns_conns[conn_id].service_head;
		while( (servp = (DNS_SERVICE *) dll_get_next(
				(DLL *) Dns_conns[conn_id].service_head,
				(DLL *) servp)) )
			servp->state = -1;
		Dns_conns[conn_id].n_services = -1;
	}
}

void get_dns_server_info(int *tag, int **bufp, int *size, int *first_time)
{
	if(tag){}
	if(*first_time)
	{

#ifdef VMS
		 sys$wake(0, 0);
#else
		wake_up = TRUE;
#ifdef WIN32
		wake_up();
#endif
#endif
		*size = 0;
	}
	else
	{
		send_dns_server_info(Last_conn_id, bufp, size);
	}
}


void send_dns_server_info(int conn_id, int **bufp, int *size)
{
	static int curr_allocated_size = 0;
	static DNS_DID *dns_info_buffer;
	DNS_SERVICE *servp;
	DNS_SERVER_INFO *dns_server_info;
	DNS_SERVICE_INFO *dns_service_info;
	DNS_CONNECTION *connp;
	int max_size;
	int n_services;

	DISABLE_AST
	connp = &Dns_conns[conn_id];
	if(connp->src_type != SRC_DIS)
	{
		ENABLE_AST
		return;
	}
	n_services = connp->n_services;
	if(n_services == -1)
		n_services = 0;
	max_size = (int)sizeof(DNS_SERVER_INFO) + 
				n_services * (int)sizeof(DNS_SERVICE_INFO);
	if(!curr_allocated_size)
	{
		dns_info_buffer = (DNS_DID *)malloc((size_t)max_size);
		curr_allocated_size = max_size;
	}
	else if (max_size > curr_allocated_size)
	{
		free(dns_info_buffer);
		dns_info_buffer = (DNS_DID *)malloc((size_t)max_size);
		curr_allocated_size = max_size;
	}
	dns_server_info = &dns_info_buffer->server;
	dns_service_info = dns_info_buffer->services;
	strncpy(dns_server_info->task, connp->task_name, (size_t)(MAX_TASK_NAME-4));
	strncpy(dns_server_info->node, connp->node_name, (size_t)MAX_NODE_NAME);
	dns_server_info->pid = htovl(connp->pid);
	dns_server_info->n_services = htovl(connp->n_services);
	servp = (DNS_SERVICE *)connp->service_head;
	while( (servp = (DNS_SERVICE *) dll_get_next((DLL *) connp->service_head,
						    (DLL *) servp)) )
	{
		strncpy(dns_service_info->name, servp->serv_name, (size_t)MAX_NAME); 
		dns_service_info->status = htovl(1);
		if(servp->serv_id & 0x10000000)
			dns_service_info->type = htovl(1);
		else
			dns_service_info->type = htovl(0);
		dns_service_info++;
	}
	*bufp = (int *)dns_info_buffer;
	*size = max_size;
	ENABLE_AST
}

void get_new_dns_server_info(int *tag, int **bufp, int *size, int *first_time)
{
	static int curr_allocated_size = 0;
	static char *info_buffer;
	static int *pid_buffer, pid_size;
	int pid_index = 0;
	DNS_CONNECTION *connp;
	int i, max_size, max_pid_size/*, j, n*/;
	int n_server = 0;
	char /*aux[MAX_NAME], *ptr, */ server[MAX_NAME], *info_buffer_ptr;
/*
	DNS_SERVICE *servp;
	int find_services();
*/

	DISABLE_AST
	if(tag){}
 	for( i = 0; i< Curr_N_Conns; i++ )
	{
		if( Dns_conns[i].src_type == SRC_DIS )
		{
			n_server++;
		}
	}
	max_size = ((int)sizeof(DNS_SERVER_INFO) + MAX_TASK_NAME) * n_server;
	max_pid_size = (int)sizeof(int) * n_server;
	if(!curr_allocated_size)
	{
		info_buffer = (char *)malloc((size_t)max_size);
		curr_allocated_size = max_size;
		pid_buffer = (int *)malloc((size_t)max_pid_size);
	}
	else if (max_size > curr_allocated_size)
	{
		free(info_buffer);
		info_buffer = (char *)malloc((size_t)max_size);
		curr_allocated_size = max_size;
		free(pid_buffer);
		pid_buffer = (int *)malloc((size_t)max_pid_size);
	}
	info_buffer[0] = '\0';
	pid_buffer[0] = 0;

	info_buffer_ptr = info_buffer;
	if(*first_time)
	{
	 	for( i = 0; i< Curr_N_Conns; i++ )
		{
			if( Dns_conns[i].src_type == SRC_DIS )
			{
				connp = &Dns_conns[i];
/*
				if((int)strlen(connp->task_name) == MAX_TASK_NAME-4-1)
				{
					strcpy(aux,connp->task_name);
					strcat(aux,"--CLIENT_LIST");
					n = find_services(aux);
					for(j = 0; j < n; j++)
					{
						servp = Service_info_list[j];
						if(i == servp->conn_id)
						{
							strcpy(aux,servp->serv_name);
							ptr = strstr(aux,"/CLIENT_LIST");
							if(ptr)
								*ptr = '\0';
							break;
						}
					}
					free(Service_info_list);
					strcpy(server, aux);
				}
				else
				{
*/
					strcpy(server, connp->long_task_name);
/*
				}
*/
				strcat(server,"@");
				strcat(server, connp->node_name);
				strcat(server,"|");
				strcpy(info_buffer_ptr, server);
				info_buffer_ptr += (int)strlen(server);
				pid_buffer[pid_index] = connp->pid;
				pid_index++;
			}
		}
	}
	else
	{
		connp = &Dns_conns[Last_conn_id];
		if(connp->n_services > 0)
			strcat(info_buffer, "+");
		else if(connp->n_services == -1)
			strcat(info_buffer, "!");
		else
			strcat(info_buffer, "-");
		strcat(info_buffer, connp->long_task_name);
		strcat(info_buffer,"@");
		strcat(info_buffer, connp->node_name);
		strcat(info_buffer,"|");
		pid_buffer[pid_index] = connp->pid;
		pid_index++;
	}
	info_buffer[(int)strlen(info_buffer) - 1] = '\0';
	info_buffer_ptr = &info_buffer[(int)strlen(info_buffer)+1];
	pid_size = 0;
	for(i = 0; i < pid_index; i++)
	{
		if(i != (pid_index -1))
			sprintf(server, "%d|",pid_buffer[i]);
		else
			sprintf(server, "%d",pid_buffer[i]);
		strcpy(info_buffer_ptr, server);
		info_buffer_ptr += (int)strlen(server);
		pid_size += (int)strlen(server);
	}
	*bufp = (int *)info_buffer;
	*size = (int)strlen(info_buffer)+1+pid_size+1;
	ENABLE_AST
}

int main(int argc, char **argv)
{
	int i, protocol, dns_port;
	int *bufp;
	int size;
	int conn_id, id;
	DIS_DNS_PACKET *dis_dns_packet;
	char node[MAX_NAME];
	void service_init();
	
	if(argc > 1)
	{
		if(!strcmp(argv[1],"-d"))
			set_debug_on();
		else
		{
			printf("Parameters: -d	Debug On\n");
			exit(0);
		}
	}
	dim_set_write_timeout(10);
	dim_set_listen_backlog(1024);
	dim_set_keepalive_timeout(90);
	dim_set_write_buffer_size(32768);
	dim_set_read_buffer_size(32768);
	dim_init();
	conn_arr_create( SRC_DNS );
	service_init();
	Timer_q = dtq_create();
	get_node_name(node);
	dim_print_date_time();
	printf(" DNS version %d starting up on %s\n",DIM_VERSION_NUMBER, node); 
	fflush(stdout);

	Server_new_info_id =(int) dis_add_service( "DIS_DNS/SERVER_LIST", "C", 0, 0, 
						get_new_dns_server_info, 0 );
	Server_info_id = (int)dis_add_service( "DIS_DNS/SERVER_INFO", 0, 0, 0, 
						get_dns_server_info, 0 );
	dis_add_cmnd( "DIS_DNS/PRINT_STATS", 0, print_stats, 0 );
	dis_add_cmnd( "DIS_DNS/DEBUG_ON", 0, set_debug_on, 0 );
	dis_add_cmnd( "DIS_DNS/DEBUG_OFF", 0, set_debug_off, 0 );
	dis_add_cmnd( "DIS_DNS/KILL_SERVERS", "I", kill_servers, 0 );
	dis_add_cmnd( "DIS_DNS/PRINT_HASH_TABLE", 0, print_hash_table, 0 );
	dis_add_cmnd( "DIS_DNS/SERVICE_INFO/RpcIn", "C", set_rpc_info, 0 );
	Rpc_id = (int)dis_add_service( "DIS_DNS/SERVICE_INFO/RpcOut", "C", 0, 0, 
						get_rpc_info, 0 );
	dns_port = get_dns_port_number();
	if( !dna_open_server(DNS_TASK, recv_rout, &protocol, &dns_port, error_handler) )
		return(0);

	id = dis_start_serving("DIS_DNS");
	dis_dns_packet = (DIS_DNS_PACKET *) id_get_ptr(id, SRC_DIS);
	id_free(id, SRC_DIS);
	conn_id = conn_get();
	handle_registration(conn_id, dis_dns_packet, 0);
	dtq_add_entry(Timer_q, 5, update_did, 0xded0000);
	while(1)
	{
#ifdef VMS
		sys$hiber(); 
#else
		wake_up = FALSE;
		while( !wake_up )
        {
			dim_wait();
        }
#endif
 		for( i = 0; i< Curr_N_Conns; i++ )
		{
			if( Dns_conns[i].src_type == SRC_DIS )
			{
				send_dns_server_info( i, &bufp, &size );
				dis_send_service( Server_info_id, bufp, size );
			}
		}
	}
	return(1);
}


void print_stats()
{
	int i;
	int n_conns = 0;
	int n_services = 0;
	int n_servers = 0;
	int n_clients = 0;

	dim_print_date_time();
	printf(" Connection Statistics :\n");
	for(i = 0; i< Curr_N_Conns; i++)
	{
		switch(Dns_conns[i].src_type)
		{
		case SRC_DIS :
			printf("%d - Server %s@%s (PID %d) %d services\n",
				i, Dns_conns[i].task_name,
				Dns_conns[i].node_name,
				Dns_conns[i].pid, Dns_conns[i].n_services);
			fflush(stdout);
			n_services +=  Dns_conns[i].n_services;
			n_servers++;
			n_conns++;
			break;
		case SRC_DIC :
			printf("%d - Client %s@%s\n",
				i, Net_conns[i].task, Net_conns[i].node); 
			fflush(stdout);
			n_conns++;
			n_clients++;
			break;
		default :
			if(Dna_conns[i].busy)
			{
				if(Net_conns[i].task[0] && Net_conns[i].node[0])
					printf("%d - Undefined %s@%s\n",
						i, Net_conns[i].task,
						Net_conns[i].node);
				else 
					printf("%d - Undefined\n", i);
				fflush(stdout);
				n_conns++;
			}
			else
			{
				printf("%d - Empty\n", i);
				fflush(stdout);
			}
		}
	}
	printf("Number of Connections = %d : %d servers, %d clients\n", n_conns,
		n_servers, n_clients);
	printf("Number of Services = %d\n", n_services);
	fflush(stdout);
}


void set_debug_on()
{
	Debug = 1;
}


void set_debug_off()
{
	Debug = 0;
}


void kill_servers(int *tag, int *code, int *size)
{
	int i;
	DNS_DIS_PACKET dis_packet;
	int soft_code = 0, soft_size = 0;
	int type;
	
	if(tag){}
	if(size)
	{
		soft_size = *size;
		if(code)
		{
			soft_code = *code;
		}
	}
	for(i = 0; i< Curr_N_Conns; i++)
	{
		if(Dns_conns[i].src_type == SRC_DIS)
		{
			if(!strcmp(Dns_conns[i].task_name,"DIS_DNS"))
				continue;
			fflush(stdout);
			type = DNS_DIS_EXIT;
			if(soft_size)
			{
				type = DNS_DIS_SOFT_EXIT;
				type |= (soft_code << (int)16) & (int)0xFFFF0000;
				dim_print_date_time();
				printf(" Killing server %s@%s with exit code %d\n",
					Dns_conns[i].task_name, Dns_conns[i].node_name, soft_code);
			}
			else
			{
				dim_print_date_time();
				printf(" Killing server %s@%s\n",
					Dns_conns[i].task_name, Dns_conns[i].node_name);
			}
			dis_packet.type = htovl(type);
			dis_packet.size = htovl(DNS_DIS_HEADER);
			if( !dna_write_nowait(i, &dis_packet, DNS_DIS_HEADER) )
			{
				dim_print_date_time();
				printf(" Kill Server: Couldn't write, releasing %d\n",i);
				fflush(stdout);
				release_conn(i);
			}
		}
	}
}


void service_init()
{
  int i;

	for( i = 0; i < MAX_HASH_ENTRIES; i++ ) {
		Service_hash_table[i] = (RED_DNS_SERVICE *) malloc(sizeof(RED_DNS_SERVICE));
		dll_init((DLL *) Service_hash_table[i]);
	}
}


void service_insert(RED_DNS_SERVICE *servp)
{
	int index;

	index = HashFunction(servp->serv_name, MAX_HASH_ENTRIES);
	dll_insert_queue((DLL *) Service_hash_table[index], 
			 (DLL *) servp);
}


void service_remove(RED_DNS_SERVICE *servp)
{
	if( servp->node_head )
		free( servp->node_head );
	dll_remove( (DLL *) servp );
}


DNS_SERVICE *service_exists(char *name)
{
	int index;
	RED_DNS_SERVICE *servp;
	char *ptr;

	index = HashFunction(name, MAX_HASH_ENTRIES);
	if( (servp = (RED_DNS_SERVICE *) dll_search(
					(DLL *) Service_hash_table[index],
			      		name, (int)strlen(name)+1)) )
	{
		ptr = (char *)servp - (2 * sizeof(void *));
		return((DNS_SERVICE *)ptr);
	}

	return((DNS_SERVICE *)0);
}			

void print_hash_table()
{
	int i;
	RED_DNS_SERVICE *servp;
	int n_entries, max_entry_index = 0;
	int max_entries = 0;

#ifdef VMS
	if( ( foutptr = fopen( "scratch$week:[cp_operator]dim_dns.log", "w" ) 
		) == (FILE *)0 )
	{
		printf("Cannot open: scratch$week:[cp_operator]dim_dns.log for writing\n");
		fflush(stdout);
		return;
	}	
#endif								 
	
	for( i = 0; i < MAX_HASH_ENTRIES; i++ ) 
	{
		n_entries = 0;
#ifdef VMS
		fprintf(foutptr,"HASH[%d] : \n",i);
#endif								 
		servp = Service_hash_table[i];
		while( (servp = (RED_DNS_SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[i],
						(DLL *) servp)) )
		{
#ifdef VMS
			fprintf(foutptr,"%s\n",servp->serv_name);
#endif								 
			n_entries++;
		}
#ifdef VMS
		fprintf(foutptr,"\n\n");
#endif								 
		if(n_entries != 0)
			printf("HASH[%d] - %d entries\n", i, n_entries);  
		if(n_entries > max_entries)
		{
			max_entries = n_entries;
			max_entry_index = i;
		}
	}
#ifdef VMS
	fclose(foutptr);
#endif								 
	printf("Maximum : HASH[%d] - %d entries\n", max_entry_index, max_entries);  
	fflush(stdout);
}

int find_services(char *wild_name)
{

	int i;
	RED_DNS_SERVICE *servp;
	DNS_SERVICE *servp1;
	char tmp[MAX_NAME], *ptr, *ptr1, *dptr, *dptr1;
	int match, count = 0;

	Service_info_list = (DNS_SERVICE **)
		malloc((size_t)(Curr_n_services*(int)sizeof(DNS_SERVICE *)));

	if(!strchr(wild_name, '*'))
	{
		servp1 = service_exists(wild_name);
		if(servp1)
		{
			if(servp1->state == 1)
			{
				Service_info_list[count] = (DNS_SERVICE *)servp1;
				count++;
				return 1;
			}
		}
		return 0;
	}
	for( i = 0; i < MAX_HASH_ENTRIES; i++ ) 
	{
		servp = Service_hash_table[i];
		while( (servp = (RED_DNS_SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[i],
						(DLL *) servp)) )
		{
			ptr = wild_name;
			dptr = servp->serv_name;
			match = 1;

            while( (ptr1 = strchr(ptr,'*')) )
			{
				if(ptr1 == ptr)
				{
					ptr++;
					if(!*ptr)
					{
						dptr = ptr; 
						break;
					}
					strcpy(tmp,ptr);
					if( (ptr1 = strchr(ptr,'*')) )
					{
						tmp[ptr1-ptr] = '\0';
					}
					if( (dptr1 = strstr(dptr, tmp)) )
					{
						if(!ptr1)
						{
							dptr = dptr1;
							break;
						}
						dptr1 += (int)strlen(tmp);
						ptr = ptr1;
						dptr = dptr1;
					}
					else
					{
						match = 0;
						break;
					}
				}
				else
				{
					strcpy(tmp,ptr);
					tmp[ptr1-ptr] = '\0';
					if(!strncmp(dptr, tmp, strlen(tmp)))
					{
						dptr += (int)strlen(tmp);
						ptr = ptr1;
					}
					else
					{
						match = 0;
						break;
					}
				}
			}			
			if(strcmp(dptr, ptr))
			{
				strcpy(tmp,ptr);
				strcat(tmp,"/RpcIn");
				if(strcmp(dptr, tmp))
					match = 0;
			}
		    if(match)
			{
				if(servp->state == 1)
				{
					ptr = (char *)servp - (2 * sizeof(void *));
					Service_info_list[count] = (DNS_SERVICE *)ptr;
					count++;
				}
			}
		}
	}
	return(count);
}

void set_rpc_info(int *tag, char *buffer, int *size)
{
	char aux[MAX_NAME], rpcaux[MAX_NAME+32], *ptr, *rpcptr;
    int i, n, rpc, id[2], conn_id;
	DNS_SERVICE *servp, *aux_servp;

	if(size){}
	if(tag){}
	if(Debug)
	{
		dim_print_date_time();
		conn_id = dis_get_conn_id();
		printf(" Got Browse Request <%s> from conn: %d %s@%s\n", buffer, conn_id,
			Net_conns[conn_id].task,Net_conns[conn_id].node);
	}
	n = find_services(buffer);
	if(Debug)
	{
		dim_print_date_time();
		conn_id = dis_get_conn_id();
		printf(" Browse Request <%s> found %d services\n", buffer, n);
	}
	if(!Rpc_info_size)
	{
		Rpc_info = malloc((size_t)(MAX_NAME*(n+1)*2));
		Rpc_info_size = MAX_NAME*(n+1)*2;
	}
	else if(Rpc_info_size < MAX_NAME*n*2)
	{
		free(Rpc_info);
		Rpc_info = malloc((size_t)(MAX_NAME*(n+1)*2));
		Rpc_info_size = MAX_NAME*(n+1)*2;
	}
	Rpc_info[0] = '\0';
	rpcptr = Rpc_info;
	for(i = 0; i < n; i++)
	{		
		rpc = 0;
		servp = Service_info_list[i];
		if(strstr(servp->serv_name,"/Rpc"))
		{
			strcpy(aux,servp->serv_name);
			if( (ptr = strstr(aux,"/RpcIn")) )
			{
				*ptr = '\0';
				rpc = 1;
				if( (ptr = strstr(Rpc_info, aux)) )
				{
					ptr += (int)strlen(aux);
					if(*ptr == '|')
						rpc = 2;
				}
			}
			if( (ptr = strstr(aux,"/RpcOut")) )
			{
				*ptr = '\0';
				rpc = 1;
				if( (ptr = strstr(Rpc_info, aux)) )
				{
					ptr += (int)strlen(aux);
					if(*ptr == '|')
						rpc = 2;
				}
			}
			if(rpc == 1)
			{
				strcpy(rpcaux, aux);
				strcat(rpcaux,"|");
				strcat(aux,"/RpcIn");
				if( (aux_servp = service_exists(aux)) )
				{
					strcat(rpcaux, aux_servp->serv_def);
					strcat(rpcaux,",");
					ptr = strstr(aux,"/RpcIn");
					*ptr = '\0';
					strcat(aux,"/RpcOut");
					if( (aux_servp = service_exists(aux)) )
					{
						strcat(rpcaux,aux_servp->serv_def);
						strcat(rpcaux,"|RPC\n");
						strcpy(rpcptr, rpcaux);
						rpcptr += (int)strlen(rpcaux);
					}
				}
			}
		}
		else
		{
			strcpy(rpcaux, servp->serv_name);
			strcat(rpcaux,"|");
			strcat(rpcaux,servp->serv_def);
			if(servp->serv_id & 0x10000000)
				strcat(rpcaux,"|CMD\n");
			else
				strcat(rpcaux,"|\n");
			strcpy(rpcptr, rpcaux);
			rpcptr += (int)strlen(rpcaux);
		}
	}
	*rpcptr = '\0';
	id[0] = dis_get_conn_id();
	id[1] = 0;
	dis_selective_update_service(Rpc_id, id); 
	free(Service_info_list);
}

void get_rpc_info(int *tag, char **buffer, int *size)
{

	if(tag){}
	*buffer = Rpc_info;
	*size = (int)strlen(Rpc_info)+1;
}

