// $Id: ICaloClusterTool.h,v 1.4 2002-04-02 11:08:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/11/29 16:02:09  ibelyaev
//  remove artificial dependency on CaloKernel package
//
// Revision 1.2  2001/11/25 16:52:08  ibelyaev
//  update for newer CaloKernel package
//
// Revision 1.1.1.2  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
// ============================================================================ 
#ifndef CALOINTERFACES_ICALOCLUSTERTOOL_H 
#define CALOINTERFACES_ICALOCLUSTERTOOL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// CaloInterfaces
#include "CaloInterfaces/IIDICaloClusterTool.h"
// forward declarations
class     CaloCluster                ; ///< from CaloEvent 

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
  public std::unary_function<CaloCluster*,StatusCode>
{
  
 public:
  
  /** static interface identification
   *  @see IInterface 
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID()    { return IID_ICaloClusterTool ; }
  
  /** The main processing method 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloCluster* cluster ) const = 0 ;
  
  /** The main processing method (functor interface) 
   *  @param cluster pointer to CaloCluster object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloCluster* cluster ) const = 0 ;
  
 protected:
  
  /// destructor, protected and virtual
  virtual ~ICaloClusterTool(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERTOOL_H
// ============================================================================
