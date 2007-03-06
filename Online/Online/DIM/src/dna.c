
/*
 * DNA (Delphi Network Access) implements the network layer for the DIM
 * (Delphi Information Managment) System.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

/* include files */

#define DIMLIB
#define DNA
#include <errno.h>
#include <dim.h>

/* global definitions */

#define READ_HEADER_SIZE	12

/*
#define TO_DBG		1 
*/

/* global variables */
typedef struct {
	char task_name[MAX_TASK_NAME];
	int port;
} PENDING_OPEN;

static PENDING_OPEN Pending_conns[MAX_CONNS];

static int DNA_Initialized = FALSE;


_DIM_PROTO( static void ast_read_h,     (int conn_id, int status, int size) );
_DIM_PROTO( static void ast_conn_h,     (int handle, int svr_conn_id,
                                     int protocol) );
_DIM_PROTO( static int dna_write_bytes, (int conn_id, void *buffer, int size,
									 int nowait) );
_DIM_PROTO( static void release_conn,   (int conn_id) );
_DIM_PROTO( static void save_node_task, (int conn_id, DNA_NET *buffer) );

/*
 * Routines common to Server and Client
 */

static int Prev_packet[3];
static int Prev_buffer[3];
static int Prev_conn_id = 0;

static int is_header( conn_id )
int conn_id;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id];
	register int ret;

	ret = 0;
	if( (vtohl(dna_connp->buffer[2]) == TRP_MAGIC) &&
	    (vtohl(dna_connp->buffer[1]) == 0) &&
	    (vtohl(dna_connp->buffer[0]) == READ_HEADER_SIZE) )
	{
		dna_connp->state = RD_HDR;
		ret = 1;
	} 
	else if( (vtohl(dna_connp->buffer[2]) == TST_MAGIC) &&
		   (vtohl(dna_connp->buffer[1]) == 0) &&
		   (vtohl(dna_connp->buffer[0]) == READ_HEADER_SIZE) )
	{
		dna_connp->state = RD_HDR;
		ret = 1;
	} 
	else if( (vtohl(dna_connp->buffer[2]) == HDR_MAGIC ) &&
		   (vtohl(dna_connp->buffer[0]) == READ_HEADER_SIZE ) )
	{
		dna_connp->state = RD_DATA;
		ret = 1;
	} 
	else 
	{
/*
		dim_print_date_time();
		printf( " conn: %d to %s@%s, expecting header\n", conn_id,
			Net_conns[conn_id].task, Net_conns[conn_id].node );
		printf( "buffer[0]=%d\n", vtohl(dna_connp->buffer[0]));
		printf( "buffer[1]=%d\n", vtohl(dna_connp->buffer[1]));
		printf( "buffer[2]=%x\n", vtohl(dna_connp->buffer[2]));
		printf( "closing the connection.\n" );
		printf( " Previous conn: %d, Previous Packet\n", Prev_conn_id);
		printf( "buffer[0]=%d\n", vtohl(Prev_packet[0]));
		printf( "buffer[1]=%d\n", vtohl(Prev_packet[1]));
		printf( "buffer[2]=%x\n", vtohl(Prev_packet[2]));
		printf( " Previous Buffer\n");
		printf( "buffer[0]=%d\n", vtohl(Prev_buffer[0]));
		printf( "buffer[1]=%d\n", vtohl(Prev_buffer[1]));
		printf( "buffer[2]=%x\n", vtohl(Prev_buffer[2]));
		fflush(stdout);
*/
		dna_connp->read_ast(conn_id, NULL, 0, STA_DISC);
		ret = 0;
	}			
	return(ret);
}

static void read_data( conn_id)
register int conn_id;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id];

	if( !dna_connp->saw_init &&
	    vtohl(dna_connp->buffer[0]) == OPN_MAGIC)
	{
		save_node_task(conn_id, (DNA_NET *) dna_connp->buffer);
		dna_connp->saw_init = TRUE;
	} 
	else
	{
		dna_connp->read_ast(conn_id, dna_connp->buffer,
			dna_connp->full_size, STA_DATA);
	}
}

