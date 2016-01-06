#ifndef DETCOND_CONDDBGENERICCNV_H
#define DETCOND_CONDDBGENERICCNV_H 1

// Include files
#include <string>
#include <functional>

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Time.h"

#include "DetCond/ICondDBReader.h"

#include "CoolKernel/types.h"

// Forward and external declarations
class ISvcLocator;
class IDetDataSvc;
class DataObject;

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
   * @return StatusCode::SUCCESS if the event time was defined.
   */
  StatusCode eventTime(Gaudi::Time &time) const;

  /**
   * Set the validity of the DataObject if it inherits from IValidity.
   */
  void setObjValidity(Gaudi::Time &since, Gaudi::Time &till, DataObject *pObject);

  /// Pointer to the DetectorDataService.
  SmartIF<IDetDataSvc> m_detDataSvc;
  /// Pointer to the ICondDBReader interface;
  SmartIF<ICondDBReader> m_condDBReader;

  /**
   * Get an object from the Conditions DB. It tries all the CondDBReaders
   * known by CondDBCnvSvc before returing a failure code.
   * @param[in]  path  the path inside the CondDB
   * @param[in]  channel  CondDB channel id
   * @param[out] obj   shared pointer to the COOL object
   * @param[out] descr folder description string (used to know the storage type by RelyConverter)
   * @param[out] since start of the IOV
   * @param[out] until end of the IOV
   * The IOV is inside the object itself as two cool::ValidityKey, the since and until are
   * used to avoid the conversion outside this method.<BR>
   * If the path point to a FolderSet, channel is ignored and the boost::shared_ptr obj is
   * set to NULL.
   */
  StatusCode getObject(const std::string &path, const cool::ChannelId &channel,
                       ICondDBReader::DataPtr &obj,
                       std::string &descr, Gaudi::Time &since, Gaudi::Time &until);

  /// Method kept for backward compatibility
  inline StatusCode getObject(const std::string &path,
                              ICondDBReader::DataPtr &obj,
                              std::string &descr, Gaudi::Time &since, Gaudi::Time &until)
  {
    return getObject(path,0,obj,descr,since,until);
  }

  /// Find the children nodes of a given FolderSet path.
  /// When using multiple databases, only the first one which contains
  /// this folderset is used, so it must have at least dummy entries for each sub-node
  /// (to be changed in the future).
  StatusCode getChildNodes(const std::string &path,std::vector<std::string> &node_names);

private:

};
#endif // DETCOND_CONDDBGENERICCNV_H
