//$Id: ConditionsDBCnvSvc.h,v 1.9 2004-12-08 17:19:17 marcocle Exp $
#ifndef DETCOND_CONDITIONSDBCNVSVC_H
#define DETCOND_CONDITIONSDBCNVSVC_H 1

/// Include files
#include "DetCond/IConditionsDBCnvSvc.h"
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
class ConditionsDBGate;
template <class TYPE> class SvcFactory;
class IDetDataSvc;
class IOpaqueAddress;

///---------------------------------------------------------------------------
/** @class ConditionsDBCnvSvc ConditionsDBCnvSvc.h Det/DetCond/ConditionsDBCnvSvc.h

    A conversion service for CERN-IT CondDB persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Andrea Valassi 
    @date February 2001
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

  // Implementation of IConditionsDBCnvSvc.
  // Create/update condition DataObject not necessarily registered in the TDS.
  
  /// Create a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store,
  /// but may register TDS addresses for its children if needed (e.g. Catalog).
  /// The string storage type is discovered at runtime in the CondDB.
  /// The entry name identifies a condition amongst the many in the string.
  StatusCode createConditionData   ( DataObject*&         refpObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const std::string&   entryName,
				     const ITime&         time,
				     const CLID&          classID,
				     IRegistry*           entry = 0);

  /// Update a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store,
  /// but may register TDS addresses for its children if needed (e.g. Catalog).
  /// The string storage type is discovered at runtime in the CondDB.
  /// The entry name identifies a condition amongst the many in the string.
  StatusCode updateConditionData   ( DataObject*          pObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const std::string&   entryName,
				     const ITime&         time,
				     const CLID&          classID,
				     IRegistry*           entry = 0);
  
  /// Decode the string storage type from the folder description string
  StatusCode decodeDescription     ( const std::string&   description,
				     long&       type);

  /// Encode the string storage type into the folder description string
  StatusCode encodeDescription     ( const long& type,
				     std::string&         description);
  
  /// Get the global tag name
  /// The global tag can only be set from the job options
  virtual const std::string& globalTag ( );

  /// Handle to the ConditionsDBGate
  IConditionsDBGate* conditionsDBGate ( );

  /// Retrieve converter from list
  virtual IConverter* converter(const CLID& clid);

 private:

  /// Global tag name (can be set using the JobOptionsSvc)
  std::string          m_globalTag;

  /// Handle to the low-level gate to the ConditionsDB
  ConditionsDBGate*    m_conditionsDBGate;

  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

  /// Handle to the IDetDataSvc interface of the DetectorDataSvc
  IDetDataSvc*         m_detDataSvc;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


