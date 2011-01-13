// $Id: ICaloClusterSelector.h,v 1.8 2008-09-22 00:50:30 odescham Exp $
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
}
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
