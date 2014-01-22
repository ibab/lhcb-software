// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
#include "L0Muon2TrackBase.h"
#include "IsMuonTile.h"
// ============================================================================
/** @file
 *  Implementation file for class Hlt::L0Muon2TrackBase
 *  @see Hlt::L0Muon2TrackBase
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-10-02
 *
 */
// ============================================================================
/* standard constructor
 *  @param name algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
Hlt::L0Muon2TrackBase::L0Muon2TrackBase
( std::string        name ,                  //     algorithm instance name
  ISvcLocator*       pSvc )                  //  pointer to Service Locator
  : Hlt::Base ( name , pSvc )
  , m_input     {      }
  , m_selection { nullptr }
  , m_output    { std::move(name) }
  , m_L0DULocation   { LHCb::L0DUReportLocation      ::Default }
  , m_L0Channel {}
  , m_makerName { "MuonSeedTool" }
  , m_maker     { nullptr }
//
  , m_l0data_names ()
{
  declareProperty
    ( "InputSelection"                  ,
      m_input                           ,
      "The name of input selection"     ) ;
  declareProperty
    ( "OutputSelection"                 ,
      m_output                          ,
      "The name of output selection"    ) ;
  declareProperty
    ( "L0DULocation"                    ,
      m_L0DULocation                    ,
      "TES location of L0DUReport"      ) ;
  declareProperty
    ( "L0Channel"                       ,
      m_L0Channel                       ,
      "L0Channel to be converted "      ) ;
  declareProperty
    ( "TrackMaker"  ,
      m_makerName       ,
      "The type/name of muon track maker tool (IMuonSeedTool)" ) ;
  //
  declareProperty
    ( "L0DataNames"  ,
      m_l0data_names = { "Muon1(Pt)", "DiMuon(Pt)"   },
      "The list of L0 data names" ) ;
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
Hlt::L0Muon2TrackBase::~L0Muon2TrackBase () {}
// ============================================================================
// initialize the algorithm
// ============================================================================
StatusCode Hlt::L0Muon2TrackBase::initialize ()
{
  /// initialize the base
  StatusCode sc = Hlt::Base::initialize () ;
  if ( sc.isFailure() ) { return sc ; }                          // REUTRN
  /// Lock the service to enable the output selection registration
  Hlt::IRegister::Lock lock { regSvc() , this } ;
  /// register input selection
  sc = lock -> registerInput  ( m_input     , this ) ;
  Assert ( sc.isSuccess () , "Unable to register INPU    selection" , sc ) ;
  /// register the output selection
  m_selection = new Hlt::TSelection<Hlt::Candidate> ( m_output ) ;
  sc = lock -> registerOutput ( m_selection , this ) ;
  Assert ( sc.isSuccess () , "Unable to register OUTPUT selection"  , sc ) ;
  // get the tool
  Assert ( 0 != maker() , "Unable to get the tool" ) ;
  //
  declareInfo ( "#accept" , "" , &counter("#accept") ,
                0, std::string ("Events accepted by "    ) + name () ) ;
  declareInfo ( "#input"  , "" , &counter("#input" ) ,
                0, std::string ("Candidates seen by "    ) + name () ) ;
  declareInfo ( "#output" , "" , &counter("#output") ,
                0, std::string ("Candidates accepted by ") + name () ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// finalise the algorithm
// ============================================================================
StatusCode Hlt::L0Muon2TrackBase::finalize   ()
{
  m_selection = nullptr ;
  m_maker     = nullptr ;
  return Hlt::Base::finalize () ;
}
// ============================================================================
/*  seek for muon in already converted tracks
 *  I do not understand the algorithm :-(
 */
// ============================================================================
const LHCb::Track*  Hlt::L0Muon2TrackBase::seekMuon
( const LHCb::L0MuonCandidate& l0muon ,
  LHCb::Track::Container*      muons  ) const
{
  if ( !muons ) { return seekMuon ( l0muon , getMuons() ) ; } // RETURN
  //
  const LHCb::MuonTileID tileM1 = l0muon.muonTileIDs(0).front() ;
  const LHCb::MuonTileID tileM2 = l0muon.muonTileIDs(1).front() ;
  //
  for ( const LHCb::Track *muon : *muons )
  {
    if ( muon && isMuonClone ( muon , tileM1 , tileM2 ) ) { return muon ; }
  }
  return nullptr ;
}
// ============================================================================
// get L0-muon cuts... I do not understand these lines :-(
// ============================================================================
std::pair<LoKi::L0::L0MuonCut,bool>
Hlt::L0Muon2TrackBase::muonCut() const
{
  using namespace LoKi::L0 ;
  L0MuonCut cut ( m_L0Channel );
  //
  bool noMuon  = false;
  if ( "AllMuon" != m_L0Channel )
  {
    const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>( m_L0DULocation );
    //
    //@TODO: Cache, depending on L0 TCK
    //@TODO: what about L0 dimuon trigger? Can we represent it by cuts on single muons?????????
    //
    L0MuonCuts cuts ;
    StatusCode sc = LoKi::L0::getL0Cuts
      ( l0 , m_L0Channel , m_l0data_names , cuts ) ;
    Assert ( sc.isSuccess  () , "Unable to extract the proper L0MuonCuts" , sc ) ;
    noMuon = ( cuts.size() != 1 );
    if (!noMuon) cut = cuts.front() ;
  }
  //
  if ( noMuon )
  {
    Warning( " L0 channel " + m_L0Channel +
             " does not use any known type of l0MuonCandidate?" +
             " no candidates converted!"  ,
             StatusCode::SUCCESS, 1 ).ignore();
  }
  //
  return { cut , noMuon } ;
}
// ========================================================================

// ============================================================================
// The END
// ============================================================================
