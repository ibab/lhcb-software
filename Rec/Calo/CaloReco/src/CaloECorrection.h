// $Id: CaloECorrection.h,v 1.4 2010-05-20 09:47:06 odescham Exp $
// ============================================================================
#ifndef CALORECO_CALOECORRECTION_H 
#define CALORECO_CALOECORRECTION_H 1
// Include files
#include <string>
#include "GaudiKernel/Point3DTypes.h"
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloCorrectionBase.h"

/** @namespace CaloECorrection_Local
 */






namespace CaloECorrection_Local
{
  /** @class DigitFromCalo 
   *  simple utility to count digits from certain calorimeter 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 31/03/2002 
   */
  class DigitFromCalo : public std::unary_function<const LHCb::CaloDigit*,bool>
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
    inline bool operator() ( const LHCb::CaloDigit* digit ) const 
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


/** @class CaloECorrection CaloECorrection.h
 *  
 *
 *  @author Deschamps Olivier

 *  @date   2003-03-10
 */
class CaloECorrection : 
  public virtual ICaloHypoTool ,
  public              CaloCorrectionBase{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloECorrection>;
  
public:
  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  virtual StatusCode initialize () ;
  virtual StatusCode finalize   () ;
  
protected:
  CaloECorrection ( const std::string& type   , 
                    const std::string& name   ,
                    const IInterface*  parent ) ;
  
  virtual ~CaloECorrection () ;
  
private:
  
  CaloECorrection () ;
  CaloECorrection           ( const CaloECorrection& ) ;
  CaloECorrection& operator=( const CaloECorrection& ) ;
  
private:

    

  
  // internal types 
  typedef std::vector<LHCb::CaloHypo::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  typedef std::vector<bool>                         BoolParameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;


  /// evaluator of "cluster area"
  LHCb::ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  LHCb::ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  CaloECorrection_Local::DigitFromCalo         m_spd       ;
  /// evaluator of calorimeter for digit 
  CaloECorrection_Local::DigitFromCalo         m_prs       ;
  std::string m_detData;
  const DeCalorimeter* m_det;  
  int m_pFilt;
  int m_sFilt;
};
// ============================================================================
#endif // CALORECO_CALOECORRECTION_H
