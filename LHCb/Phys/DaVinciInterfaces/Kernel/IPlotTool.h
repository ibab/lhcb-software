// $Id: IPlotTool.h,v 1.1.1.1 2009-07-22 20:54:51 jpalac Exp $
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
/// the unique interface identifier
static const InterfaceID IID_IPlotTool ( "IPlotTool" , 2 , 1 ) ;
// ============================================================================
/** @class IPlotTool IPlotTool.h Kernel/IPlotTool.h
 *  
 *  Interface for tools making plots with particles
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-01-05
 */
class IPlotTool : virtual public IAlgTool 
{
public: 
  
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
  StatusCode fillPlots 
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
  
public:
  
  // Return the unique interface ID
  static const InterfaceID& interfaceID() { return IID_IPlotTool; }

private:

};
// ============================================================================
// The END
// ============================================================================
#endif // KERNEL_IPLOTTOOL_H
// ============================================================================
