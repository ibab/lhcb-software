// $Id: L0Muon2Track.cpp,v 1.2 2009-03-22 17:57:42 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
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
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class L0Muon2Track
   *  Simple class which converts L0Muon candidates into "tracks" using 
   *  the special tool by Johannes albrecht 
   *  @see IMuonSeedTrack
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
      /// register the output selection
      m_selection = new Hlt::TSelection<LHCb::Track>( m_output ) ;
      sc = lock -> registerOutput ( m_selection , this ) ;
      Assert ( sc.isSuccess () , "Unable to register OUPUT selection" , sc );
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
      , m_selection ( 0    ) 
      , m_output    ( name ) 
      , m_L0DULocation   ( LHCb::L0DUReportLocation      ::Default ) 
      , m_L0MuonLocation ( LHCb::L0MuonCandidateLocation ::Default ) 
      , m_L0Channel ()
      , m_makerName ( "MuonSeedTool" )
      , m_maker     ( 0 )
    {
      declareProperty 
        ( "OutputSelection"                 , 
          m_output                          ,
          "The name of output selection"    ) ;
      declareProperty
        ( "L0DULocation"                    , 
          m_L0DULocation                    , 
          "TES location of L0DUReport"      ) ;
      declareProperty
        ( "L0MuonCandidateLocation"         , 
          m_L0MuonLocation                  , 
          "TES Location of L0MuonCandidate" ) ;
      declareProperty
        ( "L0Channel"                       , 
          m_L0Channel                       , 
          "L0Channel to be converted "      ) ;
      declareProperty 
        ( "MuonSeedTool" ,
          m_makerName    ,
          "The type/name of muon seed Tool (IMuonSeedTool)" ) ;
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
    /// the selection 
    Hlt::TSelection<LHCb::Track>* m_selection ;                // the selection 
    /// the output selection 
    std::string m_output         ;                                // the output
    /// TES Location of L0DUReport 
    std::string m_L0DULocation   ;                // TES Location of L0DUReport 
    /// TES Location of L0MuonCandidate 
    std::string m_L0MuonLocation ;           // TES Location of L0MuonCandidate 
    /// L0 channel 
    std::string m_L0Channel      ;                           // L0 Muon channel 
    /// Muon SeeD Tool 
    std::string m_makerName      ;                           //  Muon Seed Tool 
    //// the tool 
    IMuonSeedTool* m_maker       ;                                  // the tool 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Hlt 
// ============================================================================
// execute the algorithm 
// ============================================================================
StatusCode Hlt::L0Muon2Track::execute  () 
{
  typedef LHCb::L0MuonCandidate::Container L0Muons ;
  
  // get all L0 muons from TES  
  const L0Muons* l0muons = get<L0Muons> ( m_L0MuonLocation ) ;
  
  bool has_cut  = false ;
  double pt_cut = 0.0   ;
  
  if ( "AllMuon" != m_L0Channel ) 
  {
    const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>( m_L0DULocation );
    /// Retrieve const   L0DU algorithm configuration
    const LHCb::L0DUConfig* l0config = l0->configuration() ;
    Assert ( 0 != l0config , "LHCb::L0DUConfig* points to NULL" ) ;
    const LHCb::L0DUChannel::Map& channels = l0config->channels();
    LHCb::L0DUChannel::Map::const_iterator ichannel = channels.find(m_L0Channel);
    if ( channels.end() == ichannel ) 
    { return Error ( "Invalid L0-Muon channel '" + m_L0Channel + "'" ) ; }    
    // get the conditions 
    const LHCb::L0DUChannel* channel = ichannel->second ;
    Assert ( 0 != channel , "LHCb::L0DUChannel* points to NULL" ) ;
    const LHCb::L0DUElementaryCondition::Map& conditions = 
      channel->elementaryConditions();
    // loop over elementary conditions:
    for ( LHCb::L0DUElementaryCondition::Map::const_iterator
            condition = conditions.begin();
          condition!=conditions.end(); ++condition)
    {       
      // Retrieve const   L0DU Elementary data
      const LHCb::L0DUElementaryCondition* elementary = condition->second ;
      Assert ( 0 != elementary , 
               "LHCb::L0DUElementaryCondition* points to NULL" ) ;
      const LHCb::L0DUElementaryData* data = elementary->data() ;
      Assert ( 0 != data , "LHCb::L0DUElementaryData* points to NULL" ) ;
      // get the nesessary threshold: 
      if ( data->name() == "Muon1(Pt)" || 
           data->name() == "DiMuon(Pt)"  ) 
      {
        has_cut = true ;
        pt_cut  = elementary->threshold() ;
        break ;
      }
    }
    Assert ( has_cut , "Unable to find threshold for L0 Muon channel" ) ;
  }
  
  // create the container of muons/tracks ansd register it in TES 
  LHCb::Track::Container* muons = new LHCb::Track::Container() ;
  put ( muons , "Hlt/Track/" + m_selection -> id () );
  
  
  for ( L0Muons::const_iterator il0 = l0muons->begin() ; l0muons->end() != il0 ; ++il0  )
  {
    const LHCb::L0MuonCandidate* l0muon = *il0 ;
    if ( 0 == l0muon ) { continue ; }
    // check the cut:
    if ( has_cut && pt_cut > l0muon->encodedPt() ){ continue ; }
    // clone ?
    if ( checkClone ( *l0muon ) ) { continue ; }
    // create the track 
    std::auto_ptr<LHCb::Track> track( new LHCb::Track() );
    StatusCode sc = m_maker->makeTrack(*l0muon,*track);
    if ( sc.isFailure() ) 
    { return Error ( "Error from IMuonSeedTool" , sc ); }
    // push into containers :
    m_selection -> push_back ( track.get()     ) ;
    muons       -> insert    ( track.release() ) ;
  }
  
  
  counter ( "#input"  ) +=  l0muons     -> size  () ;
  counter ( "#output" ) +=  m_selection -> size  () ;
  counter ( "#accept" ) += !m_selection -> empty () ;
  
  setFilterPassed ( !m_selection->empty() ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  struct IsMuonTile : public std::unary_function<LHCb::LHCbID,bool>
  {
    /// constructor:
    IsMuonTile
    ( const LHCb::MuonTileID& tile    , 
      const unsigned int      station ) 
      : m_tile    ( tile    ) 
      , m_station ( station ) 
    {}
    /// the main method:
    bool operator() ( const LHCb::LHCbID& id ) 
    { 
      return 
        id.isMuon() && ( id.muonID().station() == m_station ) && ( id == m_tile ) ;  
    }
    ///
  private:
    // ========================================================================
    IsMuonTile() ;
    // ========================================================================
  private:
    // ========================================================================
    /// muon tile ID 
    LHCb::MuonTileID m_tile    ;
    /// muon station 
    unsigned int     m_station ;
    // ========================================================================
  };
  // ==========================================================================
}
// ============================================================================
bool Hlt::L0Muon2Track::checkClone ( const LHCb::L0MuonCandidate&  muon) const
{
  
  const LHCb::MuonTileID tileM1 = muon.muonTileIDs(0).front() ;
  const LHCb::MuonTileID tileM2 = muon.muonTileIDs(1).front() ;
  
  typedef std::vector< LHCb::LHCbID > LHCbIDs ;
  
  for ( Hlt::TSelection<LHCb::Track>::const_iterator itrack = 
          m_selection->begin() ; m_selection->end() != itrack ; ++itrack ) 
  {
    const LHCb::Track* track = *itrack ;
    if ( 0 == track ) { continue ; }
    const LHCbIDs& ids= track -> lhcbIDs() ;
    //
    LHCbIDs::const_iterator iM1 = 
      std::find_if ( ids.begin() , ids.end() , IsMuonTile(tileM1,0) ) ;
    if ( ids.end() == iM1 ) { continue    ; }    // CONTINUE 
    //
    LHCbIDs::const_iterator iM2 = 
      std::find_if ( ids.begin() , ids.end() , IsMuonTile(tileM2,1) ) ;
    if ( ids.end() != iM2 ) { return true ; }    // RETURN 
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
