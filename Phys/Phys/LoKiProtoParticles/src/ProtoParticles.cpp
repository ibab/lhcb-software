// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/ProtoParticle.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ProtoParticles.h"
#include "LoKi/GetTools.h"
#include "LoKi/Interface.h"
#include "LoKi/Services.h"
#include "LoKi/select.h"
#include "LoKi/Algs.h"
// ============================================================================
/** @file
 *  Imeplementation file for namespace LoKi:ProtoParticles
 *
 *   This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *   The package has been designed with the kind help from
 *   Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *   contributions and advices from G.Raven, J.van Tilburg, 
 *   A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *   with the campain of Dr.O.Callot et al.: 
 *    ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *     
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date  2010-05-26
 *
 *                 $Revision$
 * LastModification $Date$
 *              by $Author$ 
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  std::string pp_key ( const int key ) 
  {
    if ( key <= LHCb::ProtoParticle::NoPID         || 
         key >= LHCb::ProtoParticle::LastGlobal      ) { return "" ; }
    //
    std::ostringstream str ;
    str << (LHCb::ProtoParticle::additionalInfo) key ;
    const std::string s = str.str() ;
    if ( std::string::npos != s.find ("ERROR")      || 
         std::string::npos != s.find ("wrong value") ) { return "" ; }
    return s ;
  }
  // ==========================================================================
}
// ============================================================================
// the specific printout
// ============================================================================
template <>
std::ostream& 
LoKi::Functors::Empty<const LHCb::ProtoParticle*>::fillStream
( std::ostream& s ) const { return s << "PP_EMPTY" ; }
// ============================================================================
// the specific printpout
// ============================================================================
template <>
std::ostream& 
LoKi::Functors::Size<const LHCb::ProtoParticle*>::fillStream
( std::ostream& s ) const { return s << "PP_SIZE" ; }
// ============================================================================
template <>
std::ostream& 
LoKi::Valid<const LHCb::ProtoParticle*>::fillStream 
( std::ostream& s ) const { return s << "PP_VALID " ; }
// ============================================================================
// constructor from the filter 
// ============================================================================
LoKi::ProtoParticles::Filter::Filter ( const IProtoParticleFilter* fltr ) 
  : LoKi::PPTypes::PPCuts () 
  , m_filter ( fltr ) 
{
  Assert ( !(!m_filter) , "Invalid IParticleFilter!" ) ;
}
// ============================================================================
// constructor from the filter 
// ============================================================================
LoKi::ProtoParticles::Filter::Filter
( const std::string& nick  ) 
  : LoKi::AuxFunBase ( std::tie ( nick ) ) 
  , LoKi::PPTypes::PPCuts () 
  , m_filter () 
{
  m_filter = LoKi::GetTools::protoParticleFilter ( *this , nick ) ;
  Assert ( !(!m_filter) , "Invalid IParticleFilter!" ) ;
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::Filter::~Filter (){}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::Filter*
LoKi::ProtoParticles::Filter::clone() const 
{ return  new LoKi::ProtoParticles::Filter ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::Filter::result_type 
LoKi::ProtoParticles::Filter::operator() 
  ( LoKi::ProtoParticles::Filter::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'") ;
    return false ;                                                    // RETURN 
  }
  //
  Assert ( !(!m_filter) , "Invalid IParticleFilter!" ) ;
  //
  return m_filter->isSatisfied ( p ) ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::Filter::fillStream  ( std::ostream& s ) const 
{
  return s << " PP_FILTER( '" 
           << m_filter->type() << "/" << m_filter->name() <<"' ) " ;
}

// ============================================================================
// contructor from the index 
// ============================================================================
LoKi::ProtoParticles::HasInfo::HasInfo ( const int index ) 
  : LoKi::AuxFunBase ( std::tie ( index ) ) 
  , LoKi::PPTypes::PPCuts () 
  , m_index ( index ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasInfo::~HasInfo () {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasInfo*
LoKi::ProtoParticles::HasInfo::clone() const
{ return new LoKi::ProtoParticles::HasInfo ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasInfo::result_type
LoKi::ProtoParticles::HasInfo::operator() 
  ( LoKi::ProtoParticles::HasInfo::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  return p->hasInfo ( m_index ) ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasInfo::fillStream ( std::ostream& s ) const 
{ 
  switch ( m_index ) 
  {
  case LHCb::ProtoParticle::InAccSpd      : return s << " PP_HASCALOSPD "  ;
  case LHCb::ProtoParticle::InAccPrs      : return s << " PP_HASCALOPRS "  ;
  case LHCb::ProtoParticle::InAccEcal     : return s << " PP_HASCALOECAL " ;
  case LHCb::ProtoParticle::InAccHcal     : return s << " PP_HASCALOHCAL " ;
  case LHCb::ProtoParticle::InAccBrem     : return s << " PP_HASCALOBREM " ;
  case LHCb::ProtoParticle::InAccMuon     : return s << " PP_HASMUONINFO " ;
  case LHCb::ProtoParticle::RichPIDStatus : return s << " PP_HASRICHDLL "  ;
  case LHCb::ProtoParticle::MuonPIDStatus : return s << " PP_HASMUONDLL "  ;
  default: 
    break ; // BREAK
  }
  //
  return s << " PP_HASINFO( " << m_index << " ) " ; 
}

  

// ============================================================================
/* constructor from "info"
 *  @param key info index/mark/key
 *  @param def default valeu for info 
 *  @param bad bad value to be retured for invalid protoparticle 
 */
// ============================================================================
LoKi::ProtoParticles::Info::Info 
( const int    key , 
  const double def , 
  const double bad ) 
  : LoKi::AuxFunBase ( std::tie ( key , def, bad ) ) 
  , LoKi::PPTypes::PPFunc () 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( bad ) 
{}
// ============================================================================
/* constructor from "info"
 *  @param key info index/mark/key
 *  @param def default value for info 
 */
