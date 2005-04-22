// $Id: CondDBGenericCnv.h,v 1.2 2005-04-22 14:09:31 marcocle Exp $
#ifndef DETCOND_CONDDBGENERICCNV_H 
#define DETCOND_CONDDBGENERICCNV_H 1

// Include files
#include <string>
#include <functional>

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/TimePoint.h"

#include "DetCond/ICondDBAccessSvc.h"

// Forward and external declarations
class ISvcLocator;
class IDetDataSvc;
class DataObject;

template <class TYPE> class CnvFactory;

/** @class CondDBGenericCnv CondDBGenericCnv.h DetCond/CondDBGenericCnv.h
 *  
 *  Generic converter for the ConditionsDBCnvSvc.  This generic converter
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
   *  <li> ConditionsDBCnvSvc
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
  
  /// Shortcut to the current tag used.
  inline const std::string &tag() const {return m_dbAccSvc->tag();}

  /**
   * Set the validity of the DataObject if it inherits from IValidity.
   */
  void setObjValidity(TimePoint &since, TimePoint &till, DataObject *pObject);

  /// Pointer to the DetectorDataService.
  IDetDataSvc         *m_detDataSvc;
  /// Pointer to the CondDBAccessSvc;
  ICondDBAccessSvc    *m_dbAccSvc;
  
private:

};
#endif // DETCOND_CONDDBGENERICCNV_H
