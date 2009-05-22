// $Id: NodeParser.cpp,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/NodeGrammar.h"
#include "LoKi/NodeParser.h"
// ============================================================================
/** @file 
 *  Implementation file for node parser functions 
 *  @author Vanya BELYAEV Ivan.Belyaev@ikhef.nl
 *  @date 2009-05-12 
 */
// ============================================================================
StatusCode Decays::Parsers::parse 
( Decays::Node&                   node      , 
  const std::string&              input     , 
  const std::vector<std::string>& symbols   ,
  const std::vector<std::string>& particles , 
  std::ostream&                   stream    ) 
{
  
  Decays::Nodes::_Node _node ;
  
  using namespace phoenix ;
  using namespace boost::spirit::classic ;
  
  Decays::Grammars::Node g 
    ( &symbols   ,
      &particles ) ;
  
  parse_info<std::string::const_iterator> info =
    parse ( input.begin ()               , 
            input.end   ()               , 
            g[var(_node)=arg1] >> !end_p ,
            space_p                      ) ;
  
  if ( !info.full ) 
  { stream << "PARSE ERROR at: ##"
           << std::string ( input.begin() , info.stop ) 
           << "##" ; }
  
  node = _node.node() ;
  
  return info.full ;
  
}


// ============================================================================
// The END 
// ============================================================================
