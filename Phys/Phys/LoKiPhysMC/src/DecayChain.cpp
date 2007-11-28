// $Id: DecayChain.cpp,v 1.4 2007-11-28 14:54:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/01/19 13:14:57  ibelyaev
//  add good printout
//
// Revision 1.1  2006/05/27 11:47:14  ibelyaev
//  add DecayChain utilities
//
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <math.h>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Print.h"
#include "LoKi/ErrorReport.h"
#include "LoKi/Report.h"
#include "LoKi/ParticleProperties.h"
#include "LoKi/DecayChain.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/format.hpp"
// ============================================================================

/** @file 
 *  
 *  Implementation file for class : DecayChain
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date    2003-05-14 
 */
// ============================================================================

// ============================================================================
/** set the colors for output stream (not actiev for WIN32)
 *  @param  stream  reference for stream 
 *  @param  fg      color for foreground 
 *  @param  fg      color for foreground 
 *  @return true if color are changed 
 */
// ============================================================================
bool LoKi::Colors::setColor
( std::ostream&     stream , 
  const MSG::Color& fg     , 
  const MSG::Color& bg     )
{
#ifndef WIN32    
  int fc =  90 +       fg ;
  stream << "\033[" <<  fc ;
  int bc = 100 +       bg ;
  stream << ";"    <<  bc ;
  stream << ";1m";
  return true  ;
#else 
  return false ;
#endif 
};
// ============================================================================
/** set the colors for output stream 
 *  @param  stream  reference for stream 
 *  @param  fg      color for foreground 
 *  @param  fg      color for foreground 
 *  @return true if color are changed 
 */
// ============================================================================
bool LoKi::Colors::setColor 
( MsgStream&        stream ,
  const MSG::Color& fg     , 
  const MSG::Color& bg     )
{
  stream.setColor ( fg , bg );
  return true ;
};
// ============================================================================
/** Standard constructor
 *  @param svc       pointer to ParticleProertySvc 
 *  @param maxDepth  maximal decay depth 
 */
// ============================================================================
LoKi::DecayChain::DecayChain 
( const size_t           maxDepth , 
  const bool             vertex   , 
  const bool             vertexe  , 
  const DecayChain::Mode mode     , 
  const MSG::Color&      fg       , 
  const MSG::Color&      bg       ) 
  : m_maxDepth  ( maxDepth   ) 
  , m_vertex    ( vertex     ) 
  , m_vertexe   ( vertexe    ) 
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
{};
// ============================================================================
/// destructor 
// ============================================================================
LoKi::DecayChain::~DecayChain() {};
// ============================================================================
/// particle name by ID
// ============================================================================
const std::string LoKi::DecayChain::name ( const LHCb::ParticleID& pid ) const 
{
  std::string tmp = LoKi::Particles::nameFromPID( pid ) ;  
  while ( tmp.length() < 12 ) { tmp += " " ; }
  return tmp ;
};
// ============================================================================
/// double  to string 
// ============================================================================
const std::string LoKi::DecayChain::toString
( const double      val ) const 
{
  boost::format fmter( m_fmt_d ) ;
  fmter % val ;
  return fmter.str() ;
};
// ============================================================================
/// integer to string 
// ============================================================================
const std::string LoKi::DecayChain::toString 
( const long        val ) const 
{
  boost::format fmter( m_fmt_i ) ;
  fmter % val ;
  return fmter.str() ;
};
// ============================================================================
/// Lorentz vector as string
// ============================================================================
const std::string LoKi::DecayChain::toString 
( const LoKi::LorentzVector& v ) const 
{ return toString( v , mode() ) ; } ;


namespace 
{
  inline double adjust 
  ( const double value       , 
    const double limit       , 
    const double scale = 1.0 ) 
  { return ( ::fabs( value ) > limit ) ? value / scale : 0.0 / scale ; }
};


// ============================================================================
/// Lorentz vector as string
// ============================================================================
const std::string LoKi::DecayChain::toString 
( const LoKi::LorentzVector& v , 
  const DecayChain::Mode& mode) const 
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
  
  // default scenario 
  
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
};
// ============================================================================
// 3D-vector as string
// ============================================================================
const std::string LoKi::DecayChain::toString 
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
};
// ============================================================================
// 3D-vector as string
// ============================================================================
const std::string LoKi::DecayChain::toString 
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
};
// ============================================================================
// int    as string
// ============================================================================
const std::string LoKi::DecayChain::toString ( const int v ) const
{
  const long value = v ;
  return toString( value ) ;
};
// ============================================================================
/** print error message 
 *  @param msg error message 
 *  @param sc status code
 *  @param mx number of prints 
 */
// ============================================================================
StatusCode LoKi::DecayChain::Error
( const std::string&  msg , 
  const StatusCode&   sc  ,
  const size_t        mx  ) const 
{
  const LoKi::ErrorReport& report = LoKi::ErrorReport::instance();
  return report.Error( "LoKi::DecayChain " + msg , sc , mx ) ;
};
// ============================================================================
// atomic 
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print 
( const LHCb::Particle*       p      , 
  const LoKi::Types::Cuts&    accept , 
  const LoKi::Types::Cuts&    mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::MCParticle*     p      , 
  const LoKi::Types::MCCuts&  accept , 
  const LoKi::Types::MCCuts&  mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const HepMC::GenParticle*   p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// ranges 
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LoKi::Types::Range&   p      ,
  const LoKi::Types::Cuts&    accept , 
  const LoKi::Types::Cuts&    mark   ) const 
{ print ( p.begin() , p.end() , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LoKi::Types::MCRange& p      ,
  const LoKi::Types::MCCuts&  accept , 
  const LoKi::Types::MCCuts&  mark   ) const
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LoKi::Types::GRange&  p      ,
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// vectors 
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::Particle::Vector& p    ,
  const LoKi::Types::Cuts&    accept , 
  const LoKi::Types::Cuts&    mark   ) const 
{ print ( p.begin() , p.end() , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::MCParticle::Vector& p  ,
  const LoKi::Types::MCCuts&  accept , 
  const LoKi::Types::MCCuts&  mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::Particle::ConstVector& p ,
  const LoKi::Types::Cuts&    accept   , 
  const LoKi::Types::Cuts&    mark     ) const 
{ print ( p.begin() , p.end() , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::MCParticle::ConstVector&  p ,
  const LoKi::Types::MCCuts&  accept      , 
  const LoKi::Types::MCCuts&  mark        ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LoKi::GenTypes::GenContainer&   p ,
  const LoKi::Types::GCuts&   accept      , 
  const LoKi::Types::GCuts&   mark        ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// other containers
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::Particle::Container* p ,
  const LoKi::Types::Cuts&    accept , 
  const LoKi::Types::Cuts&    mark   ) const 
{ 
  if ( 0 != p ) 
  { print ( p->begin() , p->end() , std::cout , '\n' , accept , mark ) ; } 
}
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::MCParticle::Container* p ,
  const LoKi::Types::MCCuts&   accept , 
  const LoKi::Types::MCCuts&   mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const HepMC::GenEvent*      p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::HepMCEvent*     p      , 
  const LoKi::Types::GCuts&   accept , 
  const LoKi::Types::GCuts&   mark   ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================
/// predefined printout 
// ============================================================================
void LoKi::DecayChain::print
( const LHCb::HepMCEvent::Container* p,
  const LoKi::Types::GCuts&   accept  , 
  const LoKi::Types::GCuts&   mark    ) const 
{ print ( p , std::cout , '\n' , accept , mark ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
