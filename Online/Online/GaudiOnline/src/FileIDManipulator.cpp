// $Id: FileIDManipulator.cpp,v 1.1 2008-11-28 15:52:18 frankb Exp $
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

  /** @class FileIDManipulator FileIDManipulator.h
   *  
   *
   *  @author Albert Puig Navarro (albert.puig@cern.ch)
   *  @author M.Frank
   *  @date   2008-03-03
   */
  class FileIDManipulator : public Algorithm {
  protected:
    /// Property: remove/add file id bank
    bool         m_add;
    /// Property: Location of the raw event in the transient datastore
    std::string  m_rawLocation;
    /// Property: input data type
    int          m_type;
  protected:
    /// Add fileID bank
    StatusCode add();
    StatusCode addRaw();
    /// Remove fileID bank
    StatusCode remove();

  public: 
    /// Standard constructor
    FileIDManipulator(const std::string& nam, ISvcLocator* pSvc);
    /// Standard destructor
    virtual ~FileIDManipulator()              {}
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

using namespace std;
using namespace LHCb;
using namespace Gaudi;

#include "GaudiKernel/AlgFactory.h" 
using namespace LHCb;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(FileIDManipulator);

#define DAQ_FILEID_BANK 32

/// Standard constructor
FileIDManipulator::FileIDManipulator(const std::string& nam, ISvcLocator* pSvc) 
: Algorithm(nam,pSvc) 
{
  declareProperty("Add",          m_add=true);
  declareProperty("BankLocation", m_rawLocation=RawEventLocation::Default);    
  declareProperty("DataType",     m_type=MDFIO::MDF_RECORDS);
}


/// Issue error message
StatusCode FileIDManipulator::error(const string& msg) {
  MsgStream out(msgSvc(),name());
  out << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;		
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
	size_t len = padd->par()[0].length()+padd->par()[1].length()+2;
	switch(m_type) {
	case MDFIO::MDF_BANKS:
	case MDFIO::MDF_RECORDS:
	  // We KNOW that there was additional space reserved at the end of the
	  // data record. Just add the new bank and patch the MDF header.
	  // Do not forget to invalidate the checksum.
	  pA = dynamic_cast<RawDataAddress*>(padd);
	  if ( pA )    {
	    std::pair<char*,int> data = pA->data();
	    MDFHeader* h = (MDFHeader*)data.first;
	    if(m_type!=MDFIO::MDF_BANKS) h = (MDFHeader*)((RawBank*)data.first)->data();
	    h->setChecksum(0);
	    h->setSize(h->size0()+len);
	    b = (RawBank*)(((char*)h)+h->recordSize());
	    data.second += len;
	    pA->setData(data);
	  }
	  break;
	default:
	case MDFIO::MDF_NONE:
	  // Normal behaviour: Simply add the new bank to the RawEvent structure
	  RawEvent* raw = (RawEvent*)evt;
	  b = raw->createBank(0,RawBank::DAQ,DAQ_FILEID_BANK,sizeof(FileIdInfo)-1+len);
	  raw->adoptBank(b,true);
	  break;
	}
	if ( b ) {
	  FileIdInfo* i = b->begin<FileIdInfo>();
	  i->ip0 = padd->ipar()[0];
	  i->ip1 = padd->ipar()[1];
	  i->l0  = padd->par()[0].length();
	  i->l1  = padd->par()[1].length();
	  ::strcpy(i->pdata,padd->par()[0].c_str());
	  ::strcpy(i->pdata+i->l0+1,padd->par()[1].c_str());
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

StatusCode FileIDManipulator::remove() {
  //
  // Move file information from the raw bank to the
  // transient datastore, where it later can be picked up by the 
  // data writer algorithm.
  //
  string loc = m_type!=MDFIO::MDF_NONE ? "/Event" : m_rawLocation;
  SmartDataPtr<DataObject> evt(eventSvc(),loc);
  if ( evt ) {
    RawBank* b = 0;
    FileIdInfo* i=0;
    switch(m_type) {
      case MDFIO::MDF_NONE: {
	RawEvent* raw = (RawEvent*)evt.ptr();
	const vector<RawBank*>& ids = raw->banks(RawBank::DAQ);
	for(vector<RawBank*>::const_iterator j=ids.begin(); j != ids.end(); j++)  {
	  if ( (*j)->version() == DAQ_FILEID_BANK ) {
	    b = *j;
	    i = b->begin<FileIdInfo>();
	    raw->removeBank(b);
	    break;
	  }
	}
	break;
      }
      case MDFIO::MDF_RECORDS:
      case MDFIO::MDF_BANKS: {
	IRegistry* reg = evt->registry();
	if ( reg )  {
	  RawDataAddress* pA = dynamic_cast<RawDataAddress*>(reg->address());
	  if ( pA )    {
	    std::pair<const char*,int> data = pA->data();
	    MDFHeader* h = (MDFHeader*)data.first;
	    if (m_type!=MDFIO::MDF_BANKS) h=(MDFHeader*)((RawBank*)data.first)->data();
	    const char* start = ((char*)h)+h->sizeOf(h->headerVersion());
	    const char* end   = start+h->size();
	    while(start<end)    {
	      b = (RawBank*)start;
	      if ( b->version() == DAQ_FILEID_BANK ) {
		i = b->begin<FileIdInfo>();
		break;
	      }
	      start += b->totalSize();
	    }
	  }
	}
	break;
      }
    }
    if ( 0 != i ) {
      MsgStream log(msgSvc(),name());
      log << MSG::INFO
	  << "--------------HEADER--------------" << endmsg
	  << RawEventPrintout::bankHeader(b) << endmsg
	  << "ipar1: " << i->ip0  << "    ipar2: " << i->ip1 << endmsg
	  << "par1: "  << i->pdata << "    par2: " << i->pdata+i->l0 << endmsg;
      FileIdObject* obj = new FileIdObject(sizeof(FileIdInfo)+i->l0+i->l1);
      ::memcpy(obj->data.ptr,i,sizeof(FileIdInfo)+i->l0+i->l1);
      if ( eventSvc()->registerObject("/Event/FileID",obj).isSuccess() )
	return StatusCode::SUCCESS;
      return error("Failed to register the FILE ID object in the transient datastore.");
    }
    return error("No FileID bank present in raw event at "+m_rawLocation);
  }
  return error("Could not retrieve the RawEvent at "+m_rawLocation);
}

/// Algorithm execution
StatusCode FileIDManipulator::execute() {
  return m_add ? add() : remove();
}
