// $Id: CopyAndStoreData.h,v 1.10 2007-10-24 15:37:25 jpalac Exp $
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

  //protected:

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
                               const std::string& to   ) 
  {
    verbose() << "try to get data container" << endmsg;

    if (exist<T>( from ) ) {
      const T* data = get<T>( from );
      verbose() << "now copy information" << endmsg;
      T* newData = new T(*data);
      put (newData, to );
      verbose() << "Data values set to\n" << *newData << "\n" << endmsg;
      return newData;
    } else {
      Warning("No data container found at "+ from, StatusCode::SUCCESS);
      return 0;
    } // if exist

  }
  //===========================================================================

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const std::string& from,
                               const std::string& to    ) 
  {
    debug() << "now store container for location " << from << endmsg;

    if (!exist<T>(from)) {
      debug() << "Container location does not exist" << endmsg;
      return 0;    
    } else {
      const T* data = get<T>( from );
      if (!data) {
        return 0;
      }
      T* clones = getOutputContainer<T>(to);

      verbose() << "copyKeyedContainer " << data->size() 
                << " elements from " << from 
                << " into " << to << ", size " << clones->size()
                << endmsg;
      return  copyKeyedContainer<T, ContainedItemCloner>(data, 
                                                         clones); 
    } // if !exist
  }

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const T* from, T* to    ) 
  {
    if (!from || !from ) return 0;
    verbose() << "copyContainer output container size " << to->size() 
              << endmsg;
    std::for_each(from->begin(), from->end(), 
                  ContainedItemCloner(to) );

    verbose() << "copyContainer copied # elements into output container: " 
              << to->size() << endmsg;

    return to;
  }

  template <class T, class itemCloner>
  T* cloneKeyedContainerItem(const T* item, 
                             const std::string& location)
  {
    typename T::Container* clones = getOutputContainer<typename T::Container>(location);
    if (0==clones) return 0;
    MicroDST::CloneKeyedContainerItem<T, itemCloner> cloner(clones);
    return cloner(item);
    
  }

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  const T* getStoredClone( const T* original    ) const
  {
    return getStoredClone<T>(original);
  }
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getStoredClone( const T* original    ) 
  {
    // get location of original, from that get location of clone,
    // search for it and return it if found.
    const std::string cloneLocation = 
      outputTESLocation(objectLocation(original->parent()));

    if (!exist<typename T::Container>(cloneLocation)) return 0;

    typename T::Container* clones = get<typename T::Container>(cloneLocation);
    // check what this returns if no object found.
    return clones->object(original->key() ); 
    
  }
  
  
  //===========================================================================
  
  //===========================================================================

  /**
   * Get the TES container in the TES location to be stored on the
   * MicroDST. 
   * @param locTES The address in the original TES
   * @return pointer to the container in the MicroDST TES
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getOutputContainer( const std::string& location ) 
  {
    if ( !exist<T>( location ) ) {
      T* container = new T();
      put(container, location);
    }
    return get<T>( location );     
  }
  
  //=========================================================================  

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
   * Most orignial locations start with /Event/<alg,Phys,MC, ..>/...
   *  - but not all, "/Event" is optional, some have it, some don't
   * insert the output-prefix to distinguish the microDST location
   * -> remove the "/Event" bit if found at beginning of routine
   * 
   * @author Ulrich kerzel
  */
  void getNiceLocationName(std::string& location);

private:

  std::string m_inputTESLocation;
  std::string m_outputPrefix;
  std::string m_fullOutputTESLocation;

  std::map<std::string, ObjectContainerBase*> m_containerMap; ///< map of local container - locations to store
  
};
#endif // COPYANDSTOREDATA_H
