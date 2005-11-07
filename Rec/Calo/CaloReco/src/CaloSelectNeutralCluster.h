// $Id: CaloSelectNeutralCluster.h,v 1.3 2005-11-07 12:12:43 odescham Exp $
// ===================================================q=========================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/12/10 17:12:29  ibelyaev
//  steps towards 'Fast' Calo recontruction
//
// ============================================================================
#ifndef CALOREC_CALOSELECTNeutralCLUSTER_H 
#define CALOREC_CALOSELECTNeutralCLUSTER_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloClusterSelector.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloTool.h"
// ============================================================================
// Event/CaloEvent 
// ============================================================================
#include "Event/CaloCluster.h"
// ============================================================================
// Event/TrEvent 
// ============================================================================
#include "Event/Track.h"
// ============================================================================

/** @class CaloSelectNeutralCluster CaloSelectNeutralCluster.h
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
class CaloSelectNeutralCluster : 
  public virtual ICaloClusterSelector ,
  public virtual IIncidentListener    ,
  public          CaloTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectNeutralCluster>;
public:
  
  /// associator used to extract the relations from transient store 
  typedef IRelationWeighted<CaloCluster,Track,float> ITable;
  
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
  ( const CaloCluster* cluster ) const ;
  
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
    ( const CaloCluster* cluster ) const ;
  
  /** standard initialization method 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize () ;
  
public:
  
  /** handle the incident
   *  @see IIncindentListener 
   *  @param inc incident 
   */
  virtual void handle ( const Incident& inc ) ;
  
protected:
  
  /** Standard constructor
   *  @see  CaloTool
   *  @see   AlgTool
   *  @see  IAlgTool 
   *  @param type tool type (?)
   *  @param name name of the concrete instance 
   *  @param parent pointer to the parent 
   */
  CaloSelectNeutralCluster
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloSelectNeutralCluster(); 

private:
  
  ///  default  constructor   is  private 
  CaloSelectNeutralCluster(); 
  ///  copy     constructor   is  private 
  CaloSelectNeutralCluster
  (const CaloSelectNeutralCluster& ); 
  ///  assignement operator   is  private 
  CaloSelectNeutralCluster& operator=
  (const CaloSelectNeutralCluster& );

private:
  
  std::string      m_tableLocation ;
  mutable ITable*  m_table         ;
  
  double           m_chi2cut       ;

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOREC_CALOSELECTNeutralCLUSTER_H
// ============================================================================
