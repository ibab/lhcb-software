// $Id: DaVinciGuards.h,v 1.1 2009-08-31 16:33:39 jpalac Exp $
#ifndef KERNEL_DAVINCIGUARDS_H 
#define KERNEL_DAVINCIGUARDS_H 1

// Include files

/** @class PointerContainerGuard Kernel/DaVinciGuards.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-08-31
 */
namespace DaVinci 
{
  
template <class T>
class PointerContainerGuard {
public: 
  /// Standard constructor
  PointerContainerGuard(T& container )
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

} // namespace DaVinci

#endif // KERNEL_DAVINCIGUARDS_H
