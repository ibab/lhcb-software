/*
 * DIS (Delphi Information Server) Package implements a library of
 * routines to be used by servers.
 *
 * Started on		 : 10-11-91
 * Last modification : 28-07-94
 * Written by		 : C. Gaspar
 * Adjusted by	     : G.C. Ballintijn
 *
 */

#ifdef VMS
#	include <lnmdef.h>
#	include <ssdef.h>
#	include <descrip.h>
#	include <cfortran.h>
#endif

#include <time.h>
#ifdef VAX
#include <timeb.h>
#else
#include <sys/timeb.h>
#endif

#define DIMLIB
#include <dim.h>
#include <dis.h>

#define ALL 0
#define MORE 1
#define NONE 2

typedef struct req_ent {
	struct req_ent *next;
	struct req_ent *prev;
	int conn_id;
	int service_id;
	int req_id;
	int type;
	struct serv *service_ptr;
	int timeout;
	int format;
	int first_time;
	int delay_delete;
	int to_delete;
	TIMR_ENT *timr_ent;
	struct reqp_ent *reqpp;
} REQUEST;

typedef struct serv {
	struct serv *next;
	struct serv *prev;
	char name[MAX_NAME];
	int id;
	int type;
	char def[MAX_NAME];
	FORMAT_STR format_data[MAX_NAME/4];
	int *address;
	int size;
	void (*user_routine)();
	long tag;
	int registered;
	int quality;
	int user_secs;
	int user_millisecs;
	int tid;
	REQUEST *request_head;
} SERVICE;

typedef struct reqp_ent {
	struct reqp_ent *next;
	struct reqp_ent *prev;
	REQUEST *reqp;
} REQUEST_PTR;

typedef struct cli_ent {
	struct cli_ent *next;
	struct cli_ent *prev;
	int conn_id;
	REQUEST_PTR *requestp_head; 
} CLIENT;

static CLIENT *Client_head = (CLIENT *)0;	

static char Task_name[MAX_NAME];
static TIMR_ENT *Dns_timr_ent = (TIMR_ENT *)0;
static DIS_DNS_PACKET Dis_dns_packet = {0, 0, {0}};
static int Dis_n_services = 0;
static int Dns_dis_conn_id = 0;
static int Protocol;
static int Port_number;
static int Dis_first_time = 1;
static int Dis_conn_id = 0;
static int Curr_conn_id = 0;
static int Serving = 0;
static void (*Client_exit_user_routine)() = 0;
static void (*Exit_user_routine)() = 0;
static void (*Error_user_routine)() = 0;
static int Error_conn_id = 0;
static SERVICE *Unregistered_services[MAX_REGISTRATION_UNIT];
static int N_unregistered_services;

typedef struct exit_ent {
	struct exit_ent *next;
	int conn_id;
	int exit_id;
} EXIT_H;

static EXIT_H *Exit_h_head = (EXIT_H *)0;

/* Do not forget to increase when this file is modified */
static int Version_number = DIM_VERSION_NUMBER;
static int Dis_timer_q = 0;
static int Threads_off = 0;

static unsigned int Dis_service_id, Dis_client_id;
static int Updating_service_list = 0;
static int Last_client;

_DIM_PROTO( static void dis_insert_request, (int conn_id, DIC_PACKET *dic_packet,
				  int size, int status ) );
_DIM_PROTO( int execute_service,	(int req_id) );
_DIM_PROTO( void execute_command,	(SERVICE *servp, DIC_PACKET *packet) );
_DIM_PROTO( void register_dns_services,  (int flag) );
_DIM_PROTO( void register_services,  (int flag) );
_DIM_PROTO( void std_cmnd_handler,   (long *tag, int *cmnd_buff, int *size) );
_DIM_PROTO( void client_info,		(long *tag, int **bufp, int *size) );
_DIM_PROTO( void service_info,	   (long *tag, int **bufp, int *size) );
_DIM_PROTO( void add_exit_handler,   (int *tag, int *bufp, int *size) );
_DIM_PROTO( static void exit_handler,	   (int *tag, int *bufp, int *size) );
_DIM_PROTO( static void error_handler,	   (int conn_id, int severity, int errcode, char *reason) );
_DIM_PROTO( SERVICE *find_service,   (char *name) );
_DIM_PROTO( CLIENT *find_client,   (int conn_id) );
_DIM_PROTO( static int get_format_data, (FORMAT_STR *format_data, char *def) );
_DIM_PROTO( static int release_conn, (int conn_id, int print_flag) );
_DIM_PROTO( SERVICE *dis_hash_service_exists, (char *name) );
_DIM_PROTO( SERVICE *dis_hash_service_get_next, (int start) );

void dis_no_threads()
{
	Threads_off = 1;
}

static DIS_STAMPED_PACKET *Dis_packet = 0;
static int Dis_packet_size = 0;

int dis_set_buffer_size(size)
int size;
{
	if(Dis_packet_size)
		free(Dis_packet);
	Dis_packet = (DIS_STAMPED_PACKET *)malloc(DIS_STAMPED_HEADER + size);
	if(Dis_packet)
	{
		Dis_packet_size = DIS_STAMPED_HEADER + size;
		return(1);
	}
	else
		return(0);
}

static int check_service_name(name)
char *name;
{
	if(strlen(name) > (MAX_NAME - 1))
		return(0);
	return(1);
}

static unsigned do_dis_add_service( name, type, address, size, user_routine, tag )
register char *name;
register char *type;
void *address;
int size;
void (*user_routine)();
long tag;
{
	register SERVICE *new_serv;
	register int service_id;
	char str[512];
	int dis_hash_service_init();
	int dis_hash_service_insert();

	if(!check_service_name(name))
	{
		strcpy(str,"Service name too long: ");
		strcat(str,name);
		error_handler(0, DIM_ERROR, DIMSVCTOOLG, str);
		return((unsigned) 0);
	}
	dis_hash_service_init();
	if( find_service(name) )
	{
		strcpy(str,"Duplicate Service: ");
		strcat(str,name);
		error_handler(0, DIM_ERROR, DIMSVCDUPLC, str);
		return((unsigned) 0);
	}
	new_serv = (SERVICE *)malloc( sizeof(SERVICE) );
	strncpy( new_serv->name, name, MAX_NAME );
	if(type != (char *)0)
	{
		if (! get_format_data(new_serv->format_data, type))
		{
			strcpy(str,"Bad Format: ");
			strcat(str,name);
			error_handler(0, DIM_ERROR, DIMSVCFORMT, str);
			free(new_serv);
			return((unsigned) 0);
		}
		strcpy(new_serv->def,type); 
	}
	else
	{
		new_serv->format_data[0].par_bytes = 0;
		new_serv->def[0] = '\0';
	}
	new_serv->type = 0;
	new_serv->address = (int *)address;
	new_serv->size = size;
	new_serv->user_routine = user_routine;
	new_serv->tag = tag;
	new_serv->registered = 0;
	new_serv->quality = 0;
	new_serv->user_secs = 0;
	new_serv->tid = 0;
	service_id = id_get((void *)new_serv, SRC_DIS);
	new_serv->id = service_id;
	new_serv->request_head = (REQUEST *)malloc(sizeof(REQUEST));
	dll_init( (DLL *) (new_serv->request_head) );
	dis_hash_service_insert(new_serv);
	Dis_n_services++;
	return((unsigned)service_id);
}

#ifdef VxWorks
void dis_destroy(int tid)
{
register SERVICE *servp, *prevp;
int n_left = 0;

	prevp = 0;
	while( servp = dis_hash_service_get_next(prevp))
	{
		if(servp->tid == tid)
		{
			dis_remove_service(servp->id);
		}
		else
		{
			prevp = servp;
			n_left++;
		}
	}
	if(n_left == 5)
	{
		prevp = 0;
		while( servp = dis_hash_service_get_next(prevp))
		{
			dis_remove_service(servp->id);
		}
		dna_close(Dis_conn_id);
		dna_close(Dns_dis_conn_id);
		Dns_dis_conn_id = 0;
		Dis_first_time = 1;
		dtq_rem_entry(Dis_timer_q, Dns_timr_ent);
		Dns_timr_ent = NULL;
	}
}


#endif

unsigned dis_add_service( name, type, address, size, user_routine, tag)
register char *name;
register char *type;
void *address;
int size;
void (*user_routine)();
long tag;
{
	unsigned ret;
#ifdef VxWorks
	register SERVICE *servp;
#endif

	DISABLE_AST
	ret = do_dis_add_service( name, type, address, size,user_routine,tag);
#ifdef VxWorks
	servp = (SERVICE *)id_get_ptr(ret, SRC_DIS);
	servp->tid = taskIdSelf();
#endif
	ENABLE_AST
	return(ret);
}

