// $Id: CaloDigitsFilterAlg.h,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOALGS_CALODIGITSFILTERALG_H 
#define CALOALGS_CALODIGITSFILTERALG_H 1
// Include files
// from STL
#include <string>
#include <vector>

// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"


/** @class CaloDigitsFilterAlg CaloDigitsFilterAlg.h
 *  
 *  Simple algorithm to perform filtering of CaloDigits 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-06-11
 */
class CaloDigitsFilterAlg : 
  public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloDigitsFilterAlg>;
public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @see CaloAlgorithm
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
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOALGS_CALODIGITSFILTERALG_H
// ============================================================================
