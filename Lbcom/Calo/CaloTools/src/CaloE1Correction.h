// $Id: CaloE1Correction.h,v 1.1 2002-07-21 18:56:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/06/21 11:02:47  ibelyaev
//  update in S-,L- and E-corrections
//
// ============================================================================
#ifndef CALOEX_CALOE1Correction_H 
#define CALOEX_CALOE1Correction_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"

/** @class CaloE1Correction CaloE1Correction.h
 *
 *  The simplest non-linear energy correction 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloE1Correction: 
  public virtual ICaloHypoTool , 
  public              CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloE1Correction>;
  
  typedef std::vector<double>  Params            ;
  
public:
  
  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
  /** standard initialization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;

  /** standard finalization method
   *  @see CaloTool 
   *  @see  AlgTool
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service or tool)
   */
  CaloE1Correction
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloE1Correction() ;
  
  /** correction function itself 
   *  @param energy energy 
   *  @param pars   parameters 
   *  @return corrected energy
   */
  inline double corr        ( const double  energy , 
                              const Params& pars   ) const 
  {
    if( energy <= 0 ) { return 0 ; } 
    const double sei = 1.0 / sqrt( energy );
    return energy * ( pars[0] + pars[1]*sei + pars[2]*sei*sei );  
  };
  
  /** derivative of correction function
   *  @param energy energy 
   *  @param pars   parameters 
   *  @return corrected energy
   */
  inline double corrPrime   ( const double  energy , 
                              const Params& pars   ) const 
  {
    if( energy <= 0 ) { return 1 ; } 
    const double sei = 1.0 / sqrt( energy );
    return pars[0] + 0.5 * pars[1]*sei ;  
  };

private:

  ///  default  constructor   is   private
  CaloE1Correction();
  ///  copy     constructor   is   private
  CaloE1Correction( const CaloE1Correction& );
  ///  assignement operator   is   private
  CaloE1Correction& operator=( const CaloE1Correction& );
  
private:
  
  
  // correction coefficients per area
  Params                       m_const0          ;
  Params                       m_const1          ;
  Params                       m_const2          ;
  
  // allowed hypos
  typedef std::vector<int>     Hypotheses        ;
  Hypotheses                   m_hypos           ;
  
  // evaluator fo "cluster area"
  ClusterFunctors::ClusterArea m_area            ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOE1Correction_H
// ============================================================================
