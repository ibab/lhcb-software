// $Id: CaloClusterMatchMonitor.h,v 1.2 2004-10-27 11:28:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOEX_CAloClusterMatchMonitor_H 
#define CALOEX_CAloClusterMatchMonitor_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// from CaloKernel
// ============================================================================
#include "CaloKernel/CaloHistoAlg.h"
// ============================================================================

/** @class CaloClusterMatchMonitor CaloClusterMatchMonitor.h
 *  
 *  The algorithm for trivial monitoring of matching of 
 *  "CaloClusters" with TrStored Tracks.
 *  It produces 5 histograms:
 *
 *  <ol> 
 *  <li> Total Link              distribution               </li>
 *  <li> Link multiplicity       distribution               </li>
 *  <li> Minimal Weight          distribution               </li>
 *  <li> Maximal Weight          distribution               </li>
 *  <li>         Weight          distribution               </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where 
 *  @ "Name" is the name of the algorithm
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterMatchMonitor : public CaloHistoAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMatchMonitor>;
  
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
  
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloClusterMatchMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloClusterMatchMonitor();
  
private:

  /// default  construstor  is  private 
  CaloClusterMatchMonitor(); 
  /// copy     construstor  is  private 
  CaloClusterMatchMonitor
  ( const CaloClusterMatchMonitor& );
  /// assignement operator  is  private 
  CaloClusterMatchMonitor& operator=
  ( const CaloClusterMatchMonitor& );

private:
  
  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> Asct;
  std::string     m_associatorType ;
  std::string     m_associatorName ;  
  Asct*           m_associator     ;
  
  // logarithm of total (number of links/relations + 1 ) 
  IHistogram1D*   m_links   ;
  
  // multiplicity of relations/per Hypo
  IHistogram1D*   m_rels    ;
  
  // minimal weight 
  IHistogram1D*   m_min     ;
  
  // maximal weight 
  IHistogram1D*   m_max     ;
  
  // weigth distribution 
  IHistogram1D*   m_weights ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CAloClusterMatchMonitor_H
// ============================================================================
