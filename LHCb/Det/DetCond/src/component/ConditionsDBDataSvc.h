//$Id: ConditionsDBDataSvc.h,v 1.4 2001-11-28 09:28:32 andreav Exp $
#ifndef DETCOND_CONDITIONSDBDATASVC_H
#define DETCOND_CONDITIONSDBDATASVC_H 1

/// Base classes
#include "ConditionDataSvc.h"
#include "DetCond/IConditionsDBDataSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class IAddressCreator;
class IConditionsDBCnvSvc;
class IOpaqueAddress;
class IRegistry;

///---------------------------------------------------------------------------
/** @class ConditionsDBDataSvc ConditionsDBDataSvc.h Det/DetCond/ConditionsDBDataSvc.h

    A data service for DataObjects corresponding to condition data 
    from the CERN-IT ConditionsDB.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ConditionsDBDataSvc : virtual public ConditionDataSvc, 
			    virtual public IConditionsDBDataSvc
{
  
  /// Only factories can access protected constructors
  friend class SvcFactory<ConditionsDBDataSvc>;

 protected:
  
  /// Constructor
  ConditionsDBDataSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionsDBDataSvc();

 public:
  
  // Reimplemented from IInterface

  /// Query the interface of the service
  virtual StatusCode queryInterface( const IID& riid, 
				     void** ppvInterface );  

 public:

  // Overloaded DataSvc methods

  /// Initialize the service
  virtual StatusCode initialize();

  /// Finalize the service
  virtual StatusCode finalize();
  
 public:
  
  // Implementation of the IConditionsDBDataSvc interface

  /// Convert the folder name in the CondDB to the transient data store path
  StatusCode getNameInCondDB        ( std::string&       folderName,
				      const std::string& path );
  
  /// Convert the transient data store path to the folder name in the CondDB
  StatusCode getNameInStore         ( std::string&       path,
				      const std::string& folderName );
  
  /// Create a valid DataObject by folder name (for default tag and key),
  /// then load it in the TDS using the implicit naming convention:
  /// if the DataObject exists already, update it instead (if necessary).
  /// Specify the clID of the DataObject and the technology type 
  /// for the strings stored in the CondDB 
  StatusCode retrieveValidCondition  ( DataObject*&         refpObject,
				       const std::string&   folderName,
				       const CLID&          classID,
				       const unsigned char& type       );
  
  /// If not specifed, type and classID are discovered at runtime in the CondDB
  StatusCode retrieveValidCondition  ( DataObject*&         refpObject,
				       const std::string&   folderName );
  
 private:

  // Private internal methods

  /// Retrieve a directory creating all intermediate directories if necessary
  StatusCode i_mkdir ( const std::string& pathName,
		       IRegistry*&        entry     );

 private:

  // Private data members

  /// TDS root path for the naming convention (set equal to m_root)
  std::string      m_conditionStoreRoot; // Default: m_root = "/dd"  

  /// Handle to the IAddressCreator interface of the ConditionsDBCnvSvc
  IAddressCreator* m_conditionsDBCnvSvc;

};

#endif    // DETCOND_CONDITIONSDBDATASVC_H