static unsigned do_dis_add_cmnd( name, type, user_routine, tag )
register char *name;
register char *type;
void (*user_routine)();
long tag;
{
	register SERVICE *new_serv;
	register int service_id;
	char str[512];
	int dis_hash_service_init();
	int dis_hash_service_insert();

	if(!check_service_name(name))
	{
		strcpy(str,"Command name too long: ");
		strcat(str,name);
		error_handler(0, DIM_ERROR, DIMSVCTOOLG, str);
		return((unsigned) 0);
	}
	dis_hash_service_init();
	if( find_service(name) )
	{
		return((unsigned) 0);
	}
	new_serv = (SERVICE *)malloc(sizeof(SERVICE));
	strncpy(new_serv->name, name, MAX_NAME);
	if(type != (char *)0)
	{
		if( !get_format_data(new_serv->format_data, type))
			return((unsigned) 0);
		strcpy(new_serv->def,type); 
	}
	else
	{
		new_serv->format_data[0].par_bytes = 0;
		new_serv->def[0] = '\0';
	}
	new_serv->type = COMMAND;
	new_serv->address = 0;
	new_serv->size = 0;
	if(user_routine)
		new_serv->user_routine = user_routine;
	else
		new_serv->user_routine = std_cmnd_handler;
	new_serv->tag = tag;
	new_serv->tid = 0;
	new_serv->registered = 0;
	new_serv->quality = 0;
	new_serv->user_secs = 0;
	service_id = id_get((void *)new_serv, SRC_DIS);
	new_serv->id = service_id;
	new_serv->request_head = (REQUEST *)malloc(sizeof(REQUEST));
	dll_init( (DLL *) (new_serv->request_head) );
	dis_hash_service_insert(new_serv);
	Dis_n_services++;
	return((unsigned) service_id);
}

unsigned dis_add_cmnd( name, type, user_routine, tag ) 
register char *name;
register char *type;
void (*user_routine)();
long tag;
{
	unsigned ret;

	DISABLE_AST
	ret = do_dis_add_cmnd( name, type, user_routine, tag );
	ENABLE_AST
	return(ret);
}

void dis_add_client_exit_handler( user_routine) 
void (*user_routine)();
{

	DISABLE_AST
	Client_exit_user_routine = user_routine;
	ENABLE_AST
}

void dis_add_exit_handler( user_routine) 
void (*user_routine)();
{

	DISABLE_AST
	Exit_user_routine = user_routine;
	ENABLE_AST
}

void dis_add_error_handler( user_routine) 
void (*user_routine)();
{

	DISABLE_AST
	Error_user_routine = user_routine;
	ENABLE_AST
}

static int get_format_data(format_data, def)
register FORMAT_STR *format_data;
register char *def;
{
	register char code, last_code = 0;
	int num;

	code = *def;
	while(*def)
	{
		if(code != last_code)
		{
			format_data->par_num = 0;
			format_data->flags = 0;
			switch(code)
			{
				case 'i':
				case 'I':
				case 'l':
				case 'L':
					format_data->par_bytes = SIZEOF_LONG;
					format_data->flags |= SWAPL;
					break;
				case 'x':
				case 'X':
					format_data->par_bytes = SIZEOF_DOUBLE;
					format_data->flags |= SWAPD;
					break;
				case 's':
				case 'S':
					format_data->par_bytes = SIZEOF_SHORT;
					format_data->flags |= SWAPS;
					break;
				case 'f':
				case 'F':
					format_data->par_bytes = SIZEOF_LONG;
					format_data->flags |= SWAPL;
#ifdef vms      	
					format_data->flags |= IT_IS_FLOAT;
#endif
					break;
				case 'd':
				case 'D':
					format_data->par_bytes = SIZEOF_DOUBLE;
					format_data->flags |= SWAPD;
#ifdef vms
					format_data->flags |= IT_IS_FLOAT;
#endif
					break;
				case 'c':
				case 'C':
					format_data->par_bytes = SIZEOF_CHAR;
					format_data->flags |= NOSWAP;
					break;
			}
		}
		def++;
		if(*def != ':')
		{
			if(*def)
			{
/*
				printf("Bad service definition parsing\n");
				fflush(stdout);

				error_handler("Bad service definition parsing",2);
*/
				return(0);
			}
			else
				format_data->par_num = 0;
		}
		else
		{
			def++;
			sscanf(def,"%d",&num);
			format_data->par_num += num;
			while((*def != ';') && (*def != '\0'))
				def++;
			if(*def)
				def++;
		}
		last_code = code;
		code = *def;
		if(code != last_code)
			format_data++;
	}
	format_data->par_bytes = 0;
	return(1);
}

void recv_dns_dis_rout( conn_id, packet, size, status )
int conn_id, size, status;
DNS_DIS_PACKET *packet;
{
	char str[128];
	int dns_timr_time;
	extern int rand_tmout(int, int);
	extern int open_dns(void (*)(), void (*)(), int, int, int);

	switch(status)
	{
	case STA_DISC:	   /* connection broken */
		if( Dns_timr_ent ) {
			dtq_rem_entry( Dis_timer_q, Dns_timr_ent );
			Dns_timr_ent = NULL;
		}
		dna_close(Dns_dis_conn_id);
		if(Serving)
		{
			Dns_dis_conn_id = open_dns(recv_dns_dis_rout, error_handler,
					DIS_DNS_TMOUT_MIN, DIS_DNS_TMOUT_MAX, SRC_DIS );
			if(Dns_dis_conn_id == -2)
				error_handler(0, DIM_FATAL, DIMDNSUNDEF, "DIM_DNS_NODE undefined");
		}
		break;
	case STA_CONN:		/* connection received */
		Dns_dis_conn_id = conn_id;
		register_services(ALL);
		dns_timr_time = rand_tmout(WATCHDOG_TMOUT_MIN, 
							 WATCHDOG_TMOUT_MAX);
		Dns_timr_ent = dtq_add_entry( Dis_timer_q,
						  dns_timr_time,
						  register_services, NONE ); 
		break;
	default :	   /* normal packet */
		if(vtohl(packet->size) != DNS_DIS_HEADER)
			break;
		switch( vtohl(packet->type) )
		{
		case DNS_DIS_REGISTER :
			sprintf(str, 
				"%s: Watchdog Timeout, DNS requests registration",
				Task_name);
			error_handler(0, DIM_WARNING, DIMDNSTMOUT, str);
			register_services(ALL);
			break;
		case DNS_DIS_KILL :
			sprintf(str,
				"%s: Some Services already known to DNS",
				Task_name);
			/*
			exit(2);
			*/
			error_handler(0, DIM_FATAL, DIMDNSDUPLC, str);
			dis_stop_serving();
/*
			exit_tag = 0;
			exit_code = 2;
			exit_size = sizeof(int);
			exit_handler(&exit_tag, &exit_code, &exit_size);
*/
			break;
		case DNS_DIS_STOP :
			sprintf(str, 
				"%s: DNS refuses connection",Task_name);
/*
			exit(2);
*/
			error_handler(0, DIM_FATAL, DIMDNSREFUS, str);
			dis_stop_serving();
/*
			exit_tag = 0;
			exit_code = 2;
			exit_size = sizeof(int);
			exit_handler(&exit_tag, &exit_code, &exit_size);
*/
			break;
		case DNS_DIS_EXIT :
			sprintf(str, 
				"%s: DNS requests Exit",Task_name);
			error_handler(0, DIM_FATAL, DIMDNSEXIT, str);
			break;
		}
		break;
	}
}


/* register services within the name server
 *
 * Send services uses the DNA package. services is a linked list of services
 * stored by add_service.
 */

void register_dns_services(flag)
register int flag;
{
	register DIS_DNS_PACKET *dis_dns_p = &Dis_dns_packet;
	register int n_services;
	register SERVICE *servp;
	register SERVICE_REG *serv_regp;
	extern int get_node_addr(char *);
	int dis_hash_service_registered();

	if(!dis_dns_p->src_type)
	{
		get_node_name( dis_dns_p->node_name );
/*
		strcpy( dis_dns_p->task_name, Task_name );
*/
		strncpy( dis_dns_p->task_name, Task_name,
			MAX_TASK_NAME-4 );
		dis_dns_p->task_name[MAX_TASK_NAME-4-1] = '\0';
		get_node_addr( dis_dns_p->node_addr );
/*
		dis_dns_p->port = htovl(Port_number);
*/
		dis_dns_p->pid = htovl(getpid());
		dis_dns_p->protocol = htovl(Protocol);
		dis_dns_p->src_type = htovl(SRC_DIS);
		dis_dns_p->format = htovl(MY_FORMAT);
	}
	dis_dns_p->port = htovl(Port_number);
	serv_regp = dis_dns_p->services;
	n_services = 0;
	if( flag == NONE ) {
		dis_dns_p->n_services = htovl(n_services);
		dis_dns_p->size = htovl( DIS_DNS_HEADER + 
			(n_services*sizeof(SERVICE_REG)));
		if(Dns_dis_conn_id > 0)
		{
			if(!dna_write(Dns_dis_conn_id, &Dis_dns_packet, 
				DIS_DNS_HEADER + n_services*sizeof(SERVICE_REG)))
			{
				release_conn(Dns_dis_conn_id,0);
			}
		}
		return;
	}
	servp = 0;
	while( (servp = dis_hash_service_get_next(servp)))
	{
		if( flag == MORE ) 
		{
			if( servp->registered )
			{
				continue;
			}
		}
		strcpy( serv_regp->service_name, servp->name );
		strcpy( serv_regp->service_def, servp->def );
		if(servp->type == COMMAND)
			serv_regp->service_id = htovl( servp->id | 0x10000000);
		else
			serv_regp->service_id = htovl( servp->id );
		serv_regp++;
		n_services++;
		dis_hash_service_registered(servp);
/*
		servp->registered = 1;
*/
		if( n_services == MAX_SERVICE_UNIT ) 
		{
			dis_dns_p->n_services = htovl(n_services);
			dis_dns_p->size = htovl(DIS_DNS_HEADER +
				n_services * sizeof(SERVICE_REG));
			if(Dns_dis_conn_id > 0)
			{
				if( !dna_write(Dns_dis_conn_id,
					   &Dis_dns_packet, 
					   DIS_DNS_HEADER + n_services *
						sizeof(SERVICE_REG)) )
				{
					release_conn(Dns_dis_conn_id,0);
				}
			}
			serv_regp = dis_dns_p->services;
			n_services = 0;
		}
	}
	if( n_services ) 
	{
		dis_dns_p->n_services = htovl(n_services);
		dis_dns_p->size = htovl(DIS_DNS_HEADER +
					n_services * sizeof(SERVICE_REG));
		if(Dns_dis_conn_id > 0)
		{
			if( !dna_write(Dns_dis_conn_id, &Dis_dns_packet,
				DIS_DNS_HEADER + n_services * sizeof(SERVICE_REG)))
			{
				release_conn(Dns_dis_conn_id,0);
			}
		}
	}
}

