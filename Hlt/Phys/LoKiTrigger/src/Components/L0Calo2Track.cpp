// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUReport.h"
#include "Event/State.h"
// ============================================================================
// HltBase
// ============================================================================
#include "HltBase/ICaloSeedTool.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBase.h"
#include "LoKi/HltL0.h"
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class L0Calo2Track
   *  Simple class which converts L0Calo candidates into "tracks" using
   *  the special tool
   *  @see ICaloSeedTrack
   *  The actual lines are stolen from
   *     Gerhard Raven & Jose Angel Hernando  Morata
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2000-03-23
   */
  class L0Calo2Track : public Hlt::Base
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Hlt::L0Calo2Track> ;
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
      Hlt::IRegister::Lock lock { regSvc() , this } ;
      /// register the output selection
      m_selection = new Hlt::TSelection<LHCb::Track>{ m_output } ;
      sc = lock -> registerOutput ( m_selection , this ) ;
      Assert ( sc.isSuccess () , "Unable to register OUTPUT selection" , sc );
      // get the tool
      if ( m_makerName.empty() )
      { m_maker = 0 ; }
      else
      { m_maker = tool<ICaloSeedTool>( m_makerName , this ) ;  } // PRIVATE ???
      //
      declareInfo ( "#accept" , "" , &counter("#accept") ,
                    0, std::string ("Events accepted by "    ) + name () ) ;
      declareInfo ( "#input"  , "" , &counter("#input" ) ,
                    0, std::string ("Candidates seen by "    ) + name () ) ;
      declareInfo ( "#output" , "" , &counter("#output") ,
                    0, std::string ("Candidates accepted by ") + name () ) ;
      //
      // get InfoID
      m_infoID = hltInfoID ( m_infoName ) ;
      //
      StringProperty empty ;
      update1 ( empty ) ;
      update2 ( empty ) ;

      return StatusCode::SUCCESS ;
    }
    /// execute the algorithm
    virtual StatusCode execute  () ;
    /// finalize the algorithm
    virtual StatusCode finalize ()
    {
      m_selection = nullptr ;
      m_maker     = nullptr ;
      return Hlt::Base::finalize () ;
    }
    /// =======================================================================
  protected:
    // ========================================================================
    /** standard constructor
     *  @param name algorithm instance name
     *  @param pSvc pointer to Service Locator
     */
    L0Calo2Track
    ( const std::string& name ,                  //     algorithm instance name
      ISvcLocator*       pSvc )                  //  pointer to Service Locator
      : Hlt::Base ( name , pSvc )
      , m_selection { nullptr }
      , m_output    { name }
      , m_L0DULocation   { LHCb::L0DUReportLocation      ::Default }
      , m_L0CaloLocation { LHCb::L0CaloCandidateLocation ::Default }
      , m_L0Channel { }
      , m_makerName { } // EMPTY ??
      , m_maker     { nullptr }
      //
      , m_types1    {}
      , m_types2    {}
      , m_type_map1 {}
      , m_type_map2 {}
      //
      , m_infoName { "L0ET" }
      , m_infoID   { -1     }
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
        ( "L0CaloCandidateLocation"         ,
          m_L0CaloLocation                  ,
          "TES Location of L0CaloCandidate" ) ;
      declareProperty
        ( "L0Channel"                       ,
          m_L0Channel                       ,
          "L0Channel to be converted "      ) ;
      declareProperty
        ( "TrackMaker"  ,
          m_makerName   ,
          "The type/name of calo track maker tool (ICaloSeedTool)" ) ;
      //
      { // types1
        declareProperty(  "ConditionIDs"
                       , m_type_map1 = {
                             {  "Electron(Et)" , L0DUBase::CaloType::Electron  },
                             {    "Photon(Et)" , L0DUBase::CaloType::Photon    },
                             {    "Hadron(Et)" , L0DUBase::CaloType::Hadron    },
                             {  "LocalPi0(Et)" , L0DUBase::CaloType::Pi0Local  },
                             { "GlobalPi0(Et)" , L0DUBase::CaloType::Pi0Global }
                                       }
                       , "Mapping Condition Name -> ChannelID" )
          -> declareUpdateHandler ( &Hlt::L0Calo2Track::update1 , this ) ;
      }
      { // types2
        declareProperty ( "ChannelIDs"
                        , m_type_map2  = {
                             {  "AllElectron" , L0DUBase::CaloType::Electron  },
                             {    "AllPhoton" , L0DUBase::CaloType::Photon    },
                             {    "AllHadron" , L0DUBase::CaloType::Hadron    },
                             {  "AllLocalPi0" , L0DUBase::CaloType::Pi0Local  },
                             { "AllGlobalPi0" , L0DUBase::CaloType::Pi0Global }
                                }
                        , "Mapping Channel   Name -> ChannelID" )
          -> declareUpdateHandler ( &Hlt::L0Calo2Track::update2 , this ) ;
      }
      // ======================================================================
      declareProperty ( "InfoID"
                      , m_infoName
                      , "The tag for InfoID" ) ;
    }
    /// virtual and protected destructor
    virtual ~L0Calo2Track() {}
    // ========================================================================
  protected:
    // ========================================================================
    void update1 ( Property& /* p */ ) // update the conditions map
    {
      m_types1.clear() ;
      for ( auto&  map : m_type_map1 )
      { m_types1.insert( map.first, L0DUBase::CaloType::Type( map.second ) ); }
      // no action if not yet initialized
      if ( Gaudi::StateMachine::INITIALIZED > FSMState()   )  return ; 
      //
      if ( !msgLevel ( MSG::DEBUG ) && !propsPrint () )  return ; 
      //
      MsgStream& log = msgLevel( MSG::DEBUG ) ? debug() : info () ;
      if ( !log.isActive() )       return ;                      // RETURN
      log << " ConditionIDs to be used: " ;
      for (auto&  imap : m_types1 )
      { log << std::endl <<  "'" << imap.first << "' :\t" << imap.second ; }
      //
      log << endmsg ;
    }
    // ========================================================================
    void update2 ( Property& /* p */ ) // update the channels map
    {
      m_types2.clear() ;
      for ( auto& map : m_type_map2 )
      { m_types2.insert
          ( map.first                                 ,
            L0DUBase::CaloType::Type ( map.second ) ) ; }
      // no action if not yet initialized
      if ( Gaudi::StateMachine::INITIALIZED > FSMState()   ) { return ; }
      //
      if ( !msgLevel ( MSG::DEBUG ) && !propsPrint () ) { return ; }
      MsgStream& log = msgLevel( MSG::DEBUG ) ? debug() : info () ;
      if ( !log.isActive() )       return ;                      // RETURN
      //
      log << " ChannelsIDs to be used: " ;
      for ( auto&  map : m_types2 )
      { log << std::endl <<  "'" << map.first << "' :\t" << map.second ; }
      //
      log << endmsg ;
    }
    // ========================================================================
  private:
    // ========================================================================
    void addExtras
    ( const LHCb::L0CaloCandidate& calo  ,
      LHCb::Track&                 track ) const ;
    // ========================================================================
    StatusCode makeTrack
    ( const LHCb::L0CaloCandidate& calo  ,
      LHCb::Track&                 track ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    L0Calo2Track () = delete;            // the default constructor is disabled
    /// the copy constructor is disabled
    L0Calo2Track ( const L0Calo2Track& ) = delete ;  // the copy constructor is disabled
    /// the assignement operator is disabled
    L0Calo2Track& operator=( const L0Calo2Track& ) = delete ; // assignement is disabled
    // ========================================================================
  private:
    // ========================================================================
    /// the selection
    Hlt::TSelection<LHCb::Track>* m_selection ;                // the selection
    /// the output selection
    std::string m_output         ;                                // the output
    /// TES Location of L0DUReport
    std::string m_L0DULocation   ;                // TES Location of L0DUReport
    /// TES Location of L0CaloCandidate
    std::string m_L0CaloLocation ;           // TES Location of L0CaloCandidate
    /// L0 channel
    std::string m_L0Channel      ;                           // L0 Muon channel
    /// Calo Maker Tool
    std::string m_makerName      ;                           //  Muon Seed Tool
    //// the tool
    ICaloSeedTool* m_maker       ;                                  // the tool
    // ========================================================================
  private:
    // ========================================================================
    /// map : name -> type
    LoKi::L0::CaloTypes m_types1 ;                        // map : name -> type
    /// map : name -> type
    LoKi::L0::CaloTypes m_types2 ;                        // map : name -> type
    // ========================================================================
  private:
    // ========================================================================
    typedef std::map<std::string,int>  _Map ;
    /// map : name -> type
    _Map   m_type_map1 ;                                  // map : name -> type
    /// map : name -> type
    _Map   m_type_map2 ;                                  // map : name -> type
    // ========================================================================
  private:
    // ========================================================================
    /// Info ID
    std::string  m_infoName ;                                         // InfoID
    /// InfoID
    int          m_infoID   ;                                         // InfoID
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace Hlt
// ============================================================================
// execute the algorithm
// ============================================================================
StatusCode Hlt::L0Calo2Track::execute  ()
{
  typedef LHCb::L0CaloCandidate::Container L0Calos ;

  // get all L0 Calos from TES
  const L0Calos* l0calos = get<L0Calos> ( m_L0CaloLocation ) ;

  // create the container of tracks and register it in TES
  LHCb::Track::Container* tracks = new LHCb::Track::Container() ;
  put ( tracks , std::string{"Hlt/Track/"} + m_selection -> id ().str() );

  using namespace LoKi::L0 ;

  // The cuts to be used:
  L0CaloCuts cuts ;

  // "All" ?
  if ( 0 != m_L0Channel.find ( "All") )
  {
    const LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>( m_L0DULocation );
    StatusCode sc = getL0Cuts ( l0 , m_L0Channel , m_types1 , cuts ) ;
    Assert ( sc.isSuccess () , "Unable to get proper L0CaloCuts" , sc ) ;
  } else {
    StatusCode sc = getL0Cuts ( m_L0Channel , m_types2 , cuts ) ;
    Assert ( sc.isSuccess () , "Unable to get proper L0CaloCuts"  , sc ) ;
  }

  //
  Assert ( !cuts.empty  () , "Invalid size of L0CaloCuts" ) ;

  // loop over L0 candidates:
  for ( const LHCb::L0CaloCandidate* calo : *l0calos ) 
  {
    if ( !calo )  continue ;                                   // CONTINUE
    //
    bool pass = true ;
    auto icut = cuts.begin();
    while ( pass && cuts.end() != icut ) { pass =  ( *icut++ ) (calo); }
    if ( !pass     )  continue ;                                   // CONTINUE
    //

    std::unique_ptr<LHCb::Track> track ( new LHCb::Track() );
    StatusCode sc = makeTrack ( *calo , *track ) ;
    if ( sc.isFailure() ) { return sc ; }                              // RETURN

    addExtras ( *calo , *track );

    // insert the track into output selection
    m_selection->push_back( track.get() ) ;

    // .. and into output container
    tracks->insert( track.release() );
  }

  // a bit of monitoring
  counter ( "#input"  ) +=  l0calos     -> size  ();
  counter ( "#output" ) +=  m_selection -> size  ();
  counter ( "#accept" ) += !m_selection -> empty ();
  m_selection -> setDecision ( !m_selection->empty() );

  setFilterPassed ( !m_selection -> empty() ) ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
void Hlt::L0Calo2Track::addExtras
( const LHCb::L0CaloCandidate& calo  ,
  LHCb::Track&                 track ) const
{

  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = ex;

  track.addInfo ( m_infoID , calo.et() );


  LHCb::State* state = track.stateAt( LHCb::State::MidHCal );
  if ( state )
  {
    state -> setTx ( ex ) ;                                    // what is it ??
    state -> setTy ( ey ) ;                                    // what is it ??
  }

  const LHCb::CaloCellID id = calo.id();

  LHCb::CaloCellID id1 ( id.calo() , id.area() , id.row()+1 , id.col()   ) ;
  LHCb::CaloCellID id2 ( id.calo() , id.area() , id.row()+1 , id.col()+1 ) ;
  LHCb::CaloCellID id3 ( id.calo() , id.area() , id.row()   , id.col()+1 ) ;

  track.addToLhcbIDs ( LHCb::LHCbID { id  } ) ;
  track.addToLhcbIDs ( LHCb::LHCbID { id1 } ) ;
  track.addToLhcbIDs ( LHCb::LHCbID { id2 } ) ;
  track.addToLhcbIDs ( LHCb::LHCbID { id3 } ) ;
}
// ============================================================================
StatusCode Hlt::L0Calo2Track::makeTrack
( const LHCb::L0CaloCandidate& calo  ,
  LHCb::Track&                 track ) const
{
  if ( m_maker )
  {   return m_maker->makeTrack ( calo , track ) ; }

  // Get energy and position of L0 calo candidate:
  double ex = calo.posTol()*(4./sqrt(12.0));
  double e  = fabs(calo.et())*calo.position().R()/calo.position().Rho();

   // ??? slopeX = ex, slopeY = ey ??? (and ex=ey)
   // This is only true for 'tracks' originating from L0Calo candidates
   // (and there is no way to recognize one of these -- you just 'have
   // to know'; Good luck! )
  track.addToStates( LHCb::State{
       Gaudi::TrackVector{calo.position().x(),calo.position().y(),ex,ex,1./e},
       calo.position().z(),
       LHCb::State::MidHCal     } );

  return StatusCode::SUCCESS ;
}
// ============================================================================
// the algorithm factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Hlt,L0Calo2Track)
// ============================================================================
// The END
// ============================================================================
