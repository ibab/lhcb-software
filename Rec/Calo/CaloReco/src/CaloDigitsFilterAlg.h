// $Id: CaloDigitsFilterAlg.h,v 1.4 2006-05-30 09:42:02 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/11/07 12:12:42  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.2  2004/02/17 12:08:06  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// ============================================================================
#ifndef CALORECO_CALODIGITSFILTERALG_H 
#define CALORECO_CALODIGITSFILTERALG_H 1
// Include files
// from STL
#include <string>
#include <vector>

// from GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CaloDigitsFilterAlg CaloDigitsFilterAlg.h
 *  
 *  Simple algorithm to perform filtering of CaloDigits 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-06-11
 */
class CaloDigitsFilterAlg : 
  public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloDigitsFilterAlg>;
public:
  
  /** standard algorithm execution 
   *  @see GaudiAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see GaudiAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloDigitsFilterAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloDigitsFilterAlg();
  
private:
  
  /// default  constructor  is  private 
  CaloDigitsFilterAlg(); 
  /// copy     constructor  is  private 
  CaloDigitsFilterAlg
  ( const CaloDigitsFilterAlg& );
  /// assignement operator  is  private 
  CaloDigitsFilterAlg& operator=
  ( const CaloDigitsFilterAlg& );
  
private:
  
  typedef std::vector<std::string> Addresses  ;
  typedef std::vector<int>         Statuses   ;

  Addresses                        m_hypos    ;
  Addresses                        m_clusters ;

  Statuses                         m_statuses ;
  std::string m_inputData;
  
  
};
// ============================================================================
#endif // CALORECO_CALODIGITSFILTERALG_H
