//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/src/component/ConditionsDBCnvSvc.h,v 1.1.1.1 2001-09-14 15:07:21 andreav Exp $
#ifndef DETCOND_CONDITIONSDBCNVSVC_H
#define DETCOND_CONDITIONSDBCNVSVC_H 1

/// Include files
#include "DetCond/IConditionsDBCnvSvc.h"
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
class ConditionsDBGate;
template <class TYPE> class SvcFactory;
class IOpaqueAddress;

///---------------------------------------------------------------------------
/** @class ConditionsDBCnvSvc ConditionsDBCnvSvc.h Det/DetCond/ConditionsDBCnvSvc.h

    A conversion service for CERN-IT CondDB persistency.

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

  /// Initialise the service
  virtual StatusCode initialize  ( );
  
  /// Finalise the service
  virtual StatusCode finalize    ( );
  
  /// Create a transient representation from another rep of this object.
  virtual StatusCode createObj   ( IOpaqueAddress* pAddress, 
				   DataObject*&    refpObject);
  
  /// Update a transient representation from another rep of this object.
  virtual StatusCode updateObj   ( IOpaqueAddress* pAddress, 
				   DataObject*     pObject);

 public:

  // Implementation of IConditionsDBCnvSvc
  // Handling of arbitrary condition data, not registered in the TDS
  
  /// Create a ConditionData object by folder name, tag and time.
  /// This method does not register the ConditionData in the TDS.
  StatusCode createConditionData ( ConditionData*&      refpCdata,
				   const std::string&   folderName,
				   const std::string&   tagName,
				   const ITime&         time,
				   const CLID&          classID,
				   const unsigned char& type );
  
  /// Create a ConditionData object by folder name, tag and time.
  /// This method does not register the ConditionData in the TDS.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  StatusCode createConditionData ( ConditionData*&      refpCdata,
				   const std::string&   folderName,
				   const std::string&   tagName,
				   const ITime&         time );

  /// Update a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  StatusCode updateConditionData ( ConditionData*       pCdata,
				   const std::string&   folderName,
				   const std::string&   tagName,
				   const ITime&         time,
				   const CLID&          classID,
				   const unsigned char& type );

  /// Update a ConditionData object by folder name, tag and time.
  /// This method does not register ConditionData in the transient data store.
  /// If not specifed, type and clID are discovered at runtime in the CondDB.
  StatusCode updateConditionData ( ConditionData*       pCdata,
				   const std::string&   folderName,
				   const std::string&   tagName,
				   const ITime&         time );

  /// Decode classID and storage type from the folder description string
  StatusCode decodeDescription   ( const std::string&   description,
				   CLID&                classID,
				   unsigned char&       type);

  /// Encode classID and storage type into the folder description string
  StatusCode encodeDescription   ( const CLID&          classID,
				   const unsigned char& type,
				   std::string&         description);
				   
  /// (TEMPORARY?) Handle to the ConditionsDBGate
  IConditionsDBGate* conditionsDBGate();

 private:
  
  // Private internal methods

  /// Decode an IOpaqueAddress as a ConditionsDBAddress
  StatusCode i_decodeAddress     ( IOpaqueAddress*      pAddress, 
				   std::string&         folderName,
				   std::string&         tagName, 
				   const ITime*&        evtTime,
				   CLID&                classID,
				   unsigned char&       type);
  
 private:

  /// Handle to the low-level CondDB gate
  ConditionsDBGate*    m_conditionsDBGate;

  /// IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


