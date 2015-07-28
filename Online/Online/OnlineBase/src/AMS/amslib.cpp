#include <cstring>
#include <cstdio>
#include <cerrno>
#include <memory>

#include "RTL/que.h"
#include "RTL/Lock.h"
#include "RTL/strdef.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"
#include "TAN/TanInterface.h"
#include "NET/IOPortManager.h"
// #define _USE_FULL_WT

enum  {
  SAFE_NAME_LENGTH      =  AMS_NAME_LENGTH,
  HOST_NAME_LENGTH      =  32,
  PROC_NAME_LENGTH      =  64,
  TRAILER_PATTERN       =  0xFEADBABE,
  AMS_MSG_DATA          =  2,
  AMS_K_COPY_LIMIT      = (8192*2),
  LINGER_VALUE          =  0,
  NAME_LENGTH           =  AMS_NAME_LENGTH-1,
  CHOP_SIZE             = (8192*4),
  LOWER_CHOP            = 4096,
  MAX_TCP_ERRORS        = 20
};

enum  CONNECTION_FIND_MODES  {
  CREATE_IF,
  FIND_ONLY
};

static const int DEFAULT_STYLE= DECNET_STYLE;
static const int SNDBUF_VALUE = 8192;
static const int RCVBUF_VALUE = CHOP_SIZE;

#define DB_SIZE    256
#define WITHOUT_INTERCEPT(x) lib_rtl_disable_intercept(); \
  x ;                                                     \
  lib_rtl_enable_intercept(); 


#define AMS_MAGIC 0xDEADCAFE
//#define _amsc_printf printf
static inline void _amsc_printf(const char*, ...) {}

namespace {

  struct amsheader_t {
    unsigned int    size;
    char            dest[NAME_LENGTH];
    char            source[NAME_LENGTH];
    unsigned int    reserved;
    unsigned int    msg_type;
    unsigned int    facility;
    unsigned int    magic;
    amsheader_t() : size(0), msg_type(AMS_MSG_DATA), facility(0), magic(AMS_MAGIC) {}
    void reset() {
      memset(this,0,sizeof(amsheader_t));
      magic = AMS_MAGIC;
    }
  };

  struct amsqueue_t : public qentry_t {
    unsigned int   size;
    amsheader_t	  *message;
    amsqueue_t() : qentry_t(), size(0), message(0) {}
    explicit amsqueue_t(size_t len) : qentry_t(), size(len)  {
      message  = new (::operator new(len)) amsheader_t();
      size     = len;
    }
    amsqueue_t* release()  {
      if (message) delete message;
      message = 0;
      delete this;
      return 0;
    }
  };

  struct amsentry_t : public qentry_t {
    int           chan;
    int           refCount;
    int           del_pending;
    amsqueue_t   *pending;
    amsqueue_t   *msg_ptr;
    int           sndBuffSize;
    unsigned int  received;
    unsigned int  current_size;
    char          name [NAME_LENGTH];
    char          myName [NAME_LENGTH];
    struct        sockaddr_in address;
    explicit amsentry_t(const char* dest=0, const char* me=0)  
      : chan(-1), refCount(0), del_pending(0), pending(0), msg_ptr(0), sndBuffSize(0)    {
      if ( dest )  {
        ::strncpy (name,dest ? dest : "", sizeof(name));
        name[sizeof(name)-1] = 0;
      }
      if ( me   ) {
        ::strncpy (myName,me ? me : "", sizeof(myName));
        myName[sizeof(me)] = 0;
      }
    }
    int release()  {
      if (refCount == 0) delete this;
      else del_pending = 1;
      return AMS_SUCCESS;
    }
  };

  struct  AMS  {
    AMS () : message_Q(0,0), park_Q(0,0), AMS_Q(0,0) {
      wt_enable_mask=0;
      refCount = 0;
      inited = false;
      userAst = 0;
      msgWaiting = 0;
      lockid = 0;
    }
    int            wt_enable_mask;
    amsentry_t     me;
    qentry_t       message_Q;
    qentry_t       park_Q;
    qentry_t       AMS_Q;
    char           name[PROC_NAME_LENGTH];
    char           hostName[HOST_NAME_LENGTH];
    int            hostNameLen;
    int          (*userAst)(void*);
    void*          userPar;
    amsentry_t*    db[DB_SIZE];
    u_int          inited;
    u_int          refCount;
    int            msgWaiting;
    char           reqSource [SAFE_NAME_LENGTH];
    int            reqFac;
    lib_rtl_lock_t lockid;
  };
  static AMS _ams;
}
#define CheckInitialization()   if ( !_ams.inited) {return errno = AMS_NOTINITED;}
static int _amsc_receive_rearm (amsentry_t* e);
static int _amsc_peek_rearm (amsentry_t* e);

int AMS_exit_handler(void* ) {
  amsc_close();
  return 1;
}

void amsc_full_name (char *dest, const char *src, size_t length, int style) {
  char full[SAFE_NAME_LENGTH], proc[PROC_NAME_LENGTH], host[HOST_NAME_LENGTH];
  const char *q;
  if ((q = strstr_safe(src,"::"))) {      // found DECNET style source name 
    int n = q - src;
    strncpy(host, src, n);
    host[n] = 0;
    strncpy(proc, q + 2, sizeof(proc));
    proc[sizeof(proc)-1] = 0;
  }
  else if ((q = strchr_safe(src,'@')))  {  // found INTERNET style source name 
    int n = q - src;
    strncpy(proc, src, n);
    proc[n] = 0;
    strncpy(host, q + 1, sizeof(host));
    host[sizeof(host)-1] = 0;
  }
  else  {                            // Source is process name only 
    strncpy(proc, src, sizeof(proc));
    proc[sizeof(proc)-1] = 0;
    strncpy(host, _ams.hostName, sizeof(host));
    host[sizeof(host)-1] = 0;
  }
  if (style == DECNET_STYLE)         // Build full name 
    ::snprintf(full,sizeof(full),"%s::%s",host,proc);
  else
    ::snprintf(full,sizeof(full),"%s@%s",proc,host);

  // Check destination buffer size (truncate output if necessary) 
  strncpy(dest, full, length - 1);
  dest [length - 1] = 0;

  // Upper or lowercase depending on style; trim blanks
  for(char *p = dest; *p; ++p)  {
    *p = char((style == DECNET_STYLE ) ? ::toupper(*p) : ::tolower(*p));
    if (*p == ' ')  {
      *p = 0;
      return;
    }
  }
}

