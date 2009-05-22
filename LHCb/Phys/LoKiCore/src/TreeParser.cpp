// $Id: TreeParser.cpp,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TreeGrammar.h"
#include "LoKi/TreeParser.h"
// ============================================================================
/** @file 
 *  Implementation file for tree parser functions 
 *  @author Vanya BELYAEV Ivan.Belyaev@ikhef.nl
 *  @date 2009-05-12 
 */
// ============================================================================
StatusCode Decays::Parsers::parse 
( Decays::Parsers::Tree&          tree      , 
  const std::string&              input     , 
  const std::vector<std::string>& symbols   ,
  const std::vector<std::string>& particles , 
  std::ostream&                   stream    ) 
{
  
  Decays::Parsers::Tree _tree ;
  
  using namespace phoenix ;
  using namespace boost::spirit::classic ;
  
  Decays::Grammars::Tree g 
    ( &symbols   ,
      &particles ) ;
  
  parse_info<std::string::const_iterator> info =
    parse ( input.begin ()   , 
            input.end   ()   , 
            g[var(_tree)=arg1] >> !end_p ,
            space_p          ) ;
  
  tree = _tree ;
  
  if ( !info.full ) 
  { stream << "PARSE ERROR at: ##"
           << std::string( input.begin() , info.stop ) 
           << "##" ; }
  
  return info.full ;
  
}


// ============================================================================
// The END 
// ============================================================================
