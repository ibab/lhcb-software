// $Id: Calo04ECorrection.h,v 1.4 2005-03-07 15:37:15 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTEST_CALOE04ECORRECTION_H 
#define CALOTEST_CALOE04ECORRECTION_H 1
// Include files
// from STL
#include <string>
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from Event 
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"

/** @namespace Calo04ECorrection_Local
 */
namespace Calo04ECorrection_Local
{
  /** @class DigitFromCalo 
   *  simple utility to count digits from certain calorimeter 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 31/03/2002 
   */
  class DigitFromCalo : public std::unary_function<const CaloDigit*,bool>
  { 
  public:
    /** constructor
     *  @param calo  calorimeter name 
     */
    explicit DigitFromCalo( const std::string& calo )
      : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    /** constructor
     *  @param calo  calorimeter index 
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
      return ( ( (int) digit->cellID().calo() ) == m_calo ) ;
    };
  private:
    /// default constructor is private 
    DigitFromCalo();
  private:
    int m_calo ;
  };
};
// ============================================================================


/** @class Calo04ECorrection Calo04ECorrection.h
 *  
 *
 *  @author XXX-XXX xxx@xxx.com 
 *  @date   2003-03-10
 */
class Calo04ECorrection : 
  public virtual ICaloHypoTool ,
  public              CaloTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<Calo04ECorrection>;
  
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
  
public:

  /** initialization of the tool 
   *  @see  CaloTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** finalization of the tool 
   *  @see  CaloTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent  tool parent 
   */
  Calo04ECorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~Calo04ECorrection () ;
  
private:
  
  /// default constructor is private 
  Calo04ECorrection () ;
  /// copy constructor is private 
  Calo04ECorrection           ( const Calo04ECorrection& ) ;
  /// assignement operator is private 
  Calo04ECorrection& operator=( const Calo04ECorrection& ) ;
  
private:
  
  // internal types 
  typedef std::vector<CaloHypotheses::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  typedef std::vector<bool>                         BoolParameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;

  Parameters A0 ;
  Parameters A1_a_nospd ;
  Parameters A1_b_nospd ;
  Parameters A1_c_nospd ;
  Parameters A2_a_nospd ;
  Parameters A2_b_nospd ;
  Parameters A2_c_nospd ;
  Parameters A2_d_nospd ;
  Parameters A3x_Inn0_nospd ;
  Parameters A3x_Inn1_nospd ;
  Parameters A3x_Inn2_nospd ;
  Parameters A3y_Inn0_nospd ;
  Parameters A3y_Inn1_nospd ;
  Parameters A3y_Inn2_nospd ;
  Parameters A3x_Mid0_nospd ;
  Parameters A3x_Mid1_nospd ;
  Parameters A3y_Mid0_nospd ;
  Parameters A3y_Mid1_nospd ;
  Parameters A3x_Out0_nospd ;
  Parameters A3y_Out0_nospd ;
  Parameters A1_a_spd ;
  Parameters A1_b_spd ;
  Parameters A1_c_spd ;
  Parameters A2_a_spd ;
  Parameters A2_b_spd ;
  Parameters A2_c_spd ;
  Parameters A2_d_spd ;
  Parameters A3x_Inn0_spd ;
  Parameters A3x_Inn1_spd ;
  Parameters A3x_Inn2_spd ;
  Parameters A3y_Inn0_spd ;
  Parameters A3y_Inn1_spd ;
  Parameters A3y_Inn2_spd ;
  Parameters A3x_Mid0_spd ;
  Parameters A3x_Mid1_spd ;
  Parameters A3y_Mid0_spd ;
  Parameters A3y_Mid1_spd ;
  Parameters A3x_Out0_spd ;
  Parameters A3y_Out0_spd ;
  Parameters BetaPrs_a_nospd;
  Parameters BetaPrs_b_nospd;
  Parameters BetaPrs_a_spd;
  Parameters BetaPrs_b_spd;
  Parameters SplitFactor ;
  BoolParameters  Level ;

  /// evaluator of "cluster area"
  ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  Calo04ECorrection_Local::DigitFromCalo         m_spd       ;
  /// evaluator of calorimeter for digit 
  Calo04ECorrection_Local::DigitFromCalo         m_prs       ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTEST_CALOE04ECORRECTION_H
