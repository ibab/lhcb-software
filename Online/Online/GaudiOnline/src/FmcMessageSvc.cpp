#ifndef _WIN32
#include <iostream>
#include <cstring>
#include <string>
#include <syslog.h>                                              /* syslog() */
#include <cstdarg>                                         /* va_list, etc. */
#include <cerrno>
#include <sys/types.h>                                             /* open() */
#include <sys/stat.h>                                              /* open() */
#include <fcntl.h>                                                 /* open() */
#include <unistd.h>                                         /* gethostname() */
#include "GaudiKernel/SvcFactory.h"
#include "GaudiOnline/FmcMessageSvc.h"
#include "RTL/rtl.h"
#define BUF_SZ 4096
#define SYSLOG_BUF_SZ 990
#define NO_FIFO (-1)

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,FmcMessageSvc)

/// Constructor
LHCb::FmcMessageSvc::FmcMessageSvc(const std::string& name,ISvcLocator* svcloc)
 : OnlineMessageSvc(name,svcloc)
{
  const char* fifo = ::getenv("LOGFIFO");
  setErrorLogger(this);
  getPName();
  fifoFD      = NO_FIFO;
  dfltFifoFD  = NO_FIFO;
  declareProperty("fifoPath",m_fifoPath= fifo ? fifo : "/tmp/logSrv.fifo");
  declareProperty("noDrop",  m_noDrop=false);
  declareProperty("tryN",    m_tryN=2);
  m_fifoPath.declareUpdateHandler(&FmcMessageSvc::changeFifo, this);
}

/// Destructor.
LHCb::FmcMessageSvc::~FmcMessageSvc()   {
  setErrorLogger(0);
}

/// IInterface implementation : queryInterface
StatusCode LHCb::FmcMessageSvc::queryInterface(const InterfaceID& riid, void **ppIf)  {
  if( IErrorLogger::interfaceID().versionMatch(riid) )  {
    *ppIf=(IErrorLogger*)this;
    addRef();
    return StatusCode::SUCCESS;
  }
  return OnlineMessageSvc::queryInterface(riid,ppIf);
}

/// Initialize Service 
StatusCode LHCb::FmcMessageSvc::initialize()  {
  StatusCode sc=OnlineMessageSvc::initialize();
  if(sc.isFailure())return sc;
  return openFifo();
}

/// Start Service 
StatusCode LHCb::FmcMessageSvc::start()   {
  StatusCode sc = OnlineMessageSvc::start();
  if(sc.isFailure())return sc;
  if( fifoFD != NO_FIFO )  {
    close(fifoFD);
    fifoFD = NO_FIFO;
  }
  return openFifo();
}

/// Restart Service 
StatusCode LHCb::FmcMessageSvc::restart()  {
  StatusCode sc = setProperties();
  if ( sc.isSuccess() ) {
    if( fifoFD != NO_FIFO )  {
      close(fifoFD);
      fifoFD = NO_FIFO;
    }
    return openFifo();
  }
  return sc;
}

/*-------------------------------------------------------------------------*/
void LHCb::FmcMessageSvc::changeFifo(Property& ) {
  if(fifoFD != NO_FIFO)  {
    close(fifoFD);
    fifoFD = NO_FIFO;
  }
  openFifo();
}

