//$Id: BootDetectorStore.h,v 1.2 2001-11-29 13:56:24 andreav Exp $
#ifndef DETCONDEXAMPLE_BOOTDETECTORSTORE_H
#define DETCONDEXAMPLE_BOOTDETECTORSTORE_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Type definitions
#include "GaudiKernel/ClassID.h"

//Forward declarations
class IAddressCreator;
class IDataManagerSvc;

///---------------------------------------------------------------------------
/** @class BootDetectorStore BootDetectorStore.h DetCondExample/BootDetectorStore.h

    Simple algorithm to bootstrap the detector data store.
    Emulates what will eventually be done by the XmlCnvSvc.

    @author Andrea Valassi 
    @date November 2001
*///--------------------------------------------------------------------------

class BootDetectorStore : public Algorithm {

 public:

  /// Constructor
  BootDetectorStore ( const std::string& name, ISvcLocator* pSvcLocator ); 
  
  // Algorithm standard methods
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:

  // Private helper methods

  /// Convert the folder name in the CondDB to the transient data store path
  StatusCode i_getNameInCondDB   ( std::string&       folderName,
				   const std::string& path );
  
  /// Convert the transient data store path to the folder name in the CondDB
  StatusCode i_getNameInStore    ( std::string&       path,
				   const std::string& folderName );
  
  /// Register a condition in the detector store by CondDB folder name.
  /// In the address, specify the global tag (owned by the ConditionsDBCnvSvc),
  /// the technology type for the strings stored in the CondDB and the classID.
  StatusCode i_registerCondition ( const std::string&   folderName,
				   const CLID&          classID,
				   const unsigned char& type );
  
  /// Register a condition in the detector store by CondDB folder name.
  /// If not specifed, type and classID are discovered at runtime in the CondDB
  StatusCode i_registerCondition ( const std::string&   folderName );
  
 private:

  // Private data members

  /// TDS root path for the naming convention (default is "/dd")
  std::string m_conditionStoreRoot;

  /// Handle to the IDataManagerSvc interface of the detector data service
  IDataManagerSvc* m_detDataManager;

  /// Handle to the IAddressCreator interface of the ConditionsDBCnvSvc
  IAddressCreator* m_conditionsDBCnvSvc;

};

#endif    // DETCONDEXAMPLE_BOOTDETECTORSTORE_H
