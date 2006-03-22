// $Id: ICaloClusterTool.h,v 1.8 2006-03-22 18:21:51 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.6  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.5  2004/02/17 11:42:27  ibelyaev
//  fix in Likel(i,y)hoods
//
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOCLUSTERTOOL_H 
#define CALOINTERFACES_ICALOCLUSTERTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declarations
namespace LHCb{
  class     CaloCluster                ; 
};


/** @class ICaloClusterTool ICaloClusterTool.h 
 *           CaloInterfaces/ICaloClusterTool.h
 *  
 *  The generic interface for "Calorimeter tools" , which deals with 
 *  CaloCluster objects, the potential candidates are: 
 *
 *    - cluster parameters calculation  for whole cluster 
 *    - cluster parameters calculations for maximum 4x4 submatrix 
 *    - cluster parameters calculations from 3x3 submatrix 
 *    - cluster parameters calculations from "swiss-cross" sub-cluster 
 *   
 *  @author Ivan Belyaev
 *  @date   30/10/2001
 */

class ICaloClusterTool: 
  public virtual IAlgTool                            ,
  public std::unary_function<LHCb::CaloCluster*,StatusCode>
{
  
 public:
  
  /** static interface identification
   *  @see IInterface 
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloCluster* cluster ) const = 0 ;
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloCluster* cluster ) const = 0 ;
  
 protected:
  
  /// destructor, protected and virtual
  virtual ~ICaloClusterTool();
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
