// $Id: CaloSelectNeutralCluster.h,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/07/03 10:57:57  ibelyaev
//  solve the problems with electrons and change 'weight' for relations
//
// Revision 1.2  2002/06/13 12:36:13  ibelyaev
//  new algorithms and new options
//
// Revision 1.1.1.1  2002/05/21 14:18:21  ibelyaev
// New package
// 
// ============================================================================
#ifndef CALOREC_CALOSELECTNeutralCLUSTER_H 
#define CALOREC_CALOSELECTNeutralCLUSTER_H 1
// Include files
#include "Relations/IAssociatorWeighted.h"
// from STL
#include <string>
// CaloKernel
#include "CaloKernel/CaloTool.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloClusterSelector.h"
// Event/CaloEvent 
#include "Event/CaloCluster.h"
// Event/TrEvent 
#include "Event/TrStoredTrack.h"

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
  public          CaloTool
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloSelectNeutralCluster>;
public:
  
  /// associator used to extract the relations from transient store 
  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> ITable;
  
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
  
  /** standard finalization   method 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code
   */
  virtual StatusCode finalize     () ;

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

protected:
  
  /** accessor to associator
   *  @return pointer to CONST associator! 
   */
  const ITable* associator() const { return m_associator ; }
  
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
  
  std::string m_associatorTypeName ;
  ITable*     m_associator         ;
  
  double      m_chi2cut            ;

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOREC_CALOSELECTNeutralCLUSTER_H
// ============================================================================
