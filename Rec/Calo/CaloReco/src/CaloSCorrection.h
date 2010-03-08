// $Id: CaloSCorrection.h,v 1.4 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2008/06/30 15:36:33  odescham
// prepare for HLT processing
//
// Revision 1.2  2006/06/27 16:36:53  odescham
// 3rd step toward DC06 : repackaging
//
// Revision 1.1  2006/05/30 09:42:04  odescham
// first release of the CaloReco migration
//
// Revision 1.3  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2005/03/07 15:37:15  cattanem
// fix doxygen warnings
//
// Revision 1.1  2003/04/11 09:33:42  ibelyaev
//  add new E-,S- and L-corrections from Olivier Deschamps
//
// ============================================================================
#ifndef CALORECO_CALOSCORRECTION_H 
#define CALORECO_CALOSCORRECTION_H 1
// Include files
// from STL
#include <string>
// from CaloInterfaces 
#include "CaloInterfaces/ICaloHypoTool.h"
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// from Event 
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
// from CaloUtils 
#include "CaloUtils/ClusterFunctors.h"

/** @namespace CaloSCorrection_Local
 */
namespace CaloSCorrection_Local
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


/** @class CaloSCorrection CaloSCorrection.h
 *  
 *
 *  @author XXX-XXX xxx@xxx.com 
 *  @date   2003-03-10
 */
class CaloSCorrection : 
  public virtual ICaloHypoTool ,
  public              GaudiTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloSCorrection>;
  
public:
  
  /** The main processing method
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
public:

  /** initialization of the tool 
   *  @see  GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** finalization of the tool 
   *  @see  GaudiTool 
   *  @see   AlgTool 
   *  @see  IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** Standard constructor
   *  @see GaudiTool 
   *  @see  AlgTool 
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent  tool parent 
   */
  CaloSCorrection ( const std::string& type   , 
                         const std::string& name   ,
                         const IInterface*  parent ) ;
  
  /// destructor
  virtual ~CaloSCorrection () ;
  
private:
  
  /// default constructor is private 
  CaloSCorrection () ;
  /// copy constructor is private 
  CaloSCorrection           ( const CaloSCorrection& ) ;
  /// assignement operator is private 
  CaloSCorrection& operator=( const CaloSCorrection& ) ;
  
private:
  
  // internal types 
  typedef std::vector<LHCb::CaloHypo::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  typedef std::vector<double>                       Parameters  ;
  typedef std::vector<bool>                         BoolParameters  ;
  
  /// list of allowed hypos
  Hypotheses                                        m_hypos     ;
  Hypotheses_                                       m_hypos_    ;

  Parameters                                        Par_Asinh  ;
  Parameters                                        Par_ResOut ;
  Parameters                                        Par_ResMid ;
  Parameters                                        Par_ResInn ;
  Parameters                                        Par_AsOut ;
  Parameters                                        Par_AsMid ;
  Parameters                                        Par_AsInn ;
  BoolParameters                                    Level ;

  /// evaluator of "cluster area"
  LHCb::ClusterFunctors::ClusterArea                      m_area      ;
  /// evaluator of "cluster calorimeter 
  LHCb::ClusterFunctors::ClusterFromCalo                  m_calo      ;
  /// evaluator of calorimeter for digit 
  CaloSCorrection_Local::DigitFromCalo              m_spd       ;
  /// evaluator of calorimeter for digit 
  CaloSCorrection_Local::DigitFromCalo              m_prs       ;
  std::string m_detData;
  const DeCalorimeter* m_det;
};
// ============================================================================
#endif // CALORECO_CALOSCORRECTION_H
