// $Id: ICaloLikelyhood.h,v 1.2 2002-04-26 13:26:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/02 11:08:12  ibelyaev
//  new interfaces
// 
// ============================================================================
#ifndef CALOINTERFACES_ICALOLIKELYHOOD_H 
#define CALOINTERFACES_ICALOLIKELYHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloLikelyhood.h"
// forward declaration
// namespace CaloHypotheses        // From LHCbKernel
// { 
//   class Hypothesis; 
// };
class     CaloCluster     ;     // from CaloEvent/Event

/** @class ICaloLikelyhood ICaloLikelyhood.h CaloInterfaces/ICaloLikelyhood.h
 *  
 *  An abstract interface for "Likelyhood"-tools.
 * 
 *  @attention do not forget to include 
 *  file Kernel/CaloHypotheses.h *BEFORE* using this interface  
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   30/03/2002
 */
class ICaloLikelyhood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const CaloCluster*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloLikelyhood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ICaloLikelyhood ; }
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const CaloHypotheses::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelyhood ( const CaloCluster* cluster ) const = 0 ;
  
  /** evaluate the likelyhood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const CaloCluster* cluster ) const = 0 ;
  
protected:
  
  /// dectructor (virtual and protected)
  virtual ~ICaloLikelyhood(){};

};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOLIKELYHOOD_H
// ============================================================================
