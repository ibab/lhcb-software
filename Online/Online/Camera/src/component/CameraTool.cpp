
// local
#include "CameraTool.h"
#include "client.h"

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"

// boost
#include "boost/lexical_cast.hpp"
#include <boost/regex.hpp>

// DIM
#include "dis.hxx"
#include "RTL/rtl.h"

#include <sstream>
#include <iostream>

using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : CameraTool
//
// 2007-05-30 : Claus Buszello
// 2010-02-26 : Nicola Mangiafave
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CameraTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CameraTool::CameraTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_camc(NULL)
  , m_DIMService(NULL)
  , m_out()
  , m_lastHistoNum ( 0 )
  , m_msgLev(ICameraTool::INFO)
  , m_who("")
  , m_what("")
  , m_PVSSmsgLev(ICameraTool::NONE)
  , m_PVSSwho("")
  , m_PVSSwhat("")
  , m_WarningPVSS(ICameraTool::WARNING)
  , m_ErrorPVSS(ICameraTool::ERROR)
  , m_skipCameraToPVSSFlag(false)
  , m_message("")
  , m_HistoToEntries(2500)
                      // A message with 2 histograms fills 639 KB.
  , m_MaxTextEntries(500)
                      // m_MaxTextEntries would be:
                      // archiving_space / ( ( n_archiving_days_in_sec *
                      // * average_n_messages * n_bytes_per_entry ) / m_messagePeriod)
                      // archiving_space = 30 Gb, average_n_messages = 5,
                      // where n_bytes_per_entry = 130 and m_messagePeriod = 5
                      // The actual result is 890, arbitrarely rescaled to 500.
  , m_MaxNMessagesPerAlg(10000)
                      // The maximum size for a map is way bigger (18446744073709551615)
{
  declareInterface<ICameraTool>(this);
  declareProperty("ServerName",m_servername="127.0.0.1");
  declareProperty("ServerPort",m_servport=45123);
  declareProperty("ServerNames",m_servernames);
  declareProperty("ServerPorts",m_servports);
  declareProperty("Enabled",m_dosend=true);
  declareProperty("SendMessagesToPVSS", m_SendMessagesToPVSS=false);
  declareProperty("Warning_PVSS",m_WARNING_PVSS = "WARNING");
  declareProperty("Error_PVSS",m_ERROR_PVSS = "ERROR");
  declareProperty("MessagePeriod",m_messagePeriod = 5);
  declareProperty("CheckMessageRate",m_MessageRateCheckFlag = true);
  numErrBZ = 0;
  numErrCN = 0;
}

//=============================================================================
// Destructor
//=============================================================================
CameraTool::~CameraTool() {}