int send_dns_update_packet()
{
  DIS_DNS_PACKET *dis_dns_p = &Dis_dns_packet;
  int n_services;
  SERVICE_REG *serv_regp;

  n_services = 1;
  dis_dns_p->n_services = htovl(n_services);
  dis_dns_p->size = htovl(DIS_DNS_HEADER +
					n_services * sizeof(SERVICE_REG));
  serv_regp = dis_dns_p->services;
  strcpy( serv_regp->service_name, "DUMMY_UPDATE_PACKET" );
  if(Dns_dis_conn_id > 0)
  {
      if( !dna_write(Dns_dis_conn_id, &Dis_dns_packet,
		     DIS_DNS_HEADER + n_services * sizeof(SERVICE_REG)))
	  {
		release_conn(Dns_dis_conn_id,0);
	  }
  }
  return(1);
}


void register_services(flag)
register int flag;
{
	register DIS_DNS_PACKET *dis_dns_p = &Dis_dns_packet;
	register int n_services, tot_n_services;
	register SERVICE *servp;
	register SERVICE_REG *serv_regp;
	SERVICE *dis_hash_service_get_next_register();
	extern int get_node_addr();
	int dis_hash_service_registered();

	if(!dis_dns_p->src_type)
	{
		get_node_name( dis_dns_p->node_name );
/*
		strcpy( dis_dns_p->task_name, Task_name );
*/
		strncpy( dis_dns_p->task_name, Task_name,
			MAX_TASK_NAME-4 );
		dis_dns_p->task_name[MAX_TASK_NAME-4-1] = '\0';
		get_node_addr( dis_dns_p->node_addr );
/*
		dis_dns_p->port = htovl(Port_number);
*/
		dis_dns_p->pid = htovl(getpid());
		dis_dns_p->protocol = htovl(Protocol);
		dis_dns_p->src_type = htovl(SRC_DIS);
		dis_dns_p->format = htovl(MY_FORMAT);
	}

	dis_dns_p->port = htovl(Port_number);
	serv_regp = dis_dns_p->services;
	n_services = 0;
	tot_n_services = 0;
	if( flag == NONE ) {
		dis_dns_p->n_services = htovl(n_services);
		dis_dns_p->size = htovl( DIS_DNS_HEADER + 
			(n_services*sizeof(SERVICE_REG)));
		if(Dns_dis_conn_id > 0)
		{
			if(!dna_write(Dns_dis_conn_id, &Dis_dns_packet, 
				DIS_DNS_HEADER + n_services*sizeof(SERVICE_REG)))
			{
				release_conn(Dns_dis_conn_id,0);
			}
		}
		return;
	}
	if(flag == ALL)
	{
		servp = 0;
		while( (servp = dis_hash_service_get_next(servp)))
		{
			servp->registered  = 0;
		}
	}
	servp = 0;
	while( (servp = dis_hash_service_get_next_register(servp)))
	{
		if( flag == MORE ) 
		{
			if( servp->registered )
			{
				continue;
			}
		}

		strcpy( serv_regp->service_name, servp->name );
		strcpy( serv_regp->service_def, servp->def );
		if(servp->type == COMMAND)
			serv_regp->service_id = htovl( servp->id | 0x10000000);
		else
			serv_regp->service_id = htovl( servp->id );

		serv_regp++;
		n_services++;
		dis_hash_service_registered(servp);
		if( n_services == MAX_SERVICE_UNIT )
		{
			dis_dns_p->n_services = htovl(n_services);
			dis_dns_p->size = htovl(DIS_DNS_HEADER +
				n_services * sizeof(SERVICE_REG));
			if(Dns_dis_conn_id > 0)
			{
				if( !dna_write(Dns_dis_conn_id,
					   &Dis_dns_packet, 
					   DIS_DNS_HEADER + n_services *
						sizeof(SERVICE_REG)) )
				{
					release_conn(Dns_dis_conn_id,0);
				}
			}
			serv_regp = dis_dns_p->services;
			tot_n_services += MAX_SERVICE_UNIT;
			n_services = 0;
			continue;
		}
	}
	if( n_services ) 
	{
		dis_dns_p->n_services = htovl(n_services);
		dis_dns_p->size = htovl(DIS_DNS_HEADER +
					n_services * sizeof(SERVICE_REG));
		if(Dns_dis_conn_id > 0)
		{
			if( !dna_write(Dns_dis_conn_id, &Dis_dns_packet,
				DIS_DNS_HEADER + n_services * sizeof(SERVICE_REG)))
			{
				release_conn(Dns_dis_conn_id,0);
			}

		}
		tot_n_services += n_services;
	}
	if(tot_n_services >= MAX_REGISTRATION_UNIT)
	{
	  send_dns_update_packet();
	}
}

void unregister_service(servp)
register SERVICE *servp;
{
	register DIS_DNS_PACKET *dis_dns_p = &Dis_dns_packet;
	register int n_services;
	register SERVICE_REG *serv_regp;
	extern int get_node_addr();

	if(Dns_dis_conn_id > 0)
	{
		if(!dis_dns_p->src_type)
		{
			get_node_name( dis_dns_p->node_name );
/*
			strcpy( dis_dns_p->task_name, Task_name );
*/
			strncpy( dis_dns_p->task_name, Task_name,
				MAX_TASK_NAME-4 );
			dis_dns_p->task_name[MAX_TASK_NAME-4-1] = '\0';
			get_node_addr( dis_dns_p->node_addr );
			dis_dns_p->port = htovl(Port_number);
			dis_dns_p->protocol = htovl(Protocol);
			dis_dns_p->src_type = htovl(SRC_DIS);
			dis_dns_p->format = htovl(MY_FORMAT);
		}
		serv_regp = dis_dns_p->services;
		strcpy( serv_regp->service_name, servp->name );
		strcpy( serv_regp->service_def, servp->def );
		serv_regp->service_id = htovl( servp->id | 0x80000000);
		serv_regp++;
		n_services = 1;
		servp->registered = 0;
		dis_dns_p->n_services = htovl(n_services);
		dis_dns_p->size = htovl(DIS_DNS_HEADER +
				n_services * sizeof(SERVICE_REG));

		if( !dna_write(Dns_dis_conn_id, &Dis_dns_packet, 
			DIS_DNS_HEADER + n_services * sizeof(SERVICE_REG)) )
		{
			release_conn(Dns_dis_conn_id,0);
		}
		if(Dis_service_id)
			dis_update_service(Dis_service_id);
	}
}

void do_update_service_list(service_id)
int service_id;
{
	Updating_service_list = 0;
	dis_update_service(service_id);
}

/* start serving client requests
 *
 * Using the DNA package start accepting requests from clients.
 * When a request arrives the routine "dis_insert_request" will be executed.
 */
int dis_start_serving(task)
char *task;
{
	char str0[MAX_NAME], str1[MAX_NAME],str2[MAX_NAME],
	  str3[MAX_NAME],str4[MAX_NAME];
	char task_name_aux[MAX_TASK_NAME];
	void dim_init_threads(void);
	extern int open_dns();

	if(!Threads_off)
	{
		dim_init_threads();
	}
	{
	DISABLE_AST
	  /*
#ifdef VxWorks
	taskDeleteHookAdd(remove_all_services);
	printf("Adding delete hook\n");
#endif
*/

	if(!Client_head) 
	{
		Client_head = (CLIENT *)malloc(sizeof(CLIENT));
		dll_init( (DLL *) Client_head );
	}
	Serving = 1;
	if(Dis_first_time)
	{
		Dis_first_time = 0;

		strncpy( task_name_aux, task, MAX_TASK_NAME );
		task_name_aux[MAX_TASK_NAME-1] = '\0';

		sprintf(str0, "%s/VERSION_NUMBER", task);
		sprintf(str1, "%s/CLIENT_LIST", task);
		sprintf(str2, "%s/SERVICE_LIST", task);
		sprintf(str3, "%s/SET_EXIT_HANDLER", task);
		sprintf(str4, "%s/EXIT", task);
/*
		if( strlen(task) > 16 )
			task[16] = '\0';
*/
		Port_number = SEEK_PORT;
		if( !(Dis_conn_id = dna_open_server( task_name_aux, dis_insert_request, 
			&Protocol, &Port_number, error_handler) ))
		{
			ENABLE_AST
			return(0);
		}
		do_dis_add_service( str0, "L", &Version_number,
				 sizeof(Version_number), 0, 0 );
		Dis_client_id = do_dis_add_service( str1, "C", 0, 0, client_info, 0 );
		Dis_service_id = do_dis_add_service( str2, "C", 0, 0, service_info, 0 );
		do_dis_add_cmnd( str3, "L:1", add_exit_handler, 0 );
		do_dis_add_cmnd( str4, "L:1", exit_handler, 0 );
		strcpy( Task_name, task );
	}
	if(!Dis_timer_q)
		Dis_timer_q = dtq_create();
	if( !Dns_dis_conn_id )
	{
		if(!strcmp(task,"DIS_DNS"))
		{
			register_dns_services(ALL);
			ENABLE_AST
			return(id_get(&Dis_dns_packet, SRC_DIS));
		}
		else
		{
		
			Dns_dis_conn_id = open_dns(recv_dns_dis_rout, error_handler,
					DIS_DNS_TMOUT_MIN, DIS_DNS_TMOUT_MAX, SRC_DIS );
			if(Dns_dis_conn_id == -2)
				error_handler(0, DIM_FATAL, DIMDNSUNDEF, "DIM_DNS_NODE undefined");
		}
	}
	else
	{
		register_services(MORE);
		if(Dis_service_id)
		{
/*
			dis_update_service(Dis_service_id);
*/
			if(!Updating_service_list)
			{
				dtq_start_timer(1, do_update_service_list, Dis_service_id);
				Updating_service_list = 1;
			}
		}
	}	
	ENABLE_AST
	}
	return(1);
}


