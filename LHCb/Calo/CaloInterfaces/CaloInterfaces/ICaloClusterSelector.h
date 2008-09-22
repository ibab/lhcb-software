// $Id: ICaloClusterSelector.h,v 1.8 2008-09-22 00:50:30 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2007/03/29 16:55:34  odescham
// change in ICaloMomentum + standardization of the package
//
// Revision 1.6  2006/03/22 18:21:50  odescham
// Migration to new Event Model
//
// Revision 1.5  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.4  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.3  2004/02/17 11:42:27  ibelyaev
//  fix in Likel(i,y)hoods
//
// Revision 1.2  2002/04/26 13:26:18  ibelyaev
//  bug fix for ICaloLikelihood.h
//
// Revision 1.1  2002/04/02 11:08:12  ibelyaev
//  new interfaces
// 
// ============================================================================
#ifndef CALOINTERFACES_ICALOCLUSTERSELECTOR_H 
#define CALOINTERFACES_ICALOCLUSTERSELECTOR_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration
namespace LHCb{
  class  CaloCluster     ;    
};
static const InterfaceID IID_ICaloClusterSelector ( "ICaloClusterSelector" , 1 , 0 );

/** @class ICaloClusterSelector ICaloClusterSelector.h
 *  
 *  An abstract interface for cluster "Selector"/"Preselector" 
 *  tools, which allow a fast and efficient selection/preselection 
 *  of clusters 
 *
 *  @see CaloHypotheses
 *  @see CaloCluster
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   31/03/2002
 */

class ICaloClusterSelector :
  virtual public IAlgTool                           ,
  public std::unary_function<const LHCb::CaloCluster*,bool> 
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloClusterSelector
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloClusterSelector; }
  
  
  /** "select"/"preselect" method 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     ( const LHCb::CaloCluster* cluster ) const = 0 ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::CaloCluster* cluster ) const = 0 ;
  
protected:
  
  
};
  
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERSELECTOR_H
// ============================================================================
