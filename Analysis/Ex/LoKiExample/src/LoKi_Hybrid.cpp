// $Id: LoKi_Hybrid.cpp,v 1.7 2009-04-24 08:07:29 pkoppenb Exp $
// ============================================================================
// Include files 
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
#include "Kernel/IFilterCriterion.h"
// ============================================================================
/** @file 
 *  Simple illustrationof usage of filters and especially hydrid (C++/Python) 
 *  filters 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-03-22
 */
// ============================================================================
LOKI_ALGORITHM(TestHybrid) 
{
  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  
  const StatusCode SUCCESS = StatusCode::SUCCESS ;
  
  Range all = select ( "all" , ALL ) ;
  
  
  IFilterCriterion* iFirst  = tool<IFilterCriterion> ( "First"   ) ;
  Range first  = select ( "first"  , all , FILTER ( iFirst  ) ) ;
  
  IFilterCriterion* iSecond = tool<IFilterCriterion> ( "Second" ) ;
  Range second = select ( "second" , all , FILTER ( iSecond ) ) ;
  
  IFilterCriterion* iThird  = tool<IFilterCriterion> ( "Third"  ) ;
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
