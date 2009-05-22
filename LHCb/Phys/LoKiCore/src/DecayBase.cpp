// $Id: DecayBase.cpp,v 1.1 2009-05-22 19:15:14 ibelyaev Exp $
// ============================================================================
// Include files 
// =============================================================================
// PartProp
// =============================================================================
#include "Kernel/Symbols.h"
// =============================================================================
// LoKi
// =============================================================================
#include "LoKi/DecayBase.h"
#include "LoKi/NodeParser.h"
#include "LoKi/TreeParser.h"
// =============================================================================
// Boost 
// =============================================================================
#include "boost/regex.hpp"
// =============================================================================
/** @file 
 *  Implementation file for class LoKi::DecayBase
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-05-22
 */
// =============================================================================
// constructor 
// =============================================================================
LoKi::DecayBase::DecayBase 
( const std::string& type   ,                        // the actual tool type (?)
  const std::string& name   ,                        //     the actual tool name 
  const IInterface*  parent )                        //               the parent
  : GaudiTool ( type , name , parent ) 
  //
  , m_ppSvc      ( 0 )
{
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
( Decays::Node& node , const std::string& input ) const 
{
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
    err << std::endl ;
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
( Decays::Parsers::Tree& tree , const std::string& input ) const 
{
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
    err << std::endl ;
    return Error ( "Unable to parse '"+input+"'" , sc ) ;  
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// convert the string ' [ a ]cc ' into ' ( a , Acc ) '
// ============================================================================
bool LoKi::DecayBase::_stripCC ( const std::string& input  , 
                                 std::string&       output ) const 
{
  static const boost::regex s_expr  ( "[ ]*\\[([^\\]]*)\\]cc[ ]*" ) ;
  // 
  boost::smatch what;
  if ( boost::regex_match ( input.begin() , input.end() , what , s_expr ) )
  { 
    output = std::string ( what[1].first , what[1].second ) ; 
    return true ;                                                    // RETURN 
  }
  output.clear() ;
  return false ;                                                      // RETURN 
}  
// ============================================================================
// convert the string ' [ a ]cc ' into ' ( a , aCC ) '  
// ============================================================================
std::string LoKi::DecayBase::_makeCC 
( const std::string& input ) const 
{
  std::string _cc ;
  bool isCC = _stripCC ( input , _cc ) ;
  if ( !isCC || _cc.empty() ) { return input ; }
  return "( " + _cc + " , " + ppSvc()->cc( _cc ) + " )" ;
}
// ========================================================================


// =============================================================================
// The END 
// =============================================================================
