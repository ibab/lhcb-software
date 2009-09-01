// $Id: TreeGrammarTest.cpp,v 1.3 2009-09-01 09:02:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// Include files 
// ============================================================================
// STT & STL 
// ============================================================================
#include <iostream>
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
#include "Kernel/Symbols.h"
// ============================================================================
// Decays
// ============================================================================
#include "LoKi/TreeHelpers.h"
#include "LoKi/TreeParser.h"
// ============================================================================
/** @file
 *  Simple application to test Decay Tree parsers 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-22
 */
// ============================================================================
int main() 
{
  
  Decays::Parsers::Tree tree ;
  
  
  const Decays::Symbols& syms = Decays::Symbols::instance() ;
  
  std::vector<std::string> symbols, particles ;
  syms.symbols ( symbols ) ;
  
  
  particles.push_back ( "B0"  ) ;
  particles.push_back ( "B+"  ) ;
  particles.push_back ( "B-"  ) ;
  particles.push_back ( "D0"  ) ;
  particles.push_back ( "D+"  ) ;
  particles.push_back ( "D-"  ) ;
  particles.push_back ( "chi_10"    ) ;
  particles.push_back ( "~chi_10"   ) ;
  particles.push_back ( "X(3872)+"  ) ;
  particles.push_back ( "X(3872)~+" ) ;

  std::string input ;
  std::cout << " Enter the tree " << std::endl ;
  
  while ( std::getline ( std::cin , input ) ) 
  {
    if ( input.empty() ) { break ; }
    
    StatusCode sc = Decays::Parsers::parse 
      ( tree      , 
        input     , 
        symbols   , 
        particles , 
        std::cout ) ;
    
    std::cout << " Parsing of #'"    << input      << "'# "
              << " is "              << sc         << std::endl 
              << " Result is "       << tree       << std::endl ;
    std::cout << " Enter the tree  " << std::endl ;    
  }
};
// ============================================================================
// The END 
// ============================================================================
