// $Id: CaloElectronAlg.h,v 1.8 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2009/08/13 16:04:42  jonrob
// add missing include
//
// Revision 1.6  2008/06/30 15:36:33  odescham
// prepare for HLT processing
//
// Revision 1.5  2006/05/30 09:42:02  odescham
// first release of the CaloReco migration
//
// Revision 1.4  2005/11/07 12:12:42  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.3  2003/06/23 13:11:54  ibelyaev
//  fix for gcc3.2
//
// Revision 1.2  2002/12/09 17:43:09  cattanem
// bug fixes
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.1  2002/06/13 12:36:13  ibelyaev
//  new algorithms and new options
//
// ============================================================================
#ifndef CALORECO_CALOElectronALG_H 
#define CALORECO_CALOElectronALG_H 1
// Include files
// from STL
#include <string>
#include <memory>
// from GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
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
  public GaudiAlgorithm
{  
  /// friend factory for instantiation 
  friend class AlgFactory<CaloElectronAlg>; 
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

  std::string m_inputData ;
  std::string m_outputData;
  std::string m_detData   ;
};

// ============================================================================
#endif // CALOElectronALG_H