//=============================================================================
StatusCode CameraTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Reinitialize the variables m_WarningPVSS and m_ErrorPVSS,
  // we can't do it directly in declareProperty since they are
  // of type MessageLevel.
  if(m_WARNING_PVSS == "INFO")m_WarningPVSS = ICameraTool::INFO;
  else if(m_WARNING_PVSS == "WARNING")m_WarningPVSS = ICameraTool::WARNING;
  else if(m_WARNING_PVSS == "ERROR")m_WarningPVSS = ICameraTool::ERROR;
  else debug()<<" Invalid value for WARNING_PVSS"
              <<", it is: "<<m_WARNING_PVSS
              <<", should be either INFO or WARNING or ERROR."
              <<" The default value ("<< m_WarningPVSS <<") will be used."
              <<endmsg;
  if(m_ERROR_PVSS == "INFO")m_ErrorPVSS = ICameraTool::INFO;
  else if(m_ERROR_PVSS == "WARNING")m_ErrorPVSS = ICameraTool::WARNING;
  else if(m_ERROR_PVSS == "ERROR")m_ErrorPVSS = ICameraTool::ERROR;
  else debug()<<" Invalid value for ERROR_PVSS"
              <<", it is: "<<m_ERROR_PVSS
              <<", should be either INFO or WARNING or ERROR."
              <<" The default value (" << m_ErrorPVSS << ") will be used."
              <<endmsg;

  if (m_dosend) 
  {
    if (m_servernames.size() > 0){
      unsigned int num = m_servernames.size();
      if (m_servports.size()<num) num = m_servports.size();

      for (unsigned int i = 0;i<num;++i){
        client * c = new client(m_servernames[i].c_str(), m_servports[i]);
        if (c->m_healthy==0) {
          Warning( "Failed to setup CAMERA client for server " + m_servernames[i] ).ignore();
          delete c;
          c = NULL;
        }
        else {
          m_clients.push_back(c);
        }
      }

    }
    else
    {
      m_camc = new client(m_servername.c_str(),m_servport);
      if (m_camc->m_healthy==0) 
      {
        Warning( "Failed to setup CAMERA client for server " + m_servername ).ignore();
        delete m_camc;
        m_camc = NULL;

      }

      if ( msgLevel(MSG::INFO) )
        info() << "Made CAMERA client to "<< m_servername << ":" << m_servport << endmsg;

      m_clients.push_back(m_camc);

    }
  }// if(m_dosend)
  else if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Camera set to NOT SEND." << endreq;
  }

  //
  // private DIM Services
  //

  if ( getenv("DIM_DNS_NODE") )
  {
    const std::string& utgid   = RTL::processName();
    std::string  utgidNew      = utgid;
    // now format the UTGID such that the name of the monitoring node is
    // replaced by x (such that the name published to DIM is always the
    // same and can be "guessed" without querying the DNS server)
    // Only if there are 3 "_" in the UTGID to be consistent with the MonitorSvc

    std::size_t pos1 = std::string::npos; // position of first underscore
    std::size_t pos2 = std::string::npos;
    std::size_t pos3 = std::string::npos;
    std::size_t pos4 = std::string::npos; // should stay at npos
    pos1 = utgidNew.find("_");
    if (pos1 != std::string::npos) {
      pos2 = utgidNew.find("_", pos1+1);
      if (pos2 != std::string::npos) {
        pos3 = utgidNew.find("_", pos2+1);
        if (pos3 != std::string::npos) {
          pos4 = utgidNew.find("_", pos3+1);
        } // if pos3
      } // if pos2
    } // if pos1
    if (pos1 != std::string::npos   &&
        pos2 != std::string::npos   &&
        pos3 != std::string::npos   &&
        pos4 == std::string::npos )
    {
      utgidNew = utgidNew.replace(pos1+1, pos2-pos1-1, "x");
    }//if

    const std::string serviceName = utgidNew + "/RICH_MONITORING/MESSAGES";
    info() << "Publish to DIM " << serviceName << endmsg;
    const std::string additionalString("");
    m_DIMService = new DimService(serviceName.c_str(),(char*)additionalString.c_str());
  }
  else if ( msgLevel(MSG::INFO) )
  {
    info()<<"DIM_DNS_NODE undefined"
          << "- No Message will be sent from CameraTool to PVSS"
          <<endmsg;
  }
  // END of private DIM Services creation

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Setup of CameraTool is done"<<endreq;

  // Send a message myself, announcing I am alive and well
  //this->SendAndClearTS( ICameraTool::INFO, name(), "CameraTool is ACTIVE" );

  return sc;
}

//=============================================================================
StatusCode CameraTool::finalize()
{
  delete m_DIMService;
  m_DIMService = NULL;
  delete m_camc;
  m_camc = NULL;
  return GaudiTool::finalize();
}

//=============================================================================

std::string CameraTool::NumToTextMessage(const MessageLevel l)
{
  std::string txtMessage = "NONE";
  switch (l) {
  case ICameraTool::NONE:
    txtMessage = "NONE";
    break;
  case ICameraTool::INFO:
    txtMessage = "INFO";
    break;
  case ICameraTool::WARNING:
    txtMessage = "WARNING";
    break;
  case ICameraTool::ERROR:
    txtMessage = "ERROR";
    break;
  case ICameraTool::ERROR_PVSS:
    txtMessage = "ERROR";
    break;
  case ICameraTool::WARNING_PVSS:
    txtMessage = "WARNING";
    break;
  case ICameraTool::CAM_COMMAND:
    txtMessage = "CAM_COMMAND";
    break;
    // Use the None case for default.
  default:
    txtMessage = "NONE";
    break;
  }// switch(1)
  return txtMessage;
}//operator<<(std::ostream &, ICameraTool::MessageLevel)

