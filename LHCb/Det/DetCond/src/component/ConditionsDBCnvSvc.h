//$Id: ConditionsDBCnvSvc.h,v 1.7 2001-11-28 09:35:12 andreav Exp $
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
  virtual StatusCode queryInterface( const IID& riid, 
				     void** ppvInterface );  

 public:

  // Reimplemented from ConversionSvc

  /// Initialize the service
  virtual StatusCode initialize    ( );
  
  /// Finalize the service
  virtual StatusCode finalize      ( );
  
  /// Create a transient representation from another rep of this object.
  virtual StatusCode createObj     ( IOpaqueAddress* pAddress, 
				     DataObject*&    refpObject);
  
  /// Resolve the references of the created transient object.
  virtual StatusCode fillObjRefs   ( IOpaqueAddress* pAddress, 
				     DataObject* pObject);
  
  /// Update a transient representation from another rep of this object.
  virtual StatusCode updateObj     ( IOpaqueAddress* pAddress, 
				     DataObject*     pObject);

  /// Update the references of an updated transient object.
  virtual StatusCode updateObjRefs ( IOpaqueAddress* pAddress, 
				     DataObject*     pObject);

  /// Create an address using explicit arguments to identify a single object.
  virtual StatusCode createAddress ( unsigned char svc_type,
				     const CLID& clid,
				     const std::string* par, 
				     const unsigned long* ip,
				     IOpaqueAddress*& refpAddress);
  
 public:

  // Implementation of IConditionsDBCnvSvc.
  // Create/update condition DataObject not necessarily registered in the TDS.
  
  /// Create a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store.
  StatusCode createConditionData   ( DataObject*&         refpObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time,
				     const CLID&          classID,
				     const unsigned char& type );
  
  /// Create a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  StatusCode createConditionData   ( DataObject*&         refpObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time );
  
  /// Update a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store.
  StatusCode updateConditionData   ( DataObject*          pObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time,
				     const CLID&          classID,
				     const unsigned char& type );
  
  /// Update a condition DataObject by folder name, tag and time.
  /// This method does not register DataObject in the transient data store.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  StatusCode updateConditionData   ( DataObject*          pObject,
				     const std::string&   folderName,
				     const std::string&   tagName,
				     const ITime&         time );

  /// Decode classID and storage type from the folder description string
  StatusCode decodeDescription     ( const std::string&   description,
				     CLID&                classID,
				     unsigned char&       type);

  /// Encode classID and storage type into the folder description string
  StatusCode encodeDescription     ( const CLID&          classID,
				     const unsigned char& type,
				     std::string&         description);
  
  /// Get the global tag name
  /// The global tag can only be set from the job options
  virtual const std::string& globalTag ( );

  /// Handle to the ConditionsDBGate
  IConditionsDBGate* conditionsDBGate ( );

 private:

  /// Global tag name (can be set using the JobOptionsSvc)
  std::string          m_globalTag;

  /// Handle to the low-level gate to the ConditionsDB
  ConditionsDBGate*    m_conditionsDBGate;

  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

  /// Handle to the IDetDataSvc interface of the DetectorDataSvc
  IDetDataSvc*         m_detDataSvc;

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


