// $Id: CaloClusterCorrect3x3Position.h,v 1.2 2005-03-07 15:37:15 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOEX_CaloClusterCorrect3x3Position_H 
#define CALOEX_CaloClusterCorrect3x3Position_H 1
// Include files
// from STL
#include <string>
#include <vector>
// From GaudiKernel
#include "GaudiKernel/NTuple.h"
// from   CaloKernel
#include "CaloKernel/CaloAlgorithm.h"
//CaloUtils
#include "CaloUtils/CellMatrix3x3.h"
#include "CaloUtils/CellNeighbour.h"
// forward declarations
class    CaloCluster   ;
class   ICaloHypoTool  ;

/** @class CaloClusterCorrect3x3Position CaloClusterCorrect3x3Position.h
 *  
 *  Merged pi0 reconstruction with Iterativ Method
 *
 *  @author Olivier Deschamps 
 *  @date   05/10/2002
 */

class CaloClusterCorrect3x3Position : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloClusterCorrect3x3Position>;
  
public:
  
  /** standard algorithm initialization 
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:

  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloClusterCorrect3x3Position( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /** destructor
   */
  virtual ~CaloClusterCorrect3x3Position();
  
protected:
  
  /** helper function to calculate number of digits 
   *  in the cluster with given status 
   *  @param cluster pointet to the cluster object
   *  @param status  digit statsu to be checked
   *  @return number of digits with given status.
   *       In the case of errors it returns -1
   */
  long numberOfDigits ( const CaloCluster*             cluster ,
                        const CaloDigitStatus::Status& status  ) const ;
  
 private:    
 CellMatrix3x3 m_cell3x3 ;
 CellNeighbour m_neighbour;
};

// ============================================================================
// The End 
// ============================================================================
#endif // CaloClusterCorrect3x3Position_H
// ============================================================================
