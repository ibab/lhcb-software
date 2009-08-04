// $Id: IPlotTool.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
// ============================================================================
#ifndef KERNEL_IPLOTTOOL_H 
#define KERNEL_IPLOTTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
/** @class IPlotTool IPlotTool.h Kernel/IPlotTool.h
 *  
 *  Interface for tools making plots with particles
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-05
 */
class GAUDI_API IPlotTool : virtual public IAlgTool 
{
public: 

  DeclareInterfaceID(IPlotTool, 3, 0);
  
  /// Define plot directory
  virtual StatusCode setPath(const std::string&) = 0;
  
  /// Fill plots using a LHCb::Particle::ConstVector
  virtual StatusCode fillPlots(const LHCb::Particle::ConstVector&,
                               const std::string trailer = "") = 0;
  
  /// Fill plots using a single Particle
  virtual StatusCode fillPlots(const LHCb::Particle*,
                               const std::string trailer = "") = 0;
  
  // ==========================================================================
  /** Fill plots using the arbitrary sequence of objects, convertible to 
   *  <c>const LHCb::Particle*</c> 
   *  @param begin begin-iterator for the sequence of particles 
   *  @param end   end-iterator for the sequence of particles 
   *  @param trailer the trailer
   *  @return status code
   */
  template <class PARTICLE>
  inline StatusCode fillPlots 
  ( PARTICLE           begin        , 
    PARTICLE           end          , 
    const std::string& trailer = "" ) 
  {
    for ( ; begin != end ; ++begin ) 
    {
      StatusCode sc = this->fillPlots ( *begin , trailer ) ;
      if ( sc.isFailure() ) { return sc ; }                          // RETURN
    } 
    return StatusCode::SUCCESS ;                                     // RETURN
  }
  // ==========================================================================
  

};
// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_IPLOTTOOL_H
// ============================================================================
