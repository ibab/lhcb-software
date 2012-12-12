// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/LHCbMath.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayChainBase.h"
#include "LoKi/ParticleProperties.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *
 *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
 *  @date   2011-06-03
 */
// ============================================================================
// the default indentation string 
// ============================================================================
std::string LoKi::DecayChainBase::blank ( const int len ) 
{ 
  if ( 0 >= len  ) { return blank ( 1 ) ; }  
  //
  static const int s_max = 90 ;
  return std::string ( std::min ( len , s_max ) , ' ' ) ;
}
// ============================================================================
//  Standard constructor
// ============================================================================
LoKi::DecayChainBase::DecayChainBase 
( const size_t               maxDepth , 
  const bool                 vertex   , 
  const DecayChainBase::Mode mode     , 
  const MSG::Color&          fg       , 
  const MSG::Color&          bg       )
  : LoKi::AuxFunBase () 
  , m_maxDepth  ( maxDepth   ) 
  , m_vertex    ( vertex     ) 
  , m_fg        ( fg         ) 
  , m_bg        ( bg         ) 
  , m_mode      ( mode       )
// formats 
  , m_fmt_m     ( "%|=7.5g|" ) 
  , m_fmt_pt    ( "%|=7.5g|" ) 
  , m_fmt_p     ( "%|=6.4g|" ) 
  , m_fmt_v     ( "%|=6.4g|" ) 
  , m_fmt_d     ( "%|=6.4g|" ) 
  , m_fmt_i     ( "%|=4l|"   ) 
{}
// ============================================================================
/// destructor 
// ============================================================================
LoKi::DecayChainBase::~DecayChainBase() {}
// ============================================================================
// particle name by ID
// ============================================================================
std::string
LoKi::DecayChainBase::name ( const LHCb::ParticleID& pid ) const 
{
  std::string tmp = LoKi::Particles::nameIdFromPID( pid ) ;  
  while ( tmp.length() < 12 ) { tmp += " " ; }
  return tmp ;
}
// ============================================================================
// double  to string 
// ============================================================================
std::string LoKi::DecayChainBase::toString
( const double      val ) const 
{
  boost::format fmter( m_fmt_d ) ;
  fmter % val ;
  return fmter.str() ;
}
// ============================================================================
// integer to string 
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const long        val ) const 
{
  boost::format fmter( m_fmt_i ) ;
  fmter % val ;
  return fmter.str() ;
}
// ============================================================================
// integer to string 
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const int         val ) const 
{
  const long l = val ;
  return toString ( l ) ;
}
// ============================================================================
// Lorentz vector as string
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const LoKi::LorentzVector& v ) const 
{ return toString( v , mode() ) ; } 
// ============================================================================
namespace 
{
  // ==========================================================================
  inline double adjust 
  ( const double value       , 
    const double limit       , 
    const double scale = 1.0 ) 
  { 
    return 
      0 == limit                              ? value / scale  : 
      std::abs ( value ) < std::abs ( limit ) ? 0.0   / scale  : 
      LHCb::Math::round ( value / limit ) * limit     / scale  ;  
  }
  // ==========================================================================
}
// ============================================================================
// Lorentz vector as string
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const LoKi::LorentzVector&  v    , 
  const DecayChainBase::Mode& mode ) const 
{
  //
  const double s_Min = 0.1 * Gaudi::Units::MeV ;
  //
  if      ( LV_MASS == mode ) 
  {
    boost::format fmter ( " M/E/PX/PY/PZ:" + 
                          m_fmt_m + "/"    +
                          m_fmt_p + "/"    +
                          m_fmt_p + "/"    + 
                          m_fmt_p + "/"    + 
                          m_fmt_p          + " [GeV] " ) ;
    fmter 
      %  adjust ( v.M     () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.E     () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Px    () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Py    () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pz    () , s_Min , Gaudi::Units::GeV ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_WITHPT == mode ) 
  {
    boost::format fmter ( " M/PT/E/PX/PY/PZ:" + 
                          m_fmt_m  + "/"      +
                          m_fmt_pt + "/"      +
                          m_fmt_p  + "/"      +
                          m_fmt_p  + "/"      + 
                          m_fmt_p  + "/"      + 
                          m_fmt_p             + " [GeV] " ) ;
    fmter 
      %  adjust ( v.M   () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt  () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.E   () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Px  () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Py  () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pz  () , s_Min , Gaudi::Units::GeV ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_ONLYP == mode  ) 
  {
    boost::format fmter ( " PX/PY/PZ:"   + 
                          m_fmt_p + "/"  + 
                          m_fmt_p + "/"  + 
                          m_fmt_p        + " [GeV] " ) ;
    fmter 
      %  adjust ( v.Px    () , s_Min , Gaudi::Units::GeV  )
      %  adjust ( v.Py    () , s_Min , Gaudi::Units::GeV ) 
      %  adjust ( v.Pz    () , s_Min , Gaudi::Units::GeV ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_SHORT == mode  ) 
  {
    boost::format fmter ( " M/PT/E:"     + 
                          m_fmt_m  + "/" + 
                          m_fmt_pt + "/" + 
                          m_fmt_p        + " [GeV] " ) ;
    fmter 
      %  adjust ( v.M     () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt    () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.E     () , s_Min , Gaudi::Units::GeV ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_MPTYPHI == mode  ) 
  {
    boost::format fmter ( " M/PT/Y/PHI:"       + 
                          m_fmt_m  + "/"       + 
                          m_fmt_pt + " [GeV]/" + 
                          m_fmt_m              + "/"      + 
                          m_fmt_m              + " [pi] " ) ;
    fmter 
      %  adjust ( v.M        () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt       () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Rapidity ()                    , 0.01  ) 
      %  adjust ( v.Phi      () / Gaudi::Units::pi , 0.01  ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_MPTETAPHI == mode  ) 
  {
    boost::format fmter ( " M/PT/ETA/PHI:"     + 
                          m_fmt_m  + "/"       + 
                          m_fmt_pt + " [GeV]/" + 
                          m_fmt_m              + "/"      +
                          m_fmt_m              + " [pi] " ) ;
    fmter 
      %  adjust ( v.M        () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt       () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Eta      ()                    , 0.01  )
      %  adjust ( v.Phi      () / Gaudi::Units::pi , 0.01  ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_MPTY == mode  ) 
  {
    boost::format fmter ( " M/PT/Y:"           + 
                          m_fmt_m  + "/"       + 
                          m_fmt_pt + " [GeV]/" + 
                          m_fmt_m              + " " ) ;
    fmter 
      %  adjust ( v.M        () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt       () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Rapidity () , 0.01  ) ;
    //
    return fmter.str() ;
  }
  else if ( LV_MPTETA == mode  ) 
  {
    boost::format fmter ( " M/PT/ETA:"         + 
                          m_fmt_m  + "/"       + 
                          m_fmt_pt + " [GeV]/" + 
                          m_fmt_m              + " " ) ;
    fmter 
      %  adjust ( v.M        () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Pt       () , s_Min , Gaudi::Units::GeV )
      %  adjust ( v.Eta      () , 0.01  ) ;
    //
    return fmter.str() ;
  }
  //
  // the default scenario 
  // 
  boost::format fmter ( " E/PX/PY/PZ:" + 
                        m_fmt_p + "/"  + 
                        m_fmt_p + "/"  + 
                        m_fmt_p + "/"  + 
                        m_fmt_p        + " [GeV] " ) ;
  
  fmter 
    %  adjust ( v.E     () , s_Min , Gaudi::Units::GeV )
    %  adjust ( v.Px    () , s_Min , Gaudi::Units::GeV )
    %  adjust ( v.Py    () , s_Min , Gaudi::Units::GeV )
    %  adjust ( v.Pz    () , s_Min , Gaudi::Units::GeV ) ;
  //
  return  fmter.str() ;
}
// ============================================================================
// 3D-vector as string
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const LoKi::Vector3D&       v ) const 
{
  const double s_Min = 0.1 * Gaudi::Units::micrometer ;
  
  boost::format fmter ( " X/Y/Z:"     + 
                        m_fmt_v + "/" + 
                        m_fmt_v + "/" + 
                        m_fmt_v       + " [mm] " ) ;
  fmter 
    %  adjust ( v.X() , s_Min , Gaudi::Units::mm )
    %  adjust ( v.Y() , s_Min , Gaudi::Units::mm )
    %  adjust ( v.Z() , s_Min , Gaudi::Units::mm ) ;
  
  return fmter.str() ;  
}
// ============================================================================
// 3D-vector as string
// ============================================================================
std::string LoKi::DecayChainBase::toString 
( const LoKi::Point3D&       v ) const 
{
  const double s_Min = 0.0001 * Gaudi::Units::mm ;
  
  boost::format fmter ( " X/Y/Z:"     + 
                        m_fmt_v + "/" + 
                        m_fmt_v + "/" + 
                        m_fmt_v       + " [mm] " ) ;
  fmter 
    %  adjust ( v.X() , s_Min , Gaudi::Units::mm )
    %  adjust ( v.Y() , s_Min , Gaudi::Units::mm )
    %  adjust ( v.Z() , s_Min , Gaudi::Units::mm ) ;
  
  return fmter.str() ;  
}
// ============================================================================
// The END 
// ============================================================================

