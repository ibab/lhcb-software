// $Id: LoKi_Tracks.cpp,v 1.3 2008-07-09 17:00:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
#include "TrackInterfaces/ITrackSelector.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/TrackCuts.h"
// ============================================================================
/** @file
 *  Simple algorithm to illustrate the 
 *  selection of tracks 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-10-04
 */ 
// ============================================================================
namespace LoKi
{  
  class SelectTracks : public LoKi::Algo 
  {
    /// the friend factory to instantiate algorithm
    friend class AlgFactory<LoKi::SelectTracks> ;
  public:
    /// the main method
    virtual StatusCode analyse () ;
  protected:
    SelectTracks ( const std::string& name , 
                   ISvcLocator*       pSvc )
      : LoKi::Algo ( name , pSvc ) 
    {
      declareProperty ( "TrackSelector" , m_trSelectorName ) ;
    }
    /// virtual and protected desctructor 
    virtual ~SelectTracks() = default;
  private:
    // the default constructor is disabled  
    SelectTracks (); ///< no default constructor 
    // the copy contructor is disabled 
    SelectTracks ( const SelectTracks& ) = delete;
    // assignement operator is disabled 
    SelectTracks& operator=( const SelectTracks& ) = delete;
  private:
    ITrackSelector* m_trSelector = nullptr;
    std::string     m_trSelectorName = "LoKi::Hybrid::TrackSelector";
  } ;  
}
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,SelectTracks)
// ============================================================================
// ============================================================================
// the main method
// ============================================================================
StatusCode LoKi::SelectTracks::analyse() 
{
  
  // locate tool (if not done already) 
  if ( !m_trSelector )
  { m_trSelector = tool<ITrackSelector> ( m_trSelectorName , this ) ; }
  
  
  // locate the tracks
  auto tracks = get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
  
  
  /// count "good tracks"
  size_t nTracks = std::count_if( tracks->begin() , tracks->end() ,
                                  [&](const LHCb::Track* track) {
            return track && m_trSelector->accept(*track);
  });
  
  always () << " # of \"good\"tracks is " << nTracks 
            << " from "                   << tracks->size() << endmsg ;

  counter( "#good") += nTracks ;
  
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END
// ============================================================================
