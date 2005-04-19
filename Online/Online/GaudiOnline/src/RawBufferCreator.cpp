// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawBufferCreator.cpp,v 1.2 2005-04-19 16:59:59 frankb Exp $
//	====================================================================
//  RawBufferCreator.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ConversionSvc.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "Event/RawBuffer.h"
#include <memory>

class OnlineCnvSvc : public ConversionSvc  {
public:
  OnlineCnvSvc(const std::string& nam, ISvcLocator* loc) : ConversionSvc(nam, loc, 0) {}
  virtual ~OnlineCnvSvc() {}
};

static const SvcFactory<OnlineCnvSvc>  svcFactory;
const IFactory& OnlineCnvSvcFactory = svcFactory;

class OnlineConverter : public Converter  {
public:
  OnlineConverter(ISvcLocator* svc) : Converter(storageType(), DataObject::classID(), svc)  {}
  virtual ~OnlineConverter() {}
  virtual long repSvcType() const      { return storageType();          }
  static const CLID& classID()         { return DataObject::classID();  }
  static long storageType()            { return TEST_StorageType;       }
  virtual StatusCode createObj(IOpaqueAddress* /* pAddr */, DataObject*& refpObj)  {
    refpObj = new DataObject();
    return StatusCode::SUCCESS;
  }
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* /* pObj */ )  {
    if ( pAddr )  {
      IRegistry* reg = pAddr->registry();
      if ( reg )   {
        GaudiOnline::Address* addr = dynamic_cast<GaudiOnline::Address*>(reg->address());
        const GaudiOnline::StreamDescriptor* dsc = addr->descriptor();
        std::auto_ptr<DataObject> daq(new DataObject());
        if ( dataProvider()->registerObject("DAQ", daq.get()).isSuccess() )  {
          daq.release();
          int len = dsc->length()/sizeof(raw_int);
          // Fixme: Bug in RawBuffer object requires to copy to temporary....
          std::auto_ptr<raw_int> raw_buff(new raw_int[len]);
          memcpy(raw_buff.get(), dsc->data(), len);
          std::auto_ptr<RawBuffer>  raw(new RawBuffer(len, raw_buff.release()));
          // ----------------------------------------------------------------
          if ( dataProvider()->registerObject(RawBufferLocation::Default,raw.get()).isSuccess() )  {
            raw.release();
            return StatusCode::SUCCESS;
          }
        }
      }
    }
    MsgStream err1(msgSvc(), "OnlineConverter");
    err1 << MSG::ERROR << "No valid top level event object present." << endmsg;
    return StatusCode::FAILURE;
  }
};

static const CnvFactory<OnlineConverter>  dataObjectFactory;
const IFactory& OnlineCnvFactory = dataObjectFactory;

