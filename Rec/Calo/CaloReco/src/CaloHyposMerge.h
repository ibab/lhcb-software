// $Id: CaloHyposMerge.h,v 1.1.1.1 2002-11-13 20:46:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOHYPOSMERGE_H 
#define CALOHYPOSMERGE_H 1
// Include files
#include "CaloKernel/CaloAlgorithm.h"

/** @class CaloHyposMerge CaloHyposMerge.h
 *  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */

class CaloHyposMerge : public CaloAlgorithm
{
  // friend factory for instantiation 
  friend class AlgFactory<CaloHyposMerge>;

public:
  
  /**  standard Algorithm  initialization
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code 
   */
  virtual StatusCode initialize () ;  
  
  /**  standard Algorithm execution
   *   @see CaloAlgorithm
   *   @see     Algorithm
   *   @see    IAlgorithm
   *   @return status code 
   */
  virtual StatusCode execute    () ;  
  
  
protected:
  
  /** Standard constructor
   *  @param name algorithm name 
   *  @param pSvc service locator 
   */
  CaloHyposMerge ( const std::string&  name , 
                   ISvcLocator*        pSvc );
  
  /// destructor 
  virtual ~CaloHyposMerge();
  
private:
  
  /// default constructor is private 
  CaloHyposMerge(); 
  /// copy constructor is private 
  CaloHyposMerge( const CaloHyposMerge& );
  /// assignement operator is private 
  CaloHyposMerge& operator=( const CaloHyposMerge& ) ;
  
private:
  
  typedef std::string          Address   ;
  typedef std::vector<Address> Addresses ;
  ///
  Addresses m_hypos    ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOHYPOSMERGE_H
// ============================================================================
