//$Id: CondDBCnvSvc.h,v 1.6 2006-07-14 09:27:33 marcocle Exp $
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
                                boost::shared_ptr<coral::AttributeList> &data,
                                std::string &descr, Gaudi::Time &since, Gaudi::Time &until, cool::ChannelId channel = 0);

  /// Retrieve the names of the children nodes of a FolderSet.
  virtual StatusCode getChildNodes (const std::string &path, std::vector<std::string> &node_names);

private:

  /// List of all the names of the known databases. It is filled via the option
  /// CondDBCnvSvc.CondDBReader. If none is given, "CondDBAccessSvc" is used.
  std::string    m_dbReaderName;

  /// Handles to the database Access services
  ICondDBReader* m_dbReader;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


