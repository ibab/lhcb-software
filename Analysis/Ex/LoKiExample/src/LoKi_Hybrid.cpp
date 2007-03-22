// $Id: LoKi_Hybrid.cpp,v 1.1 2007-03-22 18:33:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
//
// ============================================================================
// Include files 
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================
/** @file 
 *  Simple illustrationof usage of filters and especially hydrid (C++/Python) 
 *  filters 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_ALGORITHM(LoKi_Hybrid) 
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  Range all = select ( "all" , ALL ) ;
  
  
  IFilterCriterion* iFirst  = filterCriterion ( "First"   ) ;
  Range first  = select ( "first"  , all , FILTER ( iFirst  ) ) ;
  
  IFilterCriterion* iSecond = filterCriterion ( "Second" ) ;
  Range second = select ( "second" , all , FILTER ( iSecond ) ) ;
  
  IFilterCriterion* iThird  = filterCriterion ( "Third"  ) ;
  Range third  = select ( "third"  , all , FILTER ( iThird ) ) ;

  counter ( "#all"    ) += all    .size () ;
  counter ( "#first"  ) += first  .size () ;
  counter ( "#second" ) += second .size () ;
  counter ( "third"   ) += third  .size () ;
  
  info() << " #All/First/Second/Third : " 
         << all    .size () << "/"
         << first  .size () << "/"
         << second .size () << "/"
         << third  .size () << endreq ;
  
  return SUCCESS ;                                       // RETURN 
};
// ============================================================================
// The END 
// ============================================================================