/*-------------------------------------------------------------------------*/
StatusCode LHCb::FmcMessageSvc::openFifo() {
  const char *dfltFifoPath="/tmp/logSrv.fifo";
  int errU=0;
  struct stat statBuf;
  enum outType{L_DIM=0x1,L_STD=0x2,L_SYS=0x4};
  std::string fifo_name = m_fifoPath;
  /*-------------------------------------------------------------------------*/
  droppedN=0;
  /*-------------------------------------------------------------------------*/
  if ( fifo_name.length()>0 && fifo_name[0]=='$' )  {
    std::string fn = fifo_name.substr(1);
    if ( ::getenv(fn.c_str()) ) {
      fifo_name = ::getenv(fn.c_str());
    }
  }

  /*-------------------------------------------------------------------------*/
  errU=L_STD|L_SYS;
  /* If the logger is a secondary logger, try to open also the default       */
  /* logger to send it possible errors in opening the secondary logger.      */
  if( ::strcmp(fifo_name.c_str(),dfltFifoPath))  {
    // check if dfltFifoPath is writable 
    if( ::access(dfltFifoPath,W_OK)!=-1) {   /* write access to dfltFifoPath OK */
      // get dfltFifoPath information
      if( ::stat(dfltFifoPath,&statBuf)!=-1) {  /* dfltFifoPath information got */
        // check if dfltFifoPath is a FIFO
        if( S_ISFIFO(statBuf.st_mode))     {        /* dfltFifoPath is a FIFO */
          // open dfltFifoPath
          dfltFifoFD = ::open(dfltFifoPath,O_RDWR|O_NONBLOCK|O_APPEND);
          if(dfltFifoFD!=NO_FIFO)      {         /* dfltFifoPath open() succeeded */
            errU|=L_DIM;
          }
        }
      }
    }
  }
  /*-------------------------------------------------------------------------*/
  /* check if m_fifoPath is writable */
  if( ::access(fifo_name.c_str(),W_OK)==-1)    {              /* access denied */
    printM(errU,MSG::FATAL,__func__,"access(\"%s\"): %s!",fifo_name.c_str(),
           strerror(errno));
    if( errno==ENOENT )    {
      bool dflt = 0 == strcmp(fifo_name.c_str(),dfltFifoPath);
      printM(errU,MSG::FATAL,__func__,"Please, run \"logSrv\" %s -p %s -s %s on the node \"%s\"!",
	     dflt ? "(without" : "",
	     dflt ? "or"       : fifo_name.c_str(),
	     dflt ? "options)" : "<srv_name>",
	     RTL::nodeNameShort().c_str());
    }
    return StatusCode::FAILURE;
  }
  /* get fifo_name information */
  if(stat(fifo_name.c_str(),&statBuf)==-1)  {
    printM(errU,MSG::ERROR,__func__,"stat(\"%s\"): %s!",fifo_name.c_str(),
           strerror(errno));
    return StatusCode::FAILURE;
  }
  /* check if fifo_name is a FIFO */
  if(!S_ISFIFO(statBuf.st_mode))  {
    printM(errU,MSG::ERROR,__func__,"I-node: \"%s\" is not a FIFO!",
           fifo_name.c_str());
    return StatusCode::FAILURE;
  }
  /* open error log */
  if ( m_noDrop ) fifoFD = ::open(fifo_name.c_str(),O_WRONLY|O_NONBLOCK|O_APPEND);
  else            fifoFD = ::open(fifo_name.c_str(),O_RDWR|O_NONBLOCK|O_APPEND);
  if(fifoFD==NO_FIFO)  {
    if(errno==ENXIO)    {
      bool dflt = 0 == strcmp(fifo_name.c_str(),dfltFifoPath);
      printM(errU,MSG::FATAL,__func__,"open(\"%s\"): No process has the FIFO "
             "\"%s\" open for reading!",fifo_name.c_str(),fifo_name.c_str());
      printM(errU,MSG::FATAL,__func__,"Please, run \"logSrv\" %s -p %s -s %s on the node \"%s\"!",
	     dflt ? "(without" : "",
	     dflt ? "or"       : fifo_name.c_str(),
	     dflt ? "options)" : "<srv_name>",
	     RTL::nodeNameShort().c_str());
    }
    else    {
      printM(errU,MSG::FATAL,__func__,"open(\"%s\"): %s!",fifo_name.c_str(),::strerror(errno));
    }
    return StatusCode::FAILURE;
  }
  /*-------------------------------------------------------------------------*/
  if(m_noDrop)  {
    /* Now we are sure that another process has the FIFO open for reading.   */
    /* We unset now the O_NONBLOCK bit to have blocking write (no-drop       */
    /* behaviour).                                                           */
    int statusFlag=0;
    statusFlag=fcntl(fifoFD,F_GETFL);
    if(statusFlag<0)
    {
      printM(errU,MSG::FATAL,__func__,"fcntl(\"%s\",F_GETFL): %s!",
             fifo_name.c_str(),strerror(errno));
      return StatusCode::FAILURE;
    }
    statusFlag&=~O_NONBLOCK;                         /* unset O_NONBLOCK bit */
    if(fcntl(fifoFD,F_SETFL,statusFlag)==-1)    {
      printM(errU,MSG::FATAL,__func__,"fcntl(\"%s\",F_SETFL): %s!",
             fifo_name.c_str(),strerror(errno));
      return StatusCode::FAILURE;
    }
  }
  /*-------------------------------------------------------------------------*/
  /* If the logger is a secondary logger, close the default logger, since we */
  /* succeeded in opening the secondary logger and we have no more need of   */
  /* the default logger.                                                     */
  if(strcmp(fifo_name.c_str(),dfltFifoPath))  {
    if(dfltFifoFD!=NO_FIFO)    {
      close(dfltFifoFD);
      dfltFifoFD=NO_FIFO;
    }
  }
  return StatusCode::SUCCESS;
}

