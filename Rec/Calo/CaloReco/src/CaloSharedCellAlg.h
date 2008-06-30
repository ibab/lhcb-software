// $Id: CaloSharedCellAlg.h,v 1.6 2008-06-30 15:36:33 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/05/30 09:42:05  odescham
// first release of the CaloReco migration
//
// Revision 1.4  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.3  2004/02/17 12:08:10  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.2  2003/06/23 13:11:54  ibelyaev
//  fix for gcc3.2
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.4  2002/04/05 17:12:20  ibelyaev
//  remove bugs, bugs, bugs ....
//
// Revision 1.3  2002/04/02 11:06:31  ibelyaev
// update for new event model
//
// Revision 1.2  2001/11/25 16:25:31  ibelyaev
//  update for newer CaloKernel package
//
// ============================================================================
#ifndef CALORECO_CALOSHAREDCELLALG_H 
#define CALORECO_CALOSHAREDCELLALG_H 1
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// CaloEvent/Event 
#include "Event/CaloCluster.h"
// from GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class  CaloSharedCellAlg CaloSharedCellAlg.h
 *   
 *  A very simple algorithm, which performs the 
 *  energy redistribution
 *  between shared cells 
 *
 *  @author Ivan Belyaev
 *  @date   30/06/2001
 */

class CaloSharedCellAlg : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloSharedCellAlg>;
public:
  
  /** standard initialization method 
   *  @see CaloAlgorithm
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  virtual StatusCode finalize () ;
  
  /** standard execution method 
   *  @see CaloAlgorithm
   *  @see     Algorithm 
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
protected:
  
  /**  Standard constructor
   *   @param name           name of the algorithm
   *   @param pSvcLocator    poinetr to Service Locator 
   */
  CaloSharedCellAlg
  ( const std::string& name        , 
    ISvcLocator*       pSvcLocator );
  
  /// destructor (virtual and protected)
  virtual ~CaloSharedCellAlg( );
  
private:

  /// default constructor is private!
  CaloSharedCellAlg();
  /** copy constructor is private! 
   *  @param copy object to be copied 
   */
  CaloSharedCellAlg( const CaloSharedCellAlg& copy );
  /** assignement is private! 
   *  @param copy object to be copied 
   */
  CaloSharedCellAlg& operator=( const CaloSharedCellAlg& copy );
  
private:

  bool        m_copy   ; ///< copy flag
  /// should one use the summed cluster energy or central cell energy?
  bool                m_useSumEnergy  ;
  /// number of iterations iif one use summed cluster energy
  int                 m_numIterations ;
  /// should one take into account the the distance?  
  bool                m_useDistance  ;
  /// shower size parameters (for different areas) 
  std::vector<double> m_showerSizes ;
  std::string m_outputData;
  std::string m_inputData;
  std::string m_detData;
  
  
};
// ============================================================================
#endif // CALOSHAREDCELLALG_H 