static int _amsc_remove_node_if_mine(char *src, int style=DECNET_STYLE)  {
  char *s_ptr = strstr(src,"::");
  int   s_len = s_ptr-src;
  int   c_len =( s_len > _ams.hostNameLen ) ? s_len : _ams.hostNameLen;
  if ( style == DECNET_STYLE )  {
    int    stat = strncmp(_ams.hostName,src,c_len);
    if ( stat == 0 )  {
      strcpy (src, s_ptr+2);
    }
  }
  else {
    char full[SAFE_NAME_LENGTH];
    strncpy(full, src, s_ptr-src);
    strcpy(&full[s_ptr-src+1], s_ptr+2);
    full[s_ptr-src] = '_';
    strcpy(src, full);
  }
  return 0;
}

static int _amsc_requirements_satisfied (char *src, int fac)   {
  if (strlen(_ams.reqSource) > 0)  {
    if (strcmp (src, _ams.reqSource) != 0)    {
      return 0;
    }
    else if (_ams.reqFac != 0)    {
      return (_ams.reqFac == fac) ? 1 : ((fac == 0) ? 1 : 0);
    }
    return 1;
  }
  if (_ams.reqFac != 0)    {
    return (_ams.reqFac == fac) ? 1 : ((fac == 0) ? 1 : 0);
  }
  return 1;
}

static int _amsc_db_insert_entry (amsentry_t **db, amsentry_t *db_entry)  {
  for (int i=0; i < DB_SIZE; i++)  {
    if ( 0 == db[i] )  {
      db[i] = db_entry;
      return AMS_SUCCESS;
    }
  }
  return AMS_DATABASEFULL;
}

static int _amsc_db_remove_entry (amsentry_t **db, amsentry_t *db_entry)    {
  for (int i = 0; i < DB_SIZE; i++)  {
    if (db [i] == db_entry)    {
      db[i] = 0;
      return  AMS_SUCCESS;
    }
  }
  return AMS_TASKNOTFOUND;
}

