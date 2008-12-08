// $Id: LifeTime.h,v 1.2 2008-12-08 10:11:22 ibelyaev Exp $
// ============================================================================
#ifndef LBHIDVALLEY_LIFETIME_H 
#define LBHIDVALLEY_LIFETIME_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
namespace GaussMonitor
{
  // ==========================================================================
  /// create the counter name 
  inline std::string cntName ( const std::string& pid ) 
  { return "LifeTime " + pid ; }
  // ==========================================================================
  /// evaluate the lifetime (c*tau)
  inline double lifeTime 
  ( const Gaudi::LorentzVector& p , 
    const Gaudi::XYZPoint&      o ,
    const Gaudi::XYZPoint&      e )
  {
    /// projected distance 
    const double pdistance  = p.Vect().Dot ( e - o ) ;
    /// the projected distance / ( beta * gamma )  
    return pdistance * p.M() / p.P2() ;
  }
  // ==========================================================================
  /// calculate the high edge for the histogram 
  inline double highEdge 
  ( const LHCb::ParticleProperty& pp                                    , 
    const size_t                  num  =  11                            , 
    const double                  unit =  50 * Gaudi::Units::micrometer ) 
  {
    const double nominal = pp.ctau () ;
    const double high    = floor ( num * nominal / unit ) * unit ; 
    return std::max ( high , unit ) ; 
  }
  // ==========================================================================
} // end of namespace HidValley 
// ============================================================================
// The END 
// ============================================================================
#endif // LBHIDVALLEY_LIFETIME_H
// ============================================================================
