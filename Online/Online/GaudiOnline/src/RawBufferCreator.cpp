// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/RawBufferCreator.cpp,v 1.5 2005-05-11 17:48:33 frankb Exp $
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
#include "GaudiOnline/OnlineAddress.h"
#include "GaudiOnline/StreamDescriptor.h"
#include "Event/RawEvent.h"
#include "Event/L1Event.h"
#include <memory>
#include <stdexcept>

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

  // Helper to print errors and return bad status
  StatusCode error(const std::string& msg) {
    MsgStream err(msgSvc(), "OnlineConverter");
    err << MSG::ERROR << msg << endmsg;
    return StatusCode::FAILURE;
  }

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
      try {
        GaudiOnline::Address* addr = dynamic_cast<GaudiOnline::Address*>(pAddr);
        const GaudiOnline::StreamDescriptor* dsc = addr->descriptor();
        std::auto_ptr<DataObject> obj(new DataObject());
        std::auto_ptr<DataObject> raw;
        std::string loc;
        if ( dataProvider()->registerObject("/Event/DAQ", obj.get()).isSuccess() )  {
          obj.release();
          switch( dsc->type() ) {
          case GaudiOnline::StreamDescriptor::DAQ_BUFFER: {
              int len = dsc->length()/sizeof(raw_int);
              if ( (dsc->length()%sizeof(raw_int)) != 0 ) len++;
              raw = std::auto_ptr<DataObject>(new RawEvent((raw_int*)dsc->data(),len));
              loc = RawEventLocation::Default;
            }
            break;
          case GaudiOnline::StreamDescriptor::L1_BUFFER: {
              int len = dsc->length()/sizeof(l1_int);
              if ( (dsc->length()%sizeof(l1_int)) != 0 ) len++;
              raw = std::auto_ptr<DataObject>(new L1Event((l1_int*)dsc->data(),len));
              loc = L1EventLocation::Default;
            }
            break;
          default: {
              MsgStream err(msgSvc(), "OnlineConverter");
              err << MSG::ERROR << "Unknown data type:" << dsc->type() << endmsg;
            }
            return StatusCode::FAILURE;
          }
          if ( dataProvider()->registerObject(loc,raw.get()).isSuccess() )  {
            raw.release();
            return StatusCode::SUCCESS;
          }
          return error("Failed to register object "+loc);
        }
        return error("Failed to register object DAQ");
      }
      catch (std::exception& e) {
        return error(std::string("Exception:") + e.what());
      }
      catch(...) {
        return error("Exception: (Unknown)");
      }
    }
    return error("No valid top level event object present.");
  }
};

static const CnvFactory<OnlineConverter>  dataObjectFactory;
const IFactory& OnlineCnvFactory = dataObjectFactory;

