// $Id: CopyAndStoreData.h,v 1.13 2007-10-24 17:36:39 jpalac Exp $
#ifndef COPYANDSTOREDATA_H 
#define COPYANDSTOREDATA_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IRegistry.h>
// Local
#include "MicroDST/Functors.hpp"
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

  /**
   * Copy an object of type T from the TES onto another TES location.
   * Uses the copy constructor of type T for copying.
   * 
   * @param from The TES location of the object to be copied
   * @param to   The TES location where the copy should be stored
   * @return     Const pointer to the cloned object.
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @author Ulrich Kerzel
   */
  template <class T>
  const T* copyAndStoreObject( const std::string& from,
                               const std::string& to   );
  

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const std::string& from,
                               const std::string& to    );
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* cloneContainerToLocalStore( const std::string& from ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const T* from, T* to    ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class itemCloner>
  T* cloneKeyedContainerItem( const T* item );

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class itemCloner>
  T* cloneKeyedItemToLocalStore( const T* item );

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  const T* getStoredClone( const T* original    ) const;
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getStoredClone( const T* original    ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getLocalClone( const T* original    ) ;
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getLocalContainer( const std::string& location );
  
  /**
   * Get the TES container in the TES location to be stored on the
   * MicroDST. 
   * @param locTES The address in the original TES
   * @return pointer to the container in the MicroDST TES
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getOutputContainer( const std::string& location ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */

  StatusCode copyLocalStoreToTES();

  /**
   * Get the TES location of an object.
   * 
   * @param pObject Pointer to he onject on the TES
   * @param it's location, if available, otherwise meaningful error names.
   *
   * @author Ulrich Kerzel
   *
   */
  inline static const std::string objectLocation(const DataObject* pObject)
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

  inline const std::string& inputTESLocation() const 
  {
    return m_inputTESLocation;
  }

  inline std::string& inputTESLocation()
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

  inline void setInputTESLocation(const std::string& newLocation) 
  {
    this->inputTESLocation() = newLocation;
    setFullOutputTESLocation();
  }

  inline void setFullOutputTESLocation() 
  {
    this->fullOutputTESLocation() = "/Event/"+ this->outputPrefix() + 
      "/" + this->inputTESLocation();
  }

  inline const std::string outputTESLocation(const std::string& inputLocation)
  {
    std::string tmp(inputLocation);
    getNiceLocationName(tmp);
    return "/Event/"+ this->outputPrefix() + "/" + tmp;
  }


private:

  /**
   * Remove leading "/Event" from TES locations.
   * 
   * @author Ulrich kerzel
  */
  void getNiceLocationName(std::string& location);

  /**
   * Remove leading "/Event" from TES locations.
   *
   * @author Juan Palacios juancho@nikhef.nl
   *
   */
  const std::string getNiceLocationName(const std::string& location) const;
  
  typedef std::map<std::string, ObjectContainerBase*> LocalDataStore;

  inline LocalDataStore& localDataStore() 
  {
    return m_containerMap;
  }

private:

  std::string m_inputTESLocation;
  std::string m_outputPrefix;
  std::string m_fullOutputTESLocation;



  LocalDataStore m_containerMap; ///< map of local container - locations to store
  
};
// templated method implementations.
#include "CopyAndStoreData.icpp"
#endif // COPYANDSTOREDATA_H