/* asynchrounous reception of requests */
/*
	Called by DNA package.
	A request has arrived, queue it to process later - dis_ins_request
*/
static void dis_insert_request(conn_id, dic_packet, size, status)
register int conn_id;
register DIC_PACKET *dic_packet;
register int size, status;
{
	register SERVICE *servp;
	register REQUEST *newp, *reqp;
	CLIENT *clip;
	REQUEST_PTR *reqpp;
	int type, new_client = 0, found = 0;
	int find_release_request();

	/* status = 1 => new connection, status = -1 => conn. lost */
	if(!Client_head) 
	{
		Client_head = (CLIENT *)malloc(sizeof(CLIENT));
		dll_init( (DLL *) Client_head );
	}
	if(status != 0)
	{
		if(status == -1) /* release all requests from conn_id */
		{
			release_conn(conn_id, 0);
		}
	} 
	else 
	{
		if(!(servp = find_service(dic_packet->service_name)))
		{
			release_conn(conn_id, 0);
			return;
		}
		dic_packet->type = vtohl(dic_packet->type);
		type = dic_packet->type & 0xFFF;
		/*
		if(type == COMMAND) 
		{
			Curr_conn_id = conn_id;
			execute_command(servp, dic_packet);
			Curr_conn_id = 0;
			return;
		}
		*/
		if(type == DIM_DELETE) 
		{
			find_release_request(conn_id, vtohl(dic_packet->service_id));
			return;
		}
		newp = (REQUEST *)/*my_*/malloc(sizeof(REQUEST));
		newp->service_ptr = servp;
		newp->service_id = vtohl(dic_packet->service_id);
		newp->type = dic_packet->type;
		newp->timeout = vtohl(dic_packet->timeout);
		newp->format = vtohl(dic_packet->format);
		newp->conn_id = conn_id;
		newp->first_time = 1;
		newp->delay_delete = 0;
		newp->to_delete = 0;
		newp->timr_ent = 0;
		newp->req_id = id_get((void *)newp, SRC_DIS);
		newp->reqpp = 0;
		if(type == ONCE_ONLY) 
		{
			execute_service(newp->req_id);
			id_free(newp->req_id, SRC_DIS);
			free(newp);
			return;
		}
		if(type == COMMAND) 
		{
			Curr_conn_id = conn_id;
			execute_command(servp, dic_packet);
			Curr_conn_id = 0;
			reqp = servp->request_head;
			while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
				(DLL *) reqp)) ) 
			{
				if(reqp->conn_id == conn_id)
				{
					id_free(newp->req_id, SRC_DIS);
					free(newp);
					found = 1;
					break;
				}
			}
			if(!found)
				dll_insert_queue( (DLL *) servp->request_head, (DLL *) newp );
			return;
		}
		dll_insert_queue( (DLL *) servp->request_head, (DLL *) newp );
		if(!(clip = find_client(conn_id)))
		{
			clip = (CLIENT *)malloc(sizeof(CLIENT));
			clip->conn_id = conn_id;
			clip->requestp_head = (REQUEST_PTR *)malloc(sizeof(REQUEST_PTR));
			dll_init( (DLL *) clip->requestp_head );
			dll_insert_queue( (DLL *) Client_head, (DLL *) clip );
			new_client = 1;
		}
		reqpp = (REQUEST_PTR *)malloc(sizeof(REQUEST_PTR));
		reqpp->reqp = newp;
		dll_insert_queue( (DLL *) clip->requestp_head, (DLL *) reqpp );
		newp->reqpp = reqpp;
		if((type != MONIT_ONLY) && (type != UPDATE))
		{
			execute_service(newp->req_id);
		}
		if(type != MONIT_ONLY)
		{
			if(newp->timeout != 0)
			{
				newp->timr_ent = dtq_add_entry( Dis_timer_q,
							newp->timeout, 
							execute_service,
							newp->req_id );
			}
		}
		if(new_client)
		{
			Last_client = conn_id;
			if(Dis_client_id)
			  dis_update_service(Dis_client_id);
		}
	}
}

/* A timeout for a timed or monitored service occured, serve it. */

int execute_service( req_id )
int req_id;
{
	int *buffp, size;
	register REQUEST *reqp;
	register SERVICE *servp;
	char str[80], def[MAX_NAME];
	register char *ptr;
	int last_conn_id;
	int *pkt_buffer, header_size, aux;
#ifdef WIN32
	struct timeb timebuf;
#else
	struct timeval tv;
	struct timezone *tz;
#endif
	FORMAT_STR format_data_cp[MAX_NAME/4];

	reqp = (REQUEST *)id_get_ptr(req_id, SRC_DIS);
	if(!reqp)
		return(0);
	if(reqp->to_delete)
		return(0);
	reqp->delay_delete++;
	servp = reqp->service_ptr;
	last_conn_id = Curr_conn_id;
	Curr_conn_id = reqp->conn_id;
	ptr = servp->def;
	if(servp->type == COMMAND)
	{
		sprintf(str,"This is a COMMAND Service");
		buffp = (int *)str;
		size = 26;
		sprintf(def,"c:26");
		ptr = def;
	}
	else if( servp->user_routine != 0 ) 
	{
		(servp->user_routine)( &servp->tag, &buffp, &size,
					&reqp->first_time );
		reqp->first_time = 0;
		
	} 
	else 
	{
		buffp = servp->address;
		size = servp->size;
	}
	Curr_conn_id = last_conn_id;
/* send even if no data but not if negative */
	if( size  < 0)
	{
		reqp->delay_delete--;
		return(0);
	}
	if( DIS_STAMPED_HEADER + size > Dis_packet_size ) 
	{
		if( Dis_packet_size )
			free( Dis_packet );
		Dis_packet = (DIS_STAMPED_PACKET *)malloc(DIS_STAMPED_HEADER + size);
		if(!Dis_packet)
		{
			reqp->delay_delete--;
			return(0);
		}
		Dis_packet_size = DIS_STAMPED_HEADER + size;
	}
	Dis_packet->service_id = htovl(reqp->service_id);
	if((reqp->type & 0xFF000) == STAMPED)
	{
		pkt_buffer = ((DIS_STAMPED_PACKET *)Dis_packet)->buffer;
		header_size = DIS_STAMPED_HEADER;
		if(!servp->user_secs)
		{
#ifdef WIN32
			ftime(&timebuf);
			aux = timebuf.millitm;
			Dis_packet->time_stamp[0] = htovl(aux);
			Dis_packet->time_stamp[1] = htovl(timebuf.time);
#else
			tz = 0;
		        gettimeofday(&tv, tz);
			aux = tv.tv_usec / 1000;
			Dis_packet->time_stamp[0] = htovl(aux);
			Dis_packet->time_stamp[1] = htovl(tv.tv_sec);
#endif
		}
		else
		{
			aux = /*0xc0de0000 |*/ servp->user_millisecs;
			Dis_packet->time_stamp[0] = htovl(aux);
			Dis_packet->time_stamp[1] = htovl(servp->user_secs);
		}
		Dis_packet->reserved[0] = htovl(0xc0dec0de);
		Dis_packet->quality = htovl(servp->quality);
	}
	else
	{
		pkt_buffer = ((DIS_PACKET *)Dis_packet)->buffer;
		header_size = DIS_HEADER;
	}
	memcpy(format_data_cp, servp->format_data, sizeof(format_data_cp));
	size = copy_swap_buffer_out(reqp->format, format_data_cp, 
		pkt_buffer,
		buffp, size);
	Dis_packet->size = htovl(header_size + size);
	if( !dna_write_nowait(reqp->conn_id, Dis_packet, header_size + size) ) 
	{
		reqp->to_delete = 1;
	}
/*
	else
	{
		if((reqp->type & 0xFFF) == MONITORED)
		{
			if(reqp->timr_ent)
				dtq_clear_entry(reqp->timr_ent);
		}
	}
*/
	reqp->delay_delete--;
	return(1);
}

void remove_service( req_id )
int req_id;
{
	register REQUEST *reqp;
	register SERVICE *servp;
	static DIS_PACKET *dis_packet;
	static int packet_size = 0;
	int service_id;

	reqp = (REQUEST *)id_get_ptr(req_id, SRC_DIS);
	servp = reqp->service_ptr;
	if( !packet_size ) {
		dis_packet = (DIS_PACKET *)malloc(DIS_HEADER);
		packet_size = DIS_HEADER;
	}
	service_id = (reqp->service_id | 0x80000000);
	dis_packet->service_id = htovl(service_id);
	dis_packet->size = htovl(DIS_HEADER);
	if( !dna_write(reqp->conn_id, dis_packet, DIS_HEADER) ) 
	{
		release_conn(reqp->conn_id,0);
	}
}

