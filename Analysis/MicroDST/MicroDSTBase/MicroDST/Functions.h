// $Id: Functions.h,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
#ifndef MICRODST_FUNCTIONS_H 
#define MICRODST_FUNCTIONS_H 1

// Include files
#include <iostream>
#include <string>
#include <map>
#include <GaudiKernel/DataObject.h>
#include <GaudiKernel/IRegistry.h>

#include <MicroDST/Types.h>

/** @namespace MicroDST Functions MicroDST/Functions.h
 *  
 *
 *  Free helper functions
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-28
 */
namespace MicroDST {

  /**
   * Get the TES location of an object.
   * 
   * @param pObject Pointer to he onject on the TES
   * @param it's location, if available, otherwise meaningful error names.
   *
   * @author Ulrich Kerzel
   *
   */
  const std::string objectLocation(const DataObject* pObject);

  /**
   * Remove leading "/Event" from TES locations.
   *
   * @author Juan Palacios juancho@nikhef.nl
   *
   */
  const std::string niceLocationName(const std::string& location);

  template <class T>
  T* getContainer( const std::string& location,  
                   MicroDST::DataStore* dataStore)
  {
    std::cout << "hello from MicroDST::getContainer, location " 
              << location << std::endl;
    const std::string niceLocation = MicroDST::niceLocationName(location);
    std::cout << "MicroDST::getContainer: found niceLocation " << niceLocation << std::endl;
    MicroDST::DataStore::const_iterator iMap = dataStore->find(niceLocation);
    if (iMap != dataStore->end()) {
      std::cout << "MicroDST::getContainer: return existing container" << std::endl;;
      return dynamic_cast<T*>((*dataStore)[niceLocation]);
    } else {
      std::cout << "MicroDST::getContainer: Making a new local container with address " << niceLocation
                << std::endl;
      T* container = new T();
      (*dataStore)[niceLocation] = container;
      return dynamic_cast<T*>((*dataStore)[niceLocation]);
    }
  }


  /**
   * Get the container that should hold the clone of a DataObject
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  typename T::Container* getContainer(const T* item, 
                                      MicroDST::DataStore* dataStore) 
  {
    const std::string location( MicroDST::objectLocation( item->parent() ) );
    return  
      MicroDST::getContainer<typename T::Container>( location, dataStore );
  }
  


}
#endif // MICRODST_FUNCTIONS_H