/// Finalize Service
StatusCode LHCb::FmcMessageSvc::finalize()  {
  if(fifoFD!=NO_FIFO)  {
    close(fifoFD);
    fifoFD=NO_FIFO;
  }
  return OnlineMessageSvc::finalize();
}

/*****************************************************************************/
/// Error logger implementation: report message
// typ = severity level
// src = Message source (typically the alg/svc name)
// msg = the message string
void LHCb::FmcMessageSvc::report(int typ,const std::string& src,const std::string& msg)
{
  const char *sl[8]={"[NIL]  ","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
               "[FATAL]", "[ALWAYS]"};
  time_t now;
  struct tm lNow;
  char sNow[32];                                           /* Oct12-134923\0 */
  char header[BUF_SZ/2];/* Oct12-134923[DEBUG]pcdom: psSrv(psSrv_0): main(): */
  size_t msgLen=0;
  size_t bufAvailLen=0;
  std::string logMsg;

  /*-------------------------------------------------------------------------*/
  /* time string */
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,sizeof(sNow),"%b%d-%H%M%S",&lNow);
  sNow[sizeof(sNow)-1] = 0;
  /*-------------------------------------------------------------------------*/
  /* compose message header */
  typ = (typ>int(sizeof(sl)/sizeof(sl[0]))) ? (sizeof(sl)/sizeof(sl[0]))-1 : (typ<0 ? 0 : typ);
  snprintf(header,BUF_SZ/2,"%s%s%s: %s(%s): %s: ",sNow,sl[typ],RTL::nodeNameShort().c_str(),
           getPName(),RTL::processName().c_str(),src.c_str());
  /* NULL-terminate header if truncated */
  if(!memchr(header,0,BUF_SZ/2))header[BUF_SZ/2-1]='\0';
  /*-------------------------------------------------------------------------*/
  msgLen=msg.size();
  bufAvailLen=BUF_SZ-2-strlen(header);                           /* -2: \n\0 */
  /*-------------------------------------------------------------------------*/
  for(int i=0;i*bufAvailLen<msgLen;i++)  {
    int tryC;
    struct timespec delay={0,1000000};                            /* 0.001 s */
    logMsg=header+msg.substr(i*bufAvailLen,bufAvailLen)+"\n";
    for(tryC=0;tryC<m_tryN;tryC++)    {
      int written=write(fifoFD,logMsg.c_str(),logMsg.size());
      if(written!=-1||errno!=EAGAIN)break;
      nanosleep(&delay,NULL);
      delay.tv_sec*=2;
      delay.tv_nsec*=2;
      if(delay.tv_nsec>999999999){delay.tv_sec+=1;delay.tv_nsec-=1000000000;}
    }
    if(tryC==m_tryN)    {
      dropped=true;
      droppedN++;
    }
    else dropped=false;
  }
  /*-------------------------------------------------------------------------*/
  return;
}

