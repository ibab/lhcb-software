// $Id: CaloPrsCorrection.h,v 1.2 2003-02-04 14:24:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:41  ibelyaev
// new package 
//
// Revision 1.1  2002/09/04 14:46:16  ibelyaev
//  add new tools and options for recalibration of Ecal
//
// Revision 1.2  2002/07/22 08:19:43  ibelyaev
//  fix trivial bug
//
// Revision 1.1  2002/07/21 18:56:18  ibelyaev
//  add new energy correction
//
// Revision 1.1  2002/06/21 11:02:47  ibelyaev
//  update in S-,L- and E-corrections
//
// ============================================================================
#ifndef CALOEX_CALOPrsCorrection_H 
#define CALOEX_CALOPrsCorrection_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"


// ============================================================================
/** @class DigitFromCalo 
 *  simple utility to count digits from certain calorimeter 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 31/03/2002 
 */
// ============================================================================
namespace Local
{
  class DigitFromCalo : public std::unary_function<const CaloDigit*,bool>
  { 
  public:
    /** constructor
     *  @parameter calo  calorimeter name 
     */
    explicit DigitFromCalo( const std::string& calo )
      : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    /** constructor
     *  @parameter calo  calorimeter index 
     */
    explicit DigitFromCalo( const int  calo )
      : m_calo(                                calo   ) {} ;
    /** the only essential method
     *  @param digit pointer to CaloDigit object 
     *  @return true if digit belongs to the predefined calorimeter 
     */
    inline bool operator() ( const CaloDigit* digit ) const 
    {
      if( 0 == digit ) { return false ; }
      return (int) digit->cellID().calo() == m_calo ;
    };
  private:
    /// default constructor is private 
    DigitFromCalo();
  private:
    int m_calo ;
  };
};
// ============================================================================


/** @class CaloPrsCorrection CaloPrsCorrection.h
 *
 *  The simplest energy correction for energy 
 *  losses in Preshower
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/05/2002
 */

class CaloPrsCorrection: 
  public virtual ICaloHypoTool , 
  public              CaloTool  
{
  /// friend factory for instantiation
  friend class ToolFactory<CaloPrsCorrection>;
  
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
   *  @param parent  pointer to parent object (algorithm, service xor tool)
   */
  CaloPrsCorrection
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor (virtual and protected)
  virtual ~CaloPrsCorrection() ;
  
  /** correction function itself 
   *  @param energy energy in Ecal 
   *  @param prs    energy in Prs 
   *  @param pars   parameters 
   *  @return corrected energy
   */
  inline double corr     
  ( const double  energy ,
    const double  prs    , 
    const Params& pars   ) const 
  {
    if( energy <= 0 ) { return 0 ; }
    // convert the energy in GeV 
    const double esc = energy / GeV ;
    
    double e   =        pars[0] * energy        ;
    e         *= (1.0 + pars[3] * esc )         ;
    e         *= (1.0 + pars[4] / sqrt( esc ) ) ;
    e         += pars[1] * prs + pars[2]        ;
    
    return e ;  
  };
  
  /** derivative of the correction function (approximate)
   *  @param energy energy in Ecal 
   *  @param prs    energy in Prs 
   *  @param pars   parameters 
   *  @return corrected energy
   */
  inline double corrPrime   
  ( const double     energy    , 
    const double     /* prs */ , 
    const Params&    pars      ) const 
  {
    if( energy <= 0 ) { return 1. ; }
    const double e   = energy / GeV ;
    const double sqe = sqrt( e ) ;
    double dif = pars[0]      ;
    dif += 2.0 * pars[0] * pars[3] *           e         ;
    dif += 0.5 * pars[0] *           pars[4]       / sqe ;
    dif += 1.5 * pars[0] * pars[3] * pars[4]       * sqe ;
    return dif ;
  };
  
private:

  ///  default  constructor   is   private
  CaloPrsCorrection();
  ///  copy     constructor   is   private
  CaloPrsCorrection           ( const CaloPrsCorrection& );
  ///  assignement operator   is   private
  CaloPrsCorrection& operator=( const CaloPrsCorrection& );
  
private:
  
  
  // correction coefficients per area
  Params                         m_const0      ;
  Params                         m_const0_cnv  ;
  Params                         m_const1      ;
  Params                         m_const1_cnv  ;
  Params                         m_const2      ;  
  Params                         m_const2_cnv  ;
  
  // allowed hypos
  typedef std::vector<int>       Hypotheses        ;
  Hypotheses                     m_hypos           ;
  
  // evaluator fo "cluster area"
  ClusterFunctors::ClusterArea   m_area            ;
  
  // evaluator of calorimeter for digit 
  Local::DigitFromCalo           m_spd             ;
  // evaluator of calorimeter for digit 
  Local::DigitFromCalo           m_prs             ;

  double                         m_threshold       ;
  double                         m_spdcut          ;

};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOPrsCorrection_H
// ============================================================================
