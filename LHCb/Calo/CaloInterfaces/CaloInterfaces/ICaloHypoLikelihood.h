// $Id: ICaloHypoLikelihood.h,v 1.5 2006-03-22 18:21:51 odescham Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/11/07 12:08:25  odescham
// v5r0 - Adapt to the new Track Event Model
//
// Revision 1.3  2005/01/25 14:12:18  cattanem
// updates for CLHEP 1.9
//
// Revision 1.2  2004/02/17 11:51:54  ibelyaev
//  move IID* from CaloInterfaces to src directory
//
// Revision 1.1  2004/02/17 11:42:28  ibelyaev
//  fix in Likel(i,y)hoods
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOHYPOLIKELIHOOD_H 
#define CALOINTERFACES_ICALOHYPOLIKELIHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// LHCbKernel
#include "Event/CaloHypotheses.h"
// forward declaration
namespace LHCb{
  class     CaloHypo     ;     
};


class ICaloHypoLikelihood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const LHCb::CaloHypo*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloHypoLikelihood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const  LHCb::CaloHypotheses::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelihood of the cluster for the given hypotheses
   *  @param  hypo pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelihood ( const LHCb::CaloHypo* hypo ) const = 0 ;
  
  /** evaluate the likelihood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const LHCb::CaloHypo* cluster ) const = 0 ;
  
protected:
  
  /// dectructor (virtual and protected)
  virtual ~ICaloHypoLikelihood();
  
};

// ============================================================================
#endif // CALOINTERFACES_ICALOHYPOLIKELIHOOD_H
// ============================================================================



