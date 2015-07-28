#include <map>
#include <memory>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "RTL/rtl.h"
#include "RTL/strdef.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"
#include "CPP/Event.h"
#include "CPP/Table.h"
#include "CPP/AmsSensor.h"
#include "CPP/Interactor.h"

#define MAXMSGSIZE 64000

namespace {

  class AmsSource {
  protected:
    Address      m_address;
    AmsSource*   m_next;
  public:
    explicit AmsSource( const Address& );
    ~AmsSource() {}
    int Compare( const Address& );
    AmsSource* next()           { return m_next;     }
    void setNext(AmsSource* n)  { m_next = n;        }
    //Address&  address()         { return m_address;  }
    //AmsSource* current()        { return this;       }
    //AmsSource* first()          { return this;       }
  };
  static AmsSource* SourceHead = 0;
  typedef std::map<AmsSource*, Interactor*>  InteractorTable;
  static InteractorTable s_interactorTable;

  static const char *node()  {
    static char n[64] = "";
    if ( n[0] == 0 )  {
      ::lib_rtl_get_node_name(n,sizeof(n)-3);
      ::strncat(n,"::",3);
    }
    return n;
  }

  //----------------------------------------------------------------------------
  AmsSource::AmsSource( const Address& add ) : m_next(0)   {
    m_address = add;
    if( add.node_process.find("::") == std::string::npos )  {
      m_address.node_process = node();
      m_address.node_process += add.node_process; 
    }      
  }

  //----------------------------------------------------------------------------
  int AmsSource::Compare( const Address& cand )   {
    std::string name = cand.node_process;
    unsigned int m1 = ~0x0;
    if( cand.node_process.find("::") == std::string::npos )  {
      name = node() + cand.node_process;
    }      
    if(m_address.facility == m1 || cand.facility == m1 || m_address.facility == cand.facility)
      return ::str_match_wild(name.c_str(), m_address.node_process.c_str() );
    return 0;
  }
}

//-----------------------------------------------------------------------------
static int action( const amsuc_info* /* info */, void* /* param */ )  {
  AmsSensor::instance().dispatch(0);
  return WT_SUCCESS;
}

//-----------------------------------------------------------------------------
static int broadcast( const amsuc_info* /* info */, void* /* param */ ) {
  return AmsSensor::instance().dispatchBroadCast();
}

//----------------------------------------------------------------------------
AmsSensor::AmsSensor() : Sensor( WT_FACILITY_RO1, "AmsSensor" )  {
#define INIT_RETRY 32
  int status;
  if ( AMS_SUCCESS != (status=::amsuc_init()))  {
    ::lib_rtl_signal_message(LIB_RTL_ERRNO,"amsuc_init Failed status = %d",status);
  }
  int retry = INIT_RETRY;
  do  {
    status=::amsc_init(0);
    if ( AMS_SUCCESS == status ) {
      break;
    }
    fprintf(stderr,"Error in amsc_init. Status %d. Retrying....\n",status);
    ::lib_rtl_sleep(100);
    retry--;
  } while(retry > 0);

  if ( AMS_SUCCESS != status ) {
    ::fprintf(stderr,"This error CANNOT be recovered....EXIT...\n");
    ::lib_rtl_signal_message(LIB_RTL_ERRNO,"amsc_init Failed status = %d",status);
    ::exit(status);
  }
  status=::wtc_subscribe(WT_FACILITY_AMS,0,amsuc_dispatch);
  if ( status != WT_SUCCESS ) {
    ::lib_rtl_signal_message(LIB_RTL_ERRNO,"wtc_subscribe Failed status = %d",status);
  }
}

//----------------------------------------------------------------------------
AmsSensor::~AmsSensor()    {
  amsc_close();
  s_interactorTable.clear();
}

//----------------------------------------------------------------------------
void AmsSensor::add(Interactor* interactor, const std::string& arg, bool broadcast)  {  
  add ( interactor, Address(arg), broadcast );
}

//----------------------------------------------------------------------------
void AmsSensor::add(Interactor* interactor, void* source) {
  add (interactor, *(const Address*)source, false );
}

//----------------------------------------------------------------------------
void AmsSensor::add(Interactor* interactor, const Address& source) {
  add(interactor, source, true);
}

//----------------------------------------------------------------------------
void AmsSensor::add(Interactor* interactor, const Address& source, bool broadcast) {
  AmsSource *amssource = new AmsSource(source);
  amssource->setNext(SourceHead); 
  SourceHead = amssource; 
  s_interactorTable.insert(std::make_pair(amssource , interactor));
  subscribe( source.facility, broadcast );
}

//----------------------------------------------------------------------------
void AmsSensor::remove( Interactor* interactor, void* source )   {
  return this->remove(interactor, (const Address*)source);
}

