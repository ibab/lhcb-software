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
 *
 *  This file is a part of LoKi project - 
 *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@ikhef.nl
 *  @author Alexander Mazurov alexander.mazurov@gmail.com
 *  @date 2009-05-12 
 *  @date 2011-12-08 Spirit2 migration
 */
// ============================================================================
StatusCode Decays::Parsers::parse
( Decays::Parsers::Tree&          tree      , 
  const std::string&              input     , 
  const std::vector<std::string>& symbols   ,
  const std::vector<std::string>& particles , 
  std::ostream&                   stream    ) 
{
  // ==========================================================================
  typedef std::string::const_iterator   Iterator ;
  typedef boost::spirit::qi::space_type Skipper  ;
  
  Decays::Grammars::Tree<Iterator, Skipper> gr( symbols , particles ) ;
  Iterator begin = input.begin();
  bool ok =  boost::spirit::qi::phrase_parse
    ( begin       , 
      input.end() , 
      gr          , 
      boost::spirit::qi::space , tree ) && (begin == input.end() ) ;
  // ==========================================================================
  if ( !ok ) 
  { 
    stream << "PARSE ERROR at: ##"
           << std::string ( begin , input.end() ) 
           << "##" ;
  }
  // ==========================================================================
  return ok;
}
// ============================================================================
// The END 
// ============================================================================