static void ast_read_h( conn_id, status, size )
register int conn_id;
int status;
register int size;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id];
	int tcpip_code;
	register int read_size, next_size;
	register char *buff;

	if(!dna_connp->buffer) /* The connection has already been closed */
	{
		return;
	}
	if(status == 1)
	{
		next_size = dna_connp->curr_size;
		buff = (char *) dna_connp->curr_buffer;
  		if(size < next_size) 
		{
			Prev_conn_id = conn_id;
	  		Prev_packet[0] = ((int *)dna_connp->curr_buffer)[0];
			Prev_packet[1] = ((int *)dna_connp->curr_buffer)[1];
			Prev_packet[2] = ((int *)dna_connp->curr_buffer)[2];
			Prev_buffer[0] = dna_connp->buffer[0];
			Prev_buffer[1] = dna_connp->buffer[1];
			Prev_buffer[2] = dna_connp->buffer[2];
			read_size = ((next_size - size) > MAX_IO_DATA) ?
				MAX_IO_DATA : next_size - size;
			dna_connp->curr_size -= size;
			dna_connp->curr_buffer += size;
			tcpip_code = tcpip_start_read(conn_id, buff + size, 
				read_size, ast_read_h);
			if(tcpip_failure(tcpip_code)) 
			{
#ifndef WIN32
			  if(errno == ENOTSOCK)
			  {
				  if(dna_connp->read_ast)
					dna_connp->read_ast(conn_id, NULL, 0, STA_DISC);
			  }
			  else
#endif
			  {
				dna_report_error(conn_id, tcpip_code,
					"Reading from", DIM_ERROR, DIMTCPRDERR);
			  }
			}
			return;
		}
		switch(dna_connp->state)
		{
			case RD_HDR :
				if(is_header(conn_id))
				{
					if( dna_connp->state == RD_DATA )
					{
						next_size = vtohl(dna_connp->buffer[1]);
						dna_start_read(conn_id, next_size);
					}
					else
					{
						dna_connp->state = RD_HDR;
						dna_start_read(conn_id, READ_HEADER_SIZE);
					}
				}
				break;
			case RD_DATA :
				read_data(conn_id);
				dna_connp->state = RD_HDR;
				dna_start_read(conn_id, READ_HEADER_SIZE);
				break;
			default:
				break;
		}
		if(dna_connp->buffer)
		{
			Prev_conn_id = conn_id;
			Prev_packet[0] = ((int *)dna_connp->curr_buffer)[0];
			Prev_packet[1] = ((int *)dna_connp->curr_buffer)[1];
			Prev_packet[2] = ((int *)dna_connp->curr_buffer)[2];
			Prev_buffer[0] = dna_connp->buffer[0];
			Prev_buffer[1] = dna_connp->buffer[1];
			Prev_buffer[2] = dna_connp->buffer[2];
		}
	} 
	else 
	{
	  /*
	  printf("Connection lost. Signal upper layer\n");
	  */
		if(dna_connp->read_ast)
			dna_connp->read_ast(conn_id, NULL, 0, STA_DISC);
	}
}


int dna_start_read(conn_id, size)
register int conn_id, size;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id];
	register int tcpip_code, read_size;
	
	if(!dna_connp->busy)
	{
		return(0);
	}

	dna_connp->curr_size = size;
	dna_connp->full_size = size;
	if(size > dna_connp->buffer_size) 
	{
		dna_connp->buffer =
				(int *) realloc(dna_connp->buffer, size);
		dna_connp->buffer_size = size;
	}
	dna_connp->curr_buffer = (char *) dna_connp->buffer;
	read_size = (size > MAX_IO_DATA) ? MAX_IO_DATA : size ;

	tcpip_code = tcpip_start_read(conn_id, dna_connp->curr_buffer,
				  read_size, ast_read_h);
	if(tcpip_failure(tcpip_code)) {
		dna_report_error(conn_id, tcpip_code,
			"Reading from", DIM_ERROR, DIMTCPRDERR);

		return(0);
	}

	return(1);
}								