//=============================================================================

void CameraTool::ReplaceMessageParameters(const MessageLevel l, 
                                          const std::string& who, 
                                          const std::string& what)
{
  m_msgLev = l;
  m_who = who;
  m_what = what;
}

//=============================================================================

void CameraTool::ReplaceMessageParameters(const MessageLevel l, 
                                          const std::string& who, 
                                          const std::string& what,
                                          const MessageLevel PVSSl,
                                          const std::string& PVSSwho, 
                                          const std::string& PVSSwhat)
{
  m_msgLev = l;
  m_who = who;
  m_what = what;
  m_PVSSmsgLev = PVSSl;
  m_PVSSwho = PVSSwho;
  m_PVSSwhat = PVSSwhat;
}

//=============================================================================

void CameraTool::ReplacePVSSMessageParameters(const MessageLevel PVSSl, 
                                              const std::string& PVSSwho,
                                              const std::string& PVSSwhat)
{
  m_PVSSmsgLev = PVSSl;
  m_PVSSwho = PVSSwho;
  m_PVSSwhat = PVSSwhat;
}

//=============================================================================

int CameraTool::CameraToPVSSMessageLevel(const MessageLevel l)
{
  if(m_WarningPVSS == l) return ICameraTool::WARNING_PVSS;
  else if(m_ErrorPVSS == l) return ICameraTool::ERROR_PVSS;
  else return 0;
  return 1;
}

//=============================================================================

int CameraTool::CameraToPVSS(const MessageLevel l,
                             const std::string& who, 
                             const std::string& what,
                             const int messagePeriod)
{
  // Check if the message format is the standard Camera messages format:
  if( (l != ICameraTool::WARNING_PVSS) && (l != ICameraTool::ERROR_PVSS) )
  {
    ReplaceMessageParameters(l, who, what);
    // Check if messages must be sent also to PVSS
    if(!m_SendMessagesToPVSS)return 1;
    // Check correspondence between Camera message level and PVSS message level.
    else if(CameraToPVSSMessageLevel(l) != 0){
      ReplacePVSSMessageParameters(ICameraTool::MessageLevel(CameraToPVSSMessageLevel(l)), who, what);
      SendToPVSS(messagePeriod);
    }
  }
  // Check if a message is explicitly made to be sent to PVSS
  else if(l == ICameraTool::WARNING_PVSS)
  {
    ReplaceMessageParameters(ICameraTool::WARNING, who, what, l, who, what);
    SendToPVSS(messagePeriod);
  }
  else if(l == ICameraTool::ERROR_PVSS)
  {
    ReplaceMessageParameters(ICameraTool::ERROR, who, what, l, who, what);
    SendToPVSS(messagePeriod);
  }
  return 1;
}

//=============================================================================

int CameraTool::SendToPVSS(int messagePeriod)
{
  // Message periods setting:
  if(m_messagePeriod < 1)
  {
    m_messagePeriod = 1; // Check to avoid unreasonable numbers for m_messagePeriod
    debug()<<"m_messagePeriod have been set to the unreasonable number: "
           <<m_messagePeriod <<", it will be set to 1."<<endmsg;
  }
  if(messagePeriod < 1)
  {
    if(messagePeriod < 0)debug()<<"messagePeriod have not been set, "
                                <<"or it has been set to the unreasonable number: "
                                <<messagePeriod <<", it will be set to: "<< m_messagePeriod
                                <<endmsg;
    messagePeriod = m_messagePeriod;
  }
  if(m_MessageRateCheckFlag && (m_PVSSmsgLev != ICameraTool::CAM_COMMAND))
  {
    if(!MessageRateCheck(m_PVSSmsgLev, m_PVSSwho, m_PVSSwhat,(messagePeriod - 1),true) )return 0;
  }
  // The service content format is:
  // <messge_level>/<time>/<message>
  // where <message> is:
  // m_PVSSwho: m_PVSSwhat
  // (it could also have been only m_PVSSwhat)
  time_t t = time(NULL);
  std::ostringstream stream_message;
  stream_message << NumToTextMessage(m_PVSSmsgLev)
                 << "/" << t
                 << "/"<<m_PVSSwho <<": " << m_PVSSwhat;
  m_message = stream_message.str();
  if (m_DIMService)
  {
    m_DIMService->updateService((char*)m_message.c_str());
    debug()<<"The message sent to PVSS is: "<<m_message<<endmsg;
    // Use the following line only for tests porpouses.
    //declareInfo(m_PVSSwho+"/MESSAGES", "This is a message sent to PVSS",m_message);
  }
  return 1;
}

