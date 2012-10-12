// $Id: ISTClusterSelector.h,v 1.1 2009-02-05 11:57:10 mneedham Exp $
// ============================================================================
#ifndef STKERNEL_ISTCLUSTERSELECTOR_H
#define STKERNEL_ISTCLUSTERSELECTOR_H 1
// Include files
// STD & STL
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration
namespace LHCb{
  class  STCluster     ;
}
static const InterfaceID IID_ISTClusterSelector ( "ISTClusterSelector" , 1 , 0 );

/** @class ISTClusterSelector ISTClusterSelector.h
 *
 *  An abstract interface for cluster "Selector"/"Preselector"
 *  tools, which allow a fast and efficient selection/preselection
 *  of clusters
 *
 *
 *  @author M Needham
 *  @date   03/02/2009
 */

class ISTClusterSelector :
  virtual public IAlgTool                           ,
  public std::unary_function<const LHCb::STCluster*,bool>
{
public:

  /** static interface identification
   *  @see IInterface
   *  @see IID_ISTClusterSelector
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ISTClusterSelector; }


  /** "select"/"preselect" method
   *  @param  cluster pointer to calo cluster object to be selected
   *  @return true if cluster is selected
   */
  virtual bool select     ( const LHCb::STCluster* cluster ) const = 0 ;

  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STCluster* cluster ) const = 0 ;

};

// ============================================================================
#endif // ST_ISTCLUSTERSELECTOR_H
// ============================================================================