//----------------------------------------------------------------------------
void AmsSensor::remove( Interactor* interactor, const Address* source )   {
  if( source )  {
    for(AmsSource* as = SourceHead, *last_as = 0; as; as = as->next())    {
      if( as->Compare( *source ) == 0 )  {
        InteractorTable::iterator i = s_interactorTable.find(as);
        if ( i != s_interactorTable.end() )  {
          if( (*i).second == interactor )  {
            s_interactorTable.erase(i);
            if ( last_as ) last_as->setNext(as->next());
            else           SourceHead = as->next();
            delete as; 
            break;        
          }
        }
      } 
      last_as = as;
    }
  }
  else  {
    for(AmsSource* as = SourceHead, *last_as = 0; as; as = as->next())    {
      InteractorTable::iterator i = s_interactorTable.find(as);
      if ( i != s_interactorTable.end() )  {
        if( (*i).second == interactor )  {
          AmsSource* a = as;
          s_interactorTable.erase(i);
          if ( last_as ) { last_as->setNext(as->next()); as = last_as; }
          else           { as = SourceHead = as->next();               }
          delete a;
        }
      }
      else      {
        last_as = as;
      }
    } 
  }
}

//----------------------------------------------------------------------------
int AmsSensor::subscribe( int fac, bool listen_broadcast ) {
  return ::amsuc_subscribe( fac, action, listen_broadcast ? broadcast : 0, this );
}

//----------------------------------------------------------------------------
int AmsSensor::send( const Message* msg, const Address& dest)   {
  return ::amsc_send_message((int*)msg+1,msg->size,dest.node_process.c_str(),dest.facility,0);
}

//----------------------------------------------------------------------------
int AmsSensor::receive( Message** msg, Address* src, int timeout )  {
  char source[128];
  unsigned int  facility;
  char* ptr = new char[ MAXMSGSIZE + 4 ];
  Message* message = (Message*)ptr;
  size_t size = MAXMSGSIZE;
  int status = ::amsc_get_message((int*)message+1,&size,source,(char*)src->node_process.c_str(),
                                  timeout, &facility, src->facility, 0);
  if ( AMS_SUCCESS != status )  {
    delete [] ptr;
    return status;
  }
  message->size = size;
  *msg = message;
  return status;
}

//----------------------------------------------------------------------------
void AmsSensor::dispatch( void* /* id */ )  {
  char     source[128], spym[80], *ptr;
  size_t   size, len = sizeof(spym);
  Address  src;

  int status = ::amsc_spy_next_message(spym,&len,source,&src.facility,&size);
  if ( AMS_SUCCESS != status ) return;
  src.node_process = source;
  ptr = new char[ size + 8 ];
  Message* msg = (Message*)ptr;
  status = ::amsc_read_message((int*)msg+1,&size,source,&src.facility,0);
  src.node_process = source;
  if ( AMS_SUCCESS == status )  {
    msg->size = size;
    ptr[size+4] = 0;                         // terminate properly! 
    for ( AmsSource* as = SourceHead; as; as = as->next() )  {
      if( as->Compare(src) == 0 )  {
        InteractorTable::iterator i = s_interactorTable.find(as);
        if ( i != s_interactorTable.end() )  {
          Event ev((*i).second,NetEvent);
          ev.message    = msg;
          ev.source     = &src;
          ev.target->handle(ev);       
          continue;
        }
        printf("Got ASM Message not associated with any Source\n");
      }
    }        
  }
  delete [] ptr;
}

//-----------------------------------------------------------------------------
int AmsSensor::dispatchBroadCast()   {
  size_t      size = 80, len = 0;
  char        msg[84], source[128];
  Address     src;

  Message* message = (Message*)msg;
  int status  = ::amsc_spy_next_message(msg+4,&size,source,&src.facility,&len);
  if ( !(status==AMS_TASKDIED || status==AMS_CONNCLOSED) )   {
    return 1;
  }
  src.node_process = source;
  memset(msg,0,sizeof(msg));
  ::strncpy(msg+4,"IAMDEAD",sizeof(msg)-4);
  message->size = strlen(msg+4);
  src.facility = ~0;
  for( AmsSource* as = SourceHead; as; as = as->next() )  {
    if( as->Compare(src) == 0 )  {
      InteractorTable::iterator i = s_interactorTable.find(as);
      if ( i != s_interactorTable.end() )  {
        Event ev((*i).second,NetEvent);
        ev.message    = message;
        ev.source     = &src;
        ev.target->handle(ev);
      }
    }
  }
  return 1;
}

//-----------------------------------------------------------------------------
AmsSensor& AmsSensor::instance()   {
  static AmsSensor* sensor = new AmsSensor;
  return *sensor;
}
