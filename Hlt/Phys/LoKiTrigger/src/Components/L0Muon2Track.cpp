// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/L0MuonCandidate.h"
#include "Event/L0DUReport.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/MuonTileID.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/IMuonSeedTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBase.h"
#include "LoKi/HltL0.h"
// ============================================================================
// Local 
// ============================================================================
#include "IsMuonTile.h"
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class L0Muon2Track
   *  Simple class which converts L0Muon candidates into "tracks" using 
   *  the special tool by Johannes albrecht 
   *  @see IMuonSeedTrack
   *  The actual lines are stolen from 
   *     Gerhard Raven & Jose Angel Hernando  Morata
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2000-03-19
   */
  class L0Muon2Track : public Hlt::Base 
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Hlt::L0Muon2Track> ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () 
    {
      /// initialize the base 
      StatusCode sc = Hlt::Base::initialize () ;
      if ( sc.isFailure() ) { return sc ; }                          // REUTRN
      /// Lock the service to enable the output selection registration 
      Hlt::IRegister::Lock lock ( regSvc() , this ) ;
      /// register input selection 
      sc = lock -> registerInput  ( m_input     , this ) ;
      Assert ( sc.isSuccess () , "Unable to register INPU    selection" , sc ) ;
      /// register the output selection
      m_selection = new Hlt::TSelection<Hlt::Candidate> ( m_output ) ;
      sc = lock -> registerOutput ( m_selection , this ) ;
      Assert ( sc.isSuccess () , "Unable to register OUTPUT selection"  , sc ) ;
      // get the tool 
      m_maker = tool<IMuonSeedTool>( m_makerName , this ) ; // PRIVATE ???
      
      declareInfo ( "#accept" , "" , &counter("#accept") , 
                    0, std::string ("Events accepted by "    ) + name () ) ;
      declareInfo ( "#input"  , "" , &counter("#input" ) ,
                    0, std::string ("Candidates seen by "    ) + name () ) ;
      declareInfo ( "#output" , "" , &counter("#output") ,
                    0, std::string ("Candidates accepted by ") + name () ) ;
      //
      return StatusCode::SUCCESS ;
    }
    /// execute the algorithm 
    virtual StatusCode execute  () ;
    /// finalize the algorithm 
    virtual StatusCode finalize () 
    {
      m_selection = 0 ;
      m_maker     = 0 ;
      return Hlt::Base::finalize () ;
    }
    /// =======================================================================
  protected:
    // ========================================================================
    /** standard constructor  
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    L0Muon2Track
    ( const std::string& name ,                  //     algorithm instance name 
      ISvcLocator*       pSvc )                  //  pointer to Service Locator 
      : Hlt::Base ( name , pSvc ) 
      , m_input     (      ) 
      , m_selection ( 0    ) 
      , m_output    ( name )
      , m_L0DULocation   ( LHCb::L0DUReportLocation      ::Default ) 
      , m_L0Channel ()
      , m_makerName ( "MuonSeedTool" )
      , m_maker     ( 0 )
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
      m_l0data_names.push_back (   "Muon1(Pt)" ) ;
      m_l0data_names.push_back (  "DiMuon(Pt)" ) ;
      declareProperty 
        ( "L0DataNames"  , 
          m_l0data_names , 
          "The list of L0 data names" ) ;
    }
    
    /// virtual and protected destructor 
    virtual ~L0Muon2Track() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    L0Muon2Track () ;                    // the default constructor is disabled 
    /// the copy constructor is disabled 
    L0Muon2Track ( const L0Muon2Track& ) ;  // the copy constructor is disabled 
    /// the assignement operator is disabled 
    L0Muon2Track& operator=( const L0Muon2Track& ) ; // assignement is disabled
    // ========================================================================
  protected:
    // ========================================================================
    bool checkClone ( const LHCb::L0MuonCandidate&  muon) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the input selection 
    Hlt::IRegister::Key           m_input     ;                    // the input
    /// the selection 
    Hlt::TSelection<Hlt::Candidate>* m_selection ;             // the selection 
    /// the output selection 
    std::string m_output         ;                                // the output
    /// TES Location of L0DUReport 
    std::string m_L0DULocation   ;                // TES Location of L0DUReport 
    /// L0 channel 
    std::string m_L0Channel      ;                           // L0 Muon channel 
    /// Muon SeeD Tool 
    std::string m_makerName      ;                           //  Muon Seed Tool 
    //// the tool 
    IMuonSeedTool* m_maker       ;                                  // the tool 
    // ========================================================================
  private: // pure techcal stuff 
    // ========================================================================
    /// the vector of elementary L0 data names 
    std::vector<std::string>   m_l0data_names ; // the vector of L0 data names 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Hlt 
// ============================================================================
// execute the algorithm 
// ============================================================================
StatusCode Hlt::L0Muon2Track::execute  () 
{ 
  typedef  Hlt::TSelection<Hlt::Candidate> Input ;
  
  /// get input selection from Hlt-service 
  const Input* input = hltSvc()->get<Hlt::Candidate>  ( m_input , this ) ;
  Assert ( 0 != input           , "Input  selection is invalid!" ) ;
  
  /// check the output selectiom
  Assert ( m_selection->empty() , "Output selection is not empty!" ) ;
  
  /// create the container of muons/tracks and register it in TES 
  LHCb::Track::Container* muons = new LHCb::Track::Container() ;
  put ( muons , "Hlt/Track/Tracks_" + m_selection -> id ().str() );
  
  /// constaiener of stages 
  Hlt::Stage::Container* stages = hltStages() ;
  
  using namespace LoKi::L0 ;
  
  L0MuonCut cut ( m_L0Channel );  
  
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
  
  if (noMuon) 
  {
    Warning( " L0 channel " + m_L0Channel + 
             " does not use any known type of l0MuonCandidate?" + 
             " no candidates converted!"  , 
             StatusCode::SUCCESS, 1 ).ignore();
  }
  else 
  {
    // loop over input data
    for ( Input::const_iterator item = input->begin() ; 
          input->end() != item ; ++item  )
    {
      const Hlt::Candidate*   candidate = *item ;
      if ( 0 == candidate ) { continue ; }
      //
      const Hlt::Stage* stage     = candidate->currentStage() ;
      if ( 0 == stage     ) { continue ; }
      //  
      const LHCb::L0MuonCandidate* l0muon = stage->get<LHCb::L0MuonCandidate>() ;
      //
      debug() << "l0pt " << l0muon->pt() << " l0encodedPt " << l0muon->encodedPt()<< endmsg;
      // check the cut:
      if ( !cut ( l0muon )        ) { continue ; }                     // CONTINUE 
      debug() << "l0pt " << l0muon->pt() << " l0encodedPt " << l0muon->encodedPt()<< " accept " << endmsg;
      // clone ?
      if ( checkClone ( *l0muon ) ) 
      { 
        debug() << "is clone " << endmsg;
        continue ; 
      }                     // CONTINUE 
      // create the track 
      std::auto_ptr<LHCb::Track> track( new LHCb::Track()  ) ;
      StatusCode sc = m_maker->makeTrack( *l0muon , *track ) ;
      if ( sc.isFailure() ) 
      { Error ( "Error from IMuonSeedTool" , sc ) ; continue ;  } // CONTINUE 
      //
      // keep it: create new stage 
      Hlt::Stage* newstage = new Hlt::Stage() ;
      stages      -> push_back   ( newstage ) ;
      const_cast<Hlt::Candidate*>(candidate)   -> addToStages ( newstage ) ;
      //
      Hlt::Stage::Lock lock    ( newstage , this  ) ;
      newstage    -> set       ( track.get ()     ) ;      
      muons       -> insert    ( track.release () ) ; // release 
      //
      m_selection -> push_back ( candidate ) ; 
    }
  }
  
  if (msgLevel(MSG::DEBUG)) 
  {
    debug() << "# Input: "      << input       -> size() 
            << " -> # Output: " << m_selection -> size() << endreq;
  }
  
  counter ( "#input"  ) +=  input       -> size  () ;
  counter ( "#output" ) +=  m_selection -> size  () ;
  counter ( "#accept" ) += !m_selection -> empty () ;
  m_selection -> setDecision ( !m_selection->empty() );
  
  setFilterPassed ( !m_selection->empty() ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
bool Hlt::L0Muon2Track::checkClone ( const LHCb::L0MuonCandidate&  muon) const
{
  
  const LHCb::MuonTileID tileM1 = muon.muonTileIDs(0).front() ;
  const LHCb::MuonTileID tileM2 = muon.muonTileIDs(1).front() ;
  
  typedef std::vector< LHCb::LHCbID > LHCbIDs ;
  
  for ( Hlt::TSelection<Hlt::Candidate>::const_iterator icand = 
          m_selection->begin() ; m_selection->end() != icand ; ++icand ) 
  {
    // 
    const Hlt::Candidate* cand  = *icand ;
    if ( 0 == cand  ) { continue ; }
    //
    const Hlt::Stage*     stage = cand->currentStage () ;
    if ( 0 == stage ) { continue ; }
    //
    const LHCb::Track* track = stage->get<LHCb::Track>() ;
    if ( 0 == track ) { continue ; }
    //
    if ( isMuonClone ( track , tileM1 , tileM2 ) ) { return true ; }
  }
  return false ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,L0Muon2Track)
// ============================================================================
// The END 
// ============================================================================
