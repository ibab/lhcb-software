// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelationWeighted2D.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/Calo2Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysExtract.h"
#include "LoKi/Objects.h"
#include "LoKi/Photons.h"
// ============================================================================
// Local/Kali
// ============================================================================
#include "Destroyer.h"
// ============================================================================
/** @file 
 *  Implementation file for class Kali::Destroyer
 *
 *  @see Kali::Destroyer 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date xxxx-xx-xx
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
// Update handler for the property
// ============================================================================
void Kali::Destroyer::updateDestroy ( Property& /* p */ ) 
{
  if ( FSMState() < Gaudi::StateMachine::INITIALIZED ) { return ; }   // RETURN 
  // 
  if ( m_destroy )
  { Warning ( "TES containers will be destroyed!" , StatusCode::SUCCESS ) ; }
  else 
  { Warning ( "TES containers will be preserved!" , StatusCode::SUCCESS ) ; }
  //
}
// ============================================================================
// finalize 
// ============================================================================
StatusCode Kali::Destroyer::finalize () 
{
  m_tools.clear() ;
  m_brems.clear() ;
  return GaudiAlgorithm::finalize () ;
}
// ============================================================================
// initialize 
// ============================================================================
StatusCode Kali::Destroyer::initialize () 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }                               // RETURN 
  //
  m_tools.clear() ;
  for ( Names::const_iterator iname = m_toolNames.begin() ;
        m_toolNames.end() != iname ; ++iname ) 
  {
    const ICaloDigits4Track* t = tool<ICaloDigits4Track> ( *iname , this ) ;
    m_tools.push_back ( t ) ;
  }
  //
  m_brems.clear() ;
  for ( Names::const_iterator ibrem = m_bremNames.begin() ;
        m_bremNames.end() != ibrem ; ++ibrem ) 
  {
    const ICaloDigits4Track* t = tool<ICaloDigits4Track> ( *ibrem , this ) ;
    m_brems.push_back ( t ) ;
  }
  //
  return StatusCode::SUCCESS ;
}    
// ============================================================================
// collect all ``interesting'' digits & tracks 
// ============================================================================
StatusCode Kali::Destroyer::collect 
( LHCb::CaloDigit::Set&    digits , 
  Kali::Destroyer::TRACKS& tracks ) const 
{
  //
  // input particles:
  //
  LHCb::Particle::ConstVector      particles ;
  
  for ( Locations::const_iterator iparticle = m_particles.begin() ; 
        m_particles.end() != iparticle ; ++iparticle ) 
  {
    // FIX ME: it should be Range - here!!
    if ( exist<DataObject> ( *iparticle ) ) 
    {
      const LHCb::Particle::Range p = 
        get<LHCb::Particle::Range> ( *iparticle ) ;
      particles.insert ( particles.end() , p.begin() , p.end() ) ;
    }
    else { Warning ( "Invalid Location for Particles: " + (*iparticle) , 
                     StatusCode::SUCCESS ) ; }
  }
  
  // ==========================================================================
  counter ( "# particles" ) += particles.size() ;
  if  ( particles.empty() ) 
  { return Warning ( "No Input particles has been found", StatusCode::SUCCESS ) ; }
  // 
  
  // get the protoparticles 
  LHCb::ProtoParticle::ConstVector protos ;
  LoKi::Extract::protoParticles_if 
    ( particles.begin() , 
      particles.end  () , 
      std::back_inserter ( protos ) , 
      LoKi::Objects::_ALL_ ) ;
  counter ("# protos" ) += protos . size() ;
  
  // get the tracks:
  LHCb::Track::ConstVector tracks_ ;
  LoKi::Extract::tracks_if 
    ( particles.begin() , 
      particles.end  () , 
      std::back_inserter ( tracks_ ) , 
      LoKi::Objects::_ALL_ ) ;
  //
  tracks.insert ( tracks_.begin() , tracks_.end() ) ;
  counter ("# tracks" ) += tracks . size() ;
  
  // collect all hypos 
  typedef std::vector<const LHCb::CaloHypo*> Hypos ;
  Hypos hypos ;
  
  for ( LHCb::ProtoParticle::ConstVector::const_iterator iproto = 
          protos.begin() ; protos.end() != iproto ; ++iproto ) 
  {
    const LHCb::ProtoParticle* proto = *iproto ;
    if ( 0 == proto ) { continue ; }
    const SmartRefVector<LHCb::CaloHypo>& calo = proto->calo() ;
    hypos.insert ( hypos.end() , calo.begin() , calo.end  () ) ;
  }
  counter ("# hypos"  ) += hypos  . size() ;
  
  // collect all digits: 
  for ( Hypos::const_iterator ihypo = hypos.begin() ; 
        hypos.end() != ihypo ; ++ihypo ) 
  {
    LoKi::Photons::getDigits ( *ihypo , digits ) ; 
  }
  counter ( "# digits" ) += digits . size() ;
  
  // ==========================================================================
  // collect digits from tracks 
  // ==========================================================================
  
  // cluster match :  Track -> Cluster 
  const LHCb::Calo2Track::ITrClusTable* phTable = 0 ;
  // brem    match:  Track -> CaloHypo 
  const LHCb::Calo2Track::ITrHypoTable* brTable = 0 ;
  // electron match:  Track -> CaloHypo 
  const LHCb::Calo2Track::ITrHypoTable* elTable = 0 ;
  
  if ( !tracks.empty() )
  {
    const LHCb::Calo2Track::IClusTrTable2D* _table = 
      get<LHCb::Calo2Track::IClusTrTable2D>          ( LHCb::CaloIdLocation::ClusterMatch   ) ;
    if ( 0 != _table ) { phTable = _table->inverse() ; }
    brTable = get<LHCb::Calo2Track::ITrHypoTable>    ( LHCb::CaloIdLocation::BremMatch     ) ;
    elTable = get<LHCb::Calo2Track::ITrHypoTable>    ( LHCb::CaloIdLocation::ElectronMatch ) ;
  }
  
  // 1. for tracks, pickup the digits 
  LHCb::CaloDigit::Set trdigits ;
  
  for ( TRACKS::iterator itrack = tracks.begin() ; 
        tracks.end() != itrack ; ++itrack ) 
  {
    const LHCb::Track* track = *itrack ;
    if ( 0 == track ) { continue ; }                              // CONTINUE
    
    // 1a. collect digits form calorimeters:
    for ( Tools::const_iterator itool = m_tools.begin() ; 
          m_tools.end() != itool ; ++itool ) 
    {
      const ICaloDigits4Track* t = *itool ;
      if ( 0 == t ) { continue ; }                                 // CONTINUE
      //
      LHCb::CaloDigit::Set tmp ;
      StatusCode sc = t->collect ( track , tmp ) ; 
      if ( sc.isFailure() ) { continue ; }                        // CONTINUE 
      trdigits.insert ( tmp.begin() , tmp.end() ) ;   
    }
    // 1a. collect digits form calorimeters:
    for ( Tools::const_iterator ibrem = m_brems.begin() ; 
          m_brems.end() != ibrem ; ++ibrem ) 
    {
      const ICaloDigits4Track* t = *ibrem ;
      if ( 0 == t ) { continue ; }                                 // CONTINUE
      //
      LHCb::CaloDigit::Set tmp ;
      const ICaloDigits4Track::Line line 
        ( track->closestState ( 0.0 ).position () , 
          track->closestState ( 0.0 ).slopes   () ) ;
      StatusCode sc = t->collect ( line , tmp ) ; 
      if ( sc.isFailure() ) { continue ; }                        // CONTINUE 
      trdigits.insert ( tmp.begin() , tmp.end() ) ;   
    }
    // 1c. collect digits from "nearest" cluster 
    if ( 0 != phTable ) 
    {
      LHCb::Calo2Track::ITrClusTable::Range r = phTable->relations ( track ) ;
      if ( !r.empty() ) 
      {
        const LHCb::CaloCluster* cluster = r.front().to() ;
        LoKi::Photons::getDigits ( cluster , trdigits ) ;
      }
    }
    // 1d. collect digits from "the nearest" brem hypo 
    if ( 0 != brTable ) 
    {
      LHCb::Calo2Track::ITrHypoTable::Range r = brTable->relations ( track ) ;
      if ( !r.empty() ) 
      {
        const LHCb::CaloHypo* hypo = r.front().to() ;
        LoKi::Photons::getDigits ( hypo , trdigits ) ;
      }
    }
    // 1e. collect digits from "the nearest" electon hypo 
    if ( 0 != elTable ) 
    {
      LHCb::Calo2Track::ITrHypoTable::Range r = elTable->relations ( track ) ;
      if ( !r.empty() ) 
      {
        const LHCb::CaloHypo* hypo = r.front().to() ;
        LoKi::Photons::getDigits ( hypo , trdigits ) ;
      }
    }
    //
  } // end of loop over "interesting" tracks
  
  counter ( "#trdigs "        ) += trdigits.size() ;
  
  // add "track-related digits" into container of "all" digits 
  digits.insert ( trdigits.begin() , trdigits.end() ) ;
  
  counter ( "#alldigit "      ) += digits.size() ;
  
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
// destroy all ``interesting'' digits & tracks 
// ============================================================================
StatusCode Kali::Destroyer::destroy
( LHCb::CaloDigit::Set&    digits , 
  Kali::Destroyer::TRACKS& tracks ) const 
{
  
  // ==========================================================================
  // destroy tracks 
  // ==========================================================================
  
  LHCb::Track::Container* ttracks = 
    get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
  
  const std::size_t trk1 = ttracks->size() ;
  
  if ( tracks.empty() ) { ttracks->clear() ; } // ATTENTION!!!  
  LHCb::Track::Container::iterator ifind = ttracks->begin() ;
  while ( ifind != ttracks->end() ) 
  {
    const LHCb::Track* track = *ifind ;
    if ( 0 == track ) { continue ; }
    // 
    if ( tracks.end() != tracks.find( track ) )
    { ++ifind ; continue ; }                               // CONTINUE 
    //
    ttracks->erase ( *ifind ) ;
    //
    ifind = ttracks->begin() ;
  }
  const std::size_t trk2 = ttracks->size() ;
  //
  counter ( "#trk 1"        ) +=              trk1 ;
  counter ( "#trk 2"        ) +=              trk2 ;
  counter ( "#trk 2/trk 1"  ) +=  0 < trk1 ? double(trk2)/trk1 : 1.0 ;
  
  // ==========================================================================
  // destroy digits
  // ==========================================================================
  
  typedef LHCb::CaloDigit::Container Digits ;
  
  std::vector<Digits*> digs ;
  
  Digits* d1 = get<Digits> ( LHCb::CaloDigitLocation::Spd  ) ;
  digs.push_back ( d1 ) ;
  Digits* d2 = get<Digits> ( LHCb::CaloDigitLocation::Prs  ) ;
  digs.push_back ( d2 ) ;
  Digits* d3 = get<Digits> ( LHCb::CaloDigitLocation::Ecal ) ;
  digs.push_back ( d3 ) ;
  Digits* d4 = get<Digits> ( LHCb::CaloDigitLocation::Hcal ) ;
  digs.push_back ( d4 ) ;
  
  size_t size1 = 0 ;
  size_t size2 = 0 ;
  
  for ( std::vector<Digits*>::iterator ic = digs.begin() ; digs.end() != ic ; ++ic ) 
  {
    Digits* d = *ic ;
    if ( 0 == d ) { continue ; }
    //
    size1 += d->size() ;
    //
    if ( digits.empty() ) { d->clear() ; }
    //
    Digits::iterator ifind = d->begin() ;
    while ( ifind != d->end() ) 
    {
      //
      if ( digits.empty() ) 
      { 
        d->erase ( ifind , d->end() )  ; // ATTENTION!
        ifind = d->end() ;               // paranoic...
        break            ;               // BREAK  
      } 
      //
      const LHCb::CaloDigit* digit = *ifind ;
      if ( 0 == digit     ) { continue ; }
      //
      LHCb::CaloDigit::Set::iterator iset = digits.find ( digit ) ;
      if ( digits.end() != iset ) 
      { 
        ++ifind                 ; // found, keep it
        digits.erase ( iset   ) ; // erase from original 
        continue                ; // go to the next digit
      }
      //
      const size_t index = ifind - d->begin() ;
      d->erase ( ifind ) ;
      ifind = d->begin() + index ;
      //
    }
    //
    size2 += d->size() ;
    //
  }
  
  counter ( "#dig 1"        ) +=               size1 ;
  counter ( "#dig 2"        ) +=               size2 ;
  counter ( "#dig 2/dig 1"  ) += double(size2)/size1 ;
  
  counter ( "#digs Spd"     ) += d1 -> size() ;
  counter ( "#digs Prs"     ) += d2 -> size() ;
  counter ( "#digs Ecal"    ) += d3 -> size() ;
  counter ( "#digs Hcal"    ) += d4 -> size() ;

  return StatusCode::SUCCESS ;        
}
// ============================================================================
// copy all ``interesting'' digits  
// ============================================================================
StatusCode Kali::Destroyer::copy
( LHCb::CaloDigit::Set&    digits , 
  Kali::Destroyer::TRACKS& tracks ) const 
{
  // ==========================================================================
  // copy tracks 
  // ==========================================================================
  LHCb::Track::Container* otracks = 
    get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
  //
  LHCb::Track::Container* ntracks = new LHCb::Track::Container() ;
  put ( ntracks ,        kalify ( LHCb::TrackLocation::Default ) ) ;
  //
  for ( TRACKS::iterator itrack = tracks.begin() ;
        tracks.end() != itrack ; ++itrack ) 
  {
    const LHCb::Track* track = *itrack ;
    if ( 0       == track           ) { continue ; }
    if ( otracks != track->parent() ) { continue ; }
    //
    ntracks->insert ( track->cloneWithKey() ) ; // CLONE WITH KEY !!!
  }
  //
  const std::size_t trk1 = otracks->size() ;
  const std::size_t trk2 = ntracks->size() ;
  //
  counter ( "#trk 1"        ) +=              trk1 ;
  counter ( "#trk 2"        ) +=              trk2 ;
  counter ( "#trk 2/trk 1"  ) +=  0 < trk1 ? double(trk2)/trk1 : 1.0 ;
  
  // ==========================================================================
  // copy digits 
  // ==========================================================================
  
  typedef LHCb::CaloDigit::Container          Digits ;
  
  typedef std::map<std::string,const Digits*> DIGITS ;
  
  DIGITS odigs ;
  
  Digits* d1 = get<Digits> ( LHCb::CaloDigitLocation::Spd  ) ;
  odigs [ LHCb::CaloDigitLocation::Spd  ] =  d1  ;
  Digits* d2 = get<Digits> ( LHCb::CaloDigitLocation::Prs  ) ;
  odigs [ LHCb::CaloDigitLocation::Prs  ] =  d2  ;
  Digits* d3 = get<Digits> ( LHCb::CaloDigitLocation::Ecal ) ;
  odigs [ LHCb::CaloDigitLocation::Ecal ] =  d3  ;
  Digits* d4 = get<Digits> ( LHCb::CaloDigitLocation::Hcal ) ;
  odigs [ LHCb::CaloDigitLocation::Hcal ] =  d4  ;

  
  size_t size1 = 0 ;
  size_t size2 = 0 ;
  
  for ( DIGITS::const_iterator ic = odigs.begin() ; odigs.end() != ic ; ++ic ) 
  {
    const std::string& loc  = ic->first   ;
    const Digits*      digs = ic->second  ;
    if ( 0 == digs ) { continue ; }
    //
    Digits* ndigs = new Digits()   ;   // create new container 
    put ( ndigs , kalify ( loc ) ) ;   // register it in TES 
    //
    size1 += digs->size() ;
    //
    for ( LHCb::CaloDigit::Set::iterator idig = 
            digits.begin() ; digits.end() != idig ;  ++idig ) 
    {
      const LHCb::CaloDigit* dig = *idig ;
      if ( 0    == dig           ) { continue ; }
      if ( digs != dig->parent() ) { continue ; }
      //
      ndigs->insert ( dig->clone() ) ; // CLONE WITH THE KEY
    }
    //
    size2 += ndigs->size() ;
    //
  }
  
  counter ( "#dig 1"        ) +=               size1 ;
  counter ( "#dig 2"        ) +=               size2 ;
  counter ( "#dig 2/dig 1"  ) += double(size2)/size1 ;
  
  return StatusCode::SUCCESS ;        
}
// ============================================================================
// the only one essential method 
// ============================================================================
StatusCode Kali::Destroyer::execute () 
{ 
  // collect all interetsing digits 
  LHCb::CaloDigit::Set     digits ;
  // collect all interesting tracks 
  TRACKS                   tracks ;
  
  // 
  // collect info 
  StatusCode sc = collect ( digits , tracks ) ;
  if ( sc.isFailure() ) 
  {
    Warning ("Error from collect", sc ) ;
    setFilterPassed ( false ) ;
    return StatusCode::SUCCESS ;
  }
  //
  const bool OK = !digits.empty() || !tracks.empty();
  //
  // destroy TES
  sc = destroy ( digits , tracks ) ;
  if ( sc.isFailure() ) 
  {
    Warning ("Error from destroy", sc ) ;
    setFilterPassed ( false ) ;
    return StatusCode::SUCCESS ;
  }
  //
  setFilterPassed  ( OK ) ;
  //
  return StatusCode::SUCCESS ;        
}    
// ===========================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Destroyer)
// ============================================================================
// The END 
// ============================================================================

  

