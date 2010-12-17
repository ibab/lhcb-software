// $Id: CaloSinglePhotonAlg.h,v 1.8 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2008/06/30 15:36:33  odescham
// prepare for HLT processing
//
// Revision 1.6  2006/05/30 09:42:06  odescham
// first release of the CaloReco migration
//
// Revision 1.5  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.4  2004/12/10 17:12:29  ibelyaev
//  steps towards 'Fast' Calo recontruction
//
// ============================================================================
#ifndef CALOALGS_CALOSINGLEPHOTONALG_H 
#define CALOALGS_CALOSINGLEPHOTONALG_H 1
// ============================================================================
#include <string>
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloDet/DeCalorimeter.h"
class ICaloClusterSelector ;
class ICaloHypoTool        ;

/** @class CaloSinglePhotonAlg CaloSinglePhotonAlg.h
 *  
 *  The simplest algorithm of reconstruction of 
 *  single photon in electromagnetic calorimeter.
 *  The implementation is based on F.Machefert's codes.
 *
 *  @author Frederic Machefert machefer@in2p3.fr
 *  @author Vanya Belyaev      Ivan.Belyaev@itep.ru
 *  @date   31/03/2002
 */
class CaloSinglePhotonAlg : 
  public GaudiAlgorithm
{  
  /// friend factory for instantiation 
  friend class AlgFactory<CaloSinglePhotonAlg>; 
public:
  
  /// container of names
  typedef std::vector<std::string>           Names       ;
  /// container of selector tools 
  typedef std::vector<ICaloClusterSelector*> Selectors   ;
  /// containers of hypo tools 
  typedef std::vector<ICaloHypoTool*>        HypoTools   ;
  /// container of correction tools (S-,L-,...)
  typedef HypoTools                          Corrections ;
  
public:

  /**  standard Algorithm initialization
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code 
   */
  virtual StatusCode initialize ();  
  
  /**  standard Algorithm execution
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code 
   */
  virtual StatusCode execute    ();  
  
  /**  standard Algorithm finalization
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code 
   */
  virtual StatusCode finalize   ();
  
protected:
  
  /** Standard constructor
   *  @param name algorithm name 
   *  @param pSvc service locator 
   */
  CaloSinglePhotonAlg
  ( const std::string&  name , 
    ISvcLocator*        pSvc );
  
  /// destructor (protected and virtual)
  virtual ~CaloSinglePhotonAlg();
  
private:
  
  // fast cut on transverse energy 
  double       m_eTcut                     ;

  // cluster selectors 
  Names        m_selectorsTypeNames        ;
  Selectors    m_selectors                 ;
  
  // corrections
  Names        m_correctionsTypeNames       ;
  Corrections  m_corrections                ;
  
  // other hypo tools 
  Names        m_hypotoolsTypeNames         ;
  HypoTools    m_hypotools                  ;

  // corrections
  Names        m_correctionsTypeNames2      ;
  Corrections  m_corrections2               ;
  
  // other hypo tools 
  Names        m_hypotoolsTypeNames2        ;
  HypoTools    m_hypotools2                 ;

  std::string m_inputData ;
  std::string m_outputData;
  std::string m_detData   ;
  const DeCalorimeter*  m_det;
  

};
// ============================================================================
#endif // CALOSINGLEPHOTONALG_H
