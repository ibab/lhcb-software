#ifndef _WIN32
#include <iostream>
#include <string>
#include <cerrno>
#include <cstdarg>                                          // va_list, etc.
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ErrorSrv/ErrorSrv.h"
#include "RTL/rtl.h"
#include "RTL/time.h"
#include "RTL/Base64.h"

#define BUF_SZ 4096

using namespace std;
using namespace LHCb;

#define ERRSTR ::strerror(errno)

enum { S_FAIL=0,S_OK=1};

/// Constructor
ErrorSrv::ErrorSrv()   {
  m_fifo = -1;
  m_tryN =  2;
}

/// Destructor.
ErrorSrv::~ErrorSrv()   {
  if ( m_fifo > 0 ) {
    ::close(m_fifo);
    m_fifo = -1;
  }
}

int ErrorSrv::open(const string& fifo) {
  struct stat statBuf;
  const char* fn = fifo.c_str();

  // check if fifo is writable
  if( ::access(fn,W_OK) == -1 )  {  // access denied
    print("access('%s'): %s!",fn,ERRSTR);
    if ( errno == ENOENT )    {
      print("Please, run 'logSrv -p %s -s <srv_name>'"
	     " on the node '%s'!",fn,RTL::nodeName().c_str());
    }
    return S_FAIL;
  }
  // get fn information 
  if( ::stat(fn,&statBuf) == -1 )  {
    print("stat('%s'): %s!",fn,ERRSTR);
    return S_FAIL;
  }
  if(!S_ISFIFO(statBuf.st_mode))  {       // check if fn is a FIFO
    print("I-node: '%s' is not a FIFO!",fn);
    return S_FAIL;
  }
  // open error log
  if( (m_fifo=::open(fn,O_WRONLY|O_NONBLOCK|O_APPEND)) == -1 )  {
    if( errno == ENXIO )    {
      print("open('%s'): No process has the FIFO "
             "'%s' open for reading!",fn,fn);
      print("Please, run 'logSrv -p %s -s <srv_name>"
	     "' on the node '%s'!",fn,RTL::nodeName().c_str());
    }
    else    {
      print("open('%s'): %s!",fn,ERRSTR);
    }
    return S_FAIL;
  }

  /* Now we are sure that another process has the FIFO open for reading.   */
  /* We unset now the O_NONBLOCK bit to have blocking write (no-drop       */
  /* behaviour).                                                           */
  int statusFlag = ::fcntl(m_fifo,F_GETFL);
  if(statusFlag<0)    {
    print("fcntl('%s',F_GETFL): %s!",fn,ERRSTR);
    return S_FAIL;
  }
  statusFlag &= ~O_NONBLOCK;                 // unset O_NONBLOCK bit
  if( ::fcntl(m_fifo,F_SETFL,statusFlag) == -1 )    {
    print("fcntl('%s',F_SETFL): %s!",fn,ERRSTR);
    return S_FAIL;
  }
  return S_OK;
}

int ErrorSrv::initialize(const string& part, const string& fac) {
  setFacility(part,fac);
  return report("INIT",f_Init,"Initializing error logger.");
}

int ErrorSrv::setFacility(const string& part, const string& fac) {
  m_partition = part;
  m_facility = fac;
  return S_OK;
}

/// Report error condition
int ErrorSrv::report(const string& failure, int flag, const string& msg)  {
  return fullReport(m_partition,m_facility,failure,flag,msg,"");
}

/// Report with user defined (hidden) information field
int ErrorSrv::reportUser(const string& failure, int flag, const string& msg, const string& user_text) {
  return fullReport(m_partition,m_facility,failure,flag,msg,user_text);
}

