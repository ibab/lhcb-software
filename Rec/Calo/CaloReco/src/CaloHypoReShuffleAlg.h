// $Id: CaloHypoReShuffleAlg.h,v 1.1 2003-03-13 18:47:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALORECO_CALOHYPORESHUFFLEALG_H 
#define CALORECO_CALOHYPORESHUFFLEALG_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"


/** @class CaloHypoReShuffleAlg CaloHypoReShuffleAlg.h
 *
 *  Algorithm for fixing the Merged Pi0s problems 
 *  with Brunel v17r4 production.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-03-13
 */

class CaloHypoReShuffleAlg : public CaloAlgorithm 
{
  /// friend factory for instantiation 
  friend class AlgFactory<CaloHypoReShuffleAlg>;
public:

  /** execution of the algorithm 
   *  @return statsu code 
   */
  virtual StatusCode execute    () ;
  
protected:
  
  /** Standard constructor
   *  @see CaloAlgorithm
   *  @see     Algorithm 
   *  @param name name of the algorithm
   *  @param svc  service locator
   */
  CaloHypoReShuffleAlg ( const std::string& name , 
                         ISvcLocator*       svc ) ;
  
  // destructor 
  virtual ~CaloHypoReShuffleAlg () ;

private:

  // default constructor is private 
  CaloHypoReShuffleAlg () ;
  
  // copy  constructor is private 
  CaloHypoReShuffleAlg            ( const CaloHypoReShuffleAlg& );

  // assignement operator  is private 
  CaloHypoReShuffleAlg& operator= ( const CaloHypoReShuffleAlg& ) ;

};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOHYPORESHUFFLEALG_H
// ============================================================================
