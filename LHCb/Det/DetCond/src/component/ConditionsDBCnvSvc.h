//$Id: ConditionsDBCnvSvc.h,v 1.13 2005-05-12 16:17:32 marcocle Exp $
#ifndef DETCOND_CONDITIONSDBCNVSVC_H
#define DETCOND_CONDITIONSDBCNVSVC_H 1

/// Include files
#include "GaudiKernel/ConversionSvc.h"

/// Forward and external declarations
template <class TYPE> class SvcFactory;
class IDetDataSvc;
class IOpaqueAddress;
class ICondDBAccessSvc;

///---------------------------------------------------------------------------
/** @class ConditionsDBCnvSvc ConditionsDBCnvSvc.h Det/DetCond/ConditionsDBCnvSvc.h

    A conversion service for CERN-IT COOL (ex. CondDB) persistency.
    Allows to create and update condition data objects (i.e. DataObjects
    implementing IValidity).

    @author Marco Clemencic 
    @date November 2004
*///--------------------------------------------------------------------------

class ConditionsDBCnvSvc : public ConversionSvc {
  
  /// Only factories can access protected constructors
  friend class SvcFactory<ConditionsDBCnvSvc>;

 protected:
  
  /// Constructor
  ConditionsDBCnvSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor
  virtual ~ConditionsDBCnvSvc();

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

  inline std::vector<ICondDBAccessSvc*> &accessServices() { return m_dbAccSvcs; }
  inline const std::vector<ICondDBAccessSvc*> &accessServices() const { return m_dbAccSvcs; }

 private:

  /// List of all the names of the known databases. It is filled via the option
  /// ConditionsDBCnvSvc.CondDBAccessServices. If none is given, "CondDBAccessSvc" is used.
  std::vector<std::string>       m_dbAccSvcNames;

  /// Handles to the database Access services
  std::vector<ICondDBAccessSvc*> m_dbAccSvcs;

  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

  /// Handle to the IDetDataSvc interface of the DetectorDataSvc
  IDetDataSvc*         m_detDataSvc;

protected:

};

#endif    // DETCOND_CONDITIONSDBCNVSVC_H


