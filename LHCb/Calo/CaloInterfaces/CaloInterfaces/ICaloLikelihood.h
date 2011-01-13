// $Id: ICaloLikelihood.h,v 1.7 2008-09-22 00:50:30 odescham Exp $
// ============================================================================
#ifndef CALOINTERFACES_ICALOLIKELIHOOD_H 
#define CALOINTERFACES_ICALOLIKELIHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "Event/CaloHypo.h"
namespace LHCb{
  class  CaloCluster     ;     
}
static const InterfaceID IID_ICaloLikelihood( "ICaloLikelihood" , 1 , 0 );

/** @class ICaloLikelihood ICaloLikelihood.h CaloInterfaces/ICaloLikelihood.h
 *  
 *  An abstract interface for "Likelihood"-tools.
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/03/2002
 */
class ICaloLikelihood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const LHCb::CaloCluster*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloLikelyhood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloLikelihood; } 
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const LHCb::CaloHypo::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelihood ( const LHCb::CaloCluster* cluster ) const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const LHCb::CaloCluster* cluster ) const = 0 ;
  
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOLIKELIHOOD_H
// ============================================================================