void execute_command(servp, packet)
register SERVICE *servp;
DIC_PACKET *packet;
{
	int size;
	int format;
	FORMAT_STR format_data_cp[MAX_NAME/4], *formatp;
	static int *buffer;
	static int buffer_size = 0;
	int add_size;

	size = vtohl(packet->size) - DIC_HEADER;
	add_size = size + (size/2);
	if(!buffer_size)
	{
		buffer = (int *)malloc(add_size);
		buffer_size = add_size;
	} 
	else 
	{
		if( add_size > buffer_size ) 
		{
			free(buffer);
			buffer = (int *)malloc(add_size);
			buffer_size = add_size;
		}
	}

	if(servp->user_routine != 0)
	{
		format = vtohl(packet->format);
		memcpy(format_data_cp, servp->format_data, sizeof(format_data_cp));
		if((format & 0xF) == ((MY_FORMAT) & 0xF)) 
		{
			for(formatp = format_data_cp; formatp->par_bytes; formatp++)
			{
				if(formatp->flags & IT_IS_FLOAT)
					formatp->flags |= (format & 0xf0);
				formatp->flags &= 0xFFF0;	/* NOSWAP */
			}
		}
		else
		{
			for(formatp = format_data_cp; formatp->par_bytes; formatp++)
			{
				if(formatp->flags & IT_IS_FLOAT)
					formatp->flags |= (format & 0xf0);
			}
		}
		size = copy_swap_buffer_in(format_data_cp, 
						 buffer, 
						 packet->buffer, size);
		(servp->user_routine)(&servp->tag, buffer, &size);
	}
}

void dis_report_service(serv_name)
char *serv_name;
{
	register SERVICE *servp;
	register REQUEST *reqp;
	int to_delete = 0, more;

	
	DISABLE_AST
	servp = find_service(serv_name);
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) )
	{
		if((reqp->type & 0xFFF) != TIMED_ONLY)
		{
			execute_service(reqp->req_id);
			if(reqp->to_delete)
				to_delete = 1;
		}
	}
	if(to_delete)
	{
		do
		{
			more = 0;
			reqp = servp->request_head;
			while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
				(DLL *) reqp)) )
			{
				if(reqp->to_delete)
				{
					more = 1;
					release_conn(reqp->conn_id, 1);
					break;
				}
			}
		}while(more);
	}
	ENABLE_AST
}

int dis_update_service(service_id)
register unsigned service_id;
{
int do_update_service();

	return(do_update_service(service_id,0));
}

int dis_selective_update_service(service_id, client_ids)
register unsigned service_id;
int *client_ids;
{
int do_update_service();

	return(do_update_service(service_id, client_ids));
}

int check_client(reqp, client_ids)
REQUEST *reqp;
int *client_ids;
{
	if(!client_ids)
		return(1);
	while(*client_ids)
	{
		if(reqp->conn_id == *client_ids)
		{
			return(1);
		}
		client_ids++;
	}
	return(0);
}

int do_update_service(service_id, client_ids)
register unsigned service_id;
int *client_ids;
{
	register REQUEST *reqp;
	register SERVICE *servp;
	register int found = 0;
	int to_delete = 0, more;
	char str[128];

	DISABLE_AST
	if(!service_id)
	{
		sprintf(str, "Update Service - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
		ENABLE_AST
		return(found);
	}
	servp = (SERVICE *)id_get_ptr(service_id, SRC_DIS);
	if(!servp)
	{
		ENABLE_AST
		return(found);
	}
	if(servp->id != (int)service_id)
	{
		ENABLE_AST
		return(found);
	}
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) ) 
	{
		reqp->delay_delete = 1;
	}
	ENABLE_AST
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) ) 
	{
		if(check_client(reqp, client_ids))
		{
			if( (reqp->type & 0xFFF) != TIMED_ONLY ) 
			{
				DISABLE_AST
				execute_service(reqp->req_id);
				found++;
				ENABLE_AST
			}
		}
	}
	{
	DISABLE_AST
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) ) 
	{
		reqp->delay_delete = 0;
		if(reqp->to_delete)
			to_delete = 1;
	}
	ENABLE_AST
	}
	if(to_delete)
	{
		DISABLE_AST
		do
		{
			more = 0;
			reqp = servp->request_head;
			while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
				(DLL *) reqp)) ) 
			{
				if(reqp->to_delete)
				{
					more = 1;
					reqp->to_delete = 0;
					release_conn(reqp->conn_id, 1);
					break;
				}
			}
		}while(more);
		ENABLE_AST
	}
	return(found);
}

int dis_get_timeout(service_id, client_id)
register unsigned service_id;
int client_id;
{
	register REQUEST *reqp;
	register SERVICE *servp;
	char str[128];

	if(!service_id)
	{
		sprintf(str,"Get Timeout - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
		return(-1);
	}
	servp = (SERVICE *)id_get_ptr(service_id, SRC_DIS);
	if(!servp)
	{
		return(-1);
	}
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) ) 
	{
		if(reqp->conn_id == client_id)
			return(reqp->timeout);
	}
	return(-1);
}

void dis_set_quality( serv_id, quality )
unsigned serv_id;
int quality;
{
	register SERVICE *servp;
	char str[128];

	DISABLE_AST
	if(!serv_id)
	{
		sprintf(str,"Set Quality - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
	    ENABLE_AST
		return;
	}
	servp = (SERVICE *)id_get_ptr(serv_id, SRC_DIS);
	if(!servp)
	{
	    ENABLE_AST
		return;
	}
	if(servp->id != (int)serv_id)
	{
	    ENABLE_AST
		return;
	}
	servp->quality = quality;
	ENABLE_AST
}

void dis_set_timestamp( serv_id, secs, millisecs )
unsigned serv_id;
int secs, millisecs;
{
	register SERVICE *servp;
	char str[128];

	DISABLE_AST
	if(!serv_id)
	{
		sprintf(str,"Set Timestamp - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
	    ENABLE_AST
		return;
	}
	servp = (SERVICE *)id_get_ptr(serv_id, SRC_DIS);
	if(!servp)
	{
	    ENABLE_AST
		return;
	}
	if(servp->id != (int)serv_id)
	{
	    ENABLE_AST
		return;
	}
	servp->user_secs = secs;
/*
	servp->user_millisecs = (millisecs & 0xffff);
*/
	servp->user_millisecs = millisecs;
	ENABLE_AST
}

void dis_send_service(service_id, buffer, size)
register unsigned service_id;
int *buffer;
int size;
{
	register REQUEST *reqp, *prevp;
	register SERVICE *servp;
	static DIS_PACKET *dis_packet;
	static int packet_size = 0;
	int conn_id;
	char str[128];

	DISABLE_AST
	if( !service_id ) {
		sprintf(str,"Send Service - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
		ENABLE_AST
		return;
	}
	servp = (SERVICE *)id_get_ptr(service_id, SRC_DIS);
	if(!packet_size)
	{
		dis_packet = (DIS_PACKET *)malloc(DIS_HEADER+size);
		packet_size = DIS_HEADER + size;
	} 
	else 
	{
		if( DIS_HEADER+size > packet_size ) 
		{
			free(dis_packet);
			dis_packet = (DIS_PACKET *)malloc(DIS_HEADER+size);
			packet_size = DIS_HEADER+size;
		}
	}
	prevp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) prevp)) ) 
	{
		dis_packet->service_id = htovl(reqp->service_id);
		memcpy(dis_packet->buffer, buffer, size);
		dis_packet->size = htovl(DIS_HEADER + size);

		conn_id = reqp->conn_id;
		if( !dna_write_nowait(conn_id, dis_packet, size + DIS_HEADER) )
		{
			release_conn(conn_id,1);
		}
		else
			prevp = reqp;
	}
	ENABLE_AST
}

int dis_remove_service(service_id)
unsigned service_id;
{
	register REQUEST *reqp, *auxp;
	register SERVICE *servp;
	REQUEST_PTR *reqpp;
	int found = 0;
	char str[128];
	int release_request();
	int dis_hash_service_remove();

	DISABLE_AST
	if(!service_id)
	{
		sprintf(str,"Remove Service - Invalid service id");
		error_handler(0, DIM_ERROR, DIMSVCINVAL, str);
		ENABLE_AST
		return(found);
	}
	servp = (SERVICE *)id_get_ptr(service_id, SRC_DIS);
	if(!servp)
	{
		ENABLE_AST
		return(found);
	}
	if(servp->id != (int)service_id)
	{
		ENABLE_AST
		return(found);
	}
	/* remove from name server */
	
	unregister_service(servp);
	/* Release client requests and remove from actual clients */
	reqp = servp->request_head;
	while( (reqp = (REQUEST *) dll_get_next((DLL *)servp->request_head,
		(DLL *) reqp)) )
	{
		remove_service(reqp->req_id);
		auxp = reqp->prev;
		reqpp = (REQUEST_PTR *) reqp->reqpp;
		release_request(reqp, reqpp, 1);
		found = 1;
		reqp = auxp;
	}
	if(servp->id == (int)Dis_service_id)
	  Dis_service_id = 0;
	if(servp->id == (int)Dis_client_id)
	  Dis_client_id = 0;
	dis_hash_service_remove(servp);
	id_free(servp->id, SRC_DIS);
	free(servp->request_head);
	free(servp);
	ENABLE_AST
	Dis_n_services--;
	if(Serving)
	{
		if(Dis_n_services == 5)
		{
			dis_stop_serving();
		}
	}
	return(found);
}

