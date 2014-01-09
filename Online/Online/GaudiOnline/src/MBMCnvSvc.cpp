// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MBMCnvSvc.cpp,v 1.11 2006-12-14 18:59:19 frankb Exp $
//  ====================================================================
//  RawBufferCreator.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiOnline/MBMCnvSvc.h"
#include "MDF/StorageTypes.h"
#include "MDF/MDFHeader.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "RTL/rtl.h"
#include "Event/RawBank.h"

using MBM::Producer;
using MBM::EventDesc;

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MBMCnvSvc)

namespace  {
  Producer* producerFromIODescriptor(void* ioDesc)   {
    return (Producer*)ioDesc;
  }
}

/// Initializing constructor
LHCb::MBMCnvSvc::MBMCnvSvc(const std::string& nam, ISvcLocator* loc) 
: RawDataCnvSvc(nam, loc, MBM_StorageType)
{
  declareProperty("PartitionID",m_partitionID=333);
  m_genChecksum = 0;
  m_compress = 0;
}

/// Initializing constructor for inheritance
LHCb::MBMCnvSvc::MBMCnvSvc(const std::string& nam, ISvcLocator* loc, long type)
: RawDataCnvSvc(nam, loc, type)
{
  m_genChecksum = 0;
  m_compress = 0;
}

/// Standard destructor
LHCb::MBMCnvSvc::~MBMCnvSvc() {
}

/// Convert the transient object to the requested representation.
StatusCode LHCb::RawDataCnvSvc::createRep(DataObject* pObject, IOpaqueAddress*& /* refpAddress */ ) 
{
  if ( pObject )  {
    if ( m_current != m_fileMap.end() )   {
      IRegistry* reg = pObject->registry();
      if ( reg )  {
        if ( reg->identifier() == "/Event" )   {

        }
        return StatusCode::SUCCESS;
      }
      return error("createRep> Cannot write object: No registry entry found!");
    }
    return error("createRep> Cannot write object: No output file is connected!");
  }
  return error("createRep> Cannot write object: Object pointer is NULL!");
}

/// Allocate data space for output
std::pair<char*,int> LHCb::MBMCnvSvc::getDataSpace(void* const ioDesc, size_t len)   {
  if ( ioDesc )   {
    Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      if ( prod->getSpace(len) == MBM_NORMAL )  {
        EventDesc& e = prod->event();
        return std::pair<char*,int>((char*)e.data, e.len);
      }
    }
  }
  return std::pair<char*,int>(0,0);
}

/// Declare event to data space
StatusCode 
LHCb::MBMCnvSvc::writeBuffer(void* const ioDesc, const void* data, size_t len)    {
  if ( ioDesc )   {
    Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      RawBank* hdrBank = (RawBank*)data;
      MDFHeader* hdr   = (MDFHeader*)hdrBank->data();
      const unsigned int *trMask = hdr->subHeader().H1->triggerMask();
      EventDesc& e = prod->event();
      e.type       = EVENT_TYPE_EVENT;
      e.mask[0]    = trMask[0];
      e.mask[1]    = trMask[1];
      e.mask[2]    = trMask[2];
      e.mask[3]    = trMask[3];
      e.len        = len;
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
      std::string proc = RTL::processName();
      unsigned int partID = m_partitionID;
      size_t id1 = fname.find(".0x",6);
      std::string buff = fname.substr(6, id1==std::string::npos ? id1 : id1-6);
      if ( id1 != std::string::npos ) ::sscanf(fname.c_str()+id1+1,"0x%X",&partID);
      Producer* p = new Producer(buff,proc,partID);
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
    Producer* prod = producerFromIODescriptor(ioDesc);
    if ( prod )  {
      delete prod;
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

/// Read raw banks
StatusCode LHCb::MBMCnvSvc::readRawBanks(void* , std::vector<RawBank*>& , long long& )  {
  return StatusCode::FAILURE;
}