//=============================================================================

int CameraTool::SetCameraToPVSSConfig(bool sendMessagesToPVSS,
                                      MessageLevel warning_PVSS,
                                      MessageLevel error_PVSS)
{
  m_SendMessagesToPVSS = sendMessagesToPVSS;
  m_WarningPVSS = warning_PVSS;
  m_ErrorPVSS = error_PVSS;

  return 1;
}
//=============================================================================

std::string CameraTool::StripMessage(const std::string &what)
{
  const boost::regex rePattern("[\\d]*"); // Pattern to search and eliminate (all numbers).
  return boost::regex_replace(what, rePattern, std::string(""));
}

//=============================================================================

bool CameraTool::MessageRateCheck(const MessageLevel l, 
                                  const std::string& who, 
                                  const std::string& what,
                                  const int messagePeriod, 
                                  const bool IsPVSSMessageFlag)
{
  // The following string(s) include the algorithms for which camera must not perform any rate check:
  std::string SafeAlgo1 = "ToolSvc.RichUKL1Disable";
  if(who.find(SafeAlgo1) != std::string::npos)return true;
  time_t currentTime  = time(NULL);
  // Keys setting
  std::string key1;
  if(!IsPVSSMessageFlag)key1 = who + NumToTextMessage(l);
  else key1 = "PVSSMESSAGE" + who + NumToTextMessage(l);
  std::string key2 = StripMessage(what);
  if((int)m_MessageOwnerLevelMap[key1][key2].size() == 0){ // First time the message has been sent.
    // Check on map size:
    // Check if the size of the map is getting near the maximum allowed size = m_MaxNMessagesPerAlg.
    // If this is the case it means there are a lot of messages that are almost identical
    // except for a small change in the text.
    // The message period of all messages in m_MessageOwnerLevelMap[key1] will be m_messagePeriod.
    if( (int)m_MessageOwnerLevelMap[key1].size()  < m_MaxNMessagesPerAlg){
      m_MessageOwnerLevelMap[key1][key2].push_back((int)currentTime); // Time of arrival.
      m_MessageOwnerLevelMap[key1][key2].push_back(0); // Number of previous suppressed messages.
      // Save the time of the last message sent for the given key1:
      if(m_MessageOwnerLevelMap[key1]["LAST MESSAGE"].size() == 0)
        m_MessageOwnerLevelMap[key1]["LAST MESSAGE"].push_back((int)currentTime);
      return true;
    }
    else if( (currentTime - m_MessageOwnerLevelMap[key1]["LAST MESSAGE"][0] )
             > (m_messagePeriod - 1)){ // The message must be sent at the m_messagePeriod period.
      if(!IsPVSSMessageFlag){
        std::ostringstream attachMsg;
        attachMsg << "CAMERA INFORMATION: Some of the messages from "
                  << who
                  << " will be sent at a rate of 1 every "
                  << m_messagePeriod <<" seconds. "
                  << "That's because the algorithm is sending many similar "
                  << "messages with slightly different text.";
        Append("TEXT",attachMsg.str().c_str());
      }
      // Save the time of the last message sent for the given key1:
      m_MessageOwnerLevelMap[key1]["LAST MESSAGE"][0] = (int)currentTime;
      return true;
    }
    else 
    { // The message must not be sent.
      Clear();
      return false;
    }
  }
  else { // The message has been sent previously.
    int oldTime = m_MessageOwnerLevelMap[key1][key2][0];
    int oldMessages = m_MessageOwnerLevelMap[key1][key2][1];
    if((currentTime - oldTime) > messagePeriod){  // The message must be sent.
      if(oldMessages>0 && !IsPVSSMessageFlag){
        std::ostringstream attachMsg;
        attachMsg << "CAMERA INFORMATION: due to the high rate "
                  << oldMessages
                  << " messages of the same type have been suppressed "
                  << "in the last " << messagePeriod + 1 <<" seconds.";
        Append("TEXT",attachMsg.str().c_str());
      }
      m_MessageOwnerLevelMap[key1][key2][0] = (int)currentTime;
      m_MessageOwnerLevelMap[key1][key2][1] = 0;
      // Save the time of the last message sent for the given key1:
      m_MessageOwnerLevelMap[key1]["LAST MESSAGE"][0] = (int)currentTime;
      return true;
    }
    else { // The message must not be sent.
      m_MessageOwnerLevelMap[key1][key2][1]++ ;
      // Delete the containers and counters of the appended extrainfo,
      // i.e. delete the extrainfo.
      Clear();
      return false;
    }
  }
}

