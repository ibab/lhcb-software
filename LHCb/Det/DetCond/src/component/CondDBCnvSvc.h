//$Id: CondDBCnvSvc.h,v 1.11 2008-06-26 14:22:45 marcocle Exp $
#ifndef DETCOND_CONDDBCNVSVC_H
#define DETCOND_CONDDBCNVSVC_H 1

/// Include files
#include "GaudiKernel/ConversionSvc.h"

#include "DetCond/ICondDBReader.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class IDetDataSvc;
class IOpaqueAddress;

///---------------------------------------------------------------------------
/** @class CondDBCnvSvc CondDBCnvSvc.h

    A conversion service for CERN-IT COOL (ex. CondDB) persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Marco Clemencic 
    @date November 2004
*///--------------------------------------------------------------------------

class CondDBCnvSvc : public ConversionSvc,
                     virtual public ICondDBReader {
  
  /// Only factories can access protected constructors
  friend class SvcFactory<CondDBCnvSvc>;

protected:
  
  /// Constructor
  CondDBCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~CondDBCnvSvc();

public:

  // Overloaded from ConversionSvc

  /// Initialize the service
  virtual StatusCode initialize();
  
  /// Finalize the service
  virtual StatusCode finalize();

  using ConversionSvc::createAddress;
  /// Create an address using explicit arguments to identify a single object.
  virtual StatusCode createAddress (long svc_type,
				     const CLID& clid,
				     const std::string* par, 
				     const unsigned long* ip,
				     IOpaqueAddress*& refpAddress );

public:

  /// Retrieve converter from list
  virtual IConverter* converter(const CLID& clid);

  // --------- IInterface implementation
  
  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

  // --------- ICondDBReader implementation

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Try to retrieve an object from the Condition DataBase. If path points to a FolderSet,
  /// channel and when are ignored and data is set ot NULL.
  virtual StatusCode getObject (const std::string &path, const Gaudi::Time &when,
                                DataPtr &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, const std::string &channel);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);

  /// Retrieve the names of the children nodes of a FolderSet divided in folders and foldersets.
  virtual StatusCode getChildNodes (const std::string &path,
                                    std::vector<std::string> &folders,
                                    std::vector<std::string> &foldersets);

  /// Tells if the path is available in the database.
  virtual bool exists(const std::string &path);
  
  /// Tells if the path (if it exists) is a folder.
  virtual bool isFolder(const std::string &path);
  
  /// Tells if the path (if it exists) is a folderset.
  virtual bool isFolderSet(const std::string &path);

  // --------- ICondDBInfo implementation

  /** Get the current default database tags
   *  @param  tags vector of DB name, tag pairs. Empty if DB not available
   */ 
  virtual void defaultTags( std::vector<LHCb::CondDBNameTagPair>& tags) const;

private:

  /// List of all the names of the known databases. It is filled via the option
  /// CondDBCnvSvc.CondDBReader. If none is given, "CondDBAccessSvc" is used.
  std::string    m_dbReaderName;

  /// Handles to the database Access services
  ICondDBReader* m_dbReader;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


