// $Id: FileIDManipulator.cpp,v 1.3 2008-12-03 09:38:27 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : Fileidmanipulator
//
// 2008-03-03 : Albert Puig Navarro (albert.puig@cern.ch)
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#ifndef GAUDIONLINE_FILEIDMANIPULATOR_H
#define GAUDIONLINE_FILEIDMANIPULATOR_H

// Include files from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiOnline/FileIdInfo.h"

namespace LHCb {

  // Forward declarations
  class RawBank;

  /** @class FileIDManipulator FileIDManipulator.h
  *  
  *
  *  @author Albert Puig Navarro (albert.puig@cern.ch)
  *  @author M.Frank
  *  @date   2008-03-03
  */
  class FileIDManipulator : public Algorithm {
  public:
    enum Action { ADD=1, REMOVE, DUMP };
  protected:
    /// Property: remove/add file id bank
    int          m_action;
    /// Property: Location of the raw event in the transient datastore
    std::string  m_rawLocation;
    /// Property: input data type
    int          m_type;
  protected:
    /// Add fileID bank
    StatusCode add();
    /// Remove fileID bank
    StatusCode remove();
    /// Print FID bank content
    StatusCode print();
    /// Find the FID bank in the raw event structure
    std::pair<RawBank*,void*> getFIDbank();

  public: 
    /// Standard constructor
    FileIDManipulator(const std::string& nam, ISvcLocator* pSvc);
    /// Standard destructor
    virtual ~FileIDManipulator()  {}
    /// Algorithm execution
    virtual StatusCode execute();
    /// Issue error message
    StatusCode error(const std::string& msg);
  };
}
#endif // GAUDIONLINE_FILEIDMANIPULATOR_H

#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/MsgStream.h" 
#include "MDF/RawEventPrintout.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"
#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include <iomanip>

using namespace std;
using namespace LHCb;
using namespace Gaudi;

#include "GaudiKernel/AlgFactory.h" 
using namespace LHCb;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(FileIDManipulator);

#define DAQ_FILEID_BANK 32

/// Standard constructor
FileIDManipulator::FileIDManipulator(const string& nam, ISvcLocator* pSvc) 
: Algorithm(nam,pSvc) 
{
  declareProperty("Action",       m_action=DUMP);
  declareProperty("BankLocation", m_rawLocation=RawEventLocation::Default);    
  declareProperty("DataType",     m_type=MDFIO::MDF_RECORDS);
}


/// Issue error message
StatusCode FileIDManipulator::error(const string& msg) {
  MsgStream out(msgSvc(),name());
  out << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;        
}

/// Find the FID bank in the raw event structure
pair<RawBank*,void*> FileIDManipulator::getFIDbank()  {
  //
  // Move file information from the raw bank to the
  // transient datastore, where it later can be picked up by the 
  // data writer algorithm.
  //
  string loc = m_type!=MDFIO::MDF_NONE ? "/Event" : m_rawLocation;
  SmartDataPtr<DataObject> evt(eventSvc(),loc);
  if ( evt ) {
    RawEvent* raw = 0;
    IRegistry* reg = 0;
    switch(m_type) {
      case MDFIO::MDF_NONE:
        if ( (raw=(RawEvent*)evt.ptr()) )  {
          const vector<RawBank*>& ids = raw->banks(RawBank::DAQ);
          for(vector<RawBank*>::const_iterator j=ids.begin(); j != ids.end(); j++)  {
            if ( (*j)->version() == DAQ_FILEID_BANK ) {
              return pair<RawBank*,void*>(*j,raw);
            }
          }
        }
        break;
      case MDFIO::MDF_RECORDS:
      case MDFIO::MDF_BANKS:
        if ( 0 != (reg=evt->registry()) )  {
          RawDataAddress* pA = dynamic_cast<RawDataAddress*>(reg->address());
          if ( pA )    {
            pair<const char*,int> data = pA->data();
            MDFHeader* h = (MDFHeader*)data.first;
            if (m_type!=MDFIO::MDF_BANKS) h=(MDFHeader*)((RawBank*)data.first)->data();
            const char* start = ((char*)h)+h->sizeOf(h->headerVersion());
            const char* end   = start+h->size();
            while(start<end)    {
              RawBank* b = (RawBank*)start;
              if ( b->version() == DAQ_FILEID_BANK )
                return pair<RawBank*,void*>(b,h);
              start += b->totalSize();
            }
          }
        }
        break;
      default:
        error("Unknown input data type.");
        return pair<RawBank*,void*>(0,0);
    }
    error("No file identification bank found.");
    return pair<RawBank*,void*>(0,0);
  }
  error("Could not retrieve the event object at "+loc);
  return pair<RawBank*,void*>(0,0);
}