// ============================================================================
LoKi::ProtoParticles::Info::Info 
( const int    key , 
  const double def ) 
  : LoKi::AuxFunBase ( std::tie ( key , def ) ) 
  , LoKi::PPTypes::PPFunc () 
  , m_key ( key ) 
  , m_def ( def ) 
  , m_bad ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::Info::~Info () {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::Info*
LoKi::ProtoParticles::Info::clone() const 
{ return new LoKi::ProtoParticles::Info ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::ProtoParticles::Info::result_type
LoKi::ProtoParticles::Info::operator() 
  ( LoKi::ProtoParticles::HasInfo::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'bad'" ) ;
    return m_bad;                                                    // RETURN
  }
  return p -> info ( m_key , m_def ) ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::Info::fillStream ( std::ostream& s ) const 
{ 
  const std::string s_key = pp_key ( m_key ) ;
  if ( !s_key.empty() ) { s << " PP_INFO( LHCb.ProtoParticle." << s_key ; }
  else                  { s << " PP_INFO( "                    << m_key ; }
  // default 
  s << " , " << m_def ; 
  // bad 
  if ( LoKi::Constants::NegativeInfinity != m_bad ) { s << " , " << m_bad ; }
  return  s << " ) " ; 
}
// ============================================================================/
LoKi::ProtoParticles::HasRichPID::HasRichPID()
  : LoKi::PPTypes::PPCuts() 
{}
// ============================================================================/
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasRichPID::~HasRichPID() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasRichPID*
LoKi::ProtoParticles::HasRichPID::clone() const 
{ return new LoKi::ProtoParticles::HasRichPID ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasRichPID::result_type 
LoKi::ProtoParticles::HasRichPID::operator()
  ( LoKi::ProtoParticles::HasRichPID::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  return 0 != p->richPID () ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasRichPID::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASRICHPID " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasMuonPID::~HasMuonPID() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasMuonPID*
LoKi::ProtoParticles::HasMuonPID::clone() const 
{ return new LoKi::ProtoParticles::HasMuonPID ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasMuonPID::result_type 
LoKi::ProtoParticles::HasMuonPID::operator()
  ( LoKi::ProtoParticles::HasMuonPID::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  return 0 != p->richPID () ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasMuonPID::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASMUONPID " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasTrack::~HasTrack() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasTrack*
LoKi::ProtoParticles::HasTrack::clone() const 
{ return new LoKi::ProtoParticles::HasTrack ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasTrack::result_type 
LoKi::ProtoParticles::HasTrack::operator()
  ( LoKi::ProtoParticles::HasTrack::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  return ( NULL != p->track() );
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasTrack::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASTRACK " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::Charged::~Charged() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::Charged*
LoKi::ProtoParticles::Charged::clone() const 
{ return new LoKi::ProtoParticles::Charged ( *this ) ; }
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::Charged::fillStream ( std::ostream& s ) const 
{ return s << " PP_CHARGED " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::Neutral::~Neutral() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::Neutral*
LoKi::ProtoParticles::Neutral::clone() const 
{ return new LoKi::ProtoParticles::Neutral ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::Neutral::result_type 
LoKi::ProtoParticles::Neutral::operator()
  ( LoKi::ProtoParticles::Neutral::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  //
  return ( NULL == p->track() && !p->calo().empty() );
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::Neutral::fillStream ( std::ostream& s ) const 
{ return s << " PP_NEUTRAL " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::Calos::~Calos() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::Calos*
LoKi::ProtoParticles::Calos::clone() const 
{ return new LoKi::ProtoParticles::Calos ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::Calos::result_type 
LoKi::ProtoParticles::Calos::operator()
  ( LoKi::ProtoParticles::Calos::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return -1 " ) ;
    return -1 ;                                                    // RETURN
  }
  return p -> calo ().size () ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::Calos::fillStream ( std::ostream& s ) const 
{ return s << " PP_NCALOS " ; }

 
// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasAerogel::~HasAerogel() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasAerogel*
LoKi::ProtoParticles::HasAerogel::clone() const 
{ return new LoKi::ProtoParticles::HasAerogel ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasAerogel::result_type 
LoKi::ProtoParticles::HasAerogel::operator()
  ( LoKi::ProtoParticles::HasAerogel::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  const LHCb::RichPID* pid = rich ( p ) ;
  if ( 0 == pid ) 
  {
    Warning ( "LHCb::RichPID* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  //
  return pid->usedAerogel() ;
}
// ============================================================================
// get RICH pid 
// ============================================================================
const LHCb::RichPID* 
LoKi::ProtoParticles::HasAerogel::rich ( const LHCb::ProtoParticle* p ) const 
{
  if ( NULL == p   ) { return NULL ; }                           // RETURN 
  const LHCb::RichPID* pid = p->richPID() ;
  if ( NULL != pid ) { return pid  ; }                           // RETURN 
  // 
  LHCb::ProtoParticle::ExtraInfo::const_iterator ifind = 
    p->extraInfo().find ( LHCb::ProtoParticle::RichPIDStatus ) ;
  //
  if ( p->extraInfo().end() == ifind ) { return 0 ; }        // RETURN 
  //
  m_rich.setPidResultCode ( static_cast<unsigned int> ( ifind ->second ) ) ;
  //
  return &m_rich ;                                           // RETURN
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasAerogel::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASAEROGEL " ; }

// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasRich1Gas::~HasRich1Gas() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasRich1Gas*
LoKi::ProtoParticles::HasRich1Gas::clone() const 
{ return new LoKi::ProtoParticles::HasRich1Gas ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasRich1Gas::result_type 
LoKi::ProtoParticles::HasRich1Gas::operator()
  ( LoKi::ProtoParticles::HasRich1Gas::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  const LHCb::RichPID* pid = rich ( p ) ;
  if ( NULL == pid ) 
  {
    Warning ( "LHCb::RichPID* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  //
  return pid->usedRich1Gas() ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasRich1Gas::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASRICH1GAS " ; }


// ============================================================================
// MANDATORY: virtual destructor
// ============================================================================
LoKi::ProtoParticles::HasRich2Gas::~HasRich2Gas() {}
// ============================================================================
// MANDATORY: clone method ("virtual consructor")
// ============================================================================
LoKi::ProtoParticles::HasRich2Gas*
LoKi::ProtoParticles::HasRich2Gas::clone() const 
{ return new LoKi::ProtoParticles::HasRich2Gas ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential methdo 
// ============================================================================
LoKi::ProtoParticles::HasRich2Gas::result_type 
LoKi::ProtoParticles::HasRich2Gas::operator()
  ( LoKi::ProtoParticles::HasRich2Gas::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  const LHCb::RichPID* pid = rich ( p ) ;
  if ( NULL == pid ) 
  {
    Warning ( "LHCb::RichPID* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN
  }
  //
  return pid->usedRich2Gas() ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasRich2Gas::fillStream ( std::ostream& s ) const 
{ return s << " PP_HASRICH2GAS " ; }
// ============================================================================

// ============================================================================
// constructor from the detector 
// ============================================================================
LoKi::ProtoParticles::HasDetector::HasDetector
( const LoKi::ProtoParticles::HasDetector::Detector det ) 
  : LoKi::AuxFunBase ( std::tie ( det ) ) 
  , LoKi::ProtoParticles::HasRich2Gas () 
  , m_det ( det ) 
{
  switch ( det ) 
  {
    //
  case RICH          : break ;
  case CALO          : break ;
  case MUON          : break ; 
    //
  case RICH_AEROGEL  : break ; 
  case RICH_RICH1GAS : break ; 
  case RICH_RICH2GAS : break ; 
    //
  case CALO_SPDACC   : break ; 
  case CALO_PRSACC   : break ; 
  case CALO_ECALACC  : break ; 
  case CALO_HCALACC  : break ; 
  case CALO_BREMACC  : break ;
  case CALO_CALOACC  : break ;
    //
  case CALO_SPDINFO  : break ; 
  case CALO_PRSINFO  : break ; 
  case CALO_ECALINFO : break ; 
  case CALO_HCALINFO : break ; 
  case CALO_BREMINFO : break ;
  case CALO_CALOINFO : break ;
    //
  default:
    Assert ( false , "Invalid Detector type!" );
  }
}
// ============================================================================
// constructor from the detector 
// ============================================================================
LoKi::ProtoParticles::HasDetector::HasDetector
( const std::string& det ) 
  : LoKi::AuxFunBase ( std::tie ( det ) ) 
  , LoKi::ProtoParticles::HasRich2Gas () 
  , m_det ( LoKi::ProtoParticles::HasDetector::RICH ) 
{
  //
  if      ( "RICH"          == det ) { m_det = RICH ; }
  else if ( "CALO"          == det ) { m_det = CALO ; }
  else if ( "MUON"          == det ) { m_det = MUON ; }
  //
  else if ( "RICH_AEROGEL"  == det ) { m_det = RICH_AEROGEL  ; }
  else if ( "RICH_RICH1GAS" == det ) { m_det = RICH_RICH1GAS ; }
  else if ( "RICH_RICH2GAS" == det ) { m_det = RICH_RICH2GAS ; }
  //
  else if ( "CALO_SPDACC"   == det ) { m_det = CALO_SPDACC   ; }
  else if ( "CALO_PRSACC"   == det ) { m_det = CALO_PRSACC   ; }
  else if ( "CALO_ECALACC"  == det ) { m_det = CALO_ECALACC  ; }
  else if ( "CALO_HCALACC"  == det ) { m_det = CALO_HCALACC  ; }
  else if ( "CALO_BREMACC"  == det ) { m_det = CALO_BREMACC  ; }
  else if ( "CALO_CALOACC"  == det ) { m_det = CALO_CALOACC  ; }
  //
  else if ( "CALO_SPDINFO"  == det ) { m_det = CALO_SPDINFO  ; }
  else if ( "CALO_PRSINFO"  == det ) { m_det = CALO_PRSINFO  ; }
  else if ( "CALO_ECALINFO" == det ) { m_det = CALO_ECALINFO ; }
  else if ( "CALO_HCALINFO" == det ) { m_det = CALO_HCALINFO ; }
  else if ( "CALO_BREMINFO" == det ) { m_det = CALO_BREMINFO ; }
  else if ( "CALO_CALOINFO" == det ) { m_det = CALO_CALOINFO ; }
  else 
  {
    Assert ( false , "Invalid Detector type '" + det + "'" ) ;
  }
}  
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::HasDetector::~HasDetector () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::HasDetector*
LoKi::ProtoParticles::HasDetector::clone() const 
{ return new LoKi::ProtoParticles::HasDetector ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::ProtoParticles::HasDetector::result_type 
LoKi::ProtoParticles::HasDetector::operator() 
  ( LoKi::ProtoParticles::HasDetector::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }
  //
  switch ( det() ) 
  {
    //
  case RICH          : return hasRichDLL       ( p ) ;
  case CALO          : return hasCaloDLL       ( p ) ;
  case MUON          : return hasMuonInfo      ( p ) ;
    //
  case RICH_AEROGEL  : return hasRichAerogel   ( p ) ;  
  case RICH_RICH1GAS : return hasRich1Gas      ( p ) ;
  case RICH_RICH2GAS : return hasRich2Gas      ( p ) ;
    //
  case CALO_SPDACC   : return inSpdAcceptance  ( p ) ;  
  case CALO_PRSACC   : return inPrsAcceptance  ( p ) ;
  case CALO_ECALACC  : return inEcalAcceptance ( p ) ;
  case CALO_HCALACC  : return inHcalAcceptance ( p ) ;
  case CALO_BREMACC  : return inBremAcceptance ( p ) ;
  case CALO_CALOACC  : return inCaloAcceptance ( p ) ;
    //
  case CALO_SPDINFO  : return inSpdAcceptance  ( p ) && hasSpdInfo  ( p ) ;
  case CALO_PRSINFO  : return inPrsAcceptance  ( p ) && hasPrsInfo  ( p ) ;  
  case CALO_ECALINFO : return inEcalAcceptance ( p ) && hasEcalInfo ( p ) ;
  case CALO_HCALINFO : return inHcalAcceptance ( p ) && hasHcalInfo ( p ) ;
  case CALO_BREMINFO : return inBremAcceptance ( p ) && hasBremInfo ( p ) ;
  case CALO_CALOINFO : return inCaloAcceptance ( p ) && hasCaloInfo ( p ) ;
    //
  default : ;
  }
  //
  Error ("Unknown technique, return false ") ;
  return false ;
}
// ============================================================================
bool LoKi::ProtoParticles::HasDetector::hasCaloDLL 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) && 
    ( p -> hasInfo ( LHCb::ProtoParticle::EcalPIDe  ) || 
      p -> hasInfo ( LHCb::ProtoParticle::PrsPIDe   ) || 
      p -> hasInfo ( LHCb::ProtoParticle::BremPIDe  ) || 
      p -> hasInfo ( LHCb::ProtoParticle::HcalPIDe  ) || 
      p -> hasInfo ( LHCb::ProtoParticle::EcalPIDmu ) || 
      p -> hasInfo ( LHCb::ProtoParticle::HcalPIDmu ) ) ;
}
// ============================================================================
bool LoKi::ProtoParticles::HasDetector::hasSpdInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    ( // 
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedSpd    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralSpd    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloSpdE          ) ||
     p -> hasInfo ( LHCb::ProtoParticle::CaloDepositID     ) || 
     p -> hasInfo ( LHCb::ProtoParticle::PhotonID          ) ) ;
}
// ============================================================================
bool LoKi::ProtoParticles::HasDetector::hasPrsInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    ( // 
     p -> hasInfo ( LHCb::ProtoParticle::PrsPIDe           ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedPrs    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralPrs    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloPrsE          ) ||
     p -> hasInfo ( LHCb::ProtoParticle::CaloDepositID     ) || 
     p -> hasInfo ( LHCb::ProtoParticle::PhotonID          ) ) ;
}
// ============================================================================
bool LoKi::ProtoParticles::HasDetector::hasEcalInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    ( // 
     p -> hasInfo ( LHCb::ProtoParticle::EcalPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::BremPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::EcalPIDmu         ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloTrMatch       ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloElectronMatch ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremMatch     ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedEcal   ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloDepositID     ) || 
     p -> hasInfo ( LHCb::ProtoParticle::ShowerShape       ) || 
     p -> hasInfo ( LHCb::ProtoParticle::ClusterMass       ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralEcal   ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloEcalE         ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloEcalChi2      ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremChi2      ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloClusChi2      ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloTrajectoryL   ) ||
     p -> hasInfo ( LHCb::ProtoParticle::PhotonID          ) ) ;
}
// ============================================================================
bool LoKi::ProtoParticles::HasDetector::hasHcalInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    (
     // 
     p -> hasInfo ( LHCb::ProtoParticle::HcalPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::HcalPIDmu         ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloHcalE         ) ) ;
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasBremInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    ( // 
     p -> hasInfo ( LHCb::ProtoParticle::BremPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremMatch     ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremChi2      ) ) ;
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasCaloInfo 
( const LHCb::ProtoParticle* p ) const 
{
  return ( NULL != p ) &&  
    ( // 
     p -> hasInfo ( LHCb::ProtoParticle::EcalPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::PrsPIDe           ) || 
     p -> hasInfo ( LHCb::ProtoParticle::BremPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::HcalPIDe          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::EcalPIDmu         ) || 
     p -> hasInfo ( LHCb::ProtoParticle::HcalPIDmu         ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloTrMatch       ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloElectronMatch ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremMatch     ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedSpd    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedPrs    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloChargedEcal   ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloDepositID     ) || 
     p -> hasInfo ( LHCb::ProtoParticle::ShowerShape       ) || 
     p -> hasInfo ( LHCb::ProtoParticle::ClusterMass       ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralSpd    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralPrs    ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloNeutralEcal   ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloSpdE          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloPrsE          ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloEcalE         ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloHcalE         ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloEcalChi2      ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloBremChi2      ) || 
     p -> hasInfo ( LHCb::ProtoParticle::CaloClusChi2      ) || 
     //
     p -> hasInfo ( LHCb::ProtoParticle::CaloTrajectoryL   ) ||
     p -> hasInfo ( LHCb::ProtoParticle::PhotonID          ) ) ;
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasRichAerogel 
( const LHCb::ProtoParticle* p ) const 
{
  if ( !hasRichDLL  ( p ) ) { return false ; }
  const LHCb::RichPID* pid = rich ( p ) ;
  return ( NULL != pid ) && pid->usedAerogel() ; 
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasRich1Gas
( const LHCb::ProtoParticle* p ) const 
{ 
  if ( !hasRichDLL  ( p ) ) { return false ; }
  const LHCb::RichPID* pid = rich ( p ) ;
  return ( NULL != pid ) && pid->usedRich1Gas () ; 
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasRich2Gas
( const LHCb::ProtoParticle* p ) const 
{
  if ( !hasRichDLL  ( p ) ) { return false ; }
  const LHCb::RichPID* pid = rich ( p ) ;
  return ( NULL != pid ) && pid->usedRich2Gas () ; 
}
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasRichDLL 
( const LHCb::ProtoParticle* p ) const 
{ return ( NULL != p ) && p->hasInfo( LHCb::ProtoParticle::RichPIDStatus ) ; }
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::hasMuonInfo 
( const LHCb::ProtoParticle* p ) const 
{ return ( NULL != p ) && p->hasInfo( LHCb::ProtoParticle::MuonPIDStatus ) ; }
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inSpdAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ return  ( NULL != p ) && ( 0 != p -> info ( LHCb::ProtoParticle::InAccSpd  , 0 ) ) ; }  
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inPrsAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ return  ( NULL != p ) && ( 0 != p -> info ( LHCb::ProtoParticle::InAccPrs  , 0 ) ) ; }  
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inEcalAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ return  ( NULL != p ) && ( 0 != p -> info ( LHCb::ProtoParticle::InAccEcal , 0 ) ) ; }  
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inHcalAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ return  ( NULL != p ) && ( 0 != p -> info ( LHCb::ProtoParticle::InAccHcal , 0 ) ) ; }  
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inBremAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ return  ( NULL != p ) && ( 0 != p -> info ( LHCb::ProtoParticle::InAccBrem , 0 ) ) ; }  
// ======================================================================
bool LoKi::ProtoParticles::HasDetector::inCaloAcceptance 
( const LHCb::ProtoParticle* p ) const 
{ 
  return  ( NULL != p ) && 
    ( inSpdAcceptance  ( p ) || 
      inPrsAcceptance  ( p ) || 
      inEcalAcceptance ( p ) || 
      inHcalAcceptance ( p ) || 
      inBremAcceptance ( p ) ) ;
}
// ======================================================================

// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::HasDetector::fillStream ( std::ostream& s ) const 
{ 
  switch ( det() ) 
  {
    //
  case RICH          : return s << " PP_HASRICHDLL "       ;
  case CALO          : return s << " PP_HASCALODLL "       ; 
  case MUON          : return s << " PP_HASMUONINFO "      ; 
    //
  case RICH_AEROGEL  : return s << " PP_USEDAEROGEL "      ; 
  case RICH_RICH1GAS : return s << " PP_USEDRICH1GAS "     ; 
  case RICH_RICH2GAS : return s << " PP_USEDRICH2GAS "     ; 
    //
  case CALO_SPDACC   : return s << " PP_INSPDACC "         ;
  case CALO_PRSACC   : return s << " PP_INPRSACC "         ; 
  case CALO_ECALACC  : return s << " PP_INECALACC "        ;
  case CALO_HCALACC  : return s << " PP_INHCALACC "        ; 
  case CALO_BREMACC  : return s << " PP_INBREMACC "        ; 
  case CALO_CALOACC  : return s << " PP_INCALOACC "        ; 
    //
  case CALO_SPDINFO  : return s << " PP_HASSPDINFO "       ; 
  case CALO_PRSINFO  : return s << " PP_HASPRSINFO "       ; 
  case CALO_ECALINFO : return s << " PP_HASECALINFO "      ;
  case CALO_HCALINFO : return s << " PP_HASHCALINFO "      ;  
  case CALO_BREMINFO : return s << " PP_HASBREMINFO "      ; 
  case CALO_CALOINFO : return s << " PP_HASCALOINFO "      ; 
    //
  default            : ;
  }
  //
  return s << " PP_HASDETECTOR( "  << det() << " ) " ;
}



// ============================================================================
// constructor from the detector 
// ============================================================================
LoKi::ProtoParticles::OnlyDetector::OnlyDetector
( const LoKi::ProtoParticles::HasDetector::Detector det ) 
  : LoKi::AuxFunBase ( std::tie ( det ) ) 
  , LoKi::ProtoParticles::HasDetector ( det ) 
{}
// ============================================================================
// constructor from the detector 
// ============================================================================
LoKi::ProtoParticles::OnlyDetector::OnlyDetector
( const std::string& det ) 
  : LoKi::AuxFunBase ( std::tie ( det ) ) 
  , LoKi::ProtoParticles::HasDetector ( det ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::OnlyDetector::~OnlyDetector () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::OnlyDetector*
LoKi::ProtoParticles::OnlyDetector::clone() const 
{ return new LoKi::ProtoParticles::OnlyDetector ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::ProtoParticles::OnlyDetector::result_type 
LoKi::ProtoParticles::OnlyDetector::operator() 
  ( LoKi::ProtoParticles::OnlyDetector::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }
  //
  switch ( det() ) 
  {
    //
  case RICH : return hasRichDLL  ( p ) && !hasCaloDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO : return hasCaloDLL  ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case MUON : return hasMuonInfo ( p ) && !hasRichDLL ( p ) && !hasCaloDLL  ( p ) ;
    //
  case RICH_AEROGEL  : return hasRichAerogel   ( p ) && !hasMuonInfo ( p ) && !hasCaloDLL ( p ) ;
  case RICH_RICH1GAS : return hasRich1Gas      ( p ) && !hasMuonInfo ( p ) && !hasCaloDLL ( p ) ;
  case RICH_RICH2GAS : return hasRich2Gas      ( p ) && !hasMuonInfo ( p ) && !hasCaloDLL ( p ) ;
    //
  case CALO_SPDACC   : return inSpdAcceptance  ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_PRSACC   : return inPrsAcceptance  ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_ECALACC  : return inEcalAcceptance ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_HCALACC  : return inHcalAcceptance ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_BREMACC  : return inBremAcceptance ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_CALOACC  : return inCaloAcceptance ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
    //
  case CALO_SPDINFO  : return inSpdAcceptance  ( p ) && hasSpdInfo  ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_PRSINFO  : return inPrsAcceptance  ( p ) && hasPrsInfo  ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;  
  case CALO_ECALINFO : return inEcalAcceptance ( p ) && hasEcalInfo ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_HCALINFO : return inHcalAcceptance ( p ) && hasHcalInfo ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_BREMINFO : return inBremAcceptance ( p ) && hasBremInfo ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
  case CALO_CALOINFO : return inCaloAcceptance ( p ) && hasCaloInfo ( p ) && !hasRichDLL ( p ) && !hasMuonInfo ( p ) ;
    //
  default : ;
  }
  //
  Error ("Unknown technique, return false ") ;
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::OnlyDetector::fillStream ( std::ostream& s ) const 
{ 
  switch ( det() ) 
  {
    //
  case RICH          : return s << " PP_ONLYRICHDLL "      ;
  case CALO          : return s << " PP_ONLYCALODLL "      ; 
  case MUON          : return s << " PP_ONLYMUONINFO "     ; 
    //
  case RICH_AEROGEL  : return s << " PP_ONLYAEROGEL "      ; 
  case RICH_RICH1GAS : return s << " PP_ONLYRICH1GAS "     ; 
  case RICH_RICH2GAS : return s << " PP_ONLYRICH2GAS "     ; 
    //
  case CALO_SPDACC   : return s << " PP_ONLYSPDACC "       ;
  case CALO_PRSACC   : return s << " PP_ONLYPRSACC "       ; 
  case CALO_ECALACC  : return s << " PP_ONLYECALACC "      ;
  case CALO_HCALACC  : return s << " PP_ONLYHCALACC "      ; 
  case CALO_BREMACC  : return s << " PP_ONLYBREMACC "      ; 
  case CALO_CALOACC  : return s << " PP_ONLYCALOACC "      ; 
    //
  case CALO_SPDINFO  : return s << " PP_ONLYSPDINFO "      ; 
  case CALO_PRSINFO  : return s << " PP_ONLYPRSINFO "      ; 
  case CALO_ECALINFO : return s << " PP_ONLYECALINFO "     ;
  case CALO_HCALINFO : return s << " PP_ONLYHCALINFO "     ;  
  case CALO_BREMINFO : return s << " PP_ONLYBREMINFO "     ; 
  case CALO_CALOINFO : return s << " PP_ONLYCALOINFO "     ; 
    //
  default            : ;
  }
  //
  return s << " PP_ONLYDETECTOR( "  << det() << " ) " ;
}


// ======================================================================
// MANDATORY: virtual destructor 
// ======================================================================
LoKi::ProtoParticles::IsMuon::~IsMuon(){}
// ======================================================================
// MANDATORY: clone method ("virtual constructor")
// ======================================================================
LoKi::ProtoParticles::IsMuon*
LoKi::ProtoParticles::IsMuon::clone() const
{ return new LoKi::ProtoParticles::IsMuon ( *this ) ; }
// ======================================================================
// MANDATORY: the only one essential method
// ======================================================================
LoKi::ProtoParticles::IsMuon::result_type 
LoKi::ProtoParticles::IsMuon::operator() 
  ( LoKi::ProtoParticles::IsMuon::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }  
  //
  const LHCb::MuonPID* pid = muon ( p ) ;
  //
  return ( NULL != pid && pid->IsMuon() );
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::IsMuon::fillStream ( std::ostream& s ) const 
{ return s << " PP_ISMUON " ; }
// ============================================================================
// get muon-pid object
// ============================================================================
const LHCb::MuonPID* 
LoKi::ProtoParticles::IsMuon::muon ( const LHCb::ProtoParticle* p  ) const 
{
  if ( NULL == p   ) { return NULL; }                           // RETURN 
  //
  const LHCb::MuonPID* pid = p->muonPID () ;
  if ( NULL != pid ) { return pid ; }                           // RETURN 
  //
  // Access the status word
  LHCb::ProtoParticle::ExtraInfo::const_iterator ifind = 
    p->extraInfo().find ( LHCb::ProtoParticle::MuonPIDStatus );
  //
  if ( p->extraInfo().end () == ifind ) { return 0 ; }       // RETURN
  //
  m_muon.setStatus ( static_cast<unsigned int> ( ifind->second ) ) ;
  //
  return &m_muon ;
}
// ======================================================================
// MANDATORY: virtual destructor 
// ======================================================================
LoKi::ProtoParticles::IsLooseMuon::~IsLooseMuon(){}
// ======================================================================
// MANDATORY: clone method ("virtual constructor")
// ======================================================================
LoKi::ProtoParticles::IsLooseMuon*
LoKi::ProtoParticles::IsLooseMuon::clone() const
{ return new LoKi::ProtoParticles::IsLooseMuon ( *this ) ; }
// ======================================================================
// MANDATORY: the only one essential method
// ======================================================================
LoKi::ProtoParticles::IsLooseMuon::result_type 
LoKi::ProtoParticles::IsLooseMuon::operator() 
  ( LoKi::ProtoParticles::IsLooseMuon::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }  
  //
  const LHCb::MuonPID* pid = muon ( p ) ;
  //
  return ( NULL != pid && pid->IsMuonLoose() );
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::IsLooseMuon::fillStream ( std::ostream& s ) const 
{ return s << " PP_ISLOOSEMUON " ; }
// ============================================================================

// ======================================================================
// MANDATORY: virtual destructor 
// ======================================================================
LoKi::ProtoParticles::IsTightMuon::~IsTightMuon(){}
// ======================================================================
// MANDATORY: clone method ("virtual constructor")
// ======================================================================
LoKi::ProtoParticles::IsTightMuon*
LoKi::ProtoParticles::IsTightMuon::clone() const
{ return new LoKi::ProtoParticles::IsTightMuon ( *this ) ; }
// ======================================================================
// MANDATORY: the only one essential method
// ======================================================================
LoKi::ProtoParticles::IsTightMuon::result_type
LoKi::ProtoParticles::IsTightMuon::operator()
  ( LoKi::ProtoParticles::IsTightMuon::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }
  //
  const LHCb::MuonPID* pid = muon ( p ) ;
  //
  return ( NULL != pid && pid->IsMuonTight() );
}
// ============================================================================
// OPTIONAL: the nice printtout 
// ============================================================================
std::ostream& LoKi::ProtoParticles::IsTightMuon::fillStream ( std::ostream& s ) const
{ return s << " PP_ISTIGHTMUON " ; }
// ============================================================================

// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES 
( const std::string&           path , 
  IDataProviderSvc*            svc  , 
  const LoKi::PPTypes::PPCuts& cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts  ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc      ) 
  , m_cut     ( cuts     ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const std::vector<std::string>& path , 
  IDataProviderSvc*               svc  , 
  const LoKi::PPTypes::PPCuts&    cuts ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts  ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const std::string&           path , 
  const LoKi::PPTypes::PPCuts& cuts ,  
  IDataProviderSvc*            svc  ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts  ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const std::vector<std::string>& path  , 
  const LoKi::PPTypes::PPCuts&    cuts  ,  
  IDataProviderSvc*               svc   ) 
  : LoKi::AuxFunBase ( std::tie ( path , cuts  ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const LoKi::PPTypes::PPCuts& cuts  ,  
  const std::string&           path  ,
  IDataProviderSvc*            svc   ) 
  : LoKi::AuxFunBase ( std::tie ( cuts  , path ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const LoKi::PPTypes::PPCuts&    cuts ,  
  const std::vector<std::string>& path ,
  IDataProviderSvc*               svc  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts  , path ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const LoKi::PPTypes::PPCuts& cuts  ,  
  IDataProviderSvc*            svc   ,  
  const std::string&           path  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts  , path ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( 1 , path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::SourceTES::SourceTES
( const LoKi::PPTypes::PPCuts&    cuts  ,  
  IDataProviderSvc*               svc   ,  
  const std::vector<std::string>& path  ) 
  : LoKi::AuxFunBase ( std::tie ( cuts  , path ) ) 
  , LoKi::PPTypes::PPSources () 
  , m_path    ( path ) 
  , m_dataSvc ( svc  ) 
  , m_cut     ( cuts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::SourceTES::~SourceTES (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::SourceTES*
LoKi::ProtoParticles::SourceTES::clone() const 
{ return new LoKi::ProtoParticles::SourceTES ( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::ProtoParticles::SourceTES::result_type
LoKi::ProtoParticles::SourceTES::operator() 
  ( /* LoKi::ProtoParticles::SourceTES::argument a */ ) const 
{
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  Assert ( !m_path.empty() , "No TES locations are specified!" ) ;
  //
  LHCb::ProtoParticle::ConstVector output ;
  for ( std::vector<std::string>::const_iterator iaddr = 
          m_path.begin() ; m_path.end() != iaddr ; ++iaddr ) 
  { get ( *iaddr , output ) ; }
  //
  return output ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class CONTAINER, class OUTPUT, class PREDICATE>
  inline std::size_t _fill_ 
  ( CONTAINER& cnt , 
    OUTPUT&    out , 
    const PREDICATE& cut ) 
  {
    const std::size_t n = out.size() ;
    out.reserve ( n + cnt->size() ) ;
    LoKi::select ( cnt->begin () , 
                   cnt->end   () , 
                   std::back_inserter ( out ) , cut ) ;
    return out.size() - n ;
  } 
  // ==========================================================================
  template <class CONTAINER, class PREDICATE>
  inline std::size_t _count_ 
  ( CONTAINER& cnt , 
    const PREDICATE& cut ) 
  {
    return LoKi::Algs::count_if ( cnt -> begin () , 
                                  cnt -> end   () , cut ) ;
  } 
  // ==========================================================================
}
// ============================================================================
// get the protoparticles from the certain  TES location 
// ============================================================================
std::size_t LoKi::ProtoParticles::SourceTES::get
( const std::string&                location , 
  LHCb::ProtoParticle::ConstVector& output   ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::ProtoParticle::Container> pps ( m_dataSvc , location ) ;
  if ( !(!pps) ) { return _fill_ ( pps  , output , m_cut.func() ) ; }
  Assert ( false , "No valid data is found at location '" + location + "'") ;
  return 0 ;
}
// ============================================================================
// count the protoparticles from the certain  TES location 
// ============================================================================
std::size_t LoKi::ProtoParticles::SourceTES::count 
( const std::string&                location ) const 
{
  //
  if ( !m_dataSvc ) 
  {
    const LoKi::Services& svcs = LoKi::Services::instance() ;
    m_dataSvc = svcs.evtSvc() ;
    Assert ( m_dataSvc.validPointer ( )               ,
             "Could not locate valid IDataProviderSvc" ) ;
  }
  //
  SmartDataPtr<LHCb::ProtoParticle::Container> pps ( m_dataSvc , location ) ;
  if ( !(!pps) ) { return _count_ ( pps , m_cut.func() ) ; }
  Error ( "No valid data is found at location '" + location + "'") ;
  return 0 ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::SourceTES::fillStream ( std::ostream& o ) const 
{ return o << " PP_SOURCE( " 
           << Gaudi::Utils::toString( m_path ) << " , " << m_cut << " ) " ; }
// ============================================================================


// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::TESCounter::TESCounter 
( const std::string&           path , 
  const LoKi::PPTypes::PPCuts& cuts )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// constructor from the service, TES location and cuts 
// ============================================================================
LoKi::ProtoParticles::TESCounter::TESCounter 
( const std::vector<std::string>& path                    , 
  const LoKi::PPTypes::PPCuts&    cuts )
  : LoKi::AuxFunBase ( std::tie ( path , cuts ) ) 
  , LoKi::Functor<void,double> () 
  , m_source ( path , cuts ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::TESCounter::~TESCounter(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::TESCounter*
LoKi::ProtoParticles::TESCounter::clone() const 
{ return new LoKi::ProtoParticles::TESCounter( *this ) ; }
// ============================================================================
// MANDATORY: the only essential method:
// ============================================================================
LoKi::ProtoParticles::TESCounter::result_type 
LoKi::ProtoParticles::TESCounter::operator() ( /* argument */ ) const 
{
  unsigned long num = 0 ;
  
  typedef std::vector<std::string> List ;
  const List& paths = m_source.paths() ;
  for ( List::const_iterator iaddr = paths.begin() ; 
        paths.end() != iaddr ; ++iaddr ) 
  {
    // count protoparticles 
    num += m_source.count ( *iaddr ) ;
  }
  //
  return num ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::TESCounter::fillStream ( std::ostream& o ) const 
{ return o << "PP_NUM(" 
           << Gaudi::Utils::toString( m_source.paths() ) 
           << "," << m_source.cut() << ")" ; }
// ============================================================================



// ============================================================================
// contructor from predicate
// ============================================================================
LoKi::ProtoParticles::TrackCut::TrackCut
( const LoKi::BasicFunctors<LHCb::Track>::Predicate& cut ) 
  : LoKi::AuxFunBase ( std::tie ( cut ) ) 
  , LoKi::PPTypes::PPCuts ()
  , m_cut ( cut ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::TrackCut::~TrackCut (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::TrackCut*
LoKi::ProtoParticles::TrackCut::clone() const 
{ return new LoKi::ProtoParticles::TrackCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::ProtoParticles::TrackCut::result_type 
LoKi::ProtoParticles::TrackCut::operator() 
  ( LoKi::ProtoParticles::TrackCut::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }
  //
  const LHCb::Track* track = p->track() ;
  if ( NULL == track ) 
  {
    Error ( "LHCb::Track* points to NULL, return 'false'" ) ;
    return false ;                                                    // RETURN    
  }
  //
  return m_cut ( *track ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::TrackCut::fillStream ( std::ostream& o ) const 
{ return o << " PP_TRCUT( " << m_cut << " ) " ; }
// ============================================================================



// ============================================================================
// contructor from function & "bad"-value 
// ============================================================================
LoKi::ProtoParticles::TrackFun::TrackFun
( const LoKi::BasicFunctors<LHCb::Track>::Function& fun , 
  const double                                      bad ) 
  : LoKi::AuxFunBase ( std::tie ( fun , bad  ) ) 
  , LoKi::PPTypes::PPFunc ()
  , m_fun ( fun ) 
  , m_bad ( bad ) 
{}
// ============================================================================
// contructor from function 
// ============================================================================
LoKi::ProtoParticles::TrackFun::TrackFun
( const LoKi::BasicFunctors<LHCb::Track>::Function& fun )
  : LoKi::AuxFunBase ( std::tie ( fun ) ) 
  , LoKi::PPTypes::PPFunc ()
  , m_fun ( fun ) 
  , m_bad ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::TrackFun::~TrackFun (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::TrackFun*
LoKi::ProtoParticles::TrackFun::clone() const 
{ return new LoKi::ProtoParticles::TrackFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::ProtoParticles::TrackFun::result_type 
LoKi::ProtoParticles::TrackFun::operator() 
  ( LoKi::ProtoParticles::TrackFun::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'bad'" ) ;
    return m_bad ;                                                    // RETURN    
  }
  //
  const LHCb::Track* track = p->track() ;
  if ( NULL == track ) 
  {
    Error ( "LHCb::Track* points to NULL, return 'bad'" ) ;
    return m_bad ;                                                    // RETURN    
  }
  //
  return m_fun ( *track ) ;
}
// ============================================================================
// OPTIONAL: the nice printout
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::TrackFun::fillStream ( std::ostream& o ) const 
{ 
  o << " PP_TRFUN( " << m_fun ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) 
  { o << " , " << m_bad ; }
  return o << " ) " ;  
}
// ============================================================================
namespace 
{
  // Unknown = -1,   ///< Unknown particle type
  // Electron,       ///< Represents e+ or e-
  // Muon,           ///< Represents mu+ or mu-
  // Pion,           ///< Represents pi+ or pi-
  // Kaon,           ///< Represents K+ or K-
  // Proton,         ///< Represents Pr+ or Pr-
  // BelowThreshold  ///< Particle type is below threshold
  inline 
  std::string rich_type ( Rich::ParticleIDType p ) 
  {
    switch  ( p ) 
    {
    case Rich::Unknown        : return "Rich::Unknown"  ;
    case Rich::Electron       : return "Rich::Electron" ;
    case Rich::Muon           : return "Rich::Muon"     ;
    case Rich::Pion           : return "Rich::Pion"     ;
    case Rich::Kaon           : return "Rich::Kaon"     ;
    case Rich::Proton         : return "Rich::Proton"         ;
    case Rich::BelowThreshold : return "Rich::BelowThreshold" ;
    default: break ;
    }
    return Gaudi::Utils::toCpp ( p ) ;
  }
}
// ============================================================================
// constructor form the particle type 
// ============================================================================
LoKi::ProtoParticles::RichAboveThres::RichAboveThres 
( Rich::ParticleIDType particle ) 
  : LoKi::AuxFunBase ( std::make_tuple( LoKi::StrKeep( rich_type ( particle ) ) ) ) 
  , LoKi::ProtoParticles::HasRich2Gas () 
  , m_particle ( particle ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::RichAboveThres::~RichAboveThres(){}
// ============================================================================
// MANDATORY: clone mehtod ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::RichAboveThres*
LoKi::ProtoParticles::RichAboveThres::clone() const 
{ return new LoKi::ProtoParticles::RichAboveThres ( *this ) ; }
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
LoKi::ProtoParticles::RichAboveThres::result_type 
LoKi::ProtoParticles::RichAboveThres::operator() 
  ( LoKi::ProtoParticles::RichAboveThres::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return 'false'" ) ;
    return false ;                                                   // RETURN    
  }
  //
  const LHCb::RichPID* pid = rich ( p ) ;
  if ( NULL == pid ) 
  {
    Error ("Can't access the RichPID information, return false") ;
    return false ;
  }
  //
  switch ( m_particle ) 
  {
  case Rich::Electron : return pid -> electronHypoAboveThres () ;  // RETURN
  case Rich::Muon     : return pid ->     muonHypoAboveThres () ;  // RETURN
  case Rich::Pion     : return pid ->     pionHypoAboveThres () ;  // RETURN
  case Rich::Kaon     : return pid ->     kaonHypoAboveThres () ;  // RETURN
  case Rich::Proton   : return pid ->   protonHypoAboveThres () ;  // RETURN
  default: break ;
  }
  //
  Error ( "testRichPID: Unknown RichPrticleID type, return false ") ;
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::RichAboveThres::fillStream ( std::ostream& s ) const 
{
  switch ( m_particle ) 
  {
  case Rich::Electron : return s << " PP_RICHTHRES_E "  ;
  case Rich::Muon     : return s << " PP_RICHTHRES_MU " ;
  case Rich::Pion     : return s << " PP_RICHTHRES_PI " ;
  case Rich::Kaon     : return s << " PP_RICHTHRES_K "  ;
  case Rich::Proton   : return s << " PP_RICHTHRES_P "  ;
  default: break ;
  }
  //
  return s << " PP_RICHTHRES( " << m_particle << " ) " ;
  //
}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::ProtoParticles::NShared::~NShared (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::ProtoParticles::NShared*
LoKi::ProtoParticles::NShared::clone() const 
{ return new LoKi::ProtoParticles::NShared ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::ProtoParticles::NShared::result_type 
LoKi::ProtoParticles::NShared::operator() 
  ( LoKi::ProtoParticles::NShared::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::ProtoParticle* points to NULL, return -10000 " ) ;
    return -10000 ;                                                  // RETURN    
  }
  //
  if ( p -> hasInfo ( LHCb::ProtoParticle::MuonNShared  ) ) 
  { return p -> info ( LHCb::ProtoParticle::MuonNShared , -1000 ) ; }
  //
  const LHCb::MuonPID* muon = p->muonPID() ;
  if ( NULL == muon ) 
  {
    Error ( "LHCb::MuonPID* points to NULL, return -100 " ) ;
    return -100 ;                                                   // RETURN    
  }
  //
  return muon -> nShared () ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::ProtoParticles::NShared::fillStream ( std::ostream& s ) const 
{ return  s << "PP_NSHAREDMU" ; }


// ============================================================================
// The END 
// ============================================================================
