// $Id: DaVinciGuards.h,v 1.2 2009-09-01 16:08:28 jpalac Exp $
#ifndef KERNEL_DAVINCIGUARDS_H 
#define KERNEL_DAVINCIGUARDS_H 1

// Include files
#include "Kernel/IPhysDesktop.h"
#include "GaudiKernel/StatusCode.h"

/** @namespace DaVinci::Guards Kernel/DaVinciGuards.h
 *  
 *  For DVAlgorithm related guards.
 *  For DaVinci-independent guards see DaVinci::Utils, DaVinciUtils/DaVinciGuards.h.
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
