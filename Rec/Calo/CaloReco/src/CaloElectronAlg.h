// $Id: CaloElectronAlg.h,v 1.2 2002-12-09 17:43:09 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.1  2002/06/13 12:36:13  ibelyaev
//  new algorithms and new options
//
// ============================================================================
#ifndef CALOALGS_CALOElectronALG_H 
#define CALOALGS_CALOElectronALG_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"
// forward delcarations 
class ICaloClusterSelector ;
class ICaloHypoTool        ;

/** @class CaloElectronAlg CaloElectronAlg.h
 *  
 *  The simplest algorithm of reconstruction of 
 *  electrons in electromagnetic calorimeter.
 *
 *  @author Vanya Belyaev      Ivan.Belyaev@itep.ru
 *  @date   31/03/2002
 */
class CaloElectronAlg : 
  public CaloAlgorithm
{  
  /// friend factory for instantiation 
  friend AlgFactory<CaloElectronAlg>; 
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
  CaloElectronAlg
  ( const std::string&  name , 
    ISvcLocator*        pSvc );
  
  /// destructor (protected and virtual)
  virtual ~CaloElectronAlg();
  
private:
  
  // cluster selectors 
  Names        m_selectorsTypeNames    ;
  Selectors    m_selectors             ;
  
  // corrections
  Names        m_correctionsTypeNames  ;
  Corrections  m_corrections           ;
  
  // other hypo tools 
  Names        m_hypotoolsTypeNames    ;
  HypoTools    m_hypotools             ;

  // corrections
  Names        m_correctionsTypeNames2 ;
  Corrections  m_corrections2          ;
  
  // other hypo tools 
  Names        m_hypotoolsTypeNames2   ;
  HypoTools    m_hypotools2            ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOElectronALG_H
// ============================================================================
