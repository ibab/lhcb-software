// ============================================================================
#ifndef CALOTRACKMATCHALG_H 
#define CALOTRACKMATCHALG_H 1
// ============================================================================
// Include files
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IsConvertible.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloTrackAlg.h"
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// Local 
// ============================================================================
#include "ToVector.h"
// ============================================================================
/** @class CaloTrackMatchAlg CaloTrackMatchAlg.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-16
 */
class CaloTrackMatchAlg : public CaloTrackAlg
{
public: 
  /// Algorithm initialization
  virtual StatusCode initialize();  
protected:
  // standard constructor 
  CaloTrackMatchAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) ;
  /// virtual protected destructor 
  virtual ~CaloTrackMatchAlg( ){};  
private:
  CaloTrackMatchAlg () ;
  CaloTrackMatchAlg           ( const  CaloTrackMatchAlg& ) ;
  CaloTrackMatchAlg& operator=( const  CaloTrackMatchAlg& ) ;
protected:
  typedef std::vector<std::string>                      Inputs ;
protected:
  /// get the tool for matching 
  inline ICaloTrackMatch* match() const ;
protected:
  // perform the actual jobs! 
  template <class TYPE, class TABLE> 
  inline StatusCode doTheJob ( TABLE* table ) const ;  
protected:
  // input data (tracks)
  Inputs m_tracks    ; ///< input data  (tracks) 
  // input data (calorimeter objects)
  Inputs m_calos     ; ///< input data  (calorimeter objects) 
  // output data (relation table)
  std::string m_output    ; ///< output data (relation table)
  // matching tool 
  std::string m_toolName  ; ///< matching tool
  // filter 
  std::string m_filter    ;
  // threshold 
  double m_threshold ;
  // the matcher tool iteself 
  mutable ICaloTrackMatch* m_match ; ///< the tool itself 
};

// ============================================================================
/// get the tool for matching 
// ============================================================================
inline ICaloTrackMatch* 
CaloTrackMatchAlg::match() const 
{
  if ( 0 == m_match ) 
  { m_match = tool<ICaloTrackMatch>( m_toolName , this ) ; }
  return m_match ;
}
// ============================================================================
namespace 
{
  inline const LHCb::CaloPosition* 
  position ( const LHCb::CaloCluster* c ) { return &c->position() ; }

  inline const LHCb::CaloPosition* 
  position ( const LHCb::CaloHypo*    c ) { return  c->position() ; }

  template <class TABLE> 
  inline void i_push 
  ( TABLE*                   table   , 
    const LHCb::Track*       track   , 
    const LHCb::CaloCluster* cluster , 
    const double             weight  )
  {
    table->i_push( cluster , track , (float) weight ) ; // NB: i_push here 
  }
  template <class TABLE> 
  inline void i_push 
  ( TABLE*                   table   , 
    const LHCb::Track*       track   , 
    const LHCb::CaloHypo*    hypo    , 
    const double             weight  )
  {
    table->i_push( track , hypo , (float) weight ) ;  // NB: i_push here
  }
}

// ============================================================================
template <class TYPE, class TABLE> 
inline StatusCode CaloTrackMatchAlg::doTheJob ( TABLE* table ) const 
{
  typedef LHCb::Calo2Track::ITrAccTable   Filter ;
  
  typedef          TYPE                   Calo   ;
  typedef typename TYPE::Container        Calos  ;
  typedef LHCb::Track                     Track  ;
  typedef LHCb::Track::Container          Tracks ;
  typedef LHCb::Track::ConstVector        TRACKS ;
  
  // get all tracks locally
  TRACKS tracks ;
  tracks.reserve ( 100 ) ;
  
  // use the filter? 
  Filter* filter = 0 ;
  if ( !m_filter.empty() ) { filter = get<Filter> ( m_filter ) ; }
  
  // loop over the tracks containers 
  for ( Inputs::const_iterator i = m_tracks.begin() ; m_tracks.end() != i ; ++i ){

    if( !exist<Tracks>(*i) ){
      if(msgLevel(MSG::DEBUG)) debug() << " Container " << *i << " has not been found " << endmsg;
      continue;
    }    
    const Tracks* c = get<Tracks> ( *i) ;
    if(msgLevel(MSG::DEBUG)) debug() << " Found " << c->size() << "tracks in " << *i << endmsg;
    if ( 0 == c ) { continue ; }
    // loop over all tracks in container 
    for ( Tracks::const_iterator it = c->begin() ; c->end() != it ; ++it ){
      const Track* track = *it ;
      // skip track?
      if ( !use ( track ) )                 { continue ; }      // CONTINUE 
      // use filter?
      if ( 0 != filter    ){
        Filter::Range r = filter->relations( track ) ;
        if ( r.empty() || !r.front().to() ) { continue ; }      // CONTINUE  
      } 
      // collect good tracks 
      tracks.push_back ( track ) ;
    } // end of loop over tracks 
  } // end of loop over track containers
  
  const size_t nTracks = tracks.size() ;
  if ( 0 == nTracks ) { if(msgLevel(MSG::DEBUG)) debug() << "No good tracks have been selected" << endmsg; }
  
  // get the matching tool 
  ICaloTrackMatch* matcher = match() ;
  
  size_t nCalos    = 0 ;
  size_t nOverflow = 0 ;
  // loop over cluster containers 
  for ( Inputs::const_iterator ic = m_calos.begin() ; m_calos.end() != ic ; ++ic){
    if( !exist<Calos>(*ic) ){
      if(msgLevel(MSG::DEBUG)) debug() << " Container " << *ic << " has not been found " << endmsg;
      continue;
    }
    const Calos* c = get<Calos> ( *ic ) ;
    // loop over all calos
    for ( typename Calos::const_iterator icalo = c->begin() ; c->end() != icalo ; ++icalo ){
      const Calo* calo = *icalo;
      const LHCb::CaloPosition* caloPos = position(calo) ;
      if ( 0 == calo || 0 == caloPos ) { continue ; }
      // overall number of calos
      ++nCalos ; 
      // loop over all good tracks 
      for ( TRACKS::const_iterator itrack = tracks.begin() ; tracks.end() != itrack ; ++itrack ){
        const LHCb::Track* track = *itrack ;
        // use the tool 
        double chi2 = 0 ;
        StatusCode sc = matcher->match ( caloPos , track , chi2 ) ;
        if ( sc.isFailure() ){ 
          if(msgLevel(MSG::DEBUG)) debug() << "Failure from Tool::match, skip" << endmsg ;
          counter("# failure from Tool::match") += 1;
          continue ;                                            // CONTINUE 
        }
        if ( m_threshold < chi2  ) { ++nOverflow ; continue  ; } // CONTINUE 
        // FINALLY: fill the relation table! 
        i_push ( table , track , calo , chi2 ) ;                 // NB: i_push 
      }
      //
    } //end of loop over clusters
  } //end of loop over cluster containters ;
  // MANDATORY: i_sort after i_push 
  table->i_sort() ;                                             // NB: i_sort


  // a bit of statistics 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ){
    counter ( "#tracks"   ) += nTracks   ;
    counter ( "#calos"    ) += nCalos    ;
    counter ( "#overflow" ) += nOverflow ;
    typename TABLE::Range links = table->i_relations() ;
    counter ( "#links"    ) += links.size() ;
    StatEntity& weight = counter("#chi2") ;
    for ( typename TABLE::iterator link = links.begin() ; 
          links.end() != link ; ++link ) { weight += link->weight() ; }
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
#endif // CALOTRACKMATCHALG_H
// ============================================================================
