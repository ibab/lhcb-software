// $Id: RelationsTest.cpp,v 1.1 2009-04-09 13:16:58 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// STD&STL 
// ============================================================================
#include <iostream>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/RelationsDict.h"
#include "Relations/Relation1D.h"
#include "Relations/RelationWeighted1D.h"
// ============================================================================
/** @file 
 *
 *  Auxillary test-file for the package Kernel/Relations 
 *  to detect the obvious problems
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-04-09 
 */
// ============================================================================
int main() 
{
  LHCb::Relation1D<int,float>                t1 ;
  LHCb::Relation2D<std::string,int>          t2 ;
  LHCb::RelationWeighted1D<int,float,double> t3 ;
  LHCb::RelationWeighted2D<int,float,double> t4 ;
  
  t1.relate( 0 , 0.0 ) ;
  t1.relate( 1 , 1.0 ) ;
  t1.relate( 2 , 2.0 ) ;
  
  t2.relate( "0" , 0 ) ;
  t2.relate( "1" , 1 ) ;
  t2.relate( "2" , 2 ) ;
  
  t3.relate( 0 , 0.f , 0.0 ) ;
  t3.relate( 1 , 1.f , 1.0 ) ;
  t3.relate( 2 , 2.f , 2.0 ) ;
  
  t4.relate( 0 , 0.f , 0.0 ) ;
  t4.relate( 1 , 1.f , 1.0 ) ;
  t4.relate( 2 , 2.f , 2.0 ) ;
  
  std::cout << " Table size: "
            << " t1 " << t1.relations().size() 
            << " t2 " << t2.relations().size() 
            << " t3 " << t3.relations().size() 
            << " t4 " << t4.relations().size() << std::endl ;
  
} 
// ============================================================================
// The END 
// ============================================================================
