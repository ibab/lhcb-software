// $Id: CaloClusterMonitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.2  2002/11/13 20:49:11  ibelyaev
//  small update of monitoring algorithms
//
// Revision 1.1.1.1  2002/06/16 13:51:56  ibelyaev
// new package
//
// Revision 1.1  2002/06/14 11:33:23  ibelyaev
//  new algorithms and options
//
// ============================================================================
#ifndef CALOEX_CALOClusterMonitor_H 
#define CALOEX_CALOClusterMonitor_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

// forward declarations 
class IHistogram1D;  // from AIDA 

/** @class CaloClusterMonitor CaloClusterMonitor.h
 *  
 *  The algorithm for trivial monitoring of "CaloCluster" containers.
 *  The algorithm produces 3 histograms: 
 *  
 *  <ol> 
 *  <li> @p CaloCluster multiplicity                    </li>
 *  <li> @p CaloCluster size (number of cells)          </li>
 *  <li> @p CaloCluster transverse energy distribution  </li>
 *  <li> @p CaloCluster x-distribution                  </li>
 *  <li> @p CaloCluster y-distribution                  </li>
 *  </ol>
 *
 *  Histograms reside in the directory @p /stat/"Name" , where 
 *  @ "Name" is the name of the algorithm
 * 
 *  @see CaloCluster 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloClusterMonitor : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterMonitor>;

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
  CaloClusterMonitor( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloClusterMonitor();
  
private:

  /// default  construstor  is  private 
  CaloClusterMonitor(); 
  /// copy     construstor  is  private 
  CaloClusterMonitor
  ( const CaloClusterMonitor& );
  /// assignement operator  is  private 
  CaloClusterMonitor& operator=
  ( const CaloClusterMonitor& );

private:
  
  // multiplicity of clusters 
  IHistogram1D* m_multiplicity ;
  
  // cluster size (number of cells)
  IHistogram1D* m_size         ;
  
  // distribution of cluster energy 
  IHistogram1D* m_energy       ;
  double        m_energyMax    ;
  
  // distribution of cluster transverse energy 
  IHistogram1D* m_et           ;
  double        m_etMax        ;
  
  // x-distribution of clusters 
  IHistogram1D* m_x            ;
  // x-distribution of clusters 
  IHistogram1D* m_x1           ;
  double        m_yband        ;
  
  // y-distribution of clusters 
  IHistogram1D* m_y            ;
  IHistogram1D* m_y1           ;
  double        m_xband        ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOClusterMonitor_H
// ============================================================================