static amsentry_t *_amsc_db_find_entry (amsentry_t **db, const char *data)  {
  for (size_t i=0;i < DB_SIZE && data; i++)  {
#ifdef _WIN32
    if ( db[i] && _stricmp(db[i]->name,data) == 0)  {
#else
      if ( db[i] && strcasecmp(db[i]->name,data) == 0)  {
#endif
        return db[i];
      }
    }
    return 0;
  }

  /// retrieve AMS process information
  static int _amsc_get_proc_info (char *proc, size_t length)  {
    const char *s = getenv ("AMSNAME");
    if ( s )  {
      if (length < strlen (s) + 1)  {
        errno = AMS_STRBUFFOVFL;
        return AMS_ERROR;
      }
      strcpy (proc, s);
      return AMS_SUCCESS;
    }
    int sc = ::lib_rtl_get_process_name(proc, length);
    if ( !lib_rtl_is_success(sc) )  {
      errno = AMS_NONAME;
      return AMS_ERROR;
    }
    return AMS_SUCCESS;
  }

  static void _amsc_fill_header(amsheader_t *amh, size_t size, 
                                const char *src, const char *dest,
                                int fac, int mtype, const void *buf_ptr)
  {
    amh->reset();
    amh->size     = htonl(size);
    amh->facility = htonl(fac);
    amh->msg_type = htonl(mtype);
    ::strncpy (amh->dest,   dest, sizeof (amh->dest));
    ::strncpy (amh->source, src,  sizeof (amh->source));
    amh->dest[sizeof (amh->dest)-1]=0;
    amh->source[sizeof (amh->source)-1]=0;
    if (buf_ptr)  {
      ::memcpy (amh+1,buf_ptr,size-sizeof(amsheader_t ));
    }
  }

  static void _amsc_net_to_host (amsheader_t *amh)  {
    amh->size     = ntohl (amh->size);
    amh->msg_type = ntohl (amh->msg_type);
    amh->facility = ntohl (amh->facility);
  }

  static int _amsc_tcp_set_sockopts(amsentry_t *db)  {
    int on = 1;
    struct linger Linger;
    Linger.l_onoff = 1;
    Linger.l_linger = LINGER_VALUE ;
    ::setsockopt(db->chan, SOL_SOCKET, SO_LINGER, (const char*)&Linger, sizeof(Linger));
    ::setsockopt(db->chan, SOL_SOCKET, SO_SNDBUF, (const char*)&SNDBUF_VALUE, sizeof(int)) ;
    ::setsockopt(db->chan, SOL_SOCKET, SO_RCVBUF, (const char*)&RCVBUF_VALUE, sizeof(int)) ;
    ::setsockopt(db->chan, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
    ::setsockopt(db->chan, SOL_SOCKET, SO_BROADCAST, (const char*)&on, sizeof(on));
    ::setsockopt(db->chan, SOL_SOCKET, SO_OOBINLINE, (const char*)&on, sizeof(on));
    return 0;
  }

  static int _amsc_tcp_init (amsentry_t *db)   {
    int status;
    WITHOUT_INTERCEPT(db->chan = ::socket (AF_INET, SOCK_STREAM, 0) )
      if (db->chan == -1)   {
        return lib_rtl_get_error();
      }
    _amsc_tcp_set_sockopts(db);
    db->address.sin_family = AF_INET;
    db->address.sin_addr.s_addr = INADDR_ANY;
    status = ::tan_allocate_port_number (db->name, &db->address.sin_port);
    if (status != AMS_SUCCESS)  {
      return status;
    }
    WITHOUT_INTERCEPT(status=::bind(db->chan,(struct sockaddr*)&db->address,sizeof(struct sockaddr_in)))
      if (status == -1)  {
        status  = ::lib_rtl_get_error();
        WITHOUT_INTERCEPT(socket_close (db->chan));
        return status;
      }
    WITHOUT_INTERCEPT(status=::listen (db->chan, 5));
    if (status == -1)  {
      status  = ::lib_rtl_get_error();
      WITHOUT_INTERCEPT(socket_close (db->chan));
      return status;
    }
    return AMS_SUCCESS;
  }

  static void _amsc_tcp_terminate (amsentry_t *db) {
    if (db->address.sin_port)  {
      ::tan_deallocate_port_number (db->name);
      db->address.sin_addr.s_addr = 0;
      WITHOUT_INTERCEPT(
                        _amsc_tcp_set_sockopts(db);
                        socket_close (db->chan));
    }
  }

  static int _amsc_tcp_open (amsentry_t *db)   {
    int status = tan_get_address_by_name (db->name, &db->address);
    if (TAN_SS_SUCCESS == status)  {
      WITHOUT_INTERCEPT(db->chan = socket (AF_INET, SOCK_STREAM, 0); _amsc_tcp_set_sockopts(db));
      if (db->chan == -1)  {
        return lib_rtl_socket_error();
      }
      lib_rtl_disable_intercept();
      _amsc_printf("Task: %s has port: %d - %d\n",db->name, db->address.sin_port, ntohl(db->address.sin_port));
      status = connect (db->chan, (struct sockaddr*)&db->address,sizeof(struct sockaddr_in));
      lib_rtl_enable_intercept();
      if (status == -1)  {
        status  = lib_rtl_socket_error();
        WITHOUT_INTERCEPT(socket_close (db->chan));
        return status;
      }
      return AMS_SUCCESS;
    }
    return status;
  }

  static int _amsc_tcp_close (int master_port, amsentry_t *e)  {
    IOPortManager(master_port).remove(e->chan);
    WITHOUT_INTERCEPT(
                      _amsc_tcp_set_sockopts(e);
                      shutdown(e->chan,2);
                      socket_close (e->chan));
    return AMS_SUCCESS;
  }

  static int _amsc_tcp_accept (amsentry_t *mydb, amsentry_t *db)   {
    socklen_t n = sizeof (db->address);
    db->chan = accept (mydb->chan, (sockaddr*)&db->address, &n);
    if (db->chan == -1)  {
      int ret_status  = lib_rtl_socket_error();
      return ret_status;
    }
    _amsc_tcp_set_sockopts(db);
    return AMS_SUCCESS;
  }

  static int _amsc_tcp_send_exact (amsentry_t *db,const void *buffer,size_t siz,u_int flag) {
    const char* buff = (const char*)buffer;
    u_int sent = 0, tosend = siz;
    int sent_now;
    u_int Npack = siz / CHOP_SIZE;
    u_int Nrest = siz % CHOP_SIZE;
    u_int this_siz  = siz;
    u_int already_sent = 0;

    this_siz  = CHOP_SIZE;
    for (size_t i = 0; i < Npack; ++i )  {
      tosend = CHOP_SIZE;
      sent  = 0;
      while (sent != this_siz)    {
        WITHOUT_INTERCEPT(sent_now = ::send (db->chan, buff + already_sent, tosend, flag));
        if (sent_now == -1)  {
          errno = lib_rtl_socket_error();
          lib_rtl_signal_message(LIB_RTL_OS,"AMS: send error [%s] size:%d/%d/%d errno=%d",
                                 db->name,siz,already_sent,tosend,errno);
          // lib_rtl_start_debugger();
          return errno;
        }
        already_sent += sent_now;
        sent += sent_now;
        tosend -= sent_now;
      }
    }
    this_siz = Nrest;
    tosend   = Nrest;
    sent     = 0;
    if (this_siz > 0)  {
      while (sent != this_siz)  {
        WITHOUT_INTERCEPT(sent_now = send (db->chan, buff + already_sent, tosend, flag));
        if (sent_now == -1)  {
          errno = lib_rtl_socket_error();
          lib_rtl_signal_message(LIB_RTL_OS,"AMS: send error [%s] size:%d/%d/%d errno=%d",
                                 db->name,siz,already_sent,tosend,errno);
          // lib_rtl_start_debugger();
          return errno;
        }
        already_sent += sent_now;
        sent   += sent_now;
        tosend -= sent_now;
      }
    }
    return AMS_SUCCESS;
  }

  static int _amsc_tcp_recv_exact (amsentry_t *db, void *buffer, size_t siz, unsigned int flag)  {
    u_int got = 0;
    int count = 0;
    size_t toget = siz;
    char* buff = (char*)buffer;
    while (got != siz)  {
      WITHOUT_INTERCEPT(int got_now = ::recv (db->chan, buff + got, toget, flag));
      if (got_now <= 0)    {
        errno = ::lib_rtl_socket_error();
        _amsc_printf("AMS: receive error errno=%d\n",errno);
        switch(errno)  {
        case ESOCK_CONNREFUSED:
        case ESOCK_NOTCONN:
        case ESOCK_CONNABORTED:
        case ESOCK_CONNRESET:
        case ESOCK_NETRESET:
        case ESOCK_NETDOWN:
        case ESOCK_SHUTDOWN:
          return AMS_CONNCLOSED;
        case ESOCK_NOTSOCK:
        case ESOCK_FAULT:
        case ESOCK_INVAL:
        case ESOCK_BADF:
          return AMS_TERRIBLE;
        default:
          if (++count == MAX_TCP_ERRORS)
            return AMS_NODATA;
          else {
            _amsc_printf("AMS: receive error (ignored) errno=%d\n",errno);
            got_now = 0;
          }
        }
      }
      got   += got_now;
      toget -= got_now;
    }
    return AMS_SUCCESS;
  }

  static int _amsc_tcp_get_host_name (char *nodename, int length) {
    char name [HOST_NAME_LENGTH];
    WITHOUT_INTERCEPT(int status=::tan_host_name(name, sizeof (name)));
    if (status != AMS_SUCCESS) {
      return AMS_HOSTNOTFOUND;
    }
    char* p = ::strchr (name, '\0');
    int   n = (p == 0) ? ::strlen (name) : p - name;
    if (length < n + 1)
      return AMS_STRBUFFOVFL;
    strncpy (nodename, name, n);
    nodename [n] = '\0';
    return AMS_SUCCESS;
  }

  static int _amsc_tcp_set_send_buff(amsentry_t *db, u_int siz) {
    int sndbuf = (siz>CHOP_SIZE) ? CHOP_SIZE : (siz<LOWER_CHOP) ? LOWER_CHOP : siz;
    sndbuf *= 2;
    if ( sndbuf != db->sndBuffSize )     {
      lib_rtl_disable_intercept();
      ::setsockopt(db->chan, SOL_SOCKET, SO_SNDBUF, (const char*)&sndbuf, sizeof(int));
      lib_rtl_enable_intercept();
      db->sndBuffSize = sndbuf;
    }
    return 0;
  }

  /// Remove duplicated entries from AMS DB
  static int _amsc_remove_duplicate_entry (amsentry_t **db, amsentry_t *e)  {
    for (size_t i = 0; i < DB_SIZE; i++)  {
      if( db[i] && ::strcmp(e->name,db[i]->name) == 0 )  {
        _amsc_tcp_close(_ams.me.address.sin_port,db[i]);
        delete db[i];
        db[i]=0;
        return AMS_SUCCESS;
      }
    }
    return AMS_SUCCESS;
  }

  static int _amsc_test_message (void)  {
    qentry_t *m;
    if ( !lib_rtl_queue_success(remqhi(&_ams.message_Q,&m)) )
      return AMS_ERROR;
    insqhi(m, &_ams.message_Q);
    return AMS_SUCCESS;
  }

  static int _amsc_stack_next_message(qentry_t *header) {
    qentry_t *e;
    if ( lib_rtl_queue_success(remqhi(header, &e)) )
      insqhi (e, &_ams.park_Q);
    return AMS_SUCCESS;
  }

  static int _amsc_restore_stack(int *cnt)  {
    qentry_t *m;
    *cnt = 0;
    while ( lib_rtl_queue_success(remqhi (&_ams.park_Q, &m)) )    {
      insqhi (m, &_ams.message_Q);
      (*cnt)++;
    }
    return AMS_SUCCESS;
  }

  // This function returns a pointer: when this is 0 then the error is stored in errno,
  static amsentry_t *_amsc_connect_to_task (const char *dest, const char *from)  {
    if (::strlen(dest) >= NAME_LENGTH) {
      errno = AMS_STRBUFFOVFL;
      return 0;
    }
    amsentry_t *e = new amsentry_t(dest, _ams.me.name);
    int sc = _amsc_tcp_open(e);
    if (sc != AMS_SUCCESS)  {
      e->release();
      errno = sc;
      return 0;
    }
    _amsc_tcp_set_send_buff(e,NAME_LENGTH+sizeof (e->name));
    sc = _amsc_tcp_send_exact(e, from, NAME_LENGTH, 0);
    if ( sc != AMS_SUCCESS )  {
      _amsc_tcp_close(_ams.me.address.sin_port,e);
      e->release();
      errno = sc;
      return 0;
    }
    sc = _amsc_tcp_send_exact(e,e->name,sizeof (e->name), 0);
    if ( sc != AMS_SUCCESS )  {
      _amsc_tcp_close(_ams.me.address.sin_port,e);
      e->release();
      errno = sc;
      return 0;
    }
    e->received  = 0;
    e->refCount += 1;
    _amsc_peek_rearm(e);
    return e;
  }

  static int _amsc_disconnect_from_task (amsentry_t *e)   {
    _amsc_tcp_close(_ams.me.address.sin_port,e);
    /* int status = */ _amsc_db_remove_entry (_ams.db, e);
    if (e->pending != 0)  {
      e->pending->release();
      e->pending = 0;
    }
    return e->release();
  }

  static void _amsc_send_shutdown_message (amsentry_t *e)  {
    amsqueue_t *m = new amsqueue_t(sizeof(amsheader_t));
    _amsc_fill_header(m->message,m->size,e->name,_ams.me.name,0,AMS_TASKDIED,0);
    _amsc_net_to_host(m->message);
    if (_ams.userAst != 0)  {
      lib_rtl_run_ast (_ams.userAst, _ams.userPar, 3);
    }
    insqti (m, &_ams.AMS_Q);
    _amsc_disconnect_from_task (e);
    wtc_insert(WT_FACILITY_TCPAMS, 0);
  }

  int _amsc_move_msgptr_to_user (amsqueue_t *m, void **buff, size_t *size, 
                                 char *from, char *dest, unsigned int *facility)
  {
    amsheader_t *h = m->message;
    int status = AMS_TERRIBLE;
    *buff = 0;
    switch (h->msg_type)  {
    case AMS_MSG_DATA:
      if ( size)  *size = m->size - sizeof(amsheader_t);
      if ( buff ) *buff = h + 1;
      status = AMS_SUCCESS;
      break;
    case AMS_TASKDIED:
    case AMS_CONNCLOSED:
    case AMS_TIMEOUT:
      if ( size ) *size = 0;
      if ( buff ) *buff = 0;
      status = h->msg_type;
      break;
    }
    if ( facility ) *facility = h->facility;
    if ( from ) ::strncpy(from, h->source, sizeof (h->source));
    if ( dest ) ::strncpy(dest, h->dest,   sizeof (h->dest));
    return status;
  }

  static int _amsc_move_to_user (amsqueue_t *m, void *buff, size_t *size, 
                                 char *from, char *dest, unsigned int *facility, 
                                 bool partial=false)    
  {
    size_t s = 0;
    void* ptr = 0;
    int status = _amsc_move_msgptr_to_user(m, &ptr, &s, from, dest, facility);
    if ( status == AMS_SUCCESS )  {
      if ( *size >= s )  {
        ::memcpy (buff, ptr, *size=s);
        return status;
      }
      else if ( partial && *size > 0 )  {
        ::memcpy (buff, ptr, s < *size ? s : *size);
        *size = s;
        return status;
      }
      // lib_rtl_start_debugger();
      return AMS_TERRIBLE;
    }
    return status;
  }

  static int _amsc_spy_last_message (void* buffer, size_t* size, char* from, unsigned int* facility, size_t* tlen) {
    amsqueue_t *m;
    if ( (remqti (&_ams.message_Q, (qentry_t**)&m) & 1) )    {
      if (facility) *facility = 0;
      if (tlen    ) *tlen = 0;
      *size = 0;
      *from = '\0';
      return errno = AMS_NOPEND;
    }
    int status = _amsc_move_to_user (m, buffer, size, from, 0, facility, true);
    if (tlen != 0)  {
      *tlen = m->size - sizeof (amsheader_t);
    }
    insqti (m, &_ams.message_Q);
    _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
    return (status == AMS_SUCCESS) ? status : errno=status;
  }

  static int _amsc_spy_next_message (void* buffer, size_t* size, char* from, unsigned int* facility, size_t* tlen, qentry_t* header)   {
    amsqueue_t *m;
    if ( !lib_rtl_queue_success(remqhi(header,(qentry_t**)&m)) )    {
      _amsc_printf("_amsc_spy_next_message: no entry pending!\n");
      if (facility) *facility = 0;
      if (tlen)     *tlen = 0;
      *size = 0;
      *from = '\0';
      return errno = AMS_NOPEND;
    }
    int status = _amsc_move_to_user (m, buffer, size, from, 0, facility, true);
    if (tlen != 0)  {
      *tlen = m->size - sizeof (amsheader_t);
    }
    insqhi(m,header);
    _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
    _amsc_printf("_amsc_spy_next_message: all done: status=%d\n",status);
    return status==AMS_SUCCESS ? status : errno=status;
  }

  // This function returns a pointer: when this is 0 then the error is
  // stored in errno,
  static amsentry_t *_amsc_find_connection (char *dest, char *from, CONNECTION_FIND_MODES mode)   {
    amsentry_t *e = _amsc_db_find_entry (_ams.db, dest);
    if ( !e )  {
      switch (mode)  {
      case FIND_ONLY:                       // Entry not found, store error code and return 0
        errno = AMS_TASKNOTFOUND;
        return 0;
      case CREATE_IF:                       // Try to create connection
        if ( (e=_amsc_connect_to_task(dest,from)) )  {  
          int sc = _amsc_db_insert_entry (_ams.db, e);
          if (sc != AMS_SUCCESS)  {         // Cannot insert in db
            _amsc_disconnect_from_task (e);
            errno = sc;
            return 0;
          }
        }                                  // Connection ok 
        return e;                          // if 0: Cannot create connection (error code already stored)
      default:                             // Unknown mode, return 0
        errno = AMS_UNKNOWNMODE;
        return 0;
      }
    }
    // Finally done, return value
    return e;
  }

  static int _amsc_peek_action (unsigned int /* fac */, void* param)    {
    RTL::Lock lock(_ams.lockid);
    amsentry_t* e = (amsentry_t*)param;
    amsqueue_t* m = e ? e->pending : 0;
    if ( e->pending == 0 )  {  // brand new message: get its length (net_to_hosted)
      int length, status;
      status = _amsc_tcp_recv_exact(e,&length,sizeof(int),0);
      length = ntohl (length);
      _amsc_printf("amsc_peek_action: %d\n",status);
      switch (status)  {
      case AMS_SUCCESS:        // allocate memory for message
        m = new amsqueue_t(length);
        e->received = sizeof(int);
        break;
      case AMS_CONNCLOSED:
        _amsc_send_shutdown_message(e);
        return AMS_SUCCESS;
      case AMS_NODATA:
        return AMS_SUCCESS;
      default:
        _amsc_send_shutdown_message(e);
        return AMS_SUCCESS;
      }
    }
    // read some more data
    e->msg_ptr      = m;
    e->current_size = m->size;
    e->refCount++;
    return _amsc_receive_rearm(e);
  }

  static int _amsc_peek_ast (void* param)  {
    return wtc_insert(WT_FACILITY_TCP_PEEK, param);
    //return _amsc_peek_action(WT_FACILITY_TCP_PEEK, param);
  }

  static int _amsc_peek_rearm (amsentry_t* e) {
    IOPortManager(_ams.me.address.sin_port).add(1,e->chan,_amsc_peek_ast,e);
    return WT_SUCCESS;
  }

  static int _amsc_make_accept()   {
    RTL::Lock lock(_ams.lockid);
    amsentry_t *e = new amsentry_t();
    int status = _amsc_tcp_accept (&_ams.me, e);
    if (status != AMS_SUCCESS)  {
      return e->release();
    }
    _amsc_printf("Got Accept request.\n");
    // receive name of partner
    status = _amsc_tcp_recv_exact (e, e->name, sizeof(e->name),0);
    if (status != AMS_SUCCESS)  {
      _amsc_tcp_close(_ams.me.address.sin_port,e);
      return e->release();
    }
    // receive (own) name to be used when talking to partner
    status = _amsc_tcp_recv_exact(e, e->myName,sizeof(e->myName),0);
    if (status != AMS_SUCCESS)  {
      _amsc_tcp_close(_ams.me.address.sin_port,e);
      return e->release();
    }
    // Check to see if this task already in databse as I dont get a 
    // signal from partner on another machine if I am in send mode 
    status = _amsc_remove_duplicate_entry(_ams.db, e);
    status = _amsc_db_insert_entry (_ams.db, e);
    if (status != AMS_SUCCESS)  {
      _amsc_tcp_close(_ams.me.address.sin_port,e);
      return e->release();
    }
    e->received = 0;
    e->refCount = 1;
    _amsc_printf("Connection accepted from :%s [%s]\n", e->name, e->myName);
    return _amsc_peek_rearm(e);
  }

  static int _amsc_accept_ast (void* /* param */)    {
    if (_ams.userAst != 0) lib_rtl_run_ast (_ams.userAst, _ams.userPar, 3);
    int status = _amsc_make_accept();
    IOPortManager(_ams.me.address.sin_port).add(0, _ams.me.chan, _amsc_accept_ast, &_ams.me);
    return status;
  }

  int _amsc_timeout_action(unsigned int /* fac */, void* /* par */ )  {
    amsqueue_t  *m  = new amsqueue_t(sizeof(amsheader_t));
    _amsc_fill_header (m->message,m->size,_ams.me.name,_ams.me.name,0,AMS_TIMEOUT,0);
    _amsc_net_to_host (m->message);
    insqti (m, &_ams.AMS_Q);
    _amsc_printf("AMS: Timeout action called!\n");
    return wtc_insert (WT_FACILITY_TCPAMS, 0);
  }

  int _amsc_message_timeout_ast (void* param){
    *(int*)param = 1;
    if (_ams.userAst != 0)
      lib_rtl_run_ast (_ams.userAst, _ams.userPar, 3);
    _amsc_printf("AMS: Timeout ast called!\n");
    wtc_insert (WT_FACILITY_TCPAMS, (void*)-1);
    return AMS_SUCCESS;
  }

  static int _amsc_mess_action(unsigned int fac,void* par )  {
    qentry_t *m;
    int rcnt;
    _amsc_printf("Message action requested.\n");
    if (par == (void**)-1)  {
      _amsc_timeout_action(fac,par);
      _amsc_printf("Message action: timeout occurred!\n");
      return WT_SUCCESS;
    }
    if (_ams.msgWaiting == 1)  {
#ifdef _USE_FULL_WT
      RTL::Lock lock(_ams.lockid);
#endif
      unsigned int facility;
      size_t tlen, siz = 80;
      char src[64], buff[80];
      int status  = _amsc_spy_next_message(buff,&siz,src,&facility,&tlen,&_ams.AMS_Q);
      _amsc_printf("Message action: Spy on next wessage: status=%d\n",status);
      if (status == AMS_TIMEOUT)    {
        remqhi (&_ams.AMS_Q, &m);
        _amsc_restore_stack (&rcnt);
        insqhi (m, &_ams.message_Q);
        _ams.msgWaiting = 0;
        for (int i=0;i<rcnt;i++)    {
          wtc_insert_head (WT_FACILITY_AMS, 0);
        }
        _amsc_printf("Message action: Spy on next wessage:AMS_TIMEOUT->WT_FACILITY_AMSSYNCH!\n");
        return wtc_insert_head (WT_FACILITY_AMSSYNCH, 0);
      }
      else if (_amsc_requirements_satisfied(src,facility) == 0)      {
        _amsc_printf("Message action: message requirements not satisfied!\n");
        _amsc_stack_next_message(&_ams.AMS_Q);
      }
      else if ( lib_rtl_queue_success(remqhi(&_ams.AMS_Q,&m)) )    {
        _amsc_restore_stack (&rcnt);
        insqhi(m,&_ams.message_Q);
        _ams.msgWaiting  = 0;
        // Queue restore system traps
        for (int i=0;i<rcnt;i++)   {
          wtc_insert_head(WT_FACILITY_AMS,0);
        }
        _amsc_printf("Message action: WT_FACILITY_AMSSYNCH!\n");
        return wtc_insert_head(WT_FACILITY_AMSSYNCH,0);
      }
      else {
        _amsc_printf("Message action: No entry found in AMS_Queue!\n");
      }
    }
    else  {
      _amsc_printf("Message action: No message pending!\n");
#ifdef _USE_FULL_WT
      RTL::Lock lock(_ams.lockid);
#endif
      if ( lib_rtl_queue_success(remqhi(&_ams.AMS_Q,&m)) )    {
        insqti (m, &_ams.message_Q);
        wtc_insert_head (WT_FACILITY_AMS, 0);
      }
    }
    return WT_SUCCESS;
  }

  static int _amsc_receive_action (unsigned int /* fac */,void* param)   {
#ifdef _USE_FULL_WT
    RTL::Lock lock(_ams.lockid);
#endif
    amsentry_t *e = (amsentry_t*)param;
    amsqueue_t *m = e->msg_ptr;
    size_t length = m->size - e->received;
    int status = _amsc_tcp_recv_exact(e, (char*)m->message + e->received, length, 0);
    //int status = _amsc_tcp_recv (e, (char*)m->message + e->received, &length, 0);
    _amsc_printf("receive action: recv_exect:%d\n",status);

    switch (status)  {
    case AMS_SUCCESS:
      e->pending = m;
      e->received += length;
      break;
    case AMS_CONNCLOSED:
      e->msg_ptr = m->release();
      _amsc_send_shutdown_message (e);
      return AMS_SUCCESS;
    default:
      e->msg_ptr = m->release();
      _amsc_disconnect_from_task (e);
      return AMS_SUCCESS;
    }
    if (e->received < m->size)  {
      return _amsc_receive_rearm (e);
    }
    else if (e->received > m->size)  {
      e->msg_ptr = m->release();
      _amsc_disconnect_from_task (e);
      exit (AMS_TERRIBLE);
    }
    // we have the complete message: swap header and store message length
    _amsc_net_to_host (m->message);
    if (m->message->msg_type == AMS_CONNCLOSED)  {
      m->message->size = m->size;
      e->pending  = 0;
      e->received = 0;
      _amsc_disconnect_from_task (e);
    }
    else if (m->message->msg_type == AMS_MSG_DATA)  {
      m->message->size = m->size-sizeof(int);
      e->pending  = 0;
      e->received = 0;
      _amsc_peek_rearm (e);
    }
    if (_ams.userAst != 0)  {
      lib_rtl_run_ast(_ams.userAst, _ams.userPar, 3);
    }
    insqti (m, &_ams.AMS_Q);
#ifdef _USE_FULL_WT
    return wtc_insert(WT_FACILITY_TCPAMS, 0);
#else
    return _amsc_mess_action(WT_FACILITY_TCPAMS,0);
#endif
  }

  static int _amsc_receive_ast (void* param) {
    if ( _ams.userAst != 0 )    {
      lib_rtl_run_ast (_ams.userAst, _ams.userPar, 3);
    }
    return _amsc_receive_action(WT_FACILITY_TCP_RECEIVE, param);
  }

  static int _amsc_receive_rearm (amsentry_t* e)  {
    size_t size = e->current_size-e->received;
    if (size > CHOP_SIZE)    {
      size = CHOP_SIZE;
    }
#ifdef _USE_FULL_WT
    IOPortManager(_ams.me.address.sin_port).add(1, e->chan, _amsc_receive_ast, e);
    return WT_SUCCESS;
#else
    return _amsc_receive_ast(e);
#endif
  }

  static void _amsc_send_close_message(amsentry_t *e)  {
    amsheader_t amh;
    _amsc_fill_header (&amh, sizeof(amh),e->myName,e->name,0,AMS_CONNCLOSED,0);
    _amsc_tcp_set_send_buff(e,sizeof(amh));
    _amsc_tcp_send_exact (e, &amh, sizeof(amh), 0);
  }

  static int _amsc_send_message (const void* buff, size_t size, const char* dest, int fac, const char* from)  {
    char  full_dest [SAFE_NAME_LENGTH], full_from [SAFE_NAME_LENGTH];
    // If facility is 0 used default
    int status, facility = fac==0 ? FACILITIES__USER : fac;

    // Build full source and destination names 
    if (size <= 0)  {
      return AMS_ILLEGAL_LENGTH;
    }
    amsc_full_name(full_dest,dest,sizeof(full_dest),DECNET_STYLE);
    if (from != 0)  {
      amsc_full_name(full_from,from,sizeof(full_from),DECNET_STYLE);
    }
    else  {
      ::strcpy (full_from, _ams.me.name);
    }
    _amsc_printf("Sending message [%ld bytes] to: %s [%s]\n", long(size), full_dest, full_from);
    do   {
      size_t len = size+sizeof (amsheader_t);
      amsentry_t *e = _amsc_find_connection(full_dest,full_from,CREATE_IF);
      if (e == 0) {
        return errno = AMS_TASKNOTFOUND;
      }
      _amsc_tcp_set_send_buff(e, len);
      std::auto_ptr<amsheader_t> amh(new(::operator new(len)) amsheader_t);
      if (size <= AMS_K_COPY_LIMIT)    {
        _amsc_fill_header(amh.get(),len,full_from,full_dest,facility,AMS_MSG_DATA,buff);
        status = _amsc_tcp_send_exact(e,amh.get(),len, 0);
      }
      else    {
        _amsc_fill_header(amh.get(), len, full_from, full_dest, facility, AMS_MSG_DATA, 0);
        status = _amsc_tcp_send_exact(e,amh.get(),sizeof(amsheader_t), 0);
        if (status == AMS_SUCCESS)  {
          status = _amsc_tcp_send_exact(e, buff, size, 0);
        }
      }
      if (status != AMS_SUCCESS)    {
        _amsc_disconnect_from_task(e);
      }
    } while (status != AMS_SUCCESS);
    _amsc_printf("Message sent....\n");
    return AMS_SUCCESS;
  }

  static int _amsc_read_message (void* buffer, size_t* size, char* from, unsigned int* facility, char* dest)  {
    amsqueue_t *m = 0;
    CheckInitialization();
    int status  = _amsc_test_message();
    if (status == AMS_SUCCESS)  {
      remqhi (&_ams.message_Q, (qentry_t**)&m);
      status = _amsc_move_to_user (m, buffer, size, from, dest, facility, false);
      m->release();
      _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
      return (status == AMS_SUCCESS) ? status : errno=status;
    }
    return errno=AMS_NOPEND;
  }

  static int _amsc_read_message_long (void** buffer, size_t* size, char* from, unsigned int* facility, char* dest)  {
    amsqueue_t *m = 0;
    CheckInitialization();
    int status  = _amsc_test_message();
    if (status == AMS_SUCCESS)  {
      remqhi(&_ams.message_Q, (qentry_t**)&m);
      status = _amsc_move_msgptr_to_user(m,buffer,size,from,dest,facility);
      m->message = 0;
      m->release();
      _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
      return (status == AMS_SUCCESS) ? status : errno=status;
    }
    return errno=AMS_NOPEND;
  }

  static int _amsc_get_message (void* buffer, size_t* size, char* from, char* r_source_in,
                                int timeout, unsigned int* facility, unsigned int r_facility, char* dest)
  {
    unsigned int w_fac, fac;
    unsigned long timer_id = 0;
    int w_stat, status, rcnt, parking;
    void* w_par;
    amsqueue_t *m = 0;
    CheckInitialization();
    _amsc_printf("_amsc_get_message called\n");
    _ams.reqFac = 0;
    ::memset (_ams.reqSource, 0, sizeof(_ams.reqSource));
    if (r_source_in != 0)   {
      amsc_full_name(_ams.reqSource, r_source_in, sizeof (_ams.reqSource), DECNET_STYLE);
      _amsc_remove_node_if_mine(_ams.reqSource);
    }
    parking = (r_source_in != 0) || (r_facility != 0) || timeout!= 0;
    if (parking)  {
      size_t tlen, siz=80;
      char src[64], buff[80];
      _ams.reqFac = r_facility;
      _ams.msgWaiting = 1;
      do  {
        status = _amsc_spy_next_message(buff,&siz,src,&fac,&tlen,&_ams.message_Q);
        if (status == AMS_TIMEOUT)  {
          remqhi (&_ams.message_Q, (qentry_t**)&m);
          m->release();
          status = AMS_SUCCESS;
        }
        else   {
          if (status != AMS_NOPEND)  {
            if (_amsc_requirements_satisfied(src,fac) == 0)   {
              _amsc_stack_next_message(&_ams.message_Q);
            }
            else  {
              remqhi (&_ams.message_Q, (qentry_t**)&m);
              do  {
                status  = _amsc_test_message();
                if (status == AMS_SUCCESS)  {
                  _amsc_stack_next_message(&_ams.message_Q);
                }
              }
              while (status == AMS_SUCCESS);
              wtc_flush(WT_FACILITY_AMS);
              _amsc_restore_stack (&rcnt);
              for (int i=0;i<rcnt;i++)  {
                wtc_insert_head (WT_FACILITY_AMS, 0);
              }
              insqhi (m, &_ams.message_Q);
              _ams.msgWaiting  = 0;
            }
          }
        }
      } while(status==AMS_SUCCESS || status==AMS_CONNCLOSED || status==AMS_TASKDIED);
    }
    status = _amsc_test_message();
    if (status == AMS_SUCCESS)  {
      remqhi(&_ams.message_Q, (qentry_t**)&m);
      status = _amsc_move_to_user (m, buffer, size, from, dest, facility, false);
      m->release();
      _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
      return (status == AMS_SUCCESS) ? status : errno=status;
    }
    else  {
      _ams.msgWaiting = 1;
      int  message_timeout_ast_fired = 0;
      if (timeout != 0)    {
        ::lib_rtl_set_timer(timeout,_amsc_message_timeout_ast,&message_timeout_ast_fired,&timer_id);
      }
      do  {
        status = wtc_wait_with_mask(&w_fac,&w_par,&w_stat,_ams.wt_enable_mask);
        if (w_fac != WT_FACILITY_AMSSYNCH)    {
          wtc_add_stack(w_fac,w_par);
        }
      }
      while (w_fac != WT_FACILITY_AMSSYNCH && w_fac != WT_FACILITY_TIMEOUT);
      wtc_restore_stack();
      if (timeout != 0)  {
        if (message_timeout_ast_fired == 0)  {
          lib_rtl_kill_timer (timer_id);
        }
        timer_id = 0;
      }
      remqhi (&_ams.message_Q, (qentry_t**)&m);
      do  {
        status  = _amsc_test_message();
        if (status == AMS_SUCCESS)    {
          _amsc_stack_next_message(&_ams.message_Q);
        }
      }while (status==AMS_SUCCESS);
      wtc_flush(WT_FACILITY_AMS);  
      _amsc_restore_stack (&rcnt);
      for (int i=0;i<rcnt;i++) {
        wtc_insert_head (WT_FACILITY_AMS, 0);
      }
      status = _amsc_move_to_user (m, buffer, size, from, dest, facility, false);
      m->release();
      _amsc_remove_node_if_mine(from, DEFAULT_STYLE);
      return (status == AMS_SUCCESS) ? status : errno=status;
    }
  }

  static int _amsc_disconnect_task(const char* task)   {
    char  full_dest [SAFE_NAME_LENGTH];
    char  full_from [SAFE_NAME_LENGTH];
    strcpy(full_from, _ams.me.name);
    amsc_full_name(full_dest, task, sizeof(full_dest), DECNET_STYLE);
    amsentry_t *e = _amsc_find_connection(full_dest, full_from, FIND_ONLY);
    if (e != 0)    {
      _amsc_send_close_message (e);
      _amsc_disconnect_from_task (e);
      return AMS_SUCCESS;
    }
    return errno = AMS_TASKNOTFOUND;
  }

  //----------------------------------------------------------------------------------
  int amsc_init (const char *inname)   {
    if (_ams.inited)  {
      _ams.refCount++;
      return errno = AMS_ALRINITED;
    }
    _ams.refCount = 0;
    _ams.userAst = 0;
    _ams.userPar = 0;
    int status = _amsc_tcp_get_host_name (_ams.hostName, sizeof (_ams.hostName));
    if (status != AMS_SUCCESS)  {
      return errno = status;
    }
    _ams.hostNameLen   = strlen (_ams.hostName);
    if (inname == 0)  {
      status = _amsc_get_proc_info (_ams.name, sizeof (_ams.name));
      if ( status != AMS_SUCCESS )    {
        return errno = status;
      }
    }
    else  {
      strncpy (_ams.name, inname, sizeof (_ams.name));
    }
    amsc_full_name(_ams.me.name,_ams.name,sizeof(_ams.me.name),DECNET_STYLE);
    memset(_ams.db, 0, sizeof(_ams.db));
    if ( (status=wtc_init()) != WT_SUCCESS )  {
      return errno = status;
    }
    status = wtc_subscribe(WT_FACILITY_TCP_PEEK,0,_amsc_peek_action);
    status = wtc_subscribe(WT_FACILITY_TCPAMS,0,_amsc_mess_action);
    if ( status != WT_SUCCESS )  {
      return errno = status;
    }
    status = wtc_subscribe(WT_FACILITY_TIMEOUT,0,_amsc_timeout_action);
    if ( status != WT_SUCCESS )  {
      wtc_remove(WT_FACILITY_TCPAMS);
      return errno = status;
    }
    status = _amsc_tcp_init (&_ams.me);
    if ( status != WT_SUCCESS )  {
      wtc_remove(WT_FACILITY_TCPAMS);
      wtc_remove(WT_FACILITY_TIMEOUT);
      return errno = status;
    }
    status  = wtc_create_enable_mask(&_ams.wt_enable_mask);
    status  = wtc_add_to_en_fac(_ams.wt_enable_mask,WT_FACILITY_TCPAMS);
    status  = wtc_add_to_en_fac(_ams.wt_enable_mask,WT_FACILITY_AMSSYNCH);
    if ( status != WT_SUCCESS )  {
      _amsc_tcp_terminate (&_ams.me);
      wtc_remove(WT_FACILITY_TCPAMS);
      wtc_remove(WT_FACILITY_TIMEOUT);
      return errno=status;
    }
    status = lib_rtl_create_lock(0, &_ams.lockid);
    if ( !lib_rtl_is_success(status) )  {
      lib_rtl_signal_message(LIB_RTL_OS,"Error creating AMS lock. Status %d",status);
      _amsc_tcp_terminate (&_ams.me);
      wtc_remove(WT_FACILITY_TCPAMS);
      wtc_remove(WT_FACILITY_TIMEOUT);
    }
    _ams.inited = true;
    _ams.refCount = 1;
    lib_rtl_declare_exit(AMS_exit_handler,0);
    IOPortManager(_ams.me.address.sin_port).add(0, _ams.me.chan, _amsc_accept_ast, &_ams.me);
    return AMS_SUCCESS;
  }

  int amsc_close ()   {
    CheckInitialization();
    if (--_ams.refCount == 0)  {
      _amsc_tcp_terminate (&_ams.me);
      for (size_t i=0; i < DB_SIZE; i++)  {
        amsentry_t *e = _ams.db[i];
        if ( e )    {
          _amsc_send_close_message (e);
          _amsc_disconnect_from_task (e);
        }
      }
      if (_ams.lockid) amsc_flush_message_queue ();
      wtc_flush(WT_FACILITY_TCPAMS);
      wtc_remove(WT_FACILITY_TCPAMS);
      wtc_flush(WT_FACILITY_TIMEOUT);
      wtc_remove(WT_FACILITY_TIMEOUT);
      if (_ams.lockid) lib_rtl_delete_lock(_ams.lockid);
      _ams.inited = false;
    }
    return AMS_SUCCESS;
  }

  void amsc_flush_message_queue (void) {
    amsqueue_t *m;
    RTL::Lock lock(_ams.lockid);
    while( lib_rtl_queue_success(remqhi(&_ams.message_Q,(qentry_t**)&m)) )  {
      m->release();
    }
  }

  int amsc_send_message (const void* buff, size_t size, const char* dest, int fac, const char* from)  {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_send_message(buff,size,dest,fac,from);
  }

  int amsc_get_node ( char node[40])   {
    return _amsc_tcp_get_host_name( node, HOST_NAME_LENGTH );
  }

  int amsc_get_message (void* buff, size_t* size, char* from, char* r_source_in,
                        int timeout, unsigned int* facility, unsigned int r_facility, char* dest) 
  {
    RTL::Lock lock(_ams.lockid);
    return _amsc_get_message(buff,size,from,r_source_in,timeout,facility,r_facility,dest);
  }

  int amsc_read_message (void* buff, size_t* size, char* from, unsigned int* facility, char* dest)  {
    RTL::Lock lock(_ams.lockid);
    return _amsc_read_message(buff,size,from,facility,dest);
  }

  int amsc_read_message_long (void** buff, size_t* size, char* from, unsigned int* facility, char* dest)  {
    RTL::Lock lock(_ams.lockid);
    return _amsc_read_message_long(buff,size,from,facility,dest);
  }

  int amsc_release_message_long(void* buff) {
    amsheader_t *h = (amsheader_t*)buff;
    if ( h ) {
      h = h - 1;
      if ( h->magic == AMS_MAGIC ) {
        delete h;
        return AMS_SUCCESS;
      }
    }
    printf("Failed to release long message buffer:%p\n",buff);
    return AMS_NODATA;
  }

  int amsc_declare_user_ast (int (*astadd)(void*),void* astpar) {
    CheckInitialization();
    _ams.userAst = astadd;
    _ams.userPar = astpar;
    return AMS_SUCCESS;
  }

  int amsc_spy_next_message (void* buffer, size_t* size, char* from, unsigned int* facility, size_t* tlen) {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_spy_next_message(buffer,size,from,facility,tlen,&_ams.message_Q);
  }

  int amsc_spy_last_message (void* buffer, size_t* size, char* from, unsigned int* facility, size_t* tlen) {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_spy_last_message(buffer,size,from,facility,tlen);
  }

  int amsc_test_message (void)  {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_test_message();
  }

  int amsc_declare_alias (const char* name) {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    char full_name [SAFE_NAME_LENGTH];
    amsc_full_name (full_name, name, SAFE_NAME_LENGTH, DECNET_STYLE);
    int status = tan_declare_alias(full_name);
    return status ? (errno=status) : AMS_SUCCESS;
  }

  int amsc_stack_next_message()   {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_stack_next_message(&_ams.message_Q);
  }

  int amsc_restore_stack(int *cnt)  {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_restore_stack(cnt);
  }

  int amsc_test_input() {
    unsigned int fac;
    void* param;
    if (AMS_SUCCESS == amsc_test_message())
      return true;
    int status = wtc_spy_next_entry( &fac, &param);
    if ( status == WT_SUCCESS )    {
      if ((fac ==WT_FACILITY_AMSSYNCH) || (fac ==WT_FACILITY_TCPAMS)) {
        return true;
      }
      return false;
    }
    return false;
  }

  int amsc_disconnect_task(const char* task)   {
    CheckInitialization();
    RTL::Lock lock(_ams.lockid);
    return _amsc_disconnect_task(task);
  }
