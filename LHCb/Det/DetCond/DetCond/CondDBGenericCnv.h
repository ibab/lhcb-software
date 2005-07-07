// $Id: CondDBGenericCnv.h,v 1.6 2005-07-07 12:23:15 marcocle Exp $
#ifndef DETCOND_CONDDBGENERICCNV_H 
#define DETCOND_CONDDBGENERICCNV_H 1

// Include files
#include <string>
#include <functional>

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/TimePoint.h"

#include "DetDesc/ICondDBAccessSvc.h"

#include "CoolKernel/types.h"

// Forward and external declarations
class ISvcLocator;
class IDetDataSvc;
class DataObject;
class ICondDBCnvSvc;
namespace pool {
  class AttributeList;
}


template <class TYPE> class CnvFactory;

/** @class CondDBGenericCnv CondDBGenericCnv.h DetCond/CondDBGenericCnv.h
 *  
 *  Generic converter for the CondDBCnvSvc.  This generic converter
 *  provides common functions to access the CondDB in order to make it 
 *  easier to write specific converters.
 *
 *  @author Marco CLEMENCIC
 *  @date   December 2004
 */
class CondDBGenericCnv: public Converter {
public: 
  /**
   * Initializes the converter.
   * Here the pointers to common services are taken:
   *  <ul>
   *  <li> CondDBCnvSvc
   *  <li> DetectorDataSvc
   *  </ul>
   *  @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * Finalizes the converter.
   * It releases the pointers to the taken services.
   *  @return status depending on the completion of the call
   */
  virtual StatusCode finalize();

  /**
   * Accessor to the StorageType value
   * @return the storage type for this object
   */
  static long storageType() {
    return CONDDB_StorageType;
  }

  /**
   * Accessor to the StorageType value
   * @return the storage type for this object
   */
  virtual long repSvcType() const  {
    return CONDDB_StorageType;
  }

protected:

  /// Standard constructor
  CondDBGenericCnv(ISvcLocator* svc,const CLID& clid); 

  virtual ~CondDBGenericCnv( ); ///< Destructor

  /**
   * Ask to the DetectorDataSvc the curren event time.
   * @return StatusCode::SUCCECC if the event time was defined.
   */
  StatusCode eventTime(TimePoint &time) const;

  /**
   * Set the validity of the DataObject if it inherits from IValidity.
   */
  void setObjValidity(TimePoint &since, TimePoint &till, DataObject *pObject);

  /// Pointer to the DetectorDataService.
  IDetDataSvc         *m_detDataSvc;
  /// Pointer to the ICondDBCnvSvc interface;
  ICondDBCnvSvc       *m_condDBCnvSvc;
  
  /**
   * Get an object from the Conditions DB. It tries all the CondDBAccessSvcs
   * known by CondDBCnvSvc before returing a failure code.
   * @input 
   * @param path the path inside the CondDB
   * @output
   * @param obj shared pointer to the COOL object
   * @param descr folder description string (used to know the storage type by RelyConverter)
   * @param since start of the IOV
   * @param until end of the IOV
   * The IOV is inside the object itself as two cool::ValidityKey, the since and until are 
   * used to avoid the conversion outside this method.
   */
  StatusCode getObject(const std::string &path, boost::shared_ptr<pool::AttributeList> &obj,
                       std::string &descr, TimePoint &since, TimePoint &until);

private:

};
#endif // DETCOND_CONDDBGENERICCNV_H
