// $Id: MCDecayGrammarTest.cpp,v 1.1 2009-05-23 15:58:12 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
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
#include "LoKi/Trees.h"
#include "LoKi/MCTreeFactory.h"
// ============================================================================
/** @file
 *  Simple application to test Decay Tree parsers 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-22
 */
// ============================================================================
int main() 
{
  

  const Decays::Symbols& syms = Decays::Symbols::instance() ;
  
  std::vector<std::string> symbols, particles ;
  syms.symbols ( symbols ) ;
  
  typedef Decays::Tree_<const LHCb::MCParticle*>              Tree ;
  typedef Decays::Trees::Invalid_<const LHCb::MCParticle*> Invalid ;
  
  particles.push_back ( "B0"  ) ;
  particles.push_back ( "B+"  ) ;
  particles.push_back ( "B-"  ) ;
  particles.push_back ( "D0"  ) ;
  particles.push_back ( "D+"  ) ;
  particles.push_back ( "D-"  ) ;
  particles.push_back ( "X(3872)+"  ) ;
  particles.push_back ( "X(3872)~+" ) ;

  std::string input ;
  std::cout << " Enter the tree " << std::endl ;
  
  while ( std::getline ( std::cin , input ) ) 
  {
    if ( input.empty() ) { break ; }
    
    // 1) parse the intout into "generic tree" 
    Decays::Parsers::Tree tree ;
    StatusCode sc = Decays::Parsers::parse 
      ( tree      , 
        input     , 
        symbols   , 
        particles , 
        std::cout ) ;
    
    std::cout << " Parsing of #'"    << input      << "'# "
              << " is "              << sc         << std::endl 
              << " Result is "       << tree       << std::endl ;
    
    // 2) convert it into reasonable decay tree
    Tree mctree = Invalid() ;
    sc = Decays::Trees::factory ( mctree , tree , std::cout )  ;
    
    std::cout << "MCTree from #'"    << input      << "'# "
              << " is "              << sc         << std::endl 
              << " Result is "       << mctree     << std::endl ;
    
    std::cout << " Enter the tree  " << std::endl ;    
  }
};
// ============================================================================
// The END 
// ============================================================================
