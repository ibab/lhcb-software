// $Id: MDFWriterLite.cpp,v 1.2 2010-05-19 06:26:34 kakiba Exp $
//  ====================================================================
//  MDFWriterLite.cpp
//  --------------------------------------------------------------------
//
//  Author    : Sai Suman & Niko Neufeld 
//
//  ====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiUtils/IIODataManager.h"
#include "MDF/RawDataConnection.h"
#include "MDF/MDFWriterLite.h"

#include <ctime>
#include <clocale>

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFWriterLite)

using namespace std;
using namespace LHCb;
using namespace Gaudi;

/// Standard algorithm constructor
MDFWriterLite::MDFWriterLite(const string& nam, ISvcLocator* pSvc)
: MDFWriter(nam, pSvc)
{
  declareProperty("MaxFileSizeKB",  m_maxFileSizeKB=2000000);
  declareProperty("MaxFileEvents",  m_maxFileEvents=1000000);
}

MDFWriterLite::~MDFWriterLite()   {}

/// Transform file name in presence of catalogs
std::string MDFWriterLite::getConnection(const std::string& org_conn)  {
  char dateStr[40];
  ::setlocale(LC_ALL, "");
  time_t ctime = ::time(NULL);
  struct tm *currtime = ::localtime(&ctime);
  ::strftime (dateStr,sizeof(dateStr), "%EY.%0m.%0d-%0H.%0M.%0S.mdf",currtime);
  string con = org_conn+dateStr;
  MsgStream log(msgSvc(), name());
  log << MSG::ALWAYS << "New file name:" << con << endmsg;
  return con;
}

/// Execute procedure
StatusCode MDFWriterLite::execute()    {
  if (((m_bytesWritten>>10) > m_maxFileSizeKB) || (m_eventsWritten >= m_maxFileEvents)) { 
    MsgStream log(msgSvc(), name());
    string con = getConnection(m_connectParams);
    m_ioMgr->disconnect(m_connection).ignore();
    //delete m_connection;
    m_connection = new RawDataConnection(this,con);
    StatusCode status = m_ioMgr->connectWrite(m_connection,IDataConnection::RECREATE,"MDF");
    status.ignore();
    if ( m_connection->isConnected() ) {
      log << MSG::INFO << "Received event request connection." << endmsg;
    }
    else {
      log << MSG::ERROR << "FAILED receiving event request connection." << endmsg;
      return StatusCode::FAILURE;
    }
    m_bytesWritten = m_eventsWritten = 0;
  }
  return MDFWriter::execute();
}
