// $Id: ICaloLikelihood.h,v 1.1 2004-02-17 11:42:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOINTERFACES_ICALOLIKELIHOOD_H 
#define CALOINTERFACES_ICALOLIKELIHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloLikelihood.h"

class     CaloCluster     ;     // from CaloEvent/Event

/** @class ICaloLikelihood ICaloLikelihood.h CaloInterfaces/ICaloLikelihood.h
 *  
 *  An abstract interface for "Likelihood"-tools.
 * 
 *  @attention do not forget to include 
 *  file Kernel/CaloHypotheses.h *BEFORE* using this interface  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/03/2002
 */
class ICaloLikelihood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const CaloCluster*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloLikelyhood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID()  ;
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const CaloHypotheses::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelihood ( const CaloCluster* cluster ) const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const CaloCluster* cluster ) const = 0 ;
  
protected:
  
  /// dectructor (virtual and protected)
  virtual ~ICaloLikelihood();
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOLIKELIHOOD_H
// ============================================================================
