// $Id: NodeGrammarTest.cpp,v 1.2 2009-05-23 15:54:19 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
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
#include "LoKi/NodeParser.h"
// ============================================================================
/** @file
 *  Simple application to test Decay Node parsers 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-12
 */
// ============================================================================
int main() 
{
  /// initialize the node with some stuff
  Decays::Node node = Decays::Nodes::_Node().node() ;

  
  const Decays::Symbols& syms = Decays::Symbols::instance() ;
  
  std::vector<std::string> symbols, particles ;
  syms.symbols ( symbols ) ;
  
  particles.push_back ( "B0"  ) ;
  particles.push_back ( "B+"  ) ;
  particles.push_back ( "B-"  ) ;
  particles.push_back ( "D0"  ) ;
  particles.push_back ( "D+"  ) ;
  particles.push_back ( "D-"  ) ;
  particles.push_back ( "X(3872)+"  ) ;
  particles.push_back ( "X(3872)~+" ) ;


  std::string input ;
  std::cout << " Enter the node  " << std::endl ;
  
  while ( std::getline ( std::cin , input ) ) 
  {
    if ( input.empty() ) { break ; }
    
    StatusCode sc = Decays::Parsers::parse 
      ( node      , 
        input     , 
        symbols   , 
        particles , 
        std::cout ) ;
    
      std::cout << " Parsing of #'"    << input  << "'# "
                << " is "              << sc     << std::endl 
                << " Result is "       << node   << std::endl ;
    
    std::cout << " Enter the node  " << std::endl ;    
  }
};
// ============================================================================
// The END 
// ============================================================================