//=============================================================================
int CameraTool::SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                             MessageLevel p_l,const std::string& p_who,const std::string& p_what)
{
  return SendAndClear(c_l, c_who, c_what, p_l, p_who, p_what,0);
}

int CameraTool::SendAndClear(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                             MessageLevel p_l,const std::string& p_who,const std::string& p_what,
                             int messagePeriod)
{
  ReplaceMessageParameters(c_l, c_who, c_what, p_l, p_who, p_what);
  m_skipCameraToPVSSFlag = true;
  SendAndClear(c_l, c_who, c_what, messagePeriod);
  m_skipCameraToPVSSFlag = false;
  SendToPVSS(messagePeriod);
  return 1;
}

//=============================================================================
int CameraTool::SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                               MessageLevel p_l,const std::string& p_who,const std::string& p_what)
{
  return SendAndClearTS(c_l, c_who, c_what, p_l, p_who, p_what,0);
}


int CameraTool::SendAndClearTS(MessageLevel c_l,const std::string& c_who,const std::string& c_what,
                               MessageLevel p_l,const std::string& p_who,const std::string& p_what,
                               int messagePeriod)
{
  ReplaceMessageParameters(c_l, c_who, c_what, p_l, p_who, p_what);
  m_skipCameraToPVSSFlag = true;
  SendAndClearTS(c_l, c_who, c_what, messagePeriod);
  m_skipCameraToPVSSFlag = false;
  SendToPVSS(messagePeriod);
  return 1;
}

//=============================================================================
int CameraTool::SendAndClear(MessageLevel o_l,
                             const std::string& o_who,
                             const std::string& o_what)
{
  return SendAndClear(o_l, o_who, o_what, 0);
}


