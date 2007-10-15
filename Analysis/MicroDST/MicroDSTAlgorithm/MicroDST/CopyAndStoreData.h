// $Id: CopyAndStoreData.h,v 1.3 2007-10-15 16:23:33 jpalac Exp $
#ifndef COPYANDSTOREDATA_H 
#define COPYANDSTOREDATA_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IRegistry.h>

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

protected:

  /**
   * Copy an object of type T from the TES onto another TES location.
   * Uses the copy constructor of type T for copying.
   * 
   * @param from The TES location of the object to be copied
   * @param to   The TES location where the copy should be stored
   * @return     Status code
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @author Ulrich Kerzel
   */
  template <class T>
  StatusCode copyAndStoreObject( const std::string& from,
                                 const std::string& to   ) 
  {
    verbose() << "try to get data container" << endmsg;

    if (exist<T>( from ) ) {
      const T* data = get<T>( from );
      verbose() << "now copy information" << endmsg;
      T* newData = new T(*data);
      put (newData, to );
      verbose() << "Data values set to\n" << *newData << "\n" << endmsg;
    } else {
      Warning("No data container found at "+ from, StatusCode::SUCCESS);
    } // if exist

    return StatusCode::SUCCESS;
  }
  //===========================================================================

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  StatusCode copyAndStoreContainer( const std::string& from,
                                    const std::string& to   ) 
  {
    debug() << "now store container for location " << from << endmsg;

    if (!exist<T>(from)) {
      debug() << "Container location does not exist" << endmsg;
      return StatusCode::FAILURE;    
    } else {
      const T* data = get<T>( from );
      if (!data) {
        return StatusCode::FAILURE;
      }
      verbose() << "got # elements in container: " << data->size() << endmsg;
      T* clones = getOutputContainer<T>();
      return cloneContainer<T>(data, clones);
    } // if !exist
  }
  //===========================================================================

  /**
   * Clone the contents of a container and put into another one.
   * Designed for keyed containers.
   * Contained types must have a clone() method.
   * Container must have an object(key), an insert and a 
   * value_type typedef.
   * The method checks if an object of type T::value_type already
   * exists in "clones". If not, the object is cloned and inserted.
   *
   * @author Juan Palaicos juanch@nikhef.nl
   * @date 15-10-2007
   */
  template <class T> 
  StatusCode cloneContainer(const T* data, T* clones) 
  {
    for (typename T::const_iterator i = data->begin(); 
         i != data->end(); 
         ++i) {
      typename T::value_type item = clones->object( (*i)->key() );
      if (!item) {
        item = (*i)->clone();
        clones->insert(item, (*i)->key());
      }
    }
    return StatusCode::SUCCESS;
  }
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
  T* getOutputContainer() 
  {
    const std::string& location = fullOutputTESLocation();
  
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
  inline const std::string objectLocation(const DataObject* pObject) const
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
  
};
#endif // COPYANDSTOREDATA_H
