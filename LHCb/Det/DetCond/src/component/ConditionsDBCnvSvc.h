//$Id: ConditionsDBCnvSvc.h,v 1.11 2005-02-09 08:49:29 marcocle Exp $
#ifndef DETCOND_CONDITIONSDBCNVSVC_H
#define DETCOND_CONDITIONSDBCNVSVC_H 1

/// Include files
#include "DetCond/IConditionsDBCnvSvc.h"
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class IDetDataSvc;
class IOpaqueAddress;

///---------------------------------------------------------------------------
/** @class ConditionsDBCnvSvc ConditionsDBCnvSvc.h Det/DetCond/ConditionsDBCnvSvc.h

    A conversion service for CERN-IT COOL (ex. CondDB) persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Marco Clemencic 
    @date November 2004
*///--------------------------------------------------------------------------

class ConditionsDBCnvSvc : public ConversionSvc, 
                           virtual public IConditionsDBCnvSvc
{
  
  /// Only factories can access protected constructors
  friend class SvcFactory<ConditionsDBCnvSvc>;

 protected:
  
  /// Constructor
  ConditionsDBCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionsDBCnvSvc();


 public:
  
  // Reimplemented from IInterface

  /// Query the interface of the service
  virtual StatusCode queryInterface( const InterfaceID& riid, 
				     void** ppvInterface );  

 public:

  // Overloaded from ConversionSvc

  /// Initialize the service
  virtual StatusCode initialize();
  
  /// Finalize the service
  virtual StatusCode finalize();

  /// Create an address using explicit arguments to identify a single object.
  virtual StatusCode createAddress (long svc_type,
				     const CLID& clid,
				     const std::string* par, 
				     const unsigned long* ip,
				     IOpaqueAddress*& refpAddress );

  class CnvTest : public std::unary_function<WorkerEntry, bool>   {
  protected:
    const CLID m_test;
  public:
    CnvTest(const CLID& test) : m_test(test)    {
    }
    virtual ~CnvTest()    {
    }
    bool operator()( const WorkerEntry& testee )  {
        return (m_test == testee.clID() || testee.clID() == 0) ? true : false;
    }
  };

  /// Add converter object to conversion service.
  virtual StatusCode addConverter(const CLID& clid);

 public:

  /// Get the global tag name
  /// The global tag can only be set from the job options
  virtual const std::string& globalTag ( );

  /// Convert from TimePoint class to cool::ValidityKey.
  virtual cool::IValidityKey timeToValKey(const TimePoint &time);
   
  /// Convert from cool::ValidityKey to TimePoint class.
  virtual TimePoint valKeyToTime(const cool::IValidityKey &key);

  /// Retrieve converter from list
  virtual IConverter* converter(const CLID& clid);

 private:

  /// Global tag name (can be set using the JobOptionsSvc)
  std::string          m_globalTag;

  /// Handle to the databas Access service
  ICondDBAccessSvc*    m_dbAccSvc;

  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

  /// Handle to the IDetDataSvc interface of the DetectorDataSvc
  IDetDataSvc*         m_detDataSvc;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


