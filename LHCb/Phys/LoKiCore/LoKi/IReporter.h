// $Id: IReporter.h,v 1.5 2007-07-23 17:07:38 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_IREPORTER_H 
#define LOKI_IREPORTER_H 1
// ============================================================================
// Include files
// ============================================================================
// from GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/IErrorTool.h"
// ============================================================================
// forward declarations 
// ============================================================================
class GaudiException ;
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class IReporter IReporter.h LoKi/IReporter.h
   *  
   *
   *  @author Ivan Belyaev ivelyaev@physics.syr.edu
   *  @date   2003-01-16
   */
  class IReporter 
    : public virtual   IAlgTool   
    , public virtual   IErrorTool
  {
  public: 
    /** Retrieve interface ID
     *  mandatory method from IInterface 
     *  @see IInterface 
     *  @see InterfaceID 
     *  @return unique interface identifier 
     */
    static const InterfaceID& interfaceID() ;
  protected:
    // destructor 
    virtual ~IReporter() ; 
  };
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_IREPORTER_H
// ============================================================================
