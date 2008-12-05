#include "GaudiOnline/FIDManipulator.h"
#include "GaudiOnline/FileIdInfo.h"
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IRegistry.h" 
#include "GaudiKernel/MsgStream.h" 
#include "MDF/RawEventPrintout.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"
#include "MDF/MDFIO.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include <iomanip>

using namespace std;
using namespace LHCb;
using namespace Gaudi;

/// Standard constructor
FIDManipulator::FIDManipulator(const std::string& loc, int typ, IMessageSvc* m, IDataProviderSvc* d) 
  : m_dp(d), m_msg(m), m_location(loc), m_type(typ)
{
}

/// Standard destructor
FIDManipulator::~FIDManipulator() {
}

/// Issue error message
StatusCode FIDManipulator::error(const string& msg) {
  MsgStream out(m_msg,"FID");
  out << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;        
}

/// Find the FID bank in the raw event structure
pair<RawBank*,void*> FIDManipulator::getBank()  {
  //
  // Move file information from the raw bank to the
  // transient datastore, where it later can be picked up by the 
  // data writer algorithm.
  //
  string loc = m_type==MDFIO::MDF_NONE ? m_location : "/Event";
  SmartDataPtr<DataObject> evt(m_dp,loc);
  if ( evt ) {
    RawEvent* raw = 0;
    IRegistry* reg = 0;
    switch(m_type) {
      case MDFIO::MDF_NONE:
        if ( (raw=(RawEvent*)evt.ptr()) )  {
          const vector<RawBank*>& ids = raw->banks(RawBank::DAQ);
          for(vector<RawBank*>::const_iterator j=ids.begin(); j != ids.end(); j++)  {
            if ( (*j)->type() == RawBank::DAQ && (*j)->version() == DAQ_FILEID_BANK ) {
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
            if (m_type==MDFIO::MDF_BANKS) h=(MDFHeader*)((RawBank*)data.first)->data();
            const char* start = ((char*)h)+h->sizeOf(h->headerVersion());
            const char* end   = start+h->size();
	    if ( m_debug > 0 ) {
	      /// Debug section
	      MsgStream log(m_msg,"FID");
	      while(start<end)    {
		RawBank* b = (RawBank*)start;
		log << MSG::INFO << RawEventPrintout::bankHeader(b) << endmsg;
		start += b->totalSize();
	      }
	      start = ((char*)h)+h->sizeOf(h->headerVersion());
	    }
            while(start<end)    {
              RawBank* b = (RawBank*)start;
              if ( b->type() == RawBank::DAQ && b->version() == DAQ_FILEID_BANK )
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
StatusCode FIDManipulator::add(int id, const std::string& guid)   {
  //
  // Add a new bank containing the information about the original file
  // to the raw event structure.
  //
  string src_loc = m_type!=MDFIO::MDF_NONE ? "/Event" : m_location;
  SmartDataPtr<DataObject> ptr(m_dp,src_loc);
  if ( ptr )  {
    DataObject* evt = ptr;
    IRegistry*  reg = evt->registry();
    if ( reg )  {
      IOpaqueAddress* padd = reg->address();
      if ( padd ) {
        RawBank* b = 0, *tae = 0;
        RawDataAddress* pA = dynamic_cast<RawDataAddress*>(padd);
        RawEvent* raw = (RawEvent*)evt;
        size_t l0 = padd->par()[0].length()+1;
        size_t l1 = padd->par()[1].length()+1;
	size_t l2 = guid.length()+1;
        size_t len = sizeof(FileIdInfo)-1+l0+l1+l2;

        switch(m_type)   {
          case MDFIO::MDF_BANKS:
          case MDFIO::MDF_RECORDS:
            // We KNOW that there was additional space reserved at the end of the
            // data record. Just add the new bank and patch the MDF header.
            // Do not forget to invalidate the checksum.
            if ( 0 != pA )    {
              pair<char*,int> data = pA->data();
              MDFHeader* h = (MDFHeader*)data.first;
              if(m_type==MDFIO::MDF_BANKS) h = (MDFHeader*)((RawBank*)data.first)->data();
              b = (RawBank*)(((char*)h)+h->recordSize());
              b->setMagic();
              b->setType(RawBank::DAQ);
              b->setVersion(DAQ_FILEID_BANK);
              b->setSourceID(0);
              b->setSize(len);
	      // Total bank length
              len += b->hdrSize();
	      // If TAE event: need to update TAE structure to include FID bank
	      if ( (tae=getTAEBank(h->data())) ) {
		int nBlocks = tae->size()/sizeof(int)/3;  // The TAE bank is a vector of triplets
		int* block  = tae->begin<int>();
		block += 3*(nBlocks-1)+2;
		*block = *block + len;
	      }
              // Update MDF header
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
	  i->setID(id);
	  i->setipar(padd->ipar());
	  i->setpar(padd->par());
	  i->setguid(guid);
          return StatusCode::SUCCESS;
        }
        return error("I do not understand the data type(s).");
      }
      return error("Could not address of Event");
    }
    return error("We don't have a valid IRegistry in "+src_loc);
  }
  return error("Failed to retrieve event object at "+src_loc);
}

/// Remove fileID bank
StatusCode FIDManipulator::remove() {
  //
  // Move file information from the raw bank to the
  // transient datastore, where it later can be picked up by the 
  // data writer algorithm.
  //
  pair<RawBank*,void*> res = getBank();
  if ( res.first )  {
    RawBank* b = 0;
    RawEvent* e = 0;
    MDFHeader* h = 0;
    FileIdInfo* i=res.first->begin<FileIdInfo>();
    size_t len = i->sizeOf();
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
	b = (RawBank*)res.first;
	if ( b->type() == RawBank::DAQ && b->version() == DAQ_FILEID_BANK ) {
	  h->setChecksum(0); // Checksum is invalid if we remove data!
	  h->setSize(h->size()-res.first->totalSize());
	  SmartDataPtr<DataObject> evt(m_dp,"/Event");
	  if ( evt )  {      // Now update raw address in /Event
	    IRegistry* reg = evt->registry();
	    if ( reg )  {
	      IOpaqueAddress* padd = reg->address();
	      RawDataAddress* pA = dynamic_cast<RawDataAddress*>(padd);
	      if ( pA )  {
		pair<char*,int> data=pA->data();
		data.second -= res.first->totalSize();
		pA->setData(data);
		break;
	      }
	    }
	    error("Failed to update Raw Address in /Event!");
	  }
	  break;
	}
	return error("No valid File ID bank found in raw buffer from /Event-address!");
    }
    if ( m_dp->registerObject("/Event/FileID",obj).isSuccess() )
      return StatusCode::SUCCESS;
    return error("Failed to register the FILE ID object in the transient datastore.");
  }
  return StatusCode::FAILURE;
}

/// Print FID bank content
StatusCode FIDManipulator::print()  {
  pair<RawBank*,void*> res = getBank();
  if ( res.first )  {
    const FileIdInfo* i=res.first->begin<FileIdInfo>();
    MsgStream log(m_msg,"FID");
    log << MSG::INFO
	<< "FID Bank: "
	<< RawEventPrintout::bankHeader(res.first) << endmsg
	<< "id:" << i->id << " ipar: " << setw(8) << i->ip0  << " / " << setw(8) << i->ip1
	<< " par: "  << i->par0() << "/" << i->par1() << "/" << i->guid() << endmsg;
    return StatusCode::SUCCESS;
  }
  return error("No FileID bank present in raw event at "+m_location);
}
