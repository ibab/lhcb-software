// $Id: CaloDigitsFilterAlg.h,v 1.3 2005-11-07 12:12:42 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 12:08:06  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
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
