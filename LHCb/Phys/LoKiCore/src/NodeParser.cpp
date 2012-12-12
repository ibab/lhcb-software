// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/NodeGrammar.h"
#include "LoKi/NodeParser.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for node parser functions 
 *
 *  This file is a part of LoKi project - 
 *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@ikhef.nl
 *  @author Alexander MAZUROV alexander.mazurov@gmail.com
 *
 *  @date 2009-05-12 
 *  @date 2011-12-08 Spirit2 migration  
 */
// ============================================================================
StatusCode Decays::Parsers::parse
( Decays::Node&                   node      , 
  const std::string&              input     , 
  const std::vector<std::string>& symbols   ,
  const std::vector<std::string>& particles , 
  std::ostream&                   stream    ) 
{
  // ==========================================================================
  typedef std::string::const_iterator Iterator;
  typedef boost::spirit::qi::space_type Skipper;
  
  Decays::Nodes::_Node result;
  Decays::Grammars::Node<Iterator, Skipper> gr(symbols, particles);
  Iterator begin = input.begin();
  bool ok =  boost::spirit::qi::phrase_parse
    ( begin        , 
      input.end()  , 
      gr           , 
      boost::spirit::qi::space , result ) && ( begin == input.end() ) ;
  // ==========================================================================
  if ( !ok ) 
  { 
    stream << "PARSE ERROR at: ##"
           << std::string ( begin , input.end() ) 
           << "##" ;
  }
  // ==========================================================================
  node = result.node() ;
  return ok ;
}
// ============================================================================
// The END 
// ============================================================================
