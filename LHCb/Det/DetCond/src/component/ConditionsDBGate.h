//$Id: ConditionsDBGate.h,v 1.3 2002-03-01 11:27:15 andreav Exp $
#ifndef DETCOND_CONDITIONSDBGATE_H
#define DETCOND_CONDITIONSDBGATE_H 1

// Base classes
#include "DetCond/IConditionsDBGate.h"
#include "GaudiKernel/Service.h"

// Type definition
#include "ConditionsDB/CondDBKey.h"

// Forward and external declarations
class ConditionsDBCnvSvc;
class ICondDBMgr;
class ICondDBDataAccess;
class ICondDBFolderMgr;
class ITime;

///---------------------------------------------------------------------------
/** @class ConditionsDBGate ConditionsDBGate.h Det/DetCond/ConditionsDBGate.h

    A low-level gate to the ConditionsDB for management and data access.

    @author Andrea Valassi 
    @date August 2001
*///--------------------------------------------------------------------------

class ConditionsDBGate : public Service, 
			 virtual public IConditionsDBGate
{
  
  /// This service is owned by the ConditionsDBCnvSvc
  friend class ConditionsDBCnvSvc;

 protected:
  
  /// Constructor
  ConditionsDBGate( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionsDBGate();

 public:
  
  // Reimplemented from IInterface

  /// Query the interface of the service
  virtual StatusCode queryInterface( const IID& riid, 
				     void** ppvInterface );  

 public:

  // Reimplemented from Service

  /// Initialise the service
  virtual StatusCode initialize();
  
  /// Finalise the service
  virtual StatusCode finalize();
  
 public:

  // Implementation of the IConditionsDBGate interface

  /// Read range and data of a CondDBObject by folder name, tag and time
  StatusCode readCondDBObject      ( ITime&              refValidSince,
				     ITime&              refValidTill,
				     std::string&        data,
				     const std::string&  folderName,
				     const std::string&  tagName,
				     const ITime&        time);

  /// Read the description of a folder in the CondDB
  StatusCode readCondDBFolder      ( std::string&        description,
				     const std::string&  folderName );
  
  /// (TEMPORARY?) Handle to the CondDBMgr for arbitrary manipulations
  inline ICondDBMgr* condDBManager ( ) { return m_condDBmgr; };

 private:

  // Private internal methods

  /// Find a CondDB object in the CondDB by folder name, tag and key
  StatusCode i_findCondDBObject    ( ICondDBObject*&     oblock,
				     const std::string&  folderName,
				     const std::string&  tagName,
				     const CondDBKey&    key);
  
  /// Build the technology-specific init string to access the database
  StatusCode i_buildCondDBInfo ( std::string& condDBInfo );

  /// Convert CondDBKey to ITime
  StatusCode i_convertToITime ( ITime& refTime, const CondDBKey& key );

  /// Convert ITime to CondDBKey
  StatusCode i_convertToKey   ( CondDBKey& key, const ITime& refTime );

 private:

  // Private data members: CERN-IT CondDB abstract interface.

  /// CondDB Manager
  ICondDBMgr*        m_condDBmgr;

  /// CondDB Data Access
  ICondDBDataAccess* m_condDBDataAccess;

  /// CondDB Folder Manager
  ICondDBFolderMgr*  m_condDBFolderMgr;

  // Private data members specific to the ConditionsDB implementation:
  // full path to the database, which can be set using the JobOptionsSvc.

  /// Database user name
  std::string        m_condDBUser;

  /// Database user password
  std::string        m_condDBPswd;

  /// Database host name
  std::string        m_condDBHost;

  /// Database ConditionsDB name
  std::string        m_condDBName;

};

#endif    // DETCOND_CONDITIONSDBGATE_H

