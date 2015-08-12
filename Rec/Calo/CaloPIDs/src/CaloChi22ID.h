// ============================================================================
#ifndef CALOPIDS_CALOCHI22ID_H 
#define CALOPIDS_CALOCHI22ID_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/Relation1D.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloTrackAlg.h"
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
/** @class CaloChi22ID CaloChi22ID.h
 *  
 *
 *  @author Ivan BELYAEV
 *  @date   2006-06-18
 */
// ============================================================================
class CaloChi22ID : public CaloTrackAlg
{
  /// friend factory for instantiation 
  friend class AlgFactory<CaloChi22ID>;
public:
  /// algorithm execution 
  virtual StatusCode execute() ;


protected:
  /// Standard protected constructor
  CaloChi22ID 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) ;
  /// protected vertual destructor
  virtual ~CaloChi22ID(){}; 
private:
  CaloChi22ID () ;
  CaloChi22ID           ( const CaloChi22ID& ) ;
  CaloChi22ID& operator=( const CaloChi22ID& ) ;  
protected:

  
  

  /// perform the actual job 
  template <class TABLEI, class TABLEO>
  inline StatusCode doTheJob 
  ( const TABLEI* input  , 
    TABLEO*       output ) const ;
private:
  /// perform the actual job 
  template <class TABLEI, class TABLEO>
  inline StatusCode doTheJob 
  ( const LHCb::Track::Container* tracks , 
    const TABLEI*                 input  , 
    TABLEO*                       output ) const ;
protected:
  typedef std::vector<std::string> Inputs ;
  // input tracks 
  Inputs      m_tracks  ; ///< input tracks 
  // input relation table 
  std::string m_input   ; ///< input relation table 
  // output relation table 
  std::string m_output  ; ///< output relation table 
  // the "large" value 
  float       m_large   ; ///< large value 
};

// ============================================================================
/// perfromthe actual job for container of tracks 
// ============================================================================
template <class TABLEI, class TABLEO>
inline StatusCode CaloChi22ID::doTheJob
( const LHCb::Track::Container* tracks , 
  const TABLEI*                 input  ,
  TABLEO*                       output ) const 
{
  Assert ( 0 != tracks , "Invalid container of input tracks" ) ;
  
  for ( LHCb::Track::Container::const_iterator itrack = tracks->begin() ; 
        tracks->end() != itrack ; ++itrack ) 
  {
    const LHCb::Track* track = *itrack ;
    if ( !use ( track )  ) { continue ; }
    typename TABLEI::Range links = input->relations ( track ) ;
    // fill the relation table 
    const float chi2  = links.empty() ? m_large : links.front().weight() ;
    output -> i_push ( track , chi2 ) ;                      // NB!: i_push
  }
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// perfrom the actual job for all containers 
// ============================================================================
template <class TABLEI, class TABLEO>
inline StatusCode CaloChi22ID::doTheJob 
( const TABLEI* input , 
  TABLEO*       output ) const 
{
  Assert ( !m_tracks.empty() , "No input tracks are specified!" ) ;
  // loop over the container 
  for ( Inputs::const_iterator i = m_tracks.begin() ; 
        m_tracks.end() != i ; ++i ) 
  {
    if( !exist<LHCb::Track::Container>( *i )) continue;
    const LHCb::Track::Container* tracks = get<LHCb::Track::Container>(*i) ;
    StatusCode sc = doTheJob ( tracks , input , output ) ;
    if(sc.isFailure())return sc;
  }
  /// MANDATORY: i_sort after i_push 
  output->i_sort() ;                                     // NB: i_push 
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
#endif // CALOCHI22ID_H
// ============================================================================