void dis_stop_serving()
{
register SERVICE *servp, *prevp;
SERVICE *dis_hash_service_get_next_remove();
int dis_hash_service_init();
void dim_stop_threads(void);

	Serving = 0;
	dis_hash_service_init();
	prevp = 0;
	if(Dis_conn_id)
	{
		dna_close(Dis_conn_id);
		Dis_conn_id = 0;
	}
	if(Dns_dis_conn_id)
	{
		dna_close(Dns_dis_conn_id);
		Dns_dis_conn_id = 0;
	}
	while( (servp = dis_hash_service_get_next_remove(prevp)) )
	{
		prevp = servp;
		dis_remove_service(servp->id);
	}
/*
	if(Dis_conn_id)
		dna_close(Dis_conn_id);
	if(Dns_dis_conn_id)
		dna_close(Dns_dis_conn_id);
	Dns_dis_conn_id = 0;
*/
	Dis_first_time = 1;
	if(Dns_timr_ent)
	{
		dtq_rem_entry(Dis_timer_q, Dns_timr_ent);
		Dns_timr_ent = NULL;
	}
	dtq_delete(Dis_timer_q);
	Dis_timer_q = 0;
	dim_stop_threads();
}

/* find service by name */
SERVICE *find_service(name)
char *name;
{
	return(dis_hash_service_exists(name));
}

CLIENT *find_client(conn_id)
int conn_id;
{
	register CLIENT *clip;

	clip = (CLIENT *)
			dll_search( (DLL *) Client_head, &conn_id, sizeof(conn_id));
	return(clip);
}

void release_all_requests(conn_id, clip)
int conn_id;
CLIENT *clip;
{
	register REQUEST_PTR *reqpp, *auxp;
	register REQUEST *reqp;
    int found = 0;
	int release_request();

	DISABLE_AST;
	if(clip)
	{
		reqpp = clip->requestp_head;
		while( (reqpp = (REQUEST_PTR *) dll_get_next((DLL *)clip->requestp_head,
			(DLL *) reqpp)) )
		{
			auxp = reqpp->prev;
			reqp = (REQUEST *) reqpp->reqp;
			release_request(reqp, reqpp, 0);
			found = 1;
			reqpp = auxp;
		}
		dll_remove(clip);
		free(clip->requestp_head);
		free(clip);
	}
	if(found)
	{
		Last_client = -conn_id;
		if(Dis_client_id)
		  dis_update_service(Dis_client_id);
	}
	dna_close(conn_id);
	ENABLE_AST;
}

CLIENT *check_delay_delete(conn_id)
int conn_id;
{
	register REQUEST_PTR *reqpp;
	register CLIENT *clip;
	register REQUEST *reqp;

	DISABLE_AST;
	clip = find_client(conn_id);
	if(clip)
	{
		reqpp = clip->requestp_head;
		while( (reqpp = (REQUEST_PTR *) dll_get_next((DLL *)clip->requestp_head,
			(DLL *) reqpp)) )
		{
			reqp = (REQUEST *) reqpp->reqp;
			if(reqp->delay_delete)
			{
				reqp->to_delete = 1;
				ENABLE_AST;
				return((CLIENT *)-1);
			}
		}
	}
	ENABLE_AST;
	return(clip);
}

char *dis_get_error_services()
{
	return(dis_get_client_services(Error_conn_id));
}

char *dis_get_client_services(conn_id)
int conn_id;
{
	register REQUEST_PTR *reqpp;
	register CLIENT *clip;
	register REQUEST *reqp;
	register SERVICE *servp;

	int n_services = 0;
	int max_size;
	static int curr_allocated_size = 0;
	static char *service_info_buffer;
	char *buff_ptr;


	if(!conn_id)
		return((char *)0);
	{
	DISABLE_AST;
	clip = find_client(conn_id);
	if(clip)
	{
		reqpp = clip->requestp_head;
		while( (reqpp = (REQUEST_PTR *) dll_get_next((DLL *)clip->requestp_head,
			(DLL *) reqpp)))
		{
			n_services++;
		}
		if(!n_services)
		{
			ENABLE_AST
			return((char *)0);
		}
		max_size = n_services * MAX_NAME;
		if(!curr_allocated_size)
		{
			service_info_buffer = (char *)malloc(max_size);
			curr_allocated_size = max_size;
		}
		else if (max_size > curr_allocated_size)
		{
			free(service_info_buffer);
			service_info_buffer = (char *)malloc(max_size);
			curr_allocated_size = max_size;
		}
		service_info_buffer[0] = '\0';
		buff_ptr = service_info_buffer;
		reqpp = clip->requestp_head;
		while( (reqpp = (REQUEST_PTR *) dll_get_next((DLL *)clip->requestp_head,
			(DLL *) reqpp)) )
		{
			reqp = (REQUEST *) reqpp->reqp;
			servp = reqp->service_ptr;
			strcat(buff_ptr, servp->name);
			strcat(buff_ptr, "\n");
			buff_ptr += strlen(buff_ptr);
		}
	}
	else
	{
		ENABLE_AST
		return((char *)0);
	}
	ENABLE_AST;
	}
/*
	dim_print_date_time();
	dna_get_node_task(conn_id, node, task);
	printf("Client %s@%s uses services: \n", task, node);
	printf("%s\n",service_info_buffer);
*/
	return(service_info_buffer);
}

int find_release_request(conn_id, service_id)
int conn_id;
int service_id;
{
	register REQUEST_PTR *reqpp, *auxp;
	register CLIENT *clip;
	register REQUEST *reqp;
	int release_request();

	DISABLE_AST
	clip = find_client(conn_id);
	if(clip)
	{
		reqpp = clip->requestp_head;
		while( (reqpp = (REQUEST_PTR *) dll_get_next((DLL *)clip->requestp_head,
			(DLL *) reqpp)) )
		{
			reqp = (REQUEST *) reqpp->reqp;
			if(reqp->service_id == service_id)
			{
				auxp = reqpp->prev;
				release_request(reqp, reqpp, 0);
				reqpp = auxp;
			}
		}
		if( dll_empty((DLL *)clip->requestp_head) ) 
		{
			release_conn( conn_id, 0 );
		}
	}
	ENABLE_AST
	return(1);
}

int release_request(reqp, reqpp, remove)
REQUEST *reqp;
REQUEST_PTR *reqpp;
int remove;
{
	int conn_id;
	CLIENT *clip;

	DISABLE_AST
	conn_id = reqp->conn_id;
	if(reqpp)
		dll_remove((DLL *)reqpp);
	dll_remove((DLL *)reqp);
	if(reqp->timr_ent)
		dtq_rem_entry(Dis_timer_q, reqp->timr_ent);
	id_free(reqp->req_id, SRC_DIS);
	free(reqp);
	free(reqpp);
/* Would do it too early, the client will disconnect anyway
*/
	if((remove) && (!Serving))
	{
		clip = find_client(conn_id);
		if(clip)
		{
			if( dll_empty((DLL *)clip->requestp_head) ) 
			{
				release_conn( conn_id, 0 );
			}
		}
	}

	ENABLE_AST
	return(1);
}

static int release_conn(conn_id, print_flg)
int conn_id, print_flg;
{
	static int releasing = 0;
	CLIENT *clip;
	int do_exit_handler();

	DISABLE_AST
	if(conn_id == Dns_dis_conn_id)
	{
		recv_dns_dis_rout( conn_id, 0, 0, STA_DISC );
		ENABLE_AST
		return(0);
	}
#ifdef VMS
	if(print_flg)
	{
		dim_print_date_time();
		dna_get_node_task(conn_id, node, task);
		printf(" Couldn't write to client %s@%s, releasing connection %d\n",
			task, node, conn_id);
		fflush(stdout);
	}
#endif
	clip = check_delay_delete(conn_id);
	if(clip != (CLIENT *)-1)
	{
	if( Client_exit_user_routine != 0 ) 
	{
		releasing++;
		Curr_conn_id = conn_id;
		do_exit_handler(conn_id);
		releasing--;
	}
	if(!releasing)
	{
		release_all_requests(conn_id, clip);
	}
	}
	ENABLE_AST
	return(1);
}

typedef struct cmnds{
	struct cmnds *next;
	long tag;
	int size;
	int buffer[1];
} DIS_CMND;

static DIS_CMND *Cmnds_head = (DIS_CMND *)0;

void std_cmnd_handler(tag, cmnd_buff, size)
long *tag;
int *cmnd_buff, *size;
{
	register DIS_CMND *new_cmnd;
/* queue the command */

	if(!Cmnds_head)
	{
		Cmnds_head = (DIS_CMND *)malloc(sizeof(DIS_CMND));
		sll_init((SLL *) Cmnds_head);
	}
	new_cmnd = (DIS_CMND *)malloc((*size)+12);
	new_cmnd->next = 0;
	new_cmnd->tag = *tag;
	new_cmnd->size = *size;
	memcpy(new_cmnd->buffer, cmnd_buff, *size);
	sll_insert_queue((SLL *) Cmnds_head, (SLL *) new_cmnd);
}

