// $Id: Destroyer.cpp,v 1.1 2009-10-31 16:59:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloDigits4Track.h"
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
// ============================================================================
// Local 
// ============================================================================
#include "KaliUtils.h"
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  class Destroyer : public GaudiAlgorithm
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Kali::Destroyer> ;            // the friend factory
    // ========================================================================
  public:
    // ========================================================================
    /// initialize 
    virtual StatusCode initialize () ;
    /// the only one essential method 
    virtual StatusCode execute    () ;
    /// finalize 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard destructor 
     *  @param name (INPUT) the algorithm instance name 
     *  @param pSvc (INPUT) the pointer to Service Locator 
     */
    Destroyer ( const std::string& name ,     //    the algorithm instance name 
                ISvcLocator*       pSvc )     // the pointer to Service Locator 
      : GaudiAlgorithm( name , pSvc ) 
      , m_particles ()
      , m_destroy   ( true )
      , m_toolNames ()
      , m_tools ()
      , m_bremNames ()
      , m_brems ()  
    {
      //
      declareProperty 
        ( "Particles" , 
          m_particles , 
          "The list of input TES locations for particles" );
      declareProperty
        ( "Destroy" ,
          m_destroy , 
          "Destroy the content of TES containers" ) ->
        declareUpdateHandler(&Destroyer::updateDestroy, this ) ;
      //
      m_toolNames.push_back ( "SpdEnergyForTrack/SpdDigits"   ) ;
      m_toolNames.push_back ( "PrsEnergyForTrack/PrsDigits"   ) ;
      m_toolNames.push_back ( "EcalEnergyForTrack/EcalDigits" ) ;
      m_toolNames.push_back ( "HcalEnergyForTrack/HcalDigits" ) ;
      declareProperty 
        ( "Digits4Track" , 
          m_toolNames ,
          "Tools to collect the Calo-digits for the track" );
      m_bremNames.push_back ( "SpdEnergyForTrack/BremSpdDigits"   ) ;
      m_bremNames.push_back ( "PrsEnergyForTrack/BremPrsDigits"   ) ;
      m_bremNames.push_back ( "EcalEnergyForTrack/BremEcalDigits" ) ;
      declareProperty 
        ( "BremDigits4Track" , 
          m_bremNames ,
          "Tools to collect the Brem-digits for the track" );
      // ======================================================================
    }
    /// virtual & protected destructor 
    virtual ~Destroyer() {}                   // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Destroyer ();                        // the default constructor is disabled 
    /// the copy constructor is disabled 
    Destroyer ( const Destroyer& );        // the cpoy  constructor is disabled     
    /// the assignement operator is disabled 
    Destroyer& operator=( const Destroyer& );    // the assignement is disabled
    // ========================================================================
  public:
    // ========================================================================
    /// Update handler for the property
    void updateDestroy ( Property& /* p */ ) ;
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Locations ;
    /// the list of input locations for particles 
    Locations m_particles ;        // the list of input locations for particles 
    /// flag to actually destroy TES 
    bool      m_destroy   ;                    //  flag to actually destroy TES 
    // ========================================================================
    typedef std::vector<std::string>              Names ;
    typedef std::vector<const ICaloDigits4Track*> Tools ;
    /// the list of tools to accumulate the digits  
    Names m_toolNames ;           // the list of tools to accumulate the digits  
    /// the list of tools to accumulate the digits  
    Tools m_tools     ;           // the list of tools to accumulate the digits  
    /// tools for brem-digits collection 
    Names m_bremNames ;                     // tools for brem-digits collection 
    /// tools for brem-digits collection 
    Tools m_brems     ;                     // tools for brem-digits collection 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace Kali
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
// the only one essential method 
// ============================================================================
StatusCode Kali::Destroyer::execute () 
{
  
  LHCb::Particle::ConstVector      particles ;
  
  for ( Locations::const_iterator iparticle = m_particles.begin() ; 
        m_particles.end() != iparticle ; ++iparticle ) 
  {
    if ( exist<LHCb::Particle::Container> ( *iparticle ) ) 
    {
      const LHCb::Particle::Container* p = 
        get<LHCb::Particle::Container> ( *iparticle ) ;
      particles.insert ( particles.end() , p->begin() , p->end() ) ;
    }
    else { Warning ( "Invalid Location for Particles: " + (*iparticle) , 
                     StatusCode::SUCCESS ) ; }
  }
  
  // ==========================================================================
  counter ( "# particles" ) += particles.size() ;
  
  if  ( particles.empty() ) 
  {
    setFilterPassed ( false ) ;
    return Warning ( "No Input particles has been found", StatusCode::SUCCESS ) ;
  }
  
  setFilterPassed ( true ) ;
  
  // get the protoparticles 
  LHCb::ProtoParticle::ConstVector protos ;
  LoKi::Extract::protoParticles_if 
    ( particles.begin() , 
      particles.end  () , 
      std::back_inserter ( protos ) , 
      LoKi::Objects::_ALL_ ) ;
  counter("# protos" ) += protos . size() ;
  
  // get the tracks:
  LHCb::Track::ConstVector tracks ;
  LoKi::Extract::tracks_if 
    ( particles.begin() , 
      particles.end  () , 
      std::back_inserter ( tracks ) , 
      LoKi::Objects::_ALL_ ) ;
  counter("# tracks" ) += tracks . size() ;
  
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
  counter("# hypos"  ) += hypos  . size() ;
  
  // collect all digits: 
  LHCb::CaloDigit::Set digits ;
  
  for ( Hypos::const_iterator ihypo = hypos.begin() ; 
        hypos.end() != ihypo ; ++ihypo ) 
  {
    getDigits ( *ihypo , digits ) ; 
  }
  counter("# digits" ) += digits . size() ;

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
  
  for ( LHCb::Track::ConstVector::const_iterator itrack = tracks.begin() ; 
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
        getDigits ( cluster , trdigits ) ;
      }
    }
    // 1d. collect digits from "the nearest" brem hypo 
    if ( 0 != brTable ) 
    {
      LHCb::Calo2Track::ITrHypoTable::Range r = brTable->relations ( track ) ;
      if ( !r.empty() ) 
      {
        const LHCb::CaloHypo* hypo = r.front().to() ;
        getDigits ( hypo , trdigits ) ;
      }
    }
    // 1.e collect digits from "the nearest" electon hypo 
    if ( 0 != elTable ) 
    {
      LHCb::Calo2Track::ITrHypoTable::Range r = elTable->relations ( track ) ;
      if ( !r.empty() ) 
      {
        const LHCb::CaloHypo* hypo = r.front().to() ;
        getDigits ( hypo , trdigits ) ;
      }
    }
    //
  } // end of loop over "interesting" tracks
  
  counter ( "#trdigs "        ) += trdigits.size() ;
  
  // add "track-related digits" into container of "all" digits 
  digits.insert ( trdigits.begin() , trdigits.end() ) ;
  
  counter ( "#alldigit "      ) += digits.size() ;
  
  if ( !m_destroy ) { return StatusCode::SUCCESS ; }

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
    LHCb::Track::ConstVector::const_iterator itrack = 
      std::find ( tracks.begin() , tracks.end() , track ) ;
    if ( tracks.end() != itrack ) { ++ifind ; continue ; }
    //
    ttracks->erase ( *ifind ) ;
    //
    ifind = ttracks->begin() ;
  }
  const std::size_t trk2 = ttracks->size() ;
  
  counter ( "#trk 1"        ) +=              trk1 ;
  counter ( "#trk 2"        ) +=              trk2 ;
  counter ( "#trk 2/trk 1"  ) += double(trk2)/trk1 ;
  
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
  
  for ( std::vector<Digits*>::iterator ic = digs.begin() ; 
        digs.end() != ic ; ++ic ) 
  {
    Digits* d = *ic ;
    if ( 0 == d ) { continue ; }
    //
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
        d->clear()       ;               // ATTENTION!
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
        digits.erase ( iset   ) ; // erase form original 
        continue                ; // go to the next digit
      }
      //
      d->erase     ( *ifind ) ;
      //
      ifind = d->begin() ;
    }
    //
    size2 += d->size() ;
  }
  
  counter ( "#dig 1"        ) +=               size1 ;
  counter ( "#dig 2"        ) +=               size2 ;
  counter ( "#dig 2/dig 1"  ) += double(size2)/size1 ;


  return StatusCode::SUCCESS ;        
}    
// ===========================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Destroyer)
// ============================================================================
// The END 
// ============================================================================

  

