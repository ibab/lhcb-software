// $Id: CondDBAccessSvc.h,v 1.1 2005-02-09 08:30:54 marcocle Exp $
#ifndef COMPONENT_CONDDBACCESSSVC_H 
#define COMPONENT_CONDDBACCESSSVC_H 1

// Include files
#include <GaudiKernel/Service.h>
#include "DetCond/ICondDBAccessSvc.h"

//#include "CoolKernel/IDatabase.h"

// Forward declarations
template <class TYPE> class SvcFactory;

/** @class CondDBAccessSvc CondDBAccessSvc.h component/CondDBAccessSvc.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-01-11
 */

class CondDBAccessSvc: public virtual Service,
                       public virtual ICondDBAccessSvc {
public: 

  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  /// Initilize COOL (CondDB) Access Layer Service
  virtual StatusCode initialize();
  /// Finalize Service
  virtual StatusCode finalize();

  // Utilities:
  virtual cool::IDatabasePtr& database() { return m_db; }
  
  /*
  virtual cool::IObjectPtr getCondDBObject(const std::string &path,
                                           const cool::IValidityKey &timePoint,
                                           const cool::IChannelId &channelId=0);
  */

protected:
  /// Standard constructor
  CondDBAccessSvc(const std::string& name, ISvcLocator* svcloc); 

  virtual ~CondDBAccessSvc( ); ///< Destructor

private:
  // Properties

  /// Property CondDBAccessSvc.HostName: name of the database server
  std::string m_dbHostName;
  
  /** Property CondDBAccessSvc.User: name of the user needed to connect
      to the database server */
  std::string m_dbUser;

  /// Property CondDBAccessSvc.Password: password needed to connect to the DB server
  std::string m_dbPassword;
  
  /// Property CondDBAccessSvc.HidePassword: hide password in log output (default = true)
  bool m_hidePasswd;
  
  /// Property CondDBAccessSvc.Database: name of the database to use
  std::string m_dbName;

  /// Property CondDBAccessSvc.Schema: schema to use
  std::string m_dbSchema;

  /// Property CondDBAccessSvc.DefaultTAG: which tag to use if none is specified
  std::string m_dbTAG;

  /** Property CondDBAccessSvc.BackEnd: which db technology to use (oracle|mysql|sqlite) */
  std::string m_dbBackEnd;

  /** Property CondDBAccessSvc.PurgeDB: Drop and recreate the database during initialization.
      (FIX-ME: There is no way to test if the database is there or in a good shape) */
  bool m_recreateDB;

  /** Property CondDBAccessSvc.RunTest: */
  bool m_test;

  /// Shared pointer to the COOL database instance
  cool::IDatabasePtr m_db;

  /// Generate the string used to connect to COOL.
  std::string i_connection_uri() const;

  /// Connect to the COOL database. It sets 'm_db'.
  StatusCode i_openConnention();

  // Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBAccessSvc>;

};
#endif // COMPONENT_CONDDBACCESSSVC_H
