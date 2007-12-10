// $Id: Functors.hpp,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
#ifndef MICRODST_FUNCTORS_HPP 
#define MICRODST_FUNCTORS_HPP 1

// Include files
#include "MicroDST/Functions.h"
/** @namespace MicroDST Functors.hpp MicroDST/Functors.hpp
 *  
 *
 *  Collection of useful functors to do with cloning and 
 *  storing clones into TES locations.
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-24
 */
namespace MicroDST {

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


  /**
   *
   * Clone an item into a container of type T residing in a lodal datastore.
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
   * @date 28-11-2007
   */
  template <class T, class itemCloner>
  struct CloneKeyedContainerItemToStore
  {
    CloneKeyedContainerItemToStore() 
    {
    }
    
    CloneKeyedContainerItemToStore( MicroDST::DataStore* store,
                                    const itemCloner& cloner) 
      : 
      m_store(store),
      m_cloner(cloner)
    { }

    explicit CloneKeyedContainerItemToStore( MicroDST::DataStore* store ) 
      : 
      m_store(store),
      m_cloner()
    { }

    T* operator() ( const T* item )
    {
      typename T::Container* clones = 
        MicroDST::getContainer<T>( item, m_store );

      if ( !clones->object( item->key() ) ) {
        T* clonedItem = m_cloner(item);
        clones->insert( clonedItem, item->key()) ;
        return clonedItem;
      }
      return clones->object( item->key() );
    }
    
    MicroDST::DataStore* m_store;
    itemCloner m_cloner;
    
  };

  /**
   */
  template <class T>
  struct BasicItemCloner 
  {
    T* operator () (const T* item)    { return clone(item);   }
      
    static T* clone(const T* item) { return item->clone(); }
  };

  /**
   */
  template <class T>
  struct BasicCopy 
  {
    T* operator () (const T* item)    { return copy(item);   }
      
    static T* copy(const T* item) { return new T(*item); }
  };


}
#endif // MICRODST_FUNCTORS_HPP
