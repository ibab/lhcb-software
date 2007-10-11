// $Id: CopyAndStoreData.h,v 1.1 2007-10-11 17:56:55 jpalac Exp $
#ifndef COPYANDSTOREDATA_H 
#define COPYANDSTOREDATA_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class ObjectContainerBase;

/** @class CopyAndStoreData CopyAndStoreData.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-11
 */
class CopyAndStoreData : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CopyAndStoreData( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyAndStoreData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string m_inputTESLocation;
  std::string m_outputPrefix;
  std::string m_fullOutputTESLocation;

  std::map<std::string, ObjectContainerBase*> m_containerMap; ///< map of container - locations to store

protected:

  template <class T>
  StatusCode copyAndStoreData( const std::string& fromLocation,
                               const std::string& toLocation   );

  template <class T>
  T* getContainer( std::string locTES );

  inline const std::string& inputTESLocation() const 
  {
    return m_inputTESLocation;
  }

  inline const std::string& outputPrefix() const 
  {
    return m_outputPrefix;
  }

  inline const std::string& fullOutputTESLocation() const 
  {
    return m_fullOutputTESLocation;
  }

  inline std::string& fullOutputTESLocation() 
  {
    return m_fullOutputTESLocation;
  }
  
private:

  inline const std::map<std::string, ObjectContainerBase*>& containerMap() const 
  {
    return m_containerMap;
  }

  inline std::map<std::string, ObjectContainerBase*>& containerMap()
  {
    return m_containerMap;
  }
  /**
   * most orignial locations start with /Event/<alg,Phys,MC, ..>/...
   *  - but not all, "/Event" is optional, some have it, some don't
   * insert the output-prefix to distinguish the microDST location
   * -> remove the "/Event" bit if found at beginning of routine
  */
  void getNiceLocationName(std::string& location);
  
};
#endif // COPYANDSTOREDATA_H
