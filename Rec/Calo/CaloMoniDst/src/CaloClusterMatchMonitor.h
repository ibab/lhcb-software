// $Id: CaloClusterMatchMonitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/17 12:00:57  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.2  2002/07/03 11:02:51  ibelyaev
//  change 'weight' for relations
//
// Revision 1.1  2002/06/16 17:14:36  ibelyaev
//  new monitoring algorithms
//
// Revision 1.1.1.1  2002/06/16 13:51:56  ibelyaev
// new package
//
// Revision 1.1  2002/06/14 11:33:23  ibelyaev
//  new algorithms and options
//
// ============================================================================
#ifndef CALOEX_CAloClusterMatchMonitor_H 
#define CALOEX_CAloClusterMatchMonitor_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

// forward declarations 
class IHistogram1D;  // from AIDA 

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

class CaloClusterMatchMonitor : public CaloAlgorithm 
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