static int dna_write_bytes( conn_id, buffer, size, nowait )
register int conn_id, size, nowait;
void *buffer;
{
	register int size_left, wrote;
	register char *p;
#ifdef VMS
	int retries = WRITE_RETRIES, retrying = 0;
	float wait_time = 0.01;
#endif
	extern int tcpip_write_nowait(int, char *, int);

	p = (char *) buffer;
	size_left = size;
	do {
		size = (size_left > MAX_IO_DATA) ? MAX_IO_DATA : size_left ;
#ifdef VMS
		if(nowait)
		{
			while(--retries)
			{
				if((wrote = tcpip_write_nowait(conn_id, p, size)) > 0)
					break;
				if(!tcpip_would_block(wrote))
					return(0);
				if(retries == WRITE_RETRIES_WNG)
				{
					dna_report_error(conn_id, tcpip_code,
						"Writing to (retrying)", DIM_WARNING, DIMTCPWRRTY);
					retrying = 1;
				}
				lib$wait(&wait_time);
			}
			if(!retries)
			{
				return(0);
			}
		}
		else
			wrote = tcpip_write(conn_id, p, size);
#else
		if(nowait)
		{
		  wrote = tcpip_write_nowait(conn_id, p, size);
		  if(wrote == -1)
		  {
		    dna_report_error(conn_id, -1,
				     "Write timeout, disconnecting from", DIM_ERROR, DIMTCPWRTMO);
		    wrote = 0;
		  }
		}
		else
		{
			wrote = tcpip_write(conn_id, p, size);
		}
#endif
		
		if( tcpip_failure(wrote) )
			return(0);
		p += wrote;
		size_left -= wrote;
	} while(size_left > 0);
	return(1);
}

void dna_test_write(conn_id)
register int conn_id;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id];
	register int tcpip_code;
	DNA_HEADER test_pkt;
	register DNA_HEADER *test_p = &test_pkt;

	if(!dna_connp->busy)
	{
		return;
    }
	if(dna_connp->writing)
	{
		return;
    }
	test_p->header_size = htovl(READ_HEADER_SIZE);
	test_p->data_size = 0;
	test_p->header_magic = htovl(TST_MAGIC);
	tcpip_code = dna_write_bytes(conn_id, &test_pkt, READ_HEADER_SIZE,0);
	if(tcpip_failure(tcpip_code)) {
		/* Connection lost. Signal upper layer ? */
		if(dna_connp->read_ast)
			dna_connp->read_ast(conn_id, NULL, 0, STA_DISC);
		return;
	}
}

typedef struct
{
	int conn_id;
	void *buffer;
	int size;
	char dummy[MAX_NAME];
} WRITE_ITEM;

static int do_dna_write(id)
int id;
{
	register DNA_CONNECTION *dna_connp;
	int tcpip_code;
	WRITE_ITEM *ptr;
	int conn_id, size;
	void *buffer;

	ptr = (WRITE_ITEM *)id_get_ptr(id, SRC_DNA);
	conn_id = ptr->conn_id;
	buffer = ptr->buffer;
	size = ptr->size;

	dna_connp = &Dna_conns[conn_id];
	if(!dna_connp->busy)
	{
		id_free(id, SRC_DNA);
		free(buffer);
		free(ptr);
		return(2);
    }
	dna_connp->writing = TRUE;
	tcpip_code = dna_write_bytes(conn_id, buffer, size,0);
	if(tcpip_failure(tcpip_code)) 
	{
		dna_connp->writing = FALSE;
		id_free(id, SRC_DNA);
		free(buffer);
		free(ptr);
		return(0);
	}

	id_free(id, SRC_DNA);
	free(buffer);
	free(ptr);

	dna_connp->writing = FALSE;
	return(1);
}	

