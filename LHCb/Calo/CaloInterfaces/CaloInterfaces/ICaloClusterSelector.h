// $Id: ICaloClusterSelector.h,v 1.1 2002-04-02 11:08:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOINTERFACES_ICALOCLUSTERSELECTOR_H 
#define CALOINTERFACES_ICALOCLUSTERSELECTOR_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloClusterSelector.h"
// forward declaration
class     CaloCluster     ;    // from CaloEvent/Event
namespace CaloHypotheses { 
  class Hypothesis ; 
};    // From LHCbKernel

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
  public virtual   IAlgTool                           ,
  public std::unary_function<const CaloCluster*,bool> 
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloClusterSelector
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() 
  { return IID_ICaloClusterSelector ; }
  
  /** "select"/"preselect" method 
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     ( const CaloCluster* cluster ) const = 0 ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const CaloCluster* cluster ) const = 0 ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~ICaloClusterSelector(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCLUSTERSELECTOR_H
// ============================================================================
