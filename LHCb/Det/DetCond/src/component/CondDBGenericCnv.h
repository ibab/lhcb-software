// $Id: CondDBGenericCnv.h,v 1.2 2005-02-09 08:30:54 marcocle Exp $
#ifndef DETCOND_CONDDBGENERICCNV_H 
#define DETCOND_CONDDBGENERICCNV_H 1

// Include files
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/TimePoint.h"
#include <string>
#include <functional>

// Forward and external declarations
class ISvcLocator;
class IConditionsDBCnvSvc;
class ICondDBAccessSvc;
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

  //  const std::string &globalTag() const;
  /**
   * Ask the global tag string to the ConditionsDBCnvSvc.
   * The tag string has to be used to get the objects from the database.
   * @return the tag string
   */
  const std::string &globalTag();

  /**
   * Set the validity of the DataObject if it inherits from IValidity.
   */
  void setObjValidity(TimePoint &since,
                      TimePoint &till,
                      DataObject *pObject);
  enum Object_Updatability_t { UPTODATE, NONVALID, FAILURE };
    
  /**
   * Check if the conditions to update an object are satisfied (event time and validity).
   */
  Object_Updatability_t checkUpdatability(DataObject* pObject);

  /// Pointer to the ConditionsDBCnvSvc.
  IConditionsDBCnvSvc *m_condDBCnvSvc;
  /// Pointer to the DetectorDataService.
  IDetDataSvc         *m_detDataSvc;
  /// Pointer to the CondDBAccessSvc;
  ICondDBAccessSvc    *m_dbAccSvc;
  
private:

};
#endif // DETCOND_CONDDBGENERICCNV_H