int dis_get_next_cmnd(tag, buffer, size)
long *tag;
int *buffer, *size;
{
	register DIS_CMND *cmndp;
	register int ret_val = -1;

	DISABLE_AST
	if(!Cmnds_head)
	{
		Cmnds_head = (DIS_CMND *)malloc(sizeof(DIS_CMND));
		sll_init((SLL *) Cmnds_head);
	}
	if(*size == 0)
	{
		if( (cmndp = (DIS_CMND *) sll_get_head((SLL *) Cmnds_head)))
		{
			if(cmndp->size > 0)
			{
				*size = cmndp->size;
				*tag = cmndp->tag;
				ENABLE_AST
				return(-1);
			}
		}
	}
	if( (cmndp = (DIS_CMND *) sll_remove_head((SLL *) Cmnds_head)) )
	{
		if (*size >= cmndp->size)
		{
			*size = cmndp->size;
			ret_val = 1;
		}
		memcpy(buffer, cmndp->buffer, *size);
		*tag = cmndp->tag;
		free(cmndp);
		ENABLE_AST
		return(ret_val);
	}
	ENABLE_AST
	return(0);
}

int dis_get_conn_id()
{
	return(Curr_conn_id);
}

int dis_get_client(name)
char *name;
{
	int ret = 0;
	char node[MAX_NODE_NAME], task[MAX_TASK_NAME];

	DISABLE_AST

	if(Curr_conn_id)
	{
		dna_get_node_task(Curr_conn_id, node, task);
		strcpy(name,task);
		strcat(name,"@");
		strcat(name,node);
		ret = Curr_conn_id;
	}
	ENABLE_AST
	return(ret);
}

#ifdef VMS
dis_convert_str(c_str, for_str)
char *c_str;
struct dsc$descriptor_s *for_str;
{
	int i;

	strcpy(for_str->dsc$a_pointer, c_str);
	for(i = strlen(c_str); i< for_str->dsc$w_length; i++)
		for_str->dsc$a_pointer[i] = ' ';
}
#endif

void client_info(tag, bufp, size, first_time)
long *tag;
int **bufp;
int *size;
int *first_time;
{
	register CLIENT *clip;
	int curr_conns[MAX_CONNS];
	int i, index, max_size;
	static int curr_allocated_size = 0;
	static char *dns_info_buffer;
	register char *dns_client_info;
	char node[MAX_NODE_NAME], task[MAX_TASK_NAME];

	max_size = sizeof(DNS_CLIENT_INFO);
	if(!curr_allocated_size)
	{
		dns_info_buffer = malloc(max_size);
		curr_allocated_size = max_size;
	}
	dns_client_info = dns_info_buffer;
	dns_client_info[0] = '\0';
	index = 0;
	if(*first_time)
	{
		clip = Client_head;
		while( (clip = (CLIENT *)dll_get_next( (DLL *) Client_head, 
			(DLL*) clip)) )
		{
			curr_conns[index++] = clip->conn_id;
		}
		max_size = (index+1)*sizeof(DNS_CLIENT_INFO);
		if (max_size > curr_allocated_size)
		{
			free(dns_info_buffer);
			dns_info_buffer = malloc(max_size);
			curr_allocated_size = max_size;
		}
		dns_client_info = dns_info_buffer;
		dns_client_info[0] = '\0';
	}
	else
	{
		if(Last_client > 0)
		{
			strcat(dns_client_info,"+");
			curr_conns[index++] = Last_client;
		}
		else
		{
			strcat(dns_client_info,"-");
			curr_conns[index++] = -Last_client;
		}
	}
	
	for(i=0; i<index;i++)
	{
		dna_get_node_task(curr_conns[i], node, task);
		strcat(dns_client_info,task);
		strcat(dns_client_info,"@");
		strcat(dns_client_info,node);
		strcat(dns_client_info,"|");
	}
	if(index)
		dns_client_info[strlen(dns_client_info)-1] = '\0';
	*bufp = (int *)dns_info_buffer;
	*size = strlen(dns_info_buffer)+1;
}

void append_service(service_info_buffer, servp)		
char *service_info_buffer;
SERVICE *servp;
{
	char name[MAX_NAME], *ptr;

		if(strstr(servp->name,"/RpcIn"))
		{
			strcpy(name,servp->name);
			ptr = (char *)strstr(name,"/RpcIn");
			*ptr = 0;
			strcat(service_info_buffer, name);
			strcat(service_info_buffer, "|");
			if(servp->def[0])
			{
				strcat(service_info_buffer, servp->def);
			}
			strcat(name,"/RpcOut");
			if( (servp = find_service(name)) )
			{
				strcat(service_info_buffer, ",");
				if(servp->def[0])
				{
					strcat(service_info_buffer, servp->def);
				}
			}
			strcat(service_info_buffer, "|RPC");
			strcat(service_info_buffer, "\n");
		}
		else if(strstr(servp->name,"/RpcOut"))
		{
/*
			if(servp->def[0])
			{
				strcat(service_info_buffer, servp->def);
			}
			strcat(service_info_buffer, "|RPC");
			strcat(service_info_buffer, "\n");

*/
		}
		else
		{
			strcat(service_info_buffer, servp->name);
			strcat(service_info_buffer, "|");
			if(servp->def[0])
			{
				strcat(service_info_buffer, servp->def);
			}
			strcat(service_info_buffer, "|");
			if(servp->type == COMMAND)
			{
				strcat(service_info_buffer, "CMD");
			}
			strcat(service_info_buffer, "\n");
		}
}

void service_info(tag, bufp, size, first_time)
long *tag;
int **bufp;
int *size;
int *first_time;
{
	register SERVICE *servp;
	int max_size, done = 0;
	static int curr_allocated_size = 0;
	static char *service_info_buffer;
	char *buff_ptr;

	max_size = (Dis_n_services+10) * (MAX_NAME*2 + 4);
	if(!curr_allocated_size)
	{
		service_info_buffer = (char *)malloc(max_size);
		curr_allocated_size = max_size;
	}
	else if (max_size > curr_allocated_size)
	{
		free(service_info_buffer);
		service_info_buffer = (char *)malloc(max_size);
		curr_allocated_size = max_size;
	}
	service_info_buffer[0] = '\0';
	buff_ptr = service_info_buffer;
	servp = 0;
	if(*first_time)
	{
		while( (servp = dis_hash_service_get_next(servp)) )
		{
			if(servp->registered)
			{
				servp->registered = 2;
				append_service(buff_ptr, servp);
				buff_ptr += strlen(buff_ptr);
			}
		}
	}
	else
	{
		while( (servp = dis_hash_service_get_next(servp)) )
		{
			if(servp->registered == 1)
			{
				if(!done)
				{
					strcat(buff_ptr, "+");
					buff_ptr += strlen(buff_ptr);
					done = 1;
				}
				append_service(buff_ptr, servp);
				buff_ptr += strlen(buff_ptr);
				servp->registered = 2;
			}
			else if(servp->registered == 0)
			{
				strcat(buff_ptr, "-");
				buff_ptr += strlen(buff_ptr);
				append_service(buff_ptr, servp);
				buff_ptr += strlen(buff_ptr);
			}
		}
	}
	*bufp = (int *)service_info_buffer;
	*size = buff_ptr - service_info_buffer+1;
}
		
void add_exit_handler(tag, bufp, size)
int *tag;
int *bufp;
int *size;
{
	EXIT_H *newp;
	if(*bufp)
	{
		if(!Exit_h_head) 
		{
			Exit_h_head = (EXIT_H *)malloc(sizeof(EXIT_H));
			sll_init( (SLL *) Exit_h_head );
		}
		newp = (EXIT_H *)malloc(sizeof(EXIT_H));
		newp->conn_id = Curr_conn_id;
		newp->exit_id = *bufp;
		sll_insert_queue( (SLL *) Exit_h_head, (SLL *) newp );
	}
	else
	{
		if(!Exit_h_head) 
			return;
		if((newp = (EXIT_H *)sll_search((SLL *) Exit_h_head, 
			(char *)&Curr_conn_id, 4)) )
		{
			sll_remove( (SLL *) Exit_h_head, (SLL *) newp );
		}
	}
}

void dis_set_client_exit_handler(conn_id, tag)
int conn_id;
int tag;
{
	EXIT_H *newp;

	DISABLE_AST
	if(tag)
	{
		if(!Exit_h_head) 
		{
			Exit_h_head = (EXIT_H *)malloc(sizeof(EXIT_H));
			sll_init( (SLL *) Exit_h_head );
		}
		if( (newp = (EXIT_H *)sll_search((SLL *) Exit_h_head, 
			(char *)&conn_id, 4)) )
		{
			newp->conn_id = conn_id;
			newp->exit_id = tag;
		}
		else
		{
			newp = (EXIT_H *)malloc(sizeof(EXIT_H));
			newp->conn_id = conn_id;
			newp->exit_id = tag;
			sll_insert_queue( (SLL *) Exit_h_head, (SLL *) newp );
		}
	}
	else
	{
		if(!Exit_h_head) 
		{
			ENABLE_AST
			return;
		}
		if( (newp = (EXIT_H *)sll_search((SLL *) Exit_h_head, 
			(char *)&conn_id, 4)) )
		{
			sll_remove( (SLL *) Exit_h_head, (SLL *) newp );
		}
	}
	ENABLE_AST
}

int do_exit_handler(conn_id)
int conn_id;
{
	register EXIT_H *exitp;

	DISABLE_AST;
	if(!Exit_h_head)
	{
		ENABLE_AST;
		return(0);
	}
	while( (exitp = (EXIT_H *) sll_search_next_remove((SLL *) Exit_h_head,
							 0, (char *) &conn_id, 4)) )
	{
		(Client_exit_user_routine)( &exitp->exit_id );
		free(exitp);
	}
	ENABLE_AST
	return(1);
}

