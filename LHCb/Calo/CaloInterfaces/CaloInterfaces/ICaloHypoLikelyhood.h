// ============================================================================
#ifndef CALOINTERFACES_ICALOHYPOLIKELYHOOD_H 
#define CALOINTERFACES_ICALOHYPOLIKELYHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local 
#include "IIDICaloHypoLikelyhood.h"
// forward declaration
class     CaloHypo     ;     // from CaloEvent/Event

class ICaloHypoLikelyhood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const CaloHypo*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloHypoLikelyhood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloHypoLikelyhood ; }
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const CaloHypotheses::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelyhood ( const CaloHypo* hypo ) const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const CaloHypo* cluster ) const = 0 ;
  
protected:
  
  /// dectructor (virtual and protected)
  virtual ~ICaloHypoLikelyhood(){};

};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOHYPOLIKELYHOOD_H
// ============================================================================



