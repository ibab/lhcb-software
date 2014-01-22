// $Id$
// ============================================================================
#ifndef LOKITRIGGER_L0MUON2TRACKBASE_H 
#define LOKITRIGGER_L0MUON2TRACKBASE_H 1
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
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
  /** @class L0Muon2TrackBase
   *  Simple base class which converts L0Muon candidates into "tracks" using 
   *  the special tool by Johannes albrecht 
   *
   *  @see IMuonSeedTrack
   *
   *  The actual lines are stolen from 
   *     Gerhard Raven & Jose Angel Hernando  Morata
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-10-02
   *  
   *  $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class L0Muon2TrackBase : public Hlt::Base 
  {
  public:
    // ========================================================================
    /// intialize the base 
    virtual StatusCode initialize () ;    
    /// finalize the algorithm 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor  
     *  @param name algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    L0Muon2TrackBase
    ( std::string        name ,                  //     algorithm instance name 
      ISvcLocator*       pSvc ) ;                //  pointer to Service Locator
    /// virtual and protected destructor 
    virtual ~L0Muon2TrackBase () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    L0Muon2TrackBase () = delete ;             // the default constructor is disabled 
    /// the copy constructor is disabled 
    L0Muon2TrackBase ( const L0Muon2TrackBase& ) = delete ; // no copy constructor
    /// the assignement operator is disabled 
    L0Muon2TrackBase& operator=( const L0Muon2TrackBase& ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get or create the container of muons/tracks and register it in TES 
    LHCb::Track::Container*       getMuons () const 
    {
      return getOrCreate<LHCb::Track::Container,
        LHCb::Track::Container> ( evtSvc() , "Hlt/Track/L0(Di)-Muons" );
    }
    // ========================================================================
    /** seek for muon in already converted tracks 
     *  I do not understand the algorithm :-(
     */
    const LHCb::Track*             seekMuon
    ( const LHCb::L0MuonCandidate& l0muon    , 
      LHCb::Track::Container*      muons = 0 ) const  ;
    // ========================================================================
    /// make new muon 
    LHCb::Track*                   makeMuon
    ( const LHCb::L0MuonCandidate& l0muon    ,
      LHCb::Track::Container*      muons     ) const  
    {
      // create the track 
      std::unique_ptr<LHCb::Track> track( new LHCb::Track()  ) ;
      StatusCode sc = m_maker->makeTrack( l0muon , *track ) ;
      if ( sc.isFailure() ) 
      { 
        Error ( "Error from IMuonSeedTool" , sc ) ; 
        track.reset(nullptr) ;
      }
      //
      if ( track.get()  ) { muons -> insert ( track.get() ) ; }
      //
      return track.release() ;  
    }
    // ========================================================================
    /// get L0-muon cuts... I do not understand these lines :-( 
    std::pair<LoKi::L0::L0MuonCut,bool> muonCut() const ;
    // ========================================================================
    IMuonSeedTool* maker() const 
    {
      if ( !m_maker ) m_maker = tool<IMuonSeedTool>( m_makerName , this ) ; 
      return m_maker ;
    }
    // ========================================================================
  protected:
    // ========================================================================
    /// the input selection 
    Hlt::IRegister::Key              m_input     ;                 // the input
    /// the selection 
    Hlt::TSelection<Hlt::Candidate>* m_selection ;             // the selection 
    // ========================================================================
  private:
    // ========================================================================
    /// the output selection 
    std::string m_output         ;                                // the output
    /// TES Location of L0DUReport 
    std::string m_L0DULocation   ;                // TES Location of L0DUReport 
    /// L0 channel 
    std::string m_L0Channel      ;                           // L0 Muon channel 
    /// Muon Seed Tool 
    std::string m_makerName      ;                           //  Muon Seed Tool 
    //// the tool 
    mutable IMuonSeedTool* m_maker ;                                // the tool 
    // ========================================================================
  private:
    // ========================================================================
    /// the vector of elementary L0 data names 
    std::vector<std::string>   m_l0data_names ; // the vector of L0 data names 
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
// The END 
// ============================================================================
#endif // LOKITRIGGER_L0MUON2TRACKBASE_H
// ============================================================================