/*****************************************************************************/
/// Dispatch a message to the relevant streams.
void LHCb::FmcMessageSvc::i_reportMessageEx(const Message& msg,int typ)  {
  int typM = msg.getType();
  if (  typM < m_outputLevel || typ < m_outputLevel ) return;
  if ( (typM==MSG::ALWAYS || typ==MSG::ALWAYS) && !m_printAlways) return;
  report(typM,msg.getSource(),msg.getMessage());
}

/// Implementation of IMessageSvc::ouputLevel()
int LHCb::FmcMessageSvc::outputLevel()   const {
  return m_outputLevel;
}

/// Implementation of IMessageSvc::ouputLevel(source)
int LHCb::FmcMessageSvc::outputLevel(const std::string& source) const {
  ThresholdMap::const_iterator it = m_thresholdMap.find(source);
  int lvl = (it == m_thresholdMap.end()) ? int(m_outputLevel) : (*it).second;
  if ( lvl > m_outputLevel ) return lvl;
  return m_outputLevel;
}

/*****************************************************************************/
int LHCb::FmcMessageSvc::getDroppedN()  {
  return droppedN;
}

/*****************************************************************************/
bool LHCb::FmcMessageSvc::isDropped()   {
  return dropped;
}

/*****************************************************************************/
/* get the process name (the executable image file name, i.e. argv[0])       */
/*****************************************************************************/
const char* LHCb::FmcMessageSvc::getPName()   {
  static char* pname = 0;
  if ( pname ) {
    return pname;
  }
  else {
    char pathName[1024]="";
    int len=0;
    char buf[BUF_SZ]="";
    /*-------------------------------------------------------------------------*/
    len=readlink("/proc/self/exe",pathName,sizeof(pathName));
    if(len==-1)  {
      ::snprintf(buf,BUF_SZ,"[ERROR] %s: readlink(): %s.",__func__,strerror(errno));
      syslog(LOG_ERR|LOG_DAEMON,"%s",buf);
      fprintf(stderr,"%s\n",buf);
      exit(1);
    }
    pathName[len]='\0';
    pname = strdup(basename(pathName));
  }
  return pname;
}
/*****************************************************************************/
/* Substitute of fprintf() to print either to STDERR and SYSLOG.             */
/* Time, severity, host name, process name and function name prepended       */
/*****************************************************************************/
int LHCb::FmcMessageSvc::printM(int out,int severity,const char* fName,const char *format,...)
{
  enum outType{L_DIM=0x1,L_STD=0x2,L_SYS=0x4};
  const char *sl[8]={"[NIL]  ","[VERB] ","[DEBUG]","[INFO] ","[WARN] ","[ERROR]",
		     "[FATAL]", "[ALWAYS]"};
  time_t now;
  struct tm lNow;
  char rawMsg[BUF_SZ]="";
  char msg[BUF_SZ]="";
  char sNow[32];                                           /* Oct12-134923\0 */
  va_list ap;

  /*-------------------------------------------------------------------------*/
  va_start(ap,format);
  vsnprintf(rawMsg,BUF_SZ,format,ap);        /* truncate to BUF_SZ if longer */
  va_end(ap);
  /*-------------------------------------------------------------------------*/
  /* NULL-terminate if truncated */
  if(!memchr(rawMsg,0,BUF_SZ))rawMsg[BUF_SZ-1]='\0';
  /*-------------------------------------------------------------------------*/
  /* time string */
  now=time(NULL);
  localtime_r(&now,&lNow);
  strftime(sNow,sizeof(sNow),"%b%d-%H%M%S",&lNow);
  sNow[sizeof(sNow)-1] = 0;
  /*-------------------------------------------------------------------------*/
  /* compose message string with header */
  severity = (severity>int(sizeof(sl)/sizeof(sl[0]))) 
    ? (sizeof(sl)/sizeof(sl[0]))-1 : (severity<0 ? 0 : severity);
  snprintf(msg,BUF_SZ,"%s%s%s: %s(%s): %s(): %s\n",sNow,sl[severity],RTL::nodeNameShort().c_str(),
           getPName(),RTL::processName().c_str(),fName,rawMsg);
  /*-------------------------------------------------------------------------*/
  msg[BUF_SZ-1]='\0';
  /* add newline if missed */
  if(!strchr(msg,'\n'))  {
    int msgLen=strlen(msg);
    if(msgLen<BUF_SZ-1)strcat(msg,"\n");
    else msg[msgLen-1]='\n';
  }
  /*-------------------------------------------------------------------------*/
  if(out&L_DIM)dimLoggerMsgSend(msg,dfltFifoFD);
  if(out&L_STD)stdErrMsgSend(msg);
  if(out&L_SYS)  {
    /* compose message string with short header */
    snprintf(msg,BUF_SZ,"(%s): %s%s(): %s",RTL::processName().c_str(),sl[severity],fName,rawMsg);
    /* add string terminator if missed */
    if(!memchr(msg,0,BUF_SZ))msg[BUF_SZ-1]='\0';
    sysLogMsgSend(msg,severity);
  }
  return 0;
}
/*****************************************************************************/
/* send a message to the DIM logger */
int LHCb::FmcMessageSvc::dimLoggerMsgSend(char *buf,int fifoFD)    {
  if(fifoFD==NO_FIFO)return -1;
  return write(fifoFD,buf,strlen(buf));
}

