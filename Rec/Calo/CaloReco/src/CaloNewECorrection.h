// $Id: CaloNewECorrection.h,v 1.1 2003-04-11 09:33:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTEST_CALOENEWECORRECTION_H 
#define CALOTEST_CALOENEWECORRECTION_H 1
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

/** @namespace CaloNewECorrection_Local
 */
namespace CaloNewECorrection_Local
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


/** @class CaloNewECorrection CaloNewECorrection.h
 *  
 *
 *  @author XXX-XXX xxx@xxx.com 
 *  @date   2003-03-10
 */
class CaloNewECorrection : 
  public virtual ICaloHypoTool ,
  public              CaloTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloNewECorrection>;
  
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
  CaloNewECorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~CaloNewECorrection () ;
  
private:
  
  /// default constructor is private 
  CaloNewECorrection () ;
  /// copy constructor is private 
  CaloNewECorrection           ( const CaloNewECorrection& ) ;
  /// assignement operator is private 
  CaloNewECorrection& operator=( const CaloNewECorrection& ) ;
  
private:
  
  // internal types 
  typedef std::vector<CaloHypotheses::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  typedef std::vector<bool>                         BoolParameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;

  Parameters                                        A1_a ;
  Parameters                                        A1_b ;
  Parameters                                        A2_a ;
  Parameters                                        A2_b ;
  Parameters                                        SlopeX ;
  Parameters                                        SlopeY ;
  Parameters                                        Beta ;
  Parameters                                        GlobalFactor ;
  BoolParameters                                    Level ;

  /// evaluator of "cluster area"
  ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  CaloNewECorrection_Local::DigitFromCalo         m_spd       ;
  /// evaluator of calorimeter for digit 
  CaloNewECorrection_Local::DigitFromCalo         m_prs       ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTEST_CALOENEWECORRECTION_H