int CameraTool::SendAndClear(MessageLevel o_l,
                             const std::string& o_who,
                             const std::string& o_what,
                             int messagePeriod)
{
  // Message periods setting:
  if(m_messagePeriod < 1)
  {
    m_messagePeriod = 1; // Check to avoid unreasonable numbers for m_messagePeriod
    debug()<<"m_messagePeriod have been set to the unreasonable number: "
           <<m_messagePeriod <<", it we be set to 1."<<endmsg;
  }
  if(messagePeriod < 1)
  {
    if(messagePeriod < 0)debug()<<"messagePeriod have been set to the unreasonable number: "
                                <<messagePeriod <<", it we be set to: "<< m_messagePeriod
                                <<endmsg;
    messagePeriod = m_messagePeriod;
  }
  // Message period calculation based on number of entries:
  // SizewiseMessagePeriod = ( 5 seconds * number of entries ) / (max number of entries allowed in 5 seconds)
  int SizewiseMessagePeriod = (int) ( (( m_out.entries() + m_lastHistoNum * m_HistoToEntries)  * 5) / m_MaxTextEntries);
  if ( SizewiseMessagePeriod > messagePeriod )
  {
    std::ostringstream attachMsg;
    attachMsg << "CAMERA INFORMATION: the message period has been scaled from "
              << messagePeriod <<" to "<<SizewiseMessagePeriod
              << " seconds, since the message size is too large.";
    Append("TEXT",attachMsg.str().c_str());
    messagePeriod = SizewiseMessagePeriod;
  }
  // Message Rate checks:
  if(m_MessageRateCheckFlag && (o_l != ICameraTool::CAM_COMMAND)){
    if(!MessageRateCheck(o_l, o_who, o_what,(messagePeriod - 1) ) )return 0;
  }

  // The hold parameters where const, to change them without changing their type I renamed
  // them as o_... where o stands for old.
  if(!m_skipCameraToPVSSFlag){
    CameraToPVSS(o_l, o_who, o_what,messagePeriod);
  }
  m_skipCameraToPVSSFlag = false;
  MessageLevel l = m_msgLev;
  // Trick to avoid redefining the parameter as non constant
  const std::string who = m_who;
  const std::string what = m_what;
  std::stringstream ss;
  std::stringstream s;

  ss << who<<"/"<<l<<"/"<<what<<"\n";

  bool success = false;
  //Loop over m_camc[] entries
  //for () {
  std::vector<client *>::iterator itc;

  if (m_dosend) 
  {
    // loop over clients
    for (itc = m_clients.begin(); itc!= m_clients.end(); itc++){
      m_camc = (*itc);
      if (m_camc==NULL) continue;

      //put attached message into the stream s
      if(l != ICameraTool::CAM_COMMAND)if (m_out.entries()>0) m_out.tostream(s);
      if (m_camc->Connect()>0) 
      {
        char buf[3];
        // NM: Lines explanation:
        // NM: m_camc = client that interfaces with the CAMERA server and handles the sending of messages.
        // NM: int client::rd(char*,int);
        // NM: Bug Fix: try to read the soket only 10 times, then give up:
        int max_socket_readings = 0;
        while (  (m_camc->rd(buf,2)==-2) && (max_socket_readings < 11) ){max_socket_readings++;}
        buf[2] = 0;
        if (strncmp(buf,"GO",2) == 0 )
        {
          success = true;
          int ret = m_camc->wr(ss.str().c_str(),ss.str().length());
          if (ret != (int)ss.str().length() ) success  = false;
          if(l != ICameraTool::CAM_COMMAND)if (m_out.entries()>0)
            ret = m_camc->wr(s.str().c_str(),s.str().length());
          if(l != ICameraTool::CAM_COMMAND)if (ret != (int)s.str().length() ) success  = false;
        }
        else 
        {
          if (numErrBZ < 5)
          {
            warning() << "All threads of camserv are busy!  -> Aborting message '" 
                      << ss.str() << endmsg;
            ++numErrBZ;
          }
          if (numErrBZ==5)
          {
            warning() << "All threads of camserv are busy!  -> Aborting message '" << ss.str()<<endmsg;
            warning() << "Above message repeated "<<numErrBZ<<" times. Aborting further messaging of this type."<<endmsg;
            numErrBZ++;

          }
        }

        m_camc->new_sock();
        // if (success) break; // break from the for loop over possible servers.
        if (success) break;
      }
    }    // end the for loop

    if (!success) {
      if (numErrCN < 5){
        warning() << "Could not connect to any camera server! -> Aborting message '" << ss.str() << endmsg;
        numErrCN++;
      }
      else if (numErrCN==5) {
        warning() << "Could not connect to any camera server!  -> Aborting message '" << ss.str() <<endmsg;
        warning() << "Above message repeated 5 times. Aborting further messaging of this type. "
                  << "An issue with the PC hist01 is possible."<<endmsg;
        numErrCN++;
      }
      // No else, just silently not print the message, after the error.
    }
  }// if(m_dosend&&m_camc)


  else {
    if (msgLevel(MSG::DEBUG))
      debug() << "Sending to CAMERA disabled. Message " << ss.str() << endmsg;
  } // else(m_dosend&&m_camc)

  Clear();

  return 1;
}

//=============================================================================
int CameraTool::SendAndClearTS(MessageLevel l,
                               const std::string& who,
                               const std::string& what)
{
  return SendAndClearTS(l,who,what,0);
}

int CameraTool::SendAndClearTS(MessageLevel l,
                               const std::string& who,
                               const std::string& what,
                               int messagePeriod)
{
  m_out.add("TEXT","Time of report: ");
  const time_t t = time(NULL);
  m_out.add("CTIME",ctime(&t));
  return SendAndClear(l,who,what,messagePeriod);
}

