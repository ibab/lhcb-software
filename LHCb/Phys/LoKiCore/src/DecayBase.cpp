// $Id: DecayBase.cpp,v 1.5 2009-12-03 12:53:20 ibelyaev Exp $
// ============================================================================
// Include files 
// =============================================================================
// PartProp
// =============================================================================
#include "Kernel/Symbols.h"
#include "Kernel/Nodes.h"
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/DecayBase.h"
#include "LoKi/NodeParser.h"
#include "LoKi/TreeParser.h"
// =============================================================================
// Boost 
// =============================================================================
#include "boost/algorithm/string/trim.hpp"
// =============================================================================
/** @file 
 *  Implementation file for class LoKi::DecayBase
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-22
 */
// =============================================================================
namespace 
{
  // ===========================================================================
  /// add the brackest to the tree descriptor 
  std::string brackets4tree ( std::string decay ) 
  {
    boost::trim ( decay ) ;
    if ( decay.empty() )                              { return decay ; } // RETURN 
    std::size_t last = decay.size() - 1 ;
    if ( '[' == decay[0] && ']' == decay[last]  )     { return decay ; } // RETURN 
    //
    return "( " + decay + " )" ; 
  } 
  // ===========================================================================
  /// add the brackets to the node descriptor 
  std::string brackets4node ( std::string node ) 
  {
    boost::trim ( node ) ;
    if ( node.empty() )                               { return node ; } // RETURN
    std::size_t last = node.size() - 1 ;
    if ( '[' == node[0] && ']' == node[last]  )       { return node ; } // RETURN 
    //
    return "( " + node + " )" ;
  }
  // ===========================================================================
}
// =============================================================================
// constructor 
// =============================================================================
LoKi::DecayBase::DecayBase 
( const std::string& type   ,                        // the actual tool type (?)
  const std::string& name   ,                        //     the actual tool name 
  const IInterface*  parent )                        //               the parent
  : GaudiTool ( type , name , parent ) 
  //
  , m_ppSvc        ( 0 )
  , m_default_node () 
  , m_default_tree ()
{
  declareProperty 
    ( "DefaultNode"  , 
      m_default_node ,
      "The default node" ) ;
  declareProperty 
    ( "DefaultTree"  , 
      m_default_tree ,
      "The default tree" ) ;
}
// =============================================================================
// destructor 
// =============================================================================
LoKi::DecayBase::~DecayBase (){}
// =============================================================================
// finalize 
// =============================================================================
StatusCode LoKi::DecayBase::finalize () 
{
  m_ppSvc = 0 ;
  return GaudiTool::finalize () ;
} 
// =============================================================================
/*  parse the node 
 *  @param node  (OUTPUT) parsed node 
 *  @param input (INPUT) input string
 *  @return status code
 */
