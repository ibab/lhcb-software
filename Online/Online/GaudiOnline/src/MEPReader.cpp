// $Id: MEPReader.cpp,v 1.1 2005-12-20 16:38:18 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 

#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "MDF/MEPEvent.h"
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

  /// Main execution
  virtual StatusCode execute()  {
    MsgStream log(msgSvc(),name());

    int len;
    int sc = read(m_file,&len,sizeof(len));
    if ( sc != sizeof(int) )  {
      log << MSG::ERROR << "End-Of-File !" << endmsg;
      return StatusCode::FAILURE;
    }
    char* buff = new char[len+4];
    sc = read(m_file,buff+4,len);
    if ( sc != len )  {
      log << MSG::ERROR << "End-Of-File !" << endmsg;
      return StatusCode::FAILURE;
    }
    *(int*)buff = len;
    std::map<int, RawEvent*> events;
    MEPEvent* me = (MEPEvent*)buff;
    int evID, eid_h = 0, eid_l = 0;
    for (MEPMultiFragment* mf = me->first(); mf<me->last(); mf=me->next(mf)) {
      eid_h = mf->eventID();
      for (MEPFragment* f = mf->first(); f<mf->last(); f=mf->next(f)) {
        eid_l = f->eventID();
        evID = eid_h + eid_l;
        RawEvent* evt = events[evID];
        if ( 0 == evt ) events[evID] = evt = new RawEvent();
        const RawBank* last = f->last();
        for (RawBank* b = f->first(); b<f->last(); b=f->next(b)) {
          if ( b->magic() != RawBank::MagicPattern )  {
            log << MSG::ERROR << "Bad magic pattern!" << endmsg;
          }
          evt->adoptBank(b, false);
        }
      }
    }
    for(std::map<int, RawEvent*>::const_iterator i=events.begin();i!=events.end();++i)  {
      m_evt++;
      log << MSG::ALWAYS << (*i).first << "..";
    }
    log << MSG::ALWAYS << "Read successfully " << m_evt << " events." << endmsg;
    for(std::map<int, RawEvent*>::const_iterator i=events.begin();i!=events.end();++i)
      delete (*i).second;
    delete buff;
    return StatusCode::SUCCESS;
  }
};

// Declaration of the Algorithm Factory
static const  AlgFactory<MEPReader>          s_factory ;
const        IAlgFactory& MEPReaderFactory = s_factory ; 