void CameraTool::Clear()
{
  m_out.reset();
  m_lastHistoNum = 0;
}

int CameraTool::Append(const char * T,const char * C)
{
  if (m_dosend)
    m_out.add(T,C);
  return 1;
}

int CameraTool::Append(const char * T,void * C, int sz)
{
  if (m_dosend)
    m_out.add(T,C,sz);
  return 1;
}

int CameraTool::Append(const char * C)
{
  if (m_dosend)
    m_out.add("TEXT",C);
  return 1;
}

int CameraTool::Append(TH1 * H,const char *opts)
{
  TH1D * hd = dynamic_cast<TH1D*>(H);
  if ( hd ) return Append(hd,opts);
  return 0;
}

int CameraTool::Append(TH1D * H, const char * opts)
{
  if (m_dosend) 
  {
    if ( !H )             { Warning("Null TH1D pointer").ignore();  return 0; }
    if ( !H->GetXaxis() ) { Warning("TH1D has no X axis").ignore(); return 0; }

    const std::string s = ( opts != NULL ? (std::string)"."+opts : "" );
    const int nXBins = H->GetNbinsX();
    const int size = (5+2*(nXBins+2));
    float* data = new float[size];

    data[0] = 1.f;
    data[1]=float(nXBins);
    data[2]=float(H->GetXaxis()->GetBinLowEdge(1));
    data[3]=float(H->GetXaxis()->GetBinLowEdge(nXBins+1));
    data[4]=float(H->GetEntries());
    for (int i=5; i<2*(nXBins+2); i++) data[i]=0.0f;
    int iData = 5;
    data[iData++]= float(H->GetBinContent(0));
    for (int ixBin=1; ixBin<=nXBins; ++ixBin)
    {
      data[iData++]=  float(H->GetBinContent(ixBin));
    }
    data[iData++]= float(H->GetBinContent(nXBins+1));

    for (int ixBin=0; ixBin<=nXBins+1; ++ixBin)
    {
      data[iData++]= float(H->GetBinError(ixBin));
    }

    m_out.add("DIM1D"+s,data,size*sizeof(float));

    delete[] data;

    // String for histo ID
    const std::string& rootH = "H" + boost::lexical_cast<std::string>(m_lastHistoNum++);

    // Title
    const std::string& title = rootH+"->SetTitle(\"" + std::string(H->GetTitle()) + "\");";
    this->Append( "EVAL", title.c_str() );

    // X axis title
    if ( H->GetXaxis() )
    {
      const std::string& xtitle = ( rootH+"->GetXaxis()->SetTitle(\"" + 
                                    std::string(H->GetXaxis()->GetTitle()) + "\");" );
      this->Append( "EVAL", xtitle.c_str() );
    }

  }// if(m_dosend)
  return 1;
}

