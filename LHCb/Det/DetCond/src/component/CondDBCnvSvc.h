//$Id: CondDBCnvSvc.h,v 1.4 2005-09-20 11:43:44 cattanem Exp $
#ifndef DETCOND_CONDDBCNVSVC_H
#define DETCOND_CONDDBCNVSVC_H 1

/// Include files
#include "GaudiKernel/ConversionSvc.h"

#include "DetCond/ICondDBCnvSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class IDetDataSvc;
class IOpaqueAddress;
class ICondDBAccessSvc;

///---------------------------------------------------------------------------
/** @class CondDBCnvSvc CondDBCnvSvc.h

    A conversion service for CERN-IT COOL (ex. CondDB) persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Marco Clemencic 
    @date November 2004
*///--------------------------------------------------------------------------

class CondDBCnvSvc : public ConversionSvc,
                     virtual public ICondDBCnvSvc {
  
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

  // Overloaded from ICondDBCnvSvc

  /// Return a reference to the known list of access services.
  virtual std::vector<ICondDBAccessSvc*> &accessServices();
  /// Return a reference to the known list of access services. (const version)
  virtual const std::vector<ICondDBAccessSvc*> &accessServices() const;

  // Overloaded from IInterface
  
  /** Query interfaces of Interface
      @param riid       ID of Interface to be retrieved
      @param ppvUnknown Pointer to Location for interface pointer
  */
  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);

private:

  /// List of all the names of the known databases. It is filled via the option
  /// CondDBCnvSvc.CondDBAccessServices. If none is given, "CondDBAccessSvc" is used.
  std::vector<std::string>       m_dbAccSvcNames;

  /// Handles to the database Access services
  std::vector<ICondDBAccessSvc*> m_dbAccSvcs;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