/*****************************************************************************/
/* send a message to the stderr */
int LHCb::FmcMessageSvc::stdErrMsgSend(char *buf)    {
  return write(STDERR_FILENO,buf,strlen(buf));
}

/*****************************************************************************/
/* send a message to the syslog */
/* max syslog message length = SYSLOG_BUF_SZ */
int LHCb::FmcMessageSvc::sysLogMsgSend(char *buf,int severity)    {
  if ( ::strlen(buf)<SYSLOG_BUF_SZ )  {
    if     (severity==MSG::DEBUG   ) syslog(LOG_DEBUG|LOG_DAEMON,  "%s",buf);
    else if(severity==MSG::INFO    ) syslog(LOG_INFO|LOG_DAEMON,   "%s",buf);
    else if(severity==MSG::WARNING ) syslog(LOG_WARNING|LOG_DAEMON,"%s",buf);
    else if(severity==MSG::ERROR   ) syslog(LOG_ERR|LOG_DAEMON,    "%s",buf);
    else if(severity==MSG::FATAL   ) syslog(LOG_CRIT|LOG_DAEMON,   "%s",buf);
    else                             syslog(LOG_INFO|LOG_DAEMON,   "%s",buf);
  }
  else    {                /* message is longer than SYSLOG_BUF_SZ. split it */
    char pBuf[SYSLOG_BUF_SZ]="";
    char *p=NULL;
    for(p=buf;p-buf<(int)strlen(buf);p+=(SYSLOG_BUF_SZ-1))    {
      ::memset(pBuf,0,SYSLOG_BUF_SZ);
      ::strncpy(pBuf,p,SYSLOG_BUF_SZ-1);
      buf[SYSLOG_BUF_SZ-1]='\0';
      if     (severity==MSG::DEBUG   ) syslog(LOG_DEBUG|LOG_DAEMON,  "%s",pBuf);
      else if(severity==MSG::INFO    ) syslog(LOG_INFO|LOG_DAEMON,   "%s",pBuf);
      else if(severity==MSG::WARNING ) syslog(LOG_WARNING|LOG_DAEMON,"%s",pBuf);
      else if(severity==MSG::ERROR   ) syslog(LOG_ERR|LOG_DAEMON,    "%s",pBuf);
      else if(severity==MSG::FATAL   ) syslog(LOG_CRIT|LOG_DAEMON,   "%s",pBuf);
      else                             syslog(LOG_INFO|LOG_DAEMON,   "%s",pBuf);
    }
  }
  return 0;
}
/*****************************************************************************/
#endif