static void exit_handler(tag, bufp, size)
int *tag;
int *bufp;
int *size;
{

	if(Exit_user_routine)
		(Exit_user_routine)( bufp );
	else
	{
/*
		printf("%s PID %d Exiting!\n", Task_name, getpid());
*/
		exit(*bufp);
	}
}

static void error_handler(conn_id, severity, errcode, reason)
int conn_id, severity, errcode;
char *reason;
{
	int exit_tag, exit_code, exit_size;
	int last_conn_id;

	if(Error_user_routine)
	{
			Error_conn_id = conn_id;
			last_conn_id = Curr_conn_id;
			Curr_conn_id = conn_id;
			(Error_user_routine)( severity, errcode, reason);
			Error_conn_id = 0;
			Curr_conn_id = last_conn_id;
	}
	else
	{
		dim_print_msg(reason, severity);
	}
	if(severity == DIM_FATAL)
	{
		exit_tag = 0;
		exit_code = errcode;
		exit_size = sizeof(int);
		exit_handler(&exit_tag, &exit_code, &exit_size);
	}
}

#define MAX_HASH_ENTRIES 2000

static SERVICE *Service_hash_table[MAX_HASH_ENTRIES];

int dis_hash_service_init()
{
  int i;
  static int done = 0;

  if(!done)
  {
	N_unregistered_services = -1;
	for( i = 0; i < MAX_REGISTRATION_UNIT; i++)
	{
		Unregistered_services[i] = 0;
	}
	for( i = 0; i < MAX_HASH_ENTRIES; i++ ) 
	{
		Service_hash_table[i] = (SERVICE *) malloc(8);
		dll_init((DLL *) Service_hash_table[i]);
	}
	done = 1;
  }
  return(1);
}


int dis_hash_service_insert(servp)
SERVICE *servp;
{
	int index, i;
/*
	for( i = 0; i < MAX_REGISTRATION_UNIT; i++)
	{
		if(Unregistered_services[i] == 0)
		{
			Unregistered_services[i] = servp;
			break;
		}
	}
	if(N_unregistered_services == -1)
		N_unregistered_services = 0;
	N_unregistered_services++;
*/

	if(N_unregistered_services == -1)
		N_unregistered_services = 0;
	if(N_unregistered_services == MAX_REGISTRATION_UNIT)
	{
		for( i = 0; i < MAX_REGISTRATION_UNIT; i++)
		{
			Unregistered_services[i] = 0;
		}
	}
	else if(N_unregistered_services < MAX_REGISTRATION_UNIT)
	{
		Unregistered_services[N_unregistered_services] = servp;	
	}
	N_unregistered_services++;
	index = HashFunction(servp->name, MAX_HASH_ENTRIES);
	dll_insert_queue((DLL *) Service_hash_table[index], 
			 (DLL *) servp);
	return(1);
}

int dis_hash_service_registered(servp)
SERVICE *servp;
{
	int i;

	servp->registered = 1;
	N_unregistered_services--;
	for( i = 0; i < MAX_REGISTRATION_UNIT; i++)
	{
		if(Unregistered_services[i] == servp)
		{
			Unregistered_services[i] = 0;
			break;
		}
	}
	return 1;
}

int dis_hash_service_remove(servp)
SERVICE *servp;
{
	dll_remove( (DLL *) servp );
	return(1);
}


SERVICE *dis_hash_service_exists(name)
char *name;
{
	int index;
	SERVICE *servp;

	index = HashFunction(name, MAX_HASH_ENTRIES);
	if( (servp = (SERVICE *) dll_search(
					(DLL *) Service_hash_table[index],
			      		name, strlen(name)+1)) )
	{
		return(servp);
	}
	return((SERVICE *)0);
}			

SERVICE *dis_hash_service_get_next(prevp)
SERVICE *prevp;
{
	static int index;
	SERVICE *servp;

	if(!prevp)
	{
		index = -1;
	}
	if(index == -1)
	{
		index++;
		prevp = Service_hash_table[index];
	}
	do
	{
		servp = (SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[index],
						(DLL *) prevp);
		if(servp)
			break;
		index++;
		if(index == MAX_HASH_ENTRIES)
		{
			return((SERVICE *) 0);
		}
		prevp = Service_hash_table[index];
	} while(!servp);
	return(servp);
}

SERVICE *dis_hash_service_get_next_register(prevp)
SERVICE *prevp;
{
	static int index;
	SERVICE *servp;
	int i;

	if(!prevp)
	{
		index = -1;
	}
	if(index == -1)
	{
		index++;
		prevp = Service_hash_table[index];
	}
	if( (N_unregistered_services != -1) && 
		(N_unregistered_services < MAX_REGISTRATION_UNIT))
	{
		for( i = 0; i < MAX_REGISTRATION_UNIT; i++)
		{
			if(Unregistered_services[i])
			{
				return(Unregistered_services[i]);
			}
		}
		if(!N_unregistered_services)
		{
			N_unregistered_services = -1;
			return((SERVICE *) 0);
		}
	}
	do
	{
		servp = (SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[index],
						(DLL *) prevp);
		if(servp)
			break;
		index++;
		if(index == MAX_HASH_ENTRIES)
		{
			return((SERVICE *) 0);
		}
		prevp = Service_hash_table[index];
	} while(!servp);
	return(servp);
}

SERVICE *dis_hash_service_get_next_remove(prevp)
SERVICE *prevp;
{
	static int index;
	SERVICE *servp;

	if(!prevp)
	{
		index = -1;
	}
	if(index == -1)
	{
		index++;
		prevp = Service_hash_table[index];
	}
	prevp = Service_hash_table[index];
	do
	{
		servp = (SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[index],
						(DLL *) prevp);
		if(servp)
			break;
		index++;
		if(index == MAX_HASH_ENTRIES)
		{
			return((SERVICE *) 0);
		}
		prevp = Service_hash_table[index];
	} while(!servp);
	return(servp);
}


void dis_print_hash_table()
{
	SERVICE *servp;
	int i;
	int n_entries, max_entry_index;
	int max_entries = 0;

	for( i = 0; i < MAX_HASH_ENTRIES; i++ ) 
	{
		n_entries = 0;
		servp = Service_hash_table[i];
		while( (servp = (SERVICE *) dll_get_next(
						(DLL *) Service_hash_table[i],
						(DLL *) servp)) )
		{
			n_entries++;
			if(n_entries == 1)
				printf("    Name = %s\n",servp->name);
		}
		if(n_entries != 0)
			printf("HASH[%d] - %d entries\n", i, n_entries);
		if(n_entries > max_entries)
		{
			max_entries = n_entries;
			max_entry_index = i;
		}
	}
	printf("Maximum : HASH[%d] - %d entries\n", max_entry_index, max_entries);  
	fflush(stdout);
}

void dis_hash_print()
{
	SERVICE *servp;

	servp = 0;
	while( (servp = dis_hash_service_get_next(servp)) )
	{
		printf("Name = %s\n",servp->name);
	}
}

#ifdef VMS
/* CFORTRAN WRAPPERS */
FCALLSCFUN1(INT, dis_start_serving, DIS_START_SERVING, dis_start_serving,
				 STRING)
FCALLSCFUN3(INT, dis_get_next_cmnd, DIS_GET_NEXT_CMND, dis_get_next_cmnd,
				 PINT, PVOID, PINT)
FCALLSCFUN1(INT, dis_get_client, DIS_GET_CLIENT, dis_get_client,
				 PSTRING)
FCALLSCFUN6(INT, dis_add_service, DIS_ADD_SERVICE, dis_add_service,
				 STRING, PVOID, PVOID, INT, PVOID, INT)
FCALLSCSUB4(	 dis_add_cmnd, DIS_ADD_CMND, dis_add_cmnd,
				 STRING, PVOID, PVOID, INT)
FCALLSCSUB1(	 dis_add_client_exit_handler, DIS_ADD_CLIENT_EXIT_HANDLER, 
				 dis_add_client_exit_handler,
				 PVOID)
FCALLSCSUB2(	 dis_set_client_exit_handler, DIS_SET_CLIENT_EXIT_HANDLER, 
				 dis_set_client_exit_handler,
				 INT, INT)
FCALLSCSUB1(	 dis_add_exit_handler, DIS_ADD_EXIT_HANDLER, 
				 dis_add_exit_handler,
				 PVOID)
FCALLSCSUB1(	 dis_report_service, DIS_REPORT_SERVICE, dis_report_service,
				 STRING)
FCALLSCSUB2(	 dis_convert_str, DIS_CONVERT_STR, dis_convert_str,
				 PVOID, PVOID)
FCALLSCFUN1(INT, dis_update_service, DIS_UPDATE_SERVICE, dis_update_service,
				 INT)
FCALLSCFUN1(INT, dis_remove_service, DIS_REMOVE_SERVICE, dis_remove_service,
				 INT)
FCALLSCSUB3(	 dis_send_service, DIS_SEND_SERVICE, dis_send_service,
				 INT, PVOID, INT)
FCALLSCSUB2(	 dis_set_quality, DIS_SET_QUALITY, dis_set_quality,
                 INT, INT)
FCALLSCSUB3(	 dis_set_timestamp, DIS_SET_TIMESTAMP, dis_set_timestamp,
                 INT, INT, INT)
FCALLSCFUN2(INT, dis_selective_update_service, DIS_SELECTIVE_UPDATE_SERVICE, 
					dis_selective_update_service,
				 INT, PINT)
#endif
