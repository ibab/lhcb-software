// $Id: CaloHypoMonitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.3  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// ============================================================================
#ifndef CALOEX_CALOHypoMonitor_H 
#define CALOEX_CALOHypoMonitor_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

// forward declarations 
class IHistogram1D;  // from AIDA 

/** @class CaloHypoMonitor CaloHypoMonitor.h
 *  
 *  The algorithm for trivial monitoring of "CaloHypo" container
 *  The algorithm produces 10 histograms: 
 *  <ol> 
 *  <li> @p CaloHypo multiplicity                           </li>
 *  <li> @p CaloHypo energy distribution                    </li>
 *  <li> @p CaloHypo transverse momentum distribution       </li>
 *  <li> @p CaloHypo mass distribution                      </li>
 *  <li> @p CaloHypo e distribution                         </li>
 *  <li> @p CaloHypo x distribution                         </li>
 *  <li> @p CaloHypo y distribution                         </li>
 *  <li> multiplicity of     @p CaloCluster per @p CaloHypo </li>
 *  <li> multiplicity of Spd @p CaloDigit   per @p CaloHypo </li>
 *  <li> multiplicity of Prs @p CaloDigit   per @p CaloHypo </li>
 *  </ol>
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

class CaloHypoMonitor : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloHypoMonitor>;

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
  CaloHypoMonitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloHypoMonitor();
  
private:

  /// default  construstor  is  private 
  CaloHypoMonitor(); 
  /// copy     construstor  is  private 
  CaloHypoMonitor
  ( const CaloHypoMonitor& );
  /// assignement operator  is  private 
  CaloHypoMonitor& operator=
  ( const CaloHypoMonitor& );

private:

  // hypo multiplicity 
  IHistogram1D*   m_multiplicity ;
  double          m_multMax      ;
  
  // hypo energy  distribution 
  IHistogram1D*   m_energy       ;
  double          m_energyMax    ;
  
  // hypo transverse momentum distribution  
  IHistogram1D*   m_pt           ;
  double          m_ptMax        ;

  // hypo mass distribution  
  IHistogram1D*   m_mass         ;
  double          m_massMin      ;
  double          m_massMax      ;

  // hypo e-distribution  
  IHistogram1D*   m_e            ;

  // hypo x-distribution  
  IHistogram1D*   m_x            ;

  // hypo y-distribution  
  IHistogram1D*   m_y            ;

  // number of clusters 
  IHistogram1D*   m_clusters     ;
  
  // number of Spd digits 
  IHistogram1D*   m_spd          ;
  
  // number of Prs digits 
  IHistogram1D*   m_prs          ;

};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOHypoMonitor_H
// ============================================================================
