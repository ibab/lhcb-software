// $Id: CaloCluster2TrgTrackAlg.h,v 1.1 2004-10-26 17:51:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALORECO_CALOCLUSTER2TRGTRACKALG_H 
#define CALORECO_CALOCLUSTER2TRGTRACKALG_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// from CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
// forward declarations 
// ============================================================================
class ICaloTrackMatch;   // CaloInterfaces
// ============================================================================

/** @class CaloCluster2TrgTrackAlg CaloCluster2TrgTrackAlg.h
 *  
 *  Algoriothm which performs the matching of 
 *  CaloCluster objects with trigger tracks.
 *  Algorithm produces the relation table between 
 *  clusters and tracks with weight = chi2 of matching.
 *  
 *  The actual matching is performed by matching tool
 *
 *  Algorithm could be used as a member of reconstruction chain 
 *  or in as "relation builder algorithm" for 'Associator' tool 
 *
 * 
 *  Indeed one does not need this algorithm at all.
 * 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @see ICaloTrackMatch 
 *  @see Associator
 *  @see AssociatorWeighed
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-10-25
 */
class CaloCluster2TrgTrackAlg : public CaloAlgorithm
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloCluster2TrgTrackAlg>;
protected:

  typedef const TrgTrack                                 Track    ;
  typedef const TrgTracks                                Tracks   ;
  typedef const CaloCluster                              Cluster  ;
  typedef const CaloClusters                             Clusters ;
  typedef RelationWeighted2D<CaloCluster,TrgTrack,float> Table    ;
  
public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  //virtual StatusCode finalize  ();   

protected:
  
  template <class TRACK>
  StatusCode  process ( Table*          table    , 
                        const Cluster*  cluster  ,
                        TRACK           first    ,
                        TRACK           last     )
  {
    for ( ; first != last ; ++first ) 
    {
      const Track* track = *first ;
      if ( 0 == track )               { continue ; }   // CONTINUE 
      
      double chi2 = 0 ;
      
      StatusCode sc = m_match -> 
        match ( &(cluster->position()) ,  track , chi2 ) ;
      
      if ( sc.isFailure() ) 
      {
        Warning ( "Cluster/Track: matching failure, skip the pair" , sc );
        continue ;
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table->relate ( cluster , track , chi2 ); }      
      
    }; // end of loop over tracks 
    
    return StatusCode::SUCCESS ;
  };

protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloCluster2TrgTrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloCluster2TrgTrackAlg();
  
private:
  
  // default  constructor  is  private 
  CaloCluster2TrgTrackAlg(); 
  // copy     construstor  is  private 
  CaloCluster2TrgTrackAlg
  ( const CaloCluster2TrgTrackAlg& );
  // assignement operator  is  private 
  CaloCluster2TrgTrackAlg& operator=
  ( const CaloCluster2TrgTrackAlg& );

private:
  
  // Tgr Track containers 
  Inputs             m_tracks    ;
  
  // cut value 
  double             m_cut       ;
  
  // matching tool 
  std::string        m_matchType ;
  std::string        m_matchName ;
  ICaloTrackMatch*   m_match     ;

};

// ============================================================================
// The End 
// ============================================================================
#endif // CALORECO_CALOCLUSTER2TRGTRACKALG_H
// ============================================================================