/// Full report call: all fields need to be specified.
int ErrorSrv::fullReport(const string& partition, const string& facility, 
			 const string& failure, int flag, 
			 const string& msg,const string& user)
{
  if ( m_fifo > 0 ) {
    size_t len;
    char buff[2048], drbuff[2048];
    struct timeval tp;
    ::gettimeofday(&tp,0);
    struct timespec delay={0,1000000};                            /* 0.001 s */
    DiskRecord* rec = (DiskRecord*)drbuff;

    ::memset(rec,0,sizeof(DiskRecord));
    rec->flag = flag;
    rec->time[0] = int(tp.tv_sec);
    rec->time[1] = int(tp.tv_usec);
    ::strncpy(rec->partition,partition.c_str(),sizeof(rec->partition)-1);
    ::strncpy(rec->node,RTL::nodeName().c_str(),sizeof(rec->node)-1);
    ::strncpy(rec->process,RTL::processName().c_str(),sizeof(rec->process)-1);
    ::strncpy(rec->facility,facility.c_str(),sizeof(rec->facility)-1);
    ::strncpy(rec->failure,failure.c_str(),sizeof(rec->failure)-1);
    rec->setMessage(msg);
    rec->setInfo(user);

    size_t l1 = ::snprintf(buff,sizeof(buff),"%s[ERROR]%s: %s: DRec:",
			   ::lib_rtl_timestr("%b%d-%H%M%S"),
			   RTL::nodeName().c_str(),
			   RTL::processName().c_str());

    int res = ::lib_rtl_encode64(rec,rec->sizeOf(),buff+l1,sizeof(buff)-l1,&len,99999);
    buff[len+l1-1]='\n';
    len = len+l1;
    /*
    size_t len = ::snprintf(buff,sizeof(buff),"%s[ERROR]%s: %s: %s: %s: {%d}#{%d.%d}#{%s}#{%s}#{%s}\n",
			    ::lib_rtl_timestr("%b%d-%H%M%S"),
			    RTL::nodeName().c_str(),
			    RTL::processName().c_str(),
			    partition.c_str(),
			    facility.c_str(),
			    flag,int(tp.tv_sec),int(tp.tv_usec),
			    failure.c_str(),
			    msg.c_str(),
			    user.c_str());
    buff[sizeof(buff)-1] = 0;
    */
    if ( res == 1 ) {
      for(int t=0;t<m_tryN;++t)    {
	int written = ::write(m_fifo,buff,len);
	if( written!=-1 || errno!=EAGAIN ) break;
	::nanosleep(&delay,NULL);
	delay.tv_sec  *= 2;
	delay.tv_nsec *= 2;
	if(delay.tv_nsec>999999999) { 
	  delay.tv_sec  += 1;
	  delay.tv_nsec -= 1000000000;
	}
      }
      return S_OK;
    }
  }
  return S_FAIL;
}

int ErrorSrv::print(char *format,...)  {
  char rawMsg[BUF_SZ]="";
  char msg[BUF_SZ]="";
  va_list ap;
  va_start(ap,format);
  vsnprintf(rawMsg,BUF_SZ,format,ap);        /* truncate to BUF_SZ if longer */
  va_end(ap);
  rawMsg[BUF_SZ-1]='\0';
  ::snprintf(msg,BUF_SZ,"%s[ERROR]%s: %s: ErrorSrv(): %s\n",
	     ::lib_rtl_timestr("%b%d-%H%M%S"),RTL::nodeName().c_str(),
	     RTL::processName().c_str(),rawMsg);
  msg[BUF_SZ-1]='\0';
  ::write(m_fifo,msg,strlen(msg));
  return S_OK;
}

namespace {
  #define checkRC(x)  _checkRC( x , #x )
  int _checkRC(int sc, const std::string& msg) {
    if ( sc == S_OK ) {
      cout << "OK:" << msg;
    }
    else {
      cout << "Call failed: return code=" << sc << ": " << msg;
    }
    cout << endl;
    return sc;
  }
}

extern "C" int testErrorSrv(int /* argc */, char** /* argv */) {
  ErrorSrv s;
  if ( checkRC((s.open("/tmp/logAlarm.fifo"))) == S_OK ) {
    if ( checkRC((s.initialize("TEST","TestTask"))) == S_OK ) {
      checkRC((s.report("CLEAR_ALARMS",b_Clear)));
      checkRC((s.reportUser("MISS_SOURCE",f_Error,"Tell1 tdetl1_3 did not send data.","tdetl1_3")));
      checkRC((s.reportUser("TASK_START", f_Info, "New task GaudiTask_1 is just starting....","GaudiTask_1")));
      checkRC((s.reportUser("TASK_CRASH", f_Alarm,"Task MEPRx_0 crashed.","MEPRx_0")));
    }
  }
  return 1;
}


#endif