int dna_write_nowait(conn_id, buffer, size)
register int conn_id, size;
void *buffer;
{
	register DNA_CONNECTION *dna_connp;
	DNA_HEADER header_pkt;
	register DNA_HEADER *header_p = &header_pkt;
	int tcpip_code, ret = 1;

	DISABLE_AST
	dna_connp = &Dna_conns[conn_id];
	if(!dna_connp->busy)
	{
		ENABLE_AST
		return(2);
    }
	dna_connp->writing = TRUE;

	header_p->header_size = htovl(READ_HEADER_SIZE);
	header_p->data_size = htovl(size);
	header_p->header_magic = htovl(HDR_MAGIC);
	tcpip_code = dna_write_bytes(conn_id, &header_pkt, READ_HEADER_SIZE, 1);
	if(tcpip_failure(tcpip_code)) 
	{
		dna_connp->writing = FALSE;
		ENABLE_AST
		return(0);
	}
	tcpip_code = dna_write_bytes(conn_id, buffer, size, 1);
	if(tcpip_failure(tcpip_code)) 
	{
		ret = 0;
	}
	dna_connp->writing = FALSE;
	ENABLE_AST
	return(ret);
}	

typedef struct
{
	DNA_HEADER header;
	char data[1];

}WRITE_DATA;

int dna_write(conn_id, buffer, size)
int conn_id;
void *buffer;
int size;
{
	WRITE_ITEM *newp;
	int id;
	WRITE_DATA *pktp;
	DNA_HEADER *headerp;

	DISABLE_AST

	pktp = malloc(READ_HEADER_SIZE+size);
	headerp = &(pktp->header);
	headerp->header_size = htovl(READ_HEADER_SIZE);
	headerp->data_size = htovl(size);
	headerp->header_magic = htovl(HDR_MAGIC);

	memcpy(pktp->data, (char *)buffer, size);

	newp = malloc(sizeof(WRITE_ITEM));
	newp->conn_id = conn_id;
	newp->buffer = pktp;
	newp->size = size+READ_HEADER_SIZE;
	id = id_get((void *)newp, SRC_DNA);
	dtq_start_timer(0, do_dna_write, id);
	ENABLE_AST
	return(1);
}

/* Server Routines */

static void ast_conn_h(handle, svr_conn_id, protocol)
int handle;
register int svr_conn_id;
int protocol;
{
	register DNA_CONNECTION *dna_connp;
	register int tcpip_code;
	register int conn_id;

	conn_id = conn_get();
/*
	if(!conn_id)
		dim_panic("In ast_conn_h: No more connections\n");
*/
	dna_connp = &Dna_conns[conn_id] ;
	dna_connp->error_ast = Dna_conns[svr_conn_id].error_ast;
	tcpip_code = tcpip_open_connection( conn_id, handle );

	if(tcpip_failure(tcpip_code))
	{
		dna_report_error(conn_id, tcpip_code,
			"Connecting to", DIM_ERROR, DIMTCPCNERR);
		conn_free(conn_id);
	} else {
		dna_connp->state = RD_HDR;
		dna_connp->buffer = (int *)malloc(TCP_RCV_BUF_SIZE);
/*
		if(!dna_connp->buffer)
		{
			printf("Error in DNA - handle_connection malloc returned 0\n");
			fflush(stdout);
		}
*/
		dna_connp->buffer_size = TCP_RCV_BUF_SIZE;
		dna_connp->read_ast = Dna_conns[svr_conn_id].read_ast;
		dna_connp->saw_init = FALSE;
		dna_start_read(conn_id, READ_HEADER_SIZE); /* sizeof(DNA_NET) */
		/* Connection arrived. Signal upper layer ? */
		dna_connp->read_ast(conn_id, NULL, 0, STA_CONN);
	}
	tcpip_code = tcpip_start_listen(svr_conn_id, ast_conn_h);
	if(tcpip_failure(tcpip_code))
	{
		dna_report_error(svr_conn_id, tcpip_code,
			"Listening at", DIM_ERROR, DIMTCPLNERR);
	}
}

int dna_init()
{
	if(!DNA_Initialized)
	{
		conn_arr_create(SRC_DNA);
		DNA_Initialized = TRUE;
	}
	return(1);
}

