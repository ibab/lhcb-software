// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMCnvSvc.cpp,v 1.2 2006-01-10 13:45:03 frankb Exp $
//	====================================================================
//  RawBufferCreator.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/MBMCnvSvc.h"
#include "MDF/StorageTypes.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventHelpers.h"
#include "MBM/Producer.h"
#include "Event/RawEvent.h"
#include <stdexcept>

using LHCb::RawEvent;
using LHCb::RawBank;
using LHCb::MEPFragment;
using LHCb::MDFHeader;
using LHCb::MultiEventPacket;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMCnvSvc)

static MBM::Producer* producerFromIODescriptor(void* ioDesc)   {
  return (MBM::Producer*)ioDesc;
}

/// Initializing constructor
LHCb::MBMCnvSvc::MBMCnvSvc(const std::string& nam, ISvcLocator* loc) 
: RawDataCnvSvc(nam, loc, RAWDATA_StorageType)
{
}

/// Initializing constructor for inheritance
LHCb::MBMCnvSvc::MBMCnvSvc(const std::string& nam, ISvcLocator* loc, long type)
: RawDataCnvSvc(nam, loc, type)
{
}

/// Standard destructor
LHCb::MBMCnvSvc::~MBMCnvSvc() {
}

/// Allocate data space for output
char* const LHCb::MBMCnvSvc::getDataSpace(void* ioDesc, size_t len)   
{
  if ( ioDesc )   {
    MBM::Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      if ( prod->getSpace(len+sizeof(MDFHeader)) == MBM_NORMAL )  {
        MBM::EventDesc& e = prod->event();
        return (char*)e.data+sizeof(MDFHeader);
      }
    }
  }
  return 0;
}

/// Declare event to data space
StatusCode LHCb::MBMCnvSvc::writeDataSpace(void* ioDesc,
                                           size_t len, 
                                           longlong trNumber, 
                                           unsigned int trMask[4],
                                           int evType, 
                                           int hdrType)    {
  if ( ioDesc )   {
    MBM::Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      MBM::EventDesc& e = prod->event();
      makeMDFHeader(e.data,len,evType,hdrType,trNumber,trMask,0,0);
      e.type    = evType;
      e.mask[0] = trMask[0];
      e.mask[1] = trMask[1];
      e.mask[2] = trMask[2];
      e.mask[3] = trMask[3];
      e.len     = len+sizeof(MDFHeader);
      if ( prod->sendEvent() == MBM_NORMAL )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

/// Open MBM buffer manager
void* LHCb::MBMCnvSvc::openIO(const std::string& fname, const std::string&  mode) const {
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    // Writing: requires producer
    if ( fname.find("mbm://") == 0 )  {
      int partID;
      size_t id1 = fname.find(".",6);
      size_t id2 = fname.find(".0x",id1+1);
      std::string buff = fname.substr(6, id1-6);
      std::string proc = fname.substr(id1+1, id2-id1-1);
      ::sscanf(fname.c_str()+id2+1,"0x%X",&partID);
      MBM::Producer* p = new MBM::Producer(buff,proc,partID);
      return p;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    // Reading: requires consumer
  }
  return 0;
}

/// Close MBM buffer manager
StatusCode LHCb::MBMCnvSvc::closeIO(void* ioDesc) const   {
  if ( ioDesc )   {
    MBM::Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      delete prod;
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}
