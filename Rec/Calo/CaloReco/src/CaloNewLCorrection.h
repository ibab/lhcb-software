// $Id: CaloNewLCorrection.h,v 1.2 2005-03-07 15:37:15 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTEST_CALOENEWLCORRECTION_H 
#define CALOTEST_CALOENEWLCORRECTION_H 1
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

/** @namespace CaloNewLCorrection_Local
 */
namespace CaloNewLCorrection_Local
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


/** @class CaloNewLCorrection CaloNewLCorrection.h
 *  
 *
 *  @author XXX-XXX xxx@xxx.com 
 *  @date   2003-03-10
 */
class CaloNewLCorrection : 
  public virtual ICaloHypoTool ,
  public              CaloTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloNewLCorrection>;
  
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
  CaloNewLCorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~CaloNewLCorrection () ;
  
private:
  
  /// default constructor is private 
  CaloNewLCorrection () ;
  /// copy constructor is private 
  CaloNewLCorrection           ( const CaloNewLCorrection& ) ;
  /// assignement operator is private 
  CaloNewLCorrection& operator=( const CaloNewLCorrection& ) ;
  
private:
  
  // internal types 
  typedef std::vector<CaloHypotheses::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;

  Parameters                                        Par_Al1 ;
  Parameters                                        Par_Al2 ;
  Parameters                                        Par_Al3 ;
  Parameters                                        Par_Be1 ;
  Parameters                                        Par_Be2 ;
  Parameters                                        Par_Be3 ;
  Parameters                                        Par_z0  ;

  /// evaluator of "cluster area"
  ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  CaloNewLCorrection_Local::DigitFromCalo              m_spd       ;
  /// evaluator of calorimeter for digit 
  CaloNewLCorrection_Local::DigitFromCalo              m_prs       ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOTEST_CALOENEWLCORRECTION_H
