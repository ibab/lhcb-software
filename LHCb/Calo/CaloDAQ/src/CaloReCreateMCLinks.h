// $Id: CaloReCreateMCLinks.h,v 1.1 2005-01-12 09:59:14 ocallot Exp $
#ifndef CALOEVENT_CALORECREATEMCLINKS_H 
#define CALOEVENT_CALORECREATEMCLINKS_H 1

// Include files

#include "GaudiAlg/GaudiAlgorithm.h"



/** @class CaloReCreateMCLinks CaloReCreateMCLinks.h
 *
 *  Simple algorithm to be invoked after 
 *   CaloDigitFromBuffer to re-create the MC links 
 *
 *  @code 
 *
 *  // (re)create digits from Raw buffer 
 *  ApplicationMgr.TopAlg += { "CaloDigitFromRaw"} ;
 *  // (re)create Digits -> McDigits links 
 *  ApplicationMgr.TopAlg += { "CaloReCreateMCLinks" };
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-23
 */
class CaloReCreateMCLinks : public GaudiAlgorithm {

public:
  
  /** execution of the algorithm
   *  @see IAlgorithm
   *  @return status code
   */
  virtual StatusCode execute() ;
  
  /** Standard constructor
   *  @see GaudiAlgorithm
   *  @see      Algorithm 
   *  @see      AlgFactory
   *  @see     IAlgFactory 
   *  @param name algorithm instance name 
   *  @param pSvc service locator
   */
  CaloReCreateMCLinks ( const std::string& name , 
                        ISvcLocator*       pSvc ) ;

  /// destructor 
  virtual ~CaloReCreateMCLinks( ); ///< Destructor

private:
  
  typedef std::vector<std::string> Addresses ;
  Addresses m_raw ;
  Addresses m_mc  ;

};

#endif // COMPONENT_CALORECREATEMCLINKS_H
// ============================================================================
