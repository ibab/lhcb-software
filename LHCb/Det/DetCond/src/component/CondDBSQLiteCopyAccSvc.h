// $Id: CondDBSQLiteCopyAccSvc.h,v 1.1 2007-04-20 14:41:09 marcocle Exp $
#ifndef COMPONENT_CONDDBSQLITECOPYACCSVC_H 
#define COMPONENT_CONDDBSQLITECOPYACCSVC_H 1

// Include files
#include "CondDBAccessSvc.h"

/** @class CondDBSQLiteCopyAccSvc CondDBSQLiteCopyAccSvc.h component/CondDBSQLiteCopyAccSvc.h
 *
 *  Extension to CondDBAccessSvc SQLite specific. The original SQLite file is copied to
 *  a different direcory before being used. This is particularily helpful when the original
 *  file is accessible only via NFS (see http://www.sqlite.org/faq.html#q7 for details).
 *
 *  @author Marco Clemencic
 *  @date   2007-03-22
 */
class CondDBSQLiteCopyAccSvc: public CondDBAccessSvc {

public:

  /// Initilize the service
  virtual StatusCode initialize();

  /// Return the connection string used to connect to the database.
  virtual const std::string &connectionString() const;


protected:
  /// Standard constructor
  CondDBSQLiteCopyAccSvc( const std::string& name, ISvcLocator* svcloc ); 

  virtual ~CondDBSQLiteCopyAccSvc( ); ///< Destructor

private:

  /// Path to the original file
  std::string m_source_path;
  /// Path to destination file
  std::string m_dest_path;
  /// COOL database name
  std::string m_dbname;

  /// Whether to overwrite the destination file.
  bool m_force_copy;
  /// Whether ingore copy error (e.g. if the destination file exists, try to use it)
  bool m_ignore_copy_error;

  /// Needed to avoid interference with the connection string set by CondDBAccessSvc
  /// standard options (we need to overwrite it).
  std::string m_sqlite_connstring;

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<CondDBSQLiteCopyAccSvc>;
  
};
#endif // COMPONENT_CONDDBSQLITECOPYACCSVC_H
