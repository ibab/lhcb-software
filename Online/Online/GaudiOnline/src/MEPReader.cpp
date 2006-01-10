// $Id: MEPReader.cpp,v 1.2 2006-01-10 13:45:03 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventHelpers.h"
#include <io.h>
#include <fcntl.h>      /* Needed only for _O_RDWR definition */

using namespace LHCb;

/** @class MEPReader MEPReader.h tests/MEPReader.h
 *  Creates and fills dummy RawEvent  
 *
 *  @author Markus Frank
 *  @date   2005-10-13
 */
class MEPReader : public Algorithm {
public: 
  int m_file;
  int m_evt;
  /// Standard constructor
  MEPReader( const std::string& name, ISvcLocator* pSvcLocator )
  : Algorithm(name, pSvcLocator), m_evt(0)   {
    m_file = open("./mepBuffer.dat", _O_RDONLY|_O_BINARY);
  }

  /// Destructor
  virtual ~MEPReader()  {} 

  void read(void* p, size_t len)  {
    int sc = ::read(m_file,p,len);
    if ( sc != len )  {
      MsgStream log(msgSvc(),name());
      log << MSG::ERROR << "Read Error: End-Of-File !" << endmsg;
      throw std::runtime_error("Read Error: End-Of-File !");
    }
  }

  /// Main execution
  virtual StatusCode execute()  {
    MsgStream log(msgSvc(),name());
    unsigned int len, partitionID;
    try  {
      typedef std::map<unsigned int, RawEvent*> Events;
      Events events;
      read(&len,sizeof(len));
      char* buff = new char[len+4];
      read(buff+4,len);
      MEPEvent* me = (MEPEvent*)buff;
      me->setSize(len);
      decodeMEP(me, false, partitionID, events);
      for(Events::const_iterator i=events.begin();i!=events.end();++i)  {
        m_evt++;
        log << MSG::ALWAYS << (*i).first << ".";
        delete (*i).second;
      }
      delete buff;
      log << MSG::ALWAYS << "Read " << m_evt << " events." << endmsg;
      return StatusCode::SUCCESS;
    }
    catch(std::exception& e)  {
      log << MSG::ERROR << e.what() << endmsg;
    }
    return StatusCode::FAILURE;
  }
};

// Declaration of the Algorithm Factory
static const  AlgFactory<MEPReader>          s_factory ;
const        IAlgFactory& MEPReaderFactory = s_factory ; 
