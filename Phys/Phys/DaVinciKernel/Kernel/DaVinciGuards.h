// $Id: DaVinciGuards.h,v 1.2 2009-09-01 16:08:28 jpalac Exp $
#ifndef KERNEL_DAVINCIGUARDS_H 
#define KERNEL_DAVINCIGUARDS_H 1

// Include files
#include "Kernel/DaVinciFun.h"
#include "Kernel/IPhysDesktop.h"
#include "GaudiKernel/StatusCode.h"

/** @namespace DaVinci::Guards Kernel/DaVinciGuards.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-08-31
 */

namespace DaVinci 
{
  
namespace Guards
{
  
  /**
   *
   * Guard for container of newed pointers. Delete each item in a container of 
   * newed pointers.
   *
   * T must be a const_iterator defined, begin(), end() and clear() methods,
   * and holding pointers.
   *
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
      if( !DaVinci::inTES(*iObj) ) delete *iObj;
    }

  }
private:
  OrphanPointerContainerGuard();

private:
  
  T& m_container;
};

  /**
   *
   * Execute IPhysDesktop::cleanDesktop() when the scope where 
   * an instance of an IPhysDesktop has been wrapped is exited.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 01/09/2009
  **/
class CleanDesktopGuard
{
public:

  explicit CleanDesktopGuard(IPhysDesktop* desktop)
    :
    m_desktop(desktop)
  {
  }

  ~CleanDesktopGuard( ) 
  {
    m_desktop->cleanDesktop().ignore();
  }

private:

  CleanDesktopGuard();

private:
  
  IPhysDesktop* m_desktop;

};
 
} // namespace Guards
  
} // namespace DaVinci

#endif // KERNEL_DAVINCIGUARDS_H