int dna_open_server(task, read_ast, protocol, port, error_ast)
char *task;
void (*read_ast)();
void (*error_ast)();
int *protocol, *port;
{
	register DNA_CONNECTION *dna_connp;
	register int tcpip_code;
	register int conn_id;

	if(!DNA_Initialized)
	{
		conn_arr_create(SRC_DNA);
		DNA_Initialized = TRUE;
	}
	*protocol = PROTOCOL;
	conn_id = conn_get();
	dna_connp = &Dna_conns[conn_id];
/*
	if(!conn_id)
		dim_panic("In dna_open_server: No more connections\n");
*/
	dna_connp->protocol = TCPIP;
	dna_connp->error_ast = error_ast;
	tcpip_code = tcpip_open_server(conn_id, task, port);
	if(tcpip_failure(tcpip_code))
	{
		dna_report_error(conn_id, tcpip_code,
			"Opening server port", DIM_ERROR, DIMTCPOPERR);
		conn_free(conn_id);
		return(0);
	}
	dna_connp->writing = FALSE;
	dna_connp->read_ast = read_ast;
	tcpip_code = tcpip_start_listen(conn_id, ast_conn_h);
	if(tcpip_failure(tcpip_code))
	{
		dna_report_error(conn_id, tcpip_code, "Listening at", DIM_ERROR, DIMTCPLNERR);
		return(0);
	}
	return(conn_id);
}


int dna_get_node_task(conn_id, node, task)
int conn_id;
char *node;
char *task;
{
	if(Dna_conns[conn_id].busy)
		tcpip_get_node_task(conn_id, node, task);
	else
		node[0] = '\0';
	return(1);
}


/* Client Routines */

void dna_set_test_write(conn_id, time)
int conn_id, time;
{
	extern void tcpip_set_test_write(int, int);

	tcpip_set_test_write(conn_id, time);
}

void dna_rem_test_write(conn_id)
int conn_id;
{
	extern void tcpip_rem_test_write(int);

	tcpip_rem_test_write(conn_id);
}

static int ins_pend_conn( task, port )
char *task;
int port;                                      
{
	register PENDING_OPEN *pending_connp;
	register int i;

	for( i = 1, pending_connp = &Pending_conns[1]; i < MAX_CONNS; 
		i++, pending_connp++ )
	{
		if( pending_connp->task_name[0] == '\0' )
		{
			strcpy(pending_connp->task_name, task);
			pending_connp->port = port;
			return(i);
		}
	}
	return(0);
}

static int find_pend_conn( task, port )
register char *task;
register int port;
{
	register PENDING_OPEN *pending_connp;
	register int i;

	for( i = 1, pending_connp = &Pending_conns[1]; i < MAX_CONNS; 
		i++, pending_connp++ )
	{
		if( (!strcmp(pending_connp->task_name, task)) &&
			(pending_connp->port == port) )
		{
			return(i);
		}
	}
	return(0);
}


static void rel_pend_conn( conn_id )
int conn_id;
{
	Pending_conns[conn_id].task_name[0] = '\0';
}	


int dna_open_client(server_node, server_task, port, server_protocol, 
					read_ast, error_ast)
