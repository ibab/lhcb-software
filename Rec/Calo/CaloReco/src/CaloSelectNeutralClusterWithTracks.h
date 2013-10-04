// $Id: CaloSelectNeutralClusterWithTracks.h,v 1.2 2009-06-22 13:06:32 cattanem Exp $
// ===================================================q=========================
#ifndef CALORECO_CALOSELECTNEUTRALCLUSTERWITHTRACKS_H 
#define CALORECO_CALOSELECTNEUTRALCLUSTERWITHTRACKS_H 1
// ============================================================================
// Include files
#include "Relations/IRelationWeighted.h"
// from STL
#include <string>
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterSelector.h"
#include "CaloInterfaces/ICaloRelationsGetter.h"
// GaudiAlg
#include "GaudiAlg/GaudiTool.h"
//Event
#include "Event/CaloCluster.h"
#include "Event/Track.h"
// ============================================================================

/** @class CaloSelectNeutralClusterWithTracks CaloSelectNeutralClusterWithTracks.h
 *  
 *  Concrete tool, which select the Neutral clusters 
 *  (photon candidates) looking through the relation table 
 *  of 2D-matched CaloClusters and Tracks
 *  
 *  Cluster is considered to be "selected" 
 *  if there are no reconstructed tracks with 
 *  chi2 value for 2D-matching under the threshold value 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   26/04/2002
 */
class CaloSelectNeutralClusterWithTracks : 
  public virtual ICaloClusterSelector ,
  public          GaudiTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectNeutralClusterWithTracks>;
  
public:
  
  /** @brief "select"  method 
   *
   *  Cluster is considered to be "selected" 
   *  if there are no reconstructed tracks with 
   *  chi2 value for 2D-matching under the threshold value 
   *
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::CaloCluster* cluster ) const ;
  
  /** @brief "select"  method (functor interface)
   *
   *  Cluster is considered to be "selected" 
   *  if there are no reconstructed tracks with 
   *  chi2 value for 2D-matching under the threshold value 
   *
   *  @see ICaloClusterSelector
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() 
    ( const LHCb::CaloCluster* cluster ) const ;
  
  /** standard initialization method 
   *  @see GaudiTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize () ;
  
  
protected:
  
  /** Standard constructor
   *  @see  GaudiTool
   *  @see   AlgTool
   *  @see  IAlgTool 
   *  @param type tool type (?)
   *  @param name name of the concrete instance 
   *  @param parent pointer to the parent 
   */
  CaloSelectNeutralClusterWithTracks
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloSelectNeutralClusterWithTracks(); 

private:
  
  ///  default  constructor   is  private 
  CaloSelectNeutralClusterWithTracks(); 
  ///  copy     constructor   is  private 
  CaloSelectNeutralClusterWithTracks
  (const CaloSelectNeutralClusterWithTracks& ); 
  ///  assignement operator   is  private 
  CaloSelectNeutralClusterWithTracks& operator=
  (const CaloSelectNeutralClusterWithTracks& );

private:
  
  std::string      m_tableLocation ;
  float           m_chi2cut       ;
  ICaloRelationsGetter*    m_tables;  
};
#endif // CALORECO_CALOSELECTNEUTRALCLUSTERWITHTRACKS_H