int CameraTool::Append( TH2D * H, const char * opts )
{
  if (m_dosend)
  {
    if ( !H )             { Warning("Null TH2D pointer").ignore();  return 0; }
    if ( !H->GetXaxis() ) { Warning("TH2D has no X axis").ignore(); return 0; }
    if ( !H->GetYaxis() ) { Warning("TH2D has no Y axis").ignore(); return 0; }

    const std::string s = ( opts != NULL ? (std::string)"."+opts : "" );
    const int nXBins = H->GetNbinsX();
    const int nYBins = H->GetNbinsY();
    const int size = ( 8 + 2*(nXBins+2)*(nYBins+2) );
    float * data = new float[size];

    data[0]=2.0f;
    data[1]=float(nXBins);
    data[2]=float(H->GetXaxis()->GetXmin());
    data[3]=float(H->GetXaxis()->GetXmax());
    data[4]=float(nYBins);
    data[5]=float(H->GetYaxis()->GetXmin());
    data[6]=float(H->GetYaxis()->GetXmax());
    data[7]=float(H->GetEntries());
    for ( int i=8; i<2*(nXBins+2)*(nYBins+2); ++i ) { data[i] = 0.0f; }

    int iData = 8;

    data[iData++] = float(H->GetBinContent(0,0));

    for ( int iyBin=1; iyBin<=nYBins; ++iyBin )
    {
      data[iData++] = float(H->GetBinContent(0,iyBin));
    }
    data[iData++] = float( H->GetBinContent(0,nYBins+1) );

    for ( int ixBin=1; ixBin<=nXBins; ++ixBin )
    {
      data[iData++] = float(H->GetBinContent(ixBin,0));
      for ( int iyBin=1; iyBin<=nYBins; ++iyBin )
      {
        data[iData++] = float(H->GetBinContent(ixBin,iyBin));
      }
      data[iData++] = float(H->GetBinContent(ixBin,nYBins+1));
    }

    data[iData++] = (float)H->GetBinContent(nXBins+1,0);
    for ( int iyBin=1; iyBin<=nYBins; ++iyBin ) 
    {
      data[iData++] = (float)H->GetBinContent(nXBins+1,iyBin);
    }
    data[iData++] = (float)H->GetBinContent(nXBins+1,nYBins+1);


    //===== ERRORS======


    data[iData++]= (float)H->GetBinError(0,0);

    for ( int iyBin=1; iyBin<=nYBins; ++iyBin )
    {
      data[iData++] = (float)H->GetBinError(0,iyBin);
    }
    data[iData++] = (float)H->GetBinError(0, nYBins+1);

    for ( int ixBin=1; ixBin<=nXBins; ++ixBin )
    {
      data[iData++]= (float)H->GetBinError(ixBin,0);
      for ( int iyBin=1; iyBin<=nYBins; ++iyBin )
      {
        data[iData++] = (float)H->GetBinError(ixBin,iyBin);
      }
      data[iData++] = (float)H->GetBinError(ixBin,nYBins+1);
    }

    data[iData++]= (float)H->GetBinError(nXBins+1,0);
    for ( int iyBin=1; iyBin<=nYBins; ++iyBin )
    {
      data[iData++] = (float)H->GetBinError(nXBins+1,iyBin);
    }
    data[iData++] = (float)H->GetBinError(nXBins+1,nYBins+1);

    m_out.add( "DIM2D"+s, (void *)data, size*sizeof(float) );

    delete[] data;

    // Histo ID
    const std::string& rootH = "H" + boost::lexical_cast<std::string>(m_lastHistoNum++);

    // Title
    const std::string& title = rootH+"->SetTitle(\"" + std::string(H->GetTitle()) + "\");";
    this->Append( "EVAL", title.c_str() );

    // X axis title
    if ( H->GetXaxis() )
    {
      const std::string& xtitle = ( rootH+"->GetXaxis()->SetTitle(\"" + 
                                    std::string(H->GetXaxis()->GetTitle()) + "\");" );
      this->Append( "EVAL", xtitle.c_str() );
    }

    // Y axis title
    if ( H->GetYaxis() )
    {
      const std::string& ytitle = ( rootH+"->GetYaxis()->SetTitle(\"" + 
                                    std::string(H->GetYaxis()->GetTitle()) + "\");" );
      this->Append( "EVAL", ytitle.c_str() );
    }

  }// if(m_dosend)
  return 1;
}

int CameraTool::Append( TF1 * F, const char * opts )
{
  if ( m_dosend ) 
  {
    if ( !F ) { Warning("Null TF1 pointer").ignore(); return 0; }
    std::string s = (std::string)".SAME,C";
    if ( opts ) { s += (std::string)","+opts; }
    return Append( F->GetHistogram(), s.c_str() );
  }
  return 1;
}

std::ostream& operator<<(std::ostream &os, ICameraTool::MessageLevel l) 
{
  switch (l) {
  case ICameraTool::NONE:
    os << 0;
    break;
  case ICameraTool::INFO:
    os << 1;
    break;
  case ICameraTool::WARNING:
    os << 2;
    break;
  case ICameraTool::ERROR:
    os << 3;
    break;
  case ICameraTool::ERROR_PVSS:
    os << 4;
    break;
  case ICameraTool::WARNING_PVSS:
    os << 5;
    break;
  case ICameraTool::CAM_COMMAND:
    os << 9;
    break;
    // Use the None case for default.
  default:
    os << 0;
    break;
  }// switch(1)
  return os;
}//operator<<(std::ostream &, ICameraTool::MessageLevel)
