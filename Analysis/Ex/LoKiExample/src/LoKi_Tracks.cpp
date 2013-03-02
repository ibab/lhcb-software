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
    /// the frinet factory to instantiate algorithm
    friend class AlgFactory<LoKi::SelectTracks> ;
  public:
    /// the main method
    virtual StatusCode analyse () ;
  protected:
    SelectTracks ( const std::string& name , 
                   ISvcLocator*       pSvc )
      : LoKi::Algo ( name , pSvc ) 
      , m_trSelector ( 0 )
      , m_trSelectorName ( "LoKi::Hybrid::TrackSelector" ) 
    {
      declareProperty ( "TrackSelector" , m_trSelectorName ) ;
    }
    /// virtual and protected desctructor 
    virtual ~SelectTracks() {} ///< virtual and protected desctructor
  private:
    // the default constructor is disabled  
    SelectTracks (); ///< no default constructor 
    // the copy contructor is disabled 
    SelectTracks ( const SelectTracks& ) ; ///< no copy is allowed 
    // assignement operator is disabled 
    SelectTracks& operator=( const SelectTracks& ) ; ///< no assignement
  private:
    ITrackSelector* m_trSelector     ;
    std::string     m_trSelectorName ;
  } ;  
}
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LoKi,SelectTracks)
// ============================================================================
namespace 
{
  struct GoodTrack : public std::unary_function<const LHCb::Track*,bool>
  {
  public:
    /// construct
    GoodTrack ( ITrackSelector* tool ) : m_tool ( tool ) {}
    // the main method:
    inline bool operator() ( const LHCb::Track* track ) const 
    {
      return 0 == track ? false : m_tool -> accept ( *track ) ;  
    }
  private:
    GoodTrack() ;
  private:
    LoKi::Interface<ITrackSelector> m_tool ;  
  } ; 
}
// ============================================================================
// the main method
// ============================================================================
StatusCode LoKi::SelectTracks::analyse() 
{
  
  // locate tool (if not done already) 
  if ( 0 == m_trSelector ) 
  { m_trSelector = tool<ITrackSelector> ( m_trSelectorName , this ) ; }
  
  
  // locate the tracks
  const LHCb::Track::Container* tracks = 
    get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
  
  
  /// count "good tracks"
  size_t nTracks = std::count_if 
    ( tracks -> begin () , tracks -> end   () , GoodTrack( m_trSelector ) ) ;
  
  always () << " # of \"good\"tracks is " << nTracks 
            << " from "                   << tracks->size() << endreq ;

  counter( "#good") += nTracks ;
  
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The END
// ============================================================================
