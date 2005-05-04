// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawBufferCreator.cpp,v 1.3 2005-05-04 17:10:23 frankb Exp $
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

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "Event/RawBuffer.h"
#include "Event/L1Buffer.h"
#include <memory>

/** @class OnlineCnvSvc OnlineCnvSvc.cpp  GaudiOnline/OnlineCnvSvc.cpp
  *
  * Conversion service for the online data.
  * 
  * @author  M.Frank
  * @version 1.0
  * @date    01/01/2005
  */ 
class OnlineCnvSvc : public ConversionSvc  {
public:
  /// Initializing constructor
  /**  @param   nam         [IN]  Name of the service
    *  @param   loc         [IN]  Pointer to the service locator object
    *  @return Initialized reference to service object
    */
  OnlineCnvSvc(const std::string& nam, ISvcLocator* loc) : ConversionSvc(nam, loc, 0) {}

  /// Standard destructor      
  virtual ~OnlineCnvSvc() {}
};

static const SvcFactory<OnlineCnvSvc>  svcFactory;
const IFactory& OnlineCnvSvcFactory = svcFactory;

/** @class OnlineCnvSvc OnlineCnvSvc.cpp  GaudiOnline/OnlineCnvSvc.cpp
  *
  * Standard converter for online objects for the online data.
  * 
  * @author  M.Frank
  * @version 1.0
  * @date    01/01/2005
  */ 
class OnlineConverter : public Converter  {
public:
  /// Initializing constructor
  /**  @param   loc         [IN]  Pointer to the service locator object
    *  @return Initialized reference to converter object
    */
  OnlineConverter(ISvcLocator* loc) : Converter(storageType(), DataObject::classID(), loc)  {}

  /// Standard destructor      
  virtual ~OnlineConverter() {}

  /// Factory helper to indicate class type
  static const CLID& classID()         { return DataObject::classID();  }

  /// Factory helper to indicate storage type
  static long storageType()            { return TEST_StorageType;       }

  /// Concrete storage type
  virtual long repSvcType() const      { return storageType();          }

  /// Concrete class type
  virtual const CLID& objType() const  { return classID();              }

  /// Object creation callback
  /**  @param   pAddr      [IN]  Reference to opaque object address
    *  @param   refpObj   [OUT]  Reference to store pointer to created object
    *
    *  @return Status code indicating success or failure
    */
  virtual StatusCode createObj(IOpaqueAddress* /* pAddr */, DataObject*& refpObj)  {
    refpObj = new DataObject();
    return StatusCode::SUCCESS;
  }

  /// Callback for reference processing (misused to attach leaves)
  /**  @param   pAddr      [IN]  Reference to opaque object address
    *  @param   refpObj   [OUT]  Reference to store pointer to created object
    *
    *  @return Status code indicating success or failure
    */
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddr, DataObject* /* pObj */ )  {
    if ( pAddr )  {
      GaudiOnline::Address* addr = dynamic_cast<GaudiOnline::Address*>(pAddr);
      const GaudiOnline::StreamDescriptor* dsc = addr->descriptor();
      std::auto_ptr<DataObject> obj(new DataObject());
      switch( dsc->type() ) {
      case GaudiOnline::StreamDescriptor::DAQ_BUFFER:
        if ( dataProvider()->registerObject("DAQ", obj.get()).isSuccess() )  {
          obj.release();
          int len = dsc->length()/sizeof(raw_int);
          std::auto_ptr<raw_int> raw_buff(new raw_int[len]);
          ::memcpy(raw_buff.get(), dsc->data(), len);
          std::auto_ptr<RawBuffer>  raw(new RawBuffer(len, raw_buff.release()));
          if ( dataProvider()->registerObject(RawBufferLocation::Default,raw.get()).isSuccess() )  {
            raw.release();
            return StatusCode::SUCCESS;
          }
        }
        break;
      case GaudiOnline::StreamDescriptor::L1_BUFFER:
        if ( dataProvider()->registerObject("L1", obj.get()).isSuccess() )  {
          obj.release();
          /* not yet implememnted in the event model 
          int len = dsc->length()/sizeof(l1_int);
          std::auto_ptr<L1Buffer>  raw(new L1Buffer());
          raw->reallocate(len);
          ::memcpy(raw->buffer(), dsc->data(), dsc->length());
          if ( dataProvider()->registerObject(RawBufferLocation::Default,raw.get()).isSuccess() )  {
            raw.release();
            return StatusCode::SUCCESS;
          }
          */
        }
        break;
      default:
        break;
      }
    }
    MsgStream err1(msgSvc(), "OnlineConverter");
    err1 << MSG::ERROR << "No valid top level event object present." << endmsg;
    return StatusCode::FAILURE;
  }
};

static const CnvFactory<OnlineConverter>  dataObjectFactory;
const IFactory& OnlineCnvFactory = dataObjectFactory;

