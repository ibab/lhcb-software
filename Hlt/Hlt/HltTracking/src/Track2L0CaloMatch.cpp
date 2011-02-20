// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"                     // Event/TrackEvent 
#include "Event/L0CaloCandidate.h"           // Event/L0Event 
#include "Event/HltCandidate.h"              // Hlt/HltBase 
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/ITrack2CandidateMatch.h"
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  /** @class Track2L0CaloMatch 
   *  
   *  simeple placeholder for Track <--> L0CaloCandidate matching tool 
   * 
   *  @see LHCb::Track
   *  @see LHCb::L0CaloCandidate 
   *  @see Hlt::Candidate 
   *  @see Hlt::ITrack2CandidateMatch
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date 2011-02-11
   */
  class Track2L0CaloMatch 
    : virtual public extends1<GaudiTool,Hlt::ITrack2CandidateMatch>
  {
    // ========================================================================
    /// friend factory for instantiation 
    friend class ToolFactory<Hlt::Track2L0CaloMatch> ;
    // ========================================================================
  public:  // ITrack2CandidateMatch
    // ========================================================================
    /** match a track with candidate 
     *  @see Hlt::ITrack2CandidateMatch
     *  @see ITrackMatch
     *  @param track     (INPUT)  the input track 
     *  @param candidate (INPUT)  the input candidate 
     *  @param matched   (OUTPUT) the matched track 
     *  @param quality   (OUTPUT) the matching quality
     *  @param quality2  (OUTPUT) the matching quality-2
     *  @return status code 
     */ 
    virtual StatusCode match 
    ( const LHCb::Track&    track     , 
      const Hlt::Candidate& candidate ,
      LHCb::Track&          matched   , 
      double&               quality   ,
      double&               quality2  ) const ;
    // ========================================================================
    /** match a track with candidate ("filter-mode")
     *  @see Hlt::ITrack2CandidateMatch
     *  @param track    (INPUT)  the input track 
     *  @param candidat (INPUT)  the input candidate 
     *  @return true if track and candidate are "matched"
     */ 
    virtual bool match 
    ( const LHCb::Track*    track      , 
      const Hlt::Candidate* candidate  ,
      const double          quality    , 
      const double          quality2   ) const ;
    // ========================================================================    
  public:  // from IAlgTool
    // ========================================================================    
    /// the standard tool initialization 
    virtual StatusCode initialize () ;
    /// the standard tool finalization  
    virtual StatusCode finalize   () ;
    // ========================================================================    
  protected: // AlgTool technicalities 
    // ========================================================================    
    /** standard constructor 
     *  @param type   tool type (?)
     *  @param name   tool instance name 
     *  @param parent tool parent 
     */
    Track2L0CaloMatch 
    ( const std::string& type   ,   // tool type ?
      const std::string& name   ,   // tool instance name 
      const IInterface*  parent ) ; // the parent 
    /// protected and virtual destructor 
    virtual ~Track2L0CaloMatch () ;
    // ========================================================================    
  private:
    // ========================================================================    
    /// the default constructor is disabled 
    Track2L0CaloMatch () ;
    /// copy constructor is disabled 
    Track2L0CaloMatch           ( const Track2L0CaloMatch& ) ;
    /// assignement operator is disabled 
    Track2L0CaloMatch& operator=( const Track2L0CaloMatch& ) ;
    // ========================================================================    
  } ; 
  // ==========================================================================
} // end of namespace Hlt
// ============================================================================
/*  standard constructor 
 *  @param type   tool type (?)
 *  @param name   tool instance name 
 *  @param parent tool parent 
 */
// ============================================================================
Hlt::Track2L0CaloMatch::Track2L0CaloMatch 
( const std::string& type   ,   // tool type ?
  const std::string& name   ,   // tool instance name 
  const IInterface*  parent )   // the parent 
  : base_class ( type , name , parent ) 
{}  
// ============================================================================
// protected and virtual destructor 
// ============================================================================
Hlt::Track2L0CaloMatch::~Track2L0CaloMatch(){}
// ============================================================================
// the standard tool initialization 
// ============================================================================
StatusCode Hlt::Track2L0CaloMatch::initialize () 
{ return GaudiTool::initialize () ; }
// ============================================================================
// the standard tool finalization 
// ============================================================================
StatusCode Hlt::Track2L0CaloMatch::finalize   () 
{ return GaudiTool::finalize   () ; }
// ============================================================================
/*  match a track with candidate 
 *  @see Hlt::ITrack2CandidateMatch
 *  @see ITrackMatch
 *  @param track     (INPUT)  the input track 
 *  @param candidate (INPUT)  the input candidate 
 *  @param matched   (OUTPUT) the matched track 
 *  @param quality   (OUTPUT) the matching quality
 *  @param quality2  (OUTPUT) the matching quality-2
 *  @return status code 
 */ 
// ============================================================================
StatusCode Hlt::Track2L0CaloMatch::match 
( const LHCb::Track&    /*track  */   , 
  const Hlt::Candidate& candidate     ,
  LHCb::Track&          /*matched */  , 
  double&               /*quality */  ,
  double&               /*quality2*/  ) const 
{
  // ==========================================================================
  // get L0Calo from Hlt-candidate 
  const LHCb::L0CaloCandidate* l0 = candidate.get<LHCb::L0CaloCandidate>() ;
  if ( 0 == l0 ) 
  { return Error ("HltCandidate is NOT L0Calo!") ; }
  // ==========================================================================
    
  
  // 1. perform the actual matching here 

  Warning ("match(1): Not implemented yet...").ignore() ;
  
  // 2. fill the resulting "matched"-track 
  
  
  // 3. continue...
  
  
  return StatusCode::SUCCESS ;
  
}
// ============================================================================
/*  match a track with candidate ("filter-mode")
 *  @see Hlt::ITrack2CandidateMatch
 *  @param track    (INPUT)  the input track 
 *  @param candidat (INPUT)  the input candidate 
 *  @return true if track and candidate are "matched"
 */ 
// ============================================================================
bool Hlt::Track2L0CaloMatch::match 
( const LHCb::Track*    track      , 
  const Hlt::Candidate* candidate  ,
  const double         /* quality  */  , 
  const double         /* quality2 */  ) const 
{
  // 
  if ( 0 == track     ) 
  {
    Error( "LHCb::Track*    points to NULL, result false").ignore();
    return false ;                                                    // RETURN 
  }
  //
  if ( 0 == candidate ) 
  {
    Error( "Hlt::Candidate* points to NULL, result false").ignore();
    return false ;                                                    // RETURN 
  }
  //
  // get L0Calo from Hlt-candidate 
  const LHCb::L0CaloCandidate* l0 = candidate->get<LHCb::L0CaloCandidate>() ;
  if ( 0 == l0 ) 
  { 
    Error ("HltCandidate is NOT L0Calo!").ignore() ;
    return false ;                                                   // RETURN 
  }
  //
  
  bool matched = false ;
  
  // 1. check for matching here ..
  
  Warning ("match(2): Not implemented yet...").ignore() ;

  
  return matched ;                                                   // RETURN 
}
// ============================================================================



// ============================================================================
//                                                the factory for instantiation 
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(Hlt,Track2L0CaloMatch)
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================