char *server_node, *server_task;
int port;
int server_protocol;
void (*read_ast)();
void (*error_ast)();
{
	register DNA_CONNECTION *dna_connp;
	char str[256];
	register int tcpip_code, conn_id, id;
	DNA_NET local_buffer;
	extern int get_proc_name(char *);

	if(!DNA_Initialized) {
		conn_arr_create(SRC_DNA);
		DNA_Initialized = TRUE;
	}
	conn_id = conn_get();
	dna_connp = &Dna_conns[conn_id] ;
/*
	if( !(conn_id = conn_get()) )
		dim_panic("In dna_open_client: No more connections\n");
*/
	dna_connp->protocol = TCPIP;
	dna_connp->error_ast = error_ast;
	tcpip_code = tcpip_open_client(conn_id, server_node, server_task, port);
	if( tcpip_failure(tcpip_code) )
	{
#ifdef VMS
		if(!strstr(server_node,"fidel"))
		{
#endif
		if(!find_pend_conn(server_task, port))
		{
			sprintf( str,"Connecting to %s on %s", 
				server_task, server_node );
			if(!strcmp(server_task,"DIM_DNS"))
				dna_report_error( conn_id, tcpip_code, str, DIM_ERROR, DIMDNSCNERR );
			else
				dna_report_error( conn_id, tcpip_code, str, DIM_ERROR, DIMTCPCNERR );
			ins_pend_conn(server_task, port);
		}
#ifdef VMS
		}
#endif
		tcpip_close(conn_id);
		conn_free( conn_id );
		return(0);
	}
	if( (id = find_pend_conn(server_task, port)) )
	{
		sprintf( str,"Connection established to");
		if(!strcmp(server_task,"DIM_DNS"))
			dna_report_error( conn_id, -1, str, DIM_INFO, DIMDNSCNEST );
		else
			dna_report_error( conn_id, -1, str, DIM_INFO, DIMTCPCNEST );
		rel_pend_conn(id);
	}
	dna_connp->state = RD_HDR;
	dna_connp->writing = FALSE;
	dna_connp->buffer = (int *)malloc(TCP_RCV_BUF_SIZE);
/*
	if(!dna_connp->buffer)
	{
		printf("Error in DNA - open_client malloc returned 0\n");
		fflush(stdout);
	}
*/
	dna_connp->buffer_size = TCP_RCV_BUF_SIZE;
	dna_connp->read_ast = read_ast;
	dna_connp->saw_init = TRUE;	/* we send it! */
	dna_start_read(conn_id, READ_HEADER_SIZE);
	local_buffer.code = htovl(OPN_MAGIC);
	get_node_name(local_buffer.node);
	get_proc_name(local_buffer.task);
	tcpip_code = dna_write_nowait(conn_id, &local_buffer, sizeof(local_buffer));
	if (tcpip_failure(tcpip_code))
	{
		return(0);
	}
	read_ast(conn_id, NULL, 0, STA_CONN);
	return(conn_id);
}
	
int dna_close(conn_id)
int conn_id;
{
	if(conn_id > 0)
		release_conn(conn_id);
	return(1);
}

/* connection managment routines */

static void release_conn(conn_id)
register int conn_id;
{
	register DNA_CONNECTION *dna_connp = &Dna_conns[conn_id] ;

	DISABLE_AST
	if(dna_connp->busy)
	{ 
		tcpip_close(conn_id);
		if(dna_connp->buffer)
		{
			free(dna_connp->buffer);
			dna_connp->buffer = 0;
			dna_connp->buffer_size = 0;
		}
		dna_connp->read_ast = NULL;
		dna_connp->error_ast = NULL;
		conn_free(conn_id);
	}
	ENABLE_AST
}


void dna_report_error_old(conn_id, code, routine_name)
int conn_id, code;
char *routine_name;
{
	char str[128];
	extern void tcpip_get_error(char *, int);
	dim_print_date_time();
	printf("%s", routine_name);
	if(conn_id)
	{
		if(Net_conns[conn_id].node[0])
			printf(" %s on node %s",
		       Net_conns[conn_id].task, Net_conns[conn_id].node);
/*
		else
			printf("\tConn %d :\n", conn_id);
*/
	}
	if(code != -1)
	{
/*
		printf("\t");
		tcpip_report_error(code);
*/
		tcpip_get_error(str, code);
		printf(": %s\n",str);
	}
	fflush(stdout);
}

void dna_report_error(conn_id, code, routine_name, severity, errcode)
int conn_id, code;
char *routine_name;
int severity, errcode;
{
	char str[128], msg[1024];
	extern void tcpip_get_error();

	sprintf(msg, "%s", routine_name);
	if(conn_id)
	{
		if(Net_conns[conn_id].node[0])
		{
			sprintf(str," %s on node %s",
		       Net_conns[conn_id].task, Net_conns[conn_id].node);
			strcat(msg, str);
		}
	}
	if(code != -1)
	{
		tcpip_get_error(str, code);
		strcat(msg,": ");
		strcat(msg, str);
	}
	if(Dna_conns[conn_id].error_ast)
	{
		Dna_conns[conn_id].error_ast(conn_id, severity, errcode, msg);
	}
}

static void save_node_task(conn_id, buffer)
int conn_id;
DNA_NET *buffer;
{
	strcpy(Net_conns[conn_id].node, buffer->node);
	strcpy(Net_conns[conn_id].task, buffer->task);
}

