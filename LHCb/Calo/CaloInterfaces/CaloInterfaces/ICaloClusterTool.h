// $Id: ICaloClusterTool.h,v 1.1.1.1 2001-11-01 13:17:37 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOCLUSTERTOOL_H 
#define CALOINTERFACES_ICALOCLUSTERTOOL_H 1
// Include files
#include "GaudiKernel/IAlgtool.h"
// local
#include "CaloInterfaces/IIDICaloClusterTool.h"

typename CaloHypotheses::Hypothesis ;  ///< from CaloGen 
class    CaloCluster                ;  ///< from CaloEvent 

/** @class ICaloClusterTool ICaloClusterTool.h CaloInterfaces/ICaloClusterTool.h
 *  
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloCluster objects, the potential candidates are: 
 *
 *    - S-shape correction tools 
 *    - longitudinal correction 
 *    - cluster parameters calculation for whole cluster 
 *    - cluster parameters calculations for maximum 4x4 submatrix 
 *    - cluster parameters calculations from 3x3 submatrix 
 *    - cluster parameters calculations from "swiss-cross" sub-cluster 
 *   
 *  @author Ivan Belyaev
 *  @date   30/10/2001
 */

class ICaloClusterTool: public IAlgTool
{
  
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID()    { return IID_ICaloClusterTool ; }
  
  /** Standard initialization of the tool
   *  @att It is not invoked  automatically! 
   *  @return status code 
   */ 
  StatusCode initialize ()  = 0 ;
  
  /** Standard finalization of the tool
   *  @att It is not invoked  automatically! 
   *  @return status code 
   */ 
  StatusCode finalize   ()  = 0 ;
  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  StatusCode process    ( CaloCluster* cluster ) = 0 ;
  
  /** The main processing method with hypothesis 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @param hypo    processing hypothesis 
   *  @return status code 
   */  
  StatusCode process    ( CaloCluster* cluster                   , 
                          const CaloHypotheses::Hypothesis& hypo ) = 0;
  
protected:
  
  /** destructor, virtual and protected  
   */
  virtual ~ICaloClusterTool(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
