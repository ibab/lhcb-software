#ifndef DAVINCIUTILS_GUARDS_H 
#define DAVINCIUTILS_GUARDS_H 1

#include "GaudiKernel/DataObject.h"
#include "DaVinciUtils/Functions.h"

/** @namespace DaVinci::Utils DaVinciUtils/Guards.h
 *  
 *  General helper functions and classes for use in DaVinci.
 *  These should only know about Gaudi classes and patterns.
 *  No knowledge of DaVinci specific code should be assumed.
 * 
 *  @author Juan PALACIOS palacios@uzh.uzh.ch
 *  @date   2010-09-21
 */

namespace DaVinci 
{
  
namespace Utils
{
  
  /**
   *
   * Guard for container of newed pointers. Delete each item in a container of 
   * newed pointers.
   *
   * T must be a const_iterator defined, begin(), end() and clear() methods,
   * and holding pointers.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 31/08/2009
   *
   **/
  template <class T>
  class PointerContainerGuard {
  public: 

    explicit PointerContainerGuard(T& container )
      :
      m_container(container)
    {
    }
  

    ~PointerContainerGuard( ) 
    {
      for (typename T::const_iterator iObj = m_container.begin();
           iObj != m_container.end(); ++iObj) delete *iObj;

      m_container.clear();

    }

  private:
    PointerContainerGuard();

  private:

    T& m_container;

  };

  /**
   *
   * Guard for container of newed pointers. Delete each item in a container of 
   * newed pointers unless they are also in the TES.
   *
   * T must be a const_iterator defined, begin(), end() and clear() methods,
   * and holding pointers.
   *
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 01/09/2009
   *
   **/
  template <class T>
  class OrphanPointerContainerGuard
  {
  public:

    explicit OrphanPointerContainerGuard(T& container)
      :
      m_container(container)
    {
    }

    ~OrphanPointerContainerGuard( ) 
    {
      for (typename T::const_iterator iObj = m_container.begin();
           iObj != m_container.end(); ++iObj) {
        if( !DaVinci::Utils::inTES(*iObj) ) delete *iObj;
      }
      m_container.clear();

    }
  private:
    OrphanPointerContainerGuard()
    {
    }

  private:
    
    T& m_container;
  };


  class DataObjectGuard
  {
  public :
    explicit DataObjectGuard(const DataObject* ptr)
    :
      m_ptr(ptr)
    {
    }

    ~DataObjectGuard( ) 
    {
      if (0==m_ptr->registry() ) {
        delete m_ptr;
        m_ptr = 0;
      }

    }

  
  private :
    DataObjectGuard() 
    {
    }

  private:
    const DataObject* m_ptr;
    
  };
  
    
 
} // namespace Utils
  
} // namespace DaVinci

#endif // KERNEL_DAVINCIGUARDS_H