// =============================================================================
StatusCode LoKi::DecayBase::_parse 
( Decays::Node& node , std::string input ) const 
{
  //
  input = brackets4node ( input ) ;
  if ( input.empty() ) 
  { 
    node = Decays::Nodes::Invalid() ;
    return Error ( "Empty node descriptor!" ) ;                      // RETURN 
  }
  //
  MsgStream& err = error() ;
  /// get the list of particles form the service:
  Decays::Symbols::Names _particles ;
  Decays::Symbols::Names _symbols   ;
  const Decays::Symbols& _s = Decays::Symbols::instance() ;
  // get symbols 
  _s.symbols   (           _symbols   ) ;
  // get particles 
  _s.particles ( ppSvc() , _particles ) ;
  // parse the node:
  StatusCode sc = Decays::Parsers::parse 
    ( node , input , _symbols , _particles , err.stream() ) ;
  if ( sc.isFailure() ) 
  {
    err << endmsg ;
    return Error ( "Unable to parse '"+input+"'" , sc ) ;  
  }
  // validate the node 
  sc = node.validate ( ppSvc() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Unable to validate the node" , sc ) ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the tree
 *  @param tree  (OUTPUT) parsed node 
 *  @param input (INPUT) input string
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::DecayBase::_parse 
( Decays::Parsers::Tree& tree , std::string input ) const 
{
  if ( input.empty() && !defaultTree().empty() ) 
  {
    if ( msgLevel( MSG::DEBUG ) ) 
    { debug () << "Use default tree '" << defaultTree() << "'" << endmsg ; }
    return _parse ( tree , defaultTree() ) ;                           // RETURN
  }
  //
  input = _makeCC ( input ) ;                                       // CC-fy it!
  input = brackets4tree ( input ) ;
  if ( input.empty() ) 
  { 
    return Error("Empty decay descriptor!") ; // RETURN 
  } 
  //
  MsgStream& err = error() ;
  /// get the list of particles form the service:
  Decays::Symbols::Names _particles ;
  Decays::Symbols::Names _symbols   ;
  const Decays::Symbols& _s = Decays::Symbols::instance() ;
  // get symbols 
  _s.symbols   (           _symbols   ) ;
  // get particles 
  _s.particles ( ppSvc() , _particles ) ;
  // parse the node:
  StatusCode sc = Decays::Parsers::parse 
    ( tree , input , _symbols , _particles , err.stream() ) ;
  if ( sc.isFailure() ) 
  {
    err << endreq ;
    return Error ( "Unable to parse '"+input+"'" , sc ) ;            // RETURN 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// Convert the substring '[ a ]CC' into ' ( a , aCC ) '  
// The lines come from Sascha Mazurov
// ============================================================================
std::string LoKi::DecayBase::_makeCC ( std::string input ) const 
{
  boost::trim ( input ) ;
  if ( input.empty() ) { return input ; }                             // RETURN 
  
  // Current position at result
  int pos = input.size()-1;
  
  int backpos = 0 ; // Position from back of string
  int level   = 0 ; // Current nesting level  INSIDE [...]CC construction
  int end     = 0 ; // Position before the ]CC
  
  for( ; 0 <= pos  ; --pos, ++backpos )
  {
    // find "]cc"
    if ( ']' == input[pos] )
    {
      if( (  1  <  backpos      ) && 
          ( 'C' == input[pos+1] ) && 
          ( 'C' == input[pos+2] ) && 
          (  0  == end          ) ) { end = pos-1; }
      else if (end != 0) { ++level; }
      //
      continue;                                                     // CONTINUE  
    }
    // find matching "["
    if ( '['== input[pos] && 0 != end )
    {
      if (level == 0) // The start of [...]CC construction was founded
      { 
        // Build new string
        std::string matched   = input.substr(pos+1,end-pos) ;
        matched = brackets4tree    ( matched ) ;
        // use ParticleProperty Service to find proper CC 
        const std::string matchedCC = ppSvc()->cc ( matched ) ;
        //
        if ( matched == matchedCC ) 
        {
          input = input.substr(0,pos) 
            + " " 
            + matched 
            + " " 
            + input.substr(end+4,input.size()-end+4) ;
        }
        else 
        {
          input = input.substr(0,pos) 
            + " [ "
            + matched 
            + " , "
            + matchedCC 
            + " ] " 
            + input.substr(end+4,input.size()-end+4) ;
        }
        // Reset positions to the default values. And we (re)start from the end.
        pos = input.size()-1;
        backpos = level = end = 0;
      }
      else { --level; }
    }
  } // end of the loop over input string 
  //
  return  input ;
}
// ========================================================================
// build the node form the node descriptor
// ============================================================================
Decays::Node LoKi::DecayBase::node ( const std::string& descriptor ) const 
{
  if ( descriptor.empty() && !defaultNode().empty() ) 
  {
    if ( msgLevel( MSG::DEBUG ) ) 
    { debug () << "Use default node '" << defaultNode() << "'" << endmsg ; }
    return node  ( defaultNode() ) ;                                  // RETURN 
  }
  //
  Decays::Node _node = Decays::Nodes::Invalid() ;
  StatusCode sc = _parse ( _node , descriptor ) ;
  //
  if ( sc.isFailure() ) 
  { 
    Error ( "Error from _parse('" + descriptor + "')" , sc ) ; 
    return Decays::Nodes::Invalid () ;                                // RETURN
  }
  if ( _node.valid() ) { return _node ; }                             // RETURN 
  // try to validate it:
  sc = _node.validate ( ppSvc () )  ;
  if ( sc.isFailure() ) 
  { 
    Error ( "Unable to validate '" + descriptor + "'", sc ) ;
    return Decays::Nodes::Invalid () ;                                // RETURN 
  } 
  return _node ;                                                      // REUTRN 
}
// ============================================================================


// =============================================================================
// The END 
// =============================================================================