/// Add fileID bank
StatusCode FileIDManipulator::add()   {
  //
  // Add a new bank containing the information about the original file
  // to the raw event structure.
  //
  string loc = m_type!=MDFIO::MDF_NONE ? "/Event" : m_rawLocation;
  SmartDataPtr<DataObject> ptr(eventSvc(),loc);
  if ( ptr )  {
    DataObject* evt = ptr;
    IRegistry*  reg = evt->registry();
    if ( reg )  {
      IOpaqueAddress* padd = reg->address();
      if ( padd ) {
        RawBank* b = 0;
        RawDataAddress* pA = 0;
        RawEvent* raw = (RawEvent*)evt;
        size_t l0 = padd->par()[0].length()+1;
        size_t l1 = padd->par()[1].length()+1;
        size_t len = sizeof(FileIdInfo)-1+l0+l1;
        switch(m_type)   {
          case MDFIO::MDF_BANKS:
          case MDFIO::MDF_RECORDS:
            // We KNOW that there was additional space reserved at the end of the
            // data record. Just add the new bank and patch the MDF header.
            // Do not forget to invalidate the checksum.
            if ( 0 != (pA=dynamic_cast<RawDataAddress*>(padd)) )    {
              pair<char*,int> data = pA->data();
              MDFHeader* h = (MDFHeader*)data.first;
              if(m_type!=MDFIO::MDF_BANKS) h = (MDFHeader*)((RawBank*)data.first)->data();
              b = (RawBank*)(((char*)h)+h->recordSize());
              b->setMagic();
              b->setType(RawBank::DAQ);
              b->setVersion(DAQ_FILEID_BANK);
              b->setSourceID(0);
              b->setSize(len);
              // Update MDF header
              len += b->hdrSize();
              h->setChecksum(0);
              h->setSize(h->size()+len);
              // And data address
              data.second += len;
              pA->setData(data);
            }
            break;
          default:
          case MDFIO::MDF_NONE:
            // Normal behaviour: Simply add the new bank to the RawEvent structure
            b = raw->createBank(0,RawBank::DAQ,DAQ_FILEID_BANK,len);
            raw->adoptBank(b,true);
            break;
        }
        if ( b ) {
          FileIdInfo* i = b->begin<FileIdInfo>();
          i->ip0 = padd->ipar()[0];
          i->ip1 = padd->ipar()[1];
          i->l0  = l0;
          i->l1  = l1;
          ::strncpy(i->pdata,padd->par()[0].c_str(),l0);
          ::strncpy(i->pdata+i->l0,padd->par()[1].c_str(),l1);
          return StatusCode::SUCCESS;
        }
        return error("I do not understand the data type(s).");
      }
      return error("Could not address of Event");
    }
    return error("We don't have a valid IRegistry in "+loc);
  }
  return error("Failed to retrieve event object at "+loc);
}

/// Remove fileID bank
StatusCode FileIDManipulator::remove() {
  //
  // Move file information from the raw bank to the
  // transient datastore, where it later can be picked up by the 
  // data writer algorithm.
  //
  pair<RawBank*,void*> res = getFIDbank();
  if ( res.first )  {
    RawEvent* e = 0;
    MDFHeader* h = 0;
    FileIdInfo* i=res.first->begin<FileIdInfo>();
    size_t len = sizeof(FileIdInfo)+i->l0+i->l1;
    FileIdObject* obj = new FileIdObject(len);
    ::memcpy(obj->data.ptr,i,len);
    switch(m_type)   {
      case MDFIO::MDF_NONE:
        e = (RawEvent*)res.second;
        e->removeBank(res.first);
        break;
      case MDFIO::MDF_BANKS:
      case MDFIO::MDF_RECORDS:
        h = (MDFHeader*)res.second;
        h->setChecksum(0);
        h->setSize(h->size()-len);
        break;
    }
    if ( eventSvc()->registerObject("/Event/FileID",obj).isSuccess() )
      return StatusCode::SUCCESS;
    return error("Failed to register the FILE ID object in the transient datastore.");
  }
  return StatusCode::FAILURE;
}

/// Print FID bank content
StatusCode FileIDManipulator::print()  {
  pair<RawBank*,void*> res = getFIDbank();
  if ( res.first )  {
    const FileIdInfo* i=res.first->begin<FileIdInfo>();
    MsgStream log(msgSvc(),name());
    log << MSG::INFO
      << "FID Bank: "
      << RawEventPrintout::bankHeader(res.first) << endmsg
      << "ipar: " << setw(8) << i->ip0  << " / " << setw(8) << i->ip1
      << " par: "  << i->pdata << " / " << i->pdata+i->l0 << endmsg;
    return StatusCode::SUCCESS;
  }
  return error("No FileID bank present in raw event at "+m_rawLocation);
}

/// Algorithm execution
StatusCode FileIDManipulator::execute() {
  switch ( m_action )  {
    case ADD:      return add();
    case REMOVE:   return remove();
    case DUMP:     return print();
    default:       return StatusCode::FAILURE;
  }
  return StatusCode::FAILURE;
}
