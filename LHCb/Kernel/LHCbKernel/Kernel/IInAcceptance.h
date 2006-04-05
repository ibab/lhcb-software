// $Id: IInAcceptance.h,v 1.2 2006-04-05 10:16:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
#ifndef KERNEL_IINACCEPTANCE_H
#define KERNEL_IINACCEPTANCE_H 1

// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb
{
  class Track ;
} ;
// ============================================================================

// ============================================================================
/** @class IInAcceptance IInAcceptance.h Kernel/IInAcceptance.h
 *
 *  An abstract interface for implementation "acceptance" tools.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date   2005-07-22
 */
// ============================================================================
class IInAcceptance : virtual public IAlgTool
{
public:

  /** check the track is in acceptance of given "detector"
   *  @param track track to be checked
   *  @return true if the track is in acceptance
   */
  virtual bool inAcceptance ( const LHCb::Track* track ) const = 0 ;

public:
  /// Return the unique interface identifier
  static const InterfaceID& interfaceID() ;
protected:
  // virtual and protected destructor
  virtual ~IInAcceptance() ;
private:
};

// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_IINACCEPTANCE_H
// ============================================================================
