// $Id: CopyAndStoreData.h,v 1.6 2007-10-22 13:33:47 jpalac Exp $
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
    CloneKeyedContainerItem<T, itemCloner> cloner(clones);
    return cloner(item);
    
  }
  
  //===========================================================================
  /**
   *
   * Clone an item into a container of type T. 
   * The functor checks if an item with the same key already exists.
   * Requirements:
   *
   * Contained type T:
   * Must export container type as T::Container. 
   * T::Container must have a method 
   * insert(T::value_type, type of T::value_type::key() )
   * Must have a method T* object( type of T::key() )
   *
   * Type T must have method T::key()
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T, class itemCloner>
  struct CloneKeyedContainerItem
  {
    CloneKeyedContainerItem(typename T::Container*& to) 
      : m_to(to) { }
    
    T* operator() ( const T* item )
    {
      if ( !m_to->object( item->key() ) ) {
        T* clonedItem = itemCloner::clone(item);
        m_to->insert( clonedItem, item->key()) ;
        return clonedItem;
      }
      return m_to->object( item->key() );
    }
    
    typename T::Container* m_to;
    
  };
  
  //===========================================================================

  /**
   */
  template <class T>
  class BasicItemCloner 
  {
  public:
    static T* clone(const T* item) 
    {
      return item->clone();
    }
  };
  
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

  inline const std::string outputTESLocation(std::string& inputLocation)
  {
    getNiceLocationName(inputLocation);
    return "/Event/"+ this->outputPrefix() + inputLocation;
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
