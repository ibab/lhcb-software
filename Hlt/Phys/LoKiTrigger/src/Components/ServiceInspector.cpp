// $Id: ServiceInspector.cpp,v 1.2 2009-03-28 13:58:48 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <ctime> 
#include <fstream> 
#include <set> 
#include <algorithm> 
// ============================================================================
// Local
// ============================================================================
#include "Service.h"
// ============================================================================
/** @file 
 *  Implementation file for class Hlt::Service
 *  The methods from Hlt::IInspector are implemented here 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-18
 */
// ============================================================================
/** get the producer for the given selection 
 *  @param selection the selection 
 *  @return the producer 
 */
// ============================================================================
const IAlgorithm*  Hlt::Service::producer 
( const Hlt::Selection*    selection ) const 
{
  if  ( 0 == selection ) 
  { 
    if ( m_pedantic || msgLevel ( MSG::DEBUG ) ) 
    { Error ( "producer: invalid selection, return NULL" ) ; }
    return 0 ;                                                        // RETURN 
  }
  return producer ( selection -> id () ) ;
}
// ============================================================================
/*  get the producer for the given selection 
 *  @param selection the selection 
 *  @return the producer 
 */
// ============================================================================
const IAlgorithm*  Hlt::Service::producer 
( const std::string& key ) const 
{
  if ( !inMap ( key ) ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ("producer: unknown selection'" + key + "', return NULL") ; }
    return 0 ;
  }
  // ==========================================================================
  for ( OutputMap::iterator iout = m_outputs.begin() ; 
        m_outputs.end() != iout ; ++iout ) 
  {
    const SelMap& sels = iout->second ;
    SelMap::iterator ifind = sels.find ( key );
    if ( sels.end() != ifind ) { return iout->first ; } // RETUTRN
  }
  // ==========================================================================
  if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
  { Warning ("producer: no producer for selection'" + key + 
             "', return NULL") ; }
  return 0 ;
}
// ============================================================================
/*  get all nominal consumers for the given selection 
 *  @param selection the selection 
 *  @param alglist (OUTPUT) list of consumers 
 *  @param number of nominal consumers 
 */
// ============================================================================
size_t Hlt::Service::consumers 
( const Hlt::Selection* selection , 
  AlgList&              alglist   ) const  
{
  alglist.clear() ;
  // ==========================================================================
  if  ( 0 == selection ) 
  { 
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Error ( "consumers: invalid selection, return empty list") ; }
    return 0 ;                                                        // RETURN 
  }
  return consumers ( selection -> id () , alglist ) ;
}
// ============================================================================
/*  get all nominal consumers for the given selection 
 *  @param selection the selection 
 *  @param alglist (OUTPUT) list of consumers 
 *  @param number of nominal consumers 
 */
// ============================================================================
size_t Hlt::Service::consumers 
( const std::string&    key       , 
  AlgList&              alglist   ) const  
{
  alglist.clear() ;
  // ==========================================================================
  if ( !inMap ( key ) ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ("consumers: unknown selection'" + key + "', return empty list") ; }
    return 0 ;
  }
  // ==========================================================================
  for ( InputMap::iterator iin = m_inputs.begin() ; 
        m_inputs.end() != iin ; ++iin ) 
  {
    const SelMap& sels = iin->second ;
    SelMap::iterator ifind = sels.find ( key );
    if ( sels.end() != ifind ) { alglist.push_back ( iin->first ) ; }
  }
  // ==========================================================================
  if ( alglist.empty() ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ("consumers: no consumers for selection'" + key + 
               "', return empty list") ; }
  }
  return alglist.size() ;
}
// ============================================================================
/*  get the inputs for the given algorithm 
 *  @param algorithm the algorithm (consumer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of input selections 
 */
// ============================================================================
size_t Hlt::Service::inputs 
( const IAlgorithm*  algorithm , 
  SelList&           selections ) const 
{
  selections.clear() ;
  if ( 0 == algorithm ) 
  {
    Error ("inputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return inputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the inputs for the given algorithm 
 *  @param algorithm the algorithm (consumer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of input selections 
 */
// ============================================================================
size_t Hlt::Service::inputs 
( const IAlgorithm*  algorithm , 
  KeyList&           selections ) const 
{
  selections.clear() ;
  if ( 0 == algorithm ) 
  {
    Error ("inputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return inputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the inputs for the given algorithm 
 *  @param algorithm the algorithm (consumer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of input selections 
 */
// ============================================================================
size_t Hlt::Service::inputs 
( const std::string& algorithm  , 
  SelList&           selections ) const 
{
  selections.clear() ;
  // =========================================================================
  for ( InputMap::iterator inp = m_inputs.begin() ; 
        m_inputs.end() != inp ; ++inp ) 
  {
    if ( algorithm != inp->first->name() ) { continue ; }
    const SelMap& sels = inp->second ;
    for ( SelMap::iterator isel = sels.begin() ; sels.end() != isel; ++isel ) 
    { selections.push_back ( isel->second ) ; }
  }
  // ==========================================================================
  if ( selections.empty() ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "inputs: no inputs for algorithm '" + algorithm + 
                "', return empty list") ; }
  }
  return selections.size() ;
}
// ============================================================================
/*  get the inputs for the given algorithm 
 *  @param algorithm the algorithm (consumer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of input selections 
 */
// ============================================================================
size_t Hlt::Service::inputs 
( const std::string& algorithm  , 
  KeyList&           selections ) const 
{
  selections.clear() ;
  // =========================================================================
  for ( InputMap::iterator inp = m_inputs.begin() ; 
        m_inputs.end() != inp ; ++inp ) 
  {
    if ( algorithm != inp->first->name() ) { continue ; }
    const SelMap& sels = inp->second ;
    for ( SelMap::iterator isel = sels.begin() ; sels.end() != isel; ++isel ) 
    { selections.push_back ( isel->first ) ; }
  }
  // ==========================================================================
  if ( selections.empty() ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "inputs: no inputs for algorithm '" + algorithm + 
                "', return empty list") ; }
  }
  // ==========================================================================
  return selections.size() ;
}
// ============================================================================
/*  get the outputs for the given algorithm 
 *  @param algorithm the algorithm (producer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of output selections 
 */
// ============================================================================
size_t Hlt::Service::outputs 
( const IAlgorithm*  algorithm , 
  SelList&           selections ) const 
{
  selections.clear() ;
  if ( 0 == algorithm ) 
  {
    Error ("outputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return outputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the outputs for the given algorithm 
 *  @param algorithm the algorithm (producer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of output selections 
 */
// ============================================================================
size_t Hlt::Service::outputs 
( const IAlgorithm*  algorithm , 
  KeyList&           selections ) const 
{
  selections.clear() ;
  if ( 0 == algorithm ) 
  {
    Error ("outputs: invalid algorithm, return empty list") ;
    return 0 ;
  }
  return outputs ( algorithm->name() , selections ) ;
}
// ============================================================================
/*  get the outputs for the given algorithm 
 *  @param algorithm the algorithm (producer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of output selections 
 */
// ============================================================================
size_t Hlt::Service::outputs 
( const std::string&  algorithm , 
  SelList&            selections ) const 
{
  selections.clear() ;
  // ==========================================================================
  for ( OutputMap::iterator out = m_outputs.begin() ; 
        m_outputs.end() != out ; ++out ) 
  {
    if ( algorithm != out->first->name() ) { continue ; }
    const SelMap& sels = out->second ;
    for ( SelMap::iterator isel = sels.begin() ; sels.end() != isel; ++isel ) 
    { selections.push_back ( isel->second ) ; }
  }
  // ==========================================================================
  if ( selections.empty() ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "outputs: no outputs for algorithm '" + algorithm + 
                "', return empty list") ; }
  }
  // ===========================================================================
  return selections.size() ;
}
// ============================================================================
/*  get the outputs for the given algorithm 
 *  @param algorithm the algorithm (producer) 
 *  @param selections (OUTPUT) the list of input selections 
 *  @return number of output selections 
 */
// ============================================================================
size_t Hlt::Service::outputs 
( const std::string&  algorithm , 
  KeyList&            selections ) const 
{
  selections.clear() ;
  // ==========================================================================
  for ( OutputMap::iterator out = m_outputs.begin() ; 
        m_outputs.end() != out ; ++out ) 
  {
    if ( algorithm != out->first->name() ) { continue ; }
    const SelMap& sels = out->second ;
    for ( SelMap::iterator isel = sels.begin() ; sels.end() != isel; ++isel ) 
    { selections.push_back ( isel->first ) ; }
  }
  // ==========================================================================
  if ( selections.empty() ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "outputs: no outputs for algorithm '" + algorithm + 
                "', return empty list") ; }
  }
  // ===========================================================================
  return selections.size() ;
}
// ============================================================================
// registered algorithm?
// ============================================================================
bool Hlt::Service::hasAlgorithm ( const IAlgorithm*  alg ) const 
{
  if ( 0 == alg ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasAlgorithm: invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return
    ( m_outputs .end () != m_outputs .find ( alg ) ) || 
    ( m_inputs  .end () != m_inputs  .find ( alg ) ) ;
}
// ============================================================================
// registered algorithm?
// ============================================================================
bool Hlt::Service::hasAlgorithm ( const std::string& alg ) const 
{
  // producer?
  for ( OutputMap::iterator io = m_outputs.begin () ; m_outputs.end () != io ; ++io )
  { if ( io -> first -> name () == alg ) { return true ; } }
  // consumer? 
  for ( InputMap::iterator  ii = m_inputs.begin  () ; m_inputs.end  () != ii ; ++ii )
  { if ( ii -> first -> name () == alg ) { return true ; } }
  //
  return false ;
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasProducer  ( const IAlgorithm*  alg ) const 
{
  if ( 0 == alg ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasProducer : invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return ( m_outputs .end () != m_outputs .find ( alg ) ) ;
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasProducer  ( const std::string& alg ) const 
{
  // producer?
  for ( OutputMap::iterator io = m_outputs.begin () ; m_outputs.end () != io ; ++io )
  { if ( io -> first -> name () == alg ) { return true ; } }
  //
  return false ;
}
// ============================================================================
// registered consumer 
// ============================================================================
bool Hlt::Service::hasConsumer  ( const IAlgorithm*  alg ) const 
{
  if ( 0 == alg ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasConsumer : invalid algorithm, return false" ) ; }
    return false ;                                                    // RETURN
  }
  return ( m_inputs  .end () != m_inputs  .find ( alg ) ) ;
}
// ============================================================================
// registered producer?
// ============================================================================
bool Hlt::Service::hasConsumer  ( const std::string& alg ) const 
{
  // consumer ?
  for ( InputMap::iterator  ii = m_inputs.begin  () ; m_inputs.end  () != ii ; ++ii )
  { if ( ii -> first -> name () == alg ) { return true ; } }
  //
  return false ;
}
// ============================================================================ 
// registered selection ?
// ============================================================================
bool Hlt::Service::hasSelection ( const Hlt::Selection* selection ) const 
{
  //
  if ( 0 == selection ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) )
    { Error ( "hasSelection: invalid selection, return false" ) ; }
    return false ;                                                   // RETURN
  }
  //
  for ( SelMap::const_iterator isel = m_selections.begin() ; 
        m_selections.end() != isel ; ++isel ) 
  { if ( isel->second == selection ) { return true ; } }             // RETURN
  //
  return false ;                                                     // RETURN 
}
// ============================================================================
// write DOT-file 
// ============================================================================
StatusCode  Hlt::Service::dump ( const std::string& fname ) const 
{
  // for empty file names use std::cout 
  if ( fname.empty() ) { return dot ( std::cout ) ; }
  // try to open the file 
  std::ofstream output ( fname.c_str() , std::ios_base::trunc ) ;
  if ( !output) { return Error ( "Unable to write DOT-file '" + fname + "'") ; }
  return dot ( output ) ;
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given algorithm 
 *  @param alg the algorithm 
 *  @param fname the name of DOT-file 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dump 
( const IAlgorithm*      alg    ,
  const std::string&     fname  ) const 
{
  if ( 0 == alg ) { return Error("dot: IAlgorithm* points to NULL") ; }
  // for empty file names use std::cout 
  if ( fname.empty() ) { return dot ( alg , std::cout ) ; }
  // try to open the file 
  std::ofstream output ( fname.c_str() , std::ios_base::trunc ) ;
  if ( !output) { return Error ( "Unable to write DOT-file '" + fname + "'") ; }
  return dot ( alg , output ) ;
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given seelction
 *  @param sel the selection 
 *  @param fname the name of DOT-file 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dump 
( const Hlt::Selection*  sel ,
  const std::string&     fname  ) const 
{
  if ( 0 == sel ) { return Error("dot: Hlt::Selection* points to NULL") ; }
  // for empty file names use std::cout 
  if ( fname.empty() ) { return dot ( sel , std::cout ) ; }
  // try to open the file 
  std::ofstream output ( fname.c_str() , std::ios_base::trunc ) ;
  if ( !output) { return Error ( "Unable to write DOT-file '" + fname + "'") ; }
  return dot ( sel , output ) ;
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given object
 *  @param  obj   the object
 *  @param  stream the stream 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dump 
( const std::string&     object ,
  const std::string&     fname  ) const 
{
  // for empty file names use std::cout 
  if ( fname.empty() ) { return dot ( object , std::cout ) ; }
  // try to open the file 
  std::ofstream output ( fname.c_str() , std::ios_base::trunc ) ;
  if ( !output) { return Error ( "Unable to write DOT-file '" + fname + "'") ; }
  return dot ( object , output ) ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  inline std::string selNode ( const std::string&    sel ) 
  { return sel ; }
  inline std::string selNode ( const Hlt::Selection* sel ) 
  { return selNode ( sel->id() ) ; }
  inline std::string selLine ( const std::string& sel ) 
  { return  selNode ( sel ) + " \t[shape=ellipse,color=yellow,style=filled];"; }
  inline std::string selLine ( const Hlt::Selection* sel ) 
  { return  selNode ( sel ) + " \t[shape=ellipse,color=yellow,style=filled];"; }
  inline std::string algNode ( const IAlgorithm*     alg ) 
  { return "Alg_" + alg->name() ; }
  inline std::string algLine ( const IAlgorithm*     alg ) 
  {
    return 
      " \t" + 
      algNode ( alg ) + " \t[label=" + 
      alg->name() + ",shape=box,color=red,style=filled];" ;
  } 
  inline std::string arrow
  ( const std::string& from , 
    const std::string& to   ) 
  { return " \t" + from  + " \t -> \t " + to + " \t[shape=vee];" ; }
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given object
 *  @param  obj   the object
 *  @param  stream the stream 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dot 
( const std::string& object ,
  std::ostream&      stream ) const 
{
  if ( !stream  ) { return Error ( "dot: the stream is invalid" ) ; }
  //
  if      ( hasSelection ( object ) ) 
  {
    const Hlt::Selection* sel = m_selections[object] ;
    return dot ( sel , stream ) ;
  }
  else if ( hasAlgorithm ( object ) ) 
  {
    const IAlgorithm* alg = 0 ;
    // producer? 
    for ( OutputMap::iterator io = m_outputs.begin () ; 
          m_outputs.end () != io && 0 != alg ; ++io )
    { if ( io -> first -> name () == object ) { alg = io -> first ; } }
    // consumer? 
    for ( InputMap::iterator  ii = m_inputs.begin  () ; 
          m_inputs.end  () != ii && 0 != alg ; ++ii )
    { if ( ii -> first -> name () == object ) { alg = ii -> first ; } }
    return dot ( alg , stream ) ;
  }
  //
  return Error ( "dot: no proper object is found") ;
}
// ============================================================================
// write DOT-file 
// ============================================================================
StatusCode  Hlt::Service::dot ( std::ostream& stream ) const 
{
  if ( !stream  ) { return Error ( "dot: the stream is invalid" ) ; }
  
  stream 
    << std::endl << std::endl << std::endl 
    << "## Graph is automatically generated by '" << name() << "'";
  
  std::time_t t;
  std::time( &t );
  tm* localt = std::localtime( &t );
  
  stream << " on " << std::asctime ( localt )  << std::endl << std::endl ;

  // open line 
  stream << "digraph HLT_Service {" << std::endl ;
  
  // ==========================================================================
  // make the list of selections
  // ==========================================================================
  stream << "## Selections : " << std::endl ;  
  for  ( SelMap::const_iterator isel = m_selections.begin() ; 
         m_selections.end() != isel ; ++isel ) 
  {
    const Hlt::Selection* sel = isel->second ;
    if ( 0 == sel ) { continue ; }                                  // CONTINUE 
    stream << selLine ( sel ) << std::endl ;
  }
  stream << std::endl ;  
  // ==========================================================================
  // make the list of all known algorithms
  // ==========================================================================
  typedef std::set<const IAlgorithm*> ALGs ;
  ALGs algs ;
  for ( OutputMap::const_iterator iout = m_outputs.begin() ;
        m_outputs.end() != iout ; ++iout ) { algs.insert  ( iout -> first ) ; }
  for ( InputMap::const_iterator iin = m_inputs.begin() ;
        m_inputs.end()  != iin  ; ++iin  ) { algs.insert  ( iin  -> first ) ; }  
  // ==========================================================================
  // make the algorithms
  // ==========================================================================
  stream << "## Algorithms: " << std::endl ;  
  for ( ALGs::const_iterator ialg = algs.begin() ; 
        algs.end() != ialg ; ++ialg )
  {
    const IAlgorithm* alg = *ialg ;
    if ( 0 == alg ) { continue ; }
    stream << algLine ( alg ) << std::endl ;
  }
  // ==========================================================================
  // make connections:
  // ==========================================================================
  stream << "## OUTPUT Selections: " << std::endl ;  
  for ( OutputMap::const_iterator iout = m_outputs.begin() ;
        m_outputs.end() != iout ; ++iout ) 
  {
    const IAlgorithm* alg  = iout->first  ;
    const SelMap&     smap = iout->second ;
    for ( SelMap::const_iterator it = smap.begin() ; smap.end() != it ; ++it ) 
    {
      const Hlt::Selection* sel = it->second ;
      if ( 0 == sel ) { continue ; }                                 // CONTINUE 
      stream << arrow ( algNode ( alg ) , selNode ( sel ) ) << std::endl ;
    }
  }
  // ==========================================================================
  // make connections:
  // ==========================================================================
  stream << "## INPUT  Selections: " << std::endl ;
  for ( InputMap::const_iterator iin = m_inputs.begin() ;
        m_inputs.end() != iin ; ++iin ) 
  {
    const IAlgorithm* alg  = iin -> first  ;
    const SelMap&     smap = iin -> second ;
    for ( SelMap::const_iterator it = smap.begin() ; smap.end() != it ; ++it ) 
    {
      const Hlt::Selection* sel = it -> second ;
      if (  0 == sel ) { continue ; }                                // CONTINUE 
      stream << arrow ( selNode ( sel ) , 
                        algNode ( alg ) ) << std::endl ;
    } 
  }
  // closing line 
  stream << "}" << std::endl << std::endl << std::endl ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given algorithm 
 *  @param  alg the algorithm 
 *  @param  stream the stream 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dot 
( const IAlgorithm*      alg    ,
  std::ostream&          stream ) const 
{
  if ( 0 == alg ) 
  { return Error ( "dot: IAlgorithm* points to NULL" ) ; }
  if ( !stream  ) 
  { return Error ( "dot: the stream is invalid"      ) ; }
  if ( !hasAlgorithm ( alg ) ) 
  { return Error ( "dot: IAlgorithm* is unknown"     ) ; }

  stream 
    << std::endl << std::endl << std::endl 
    << "## Graph is automatically generated by '" << name() << "'";
  
  std::time_t t;
  std::time( &t );
  tm* localt = std::localtime( &t );
  
  stream << " on "    << std::asctime ( localt )  << std::endl << std::endl ;
  
  // open line 
  stream << "digraph " << alg->name() << " {" << std::endl ;

  
  stream << "## Algorithm:" ;
  stream << algLine ( alg ) << std::endl ;

  KeyList in  ;
  KeyList out ;
  inputs  ( alg , in  ) ;
  outputs ( alg , out ) ;
  
  // ==========================================================================
  stream << "## Selections: " << std::endl ;  
  for ( KeyList::const_iterator ii =  in.begin() ;  in.end() != ii ; ++ii ) 
  { stream <<  selLine ( *ii ) << std::endl ; }  
  for ( KeyList::const_iterator io = out.begin() ; out.end() != io ; ++io ) 
  { stream <<  selLine ( *io ) << std::endl ; }
  // ==========================================================================
  
  stream << "## INPUT  Selections: " << std::endl ;  
  for ( KeyList::const_iterator ii =  in.begin() ;  in.end() != ii ; ++ii ) 
  { stream << arrow ( selNode ( *ii ) , algNode ( alg ) ) << std::endl ; }  
  // ==========================================================================
  stream << "## OUTPUT Selections: " << std::endl ;  
  for ( KeyList::const_iterator io = out.begin() ; out.end() != io ; ++io ) 
  { stream << arrow ( algNode ( alg ) , selNode ( *io ) ) << std::endl ; }  
  // ==========================================================================
  
  // closing line 
  stream << "}" << std::endl << std::endl << std::endl ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  produce "DOT"-file with the given name for the given selection
 *  @param  sel the selection  
 *  @param  stream the stream 
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::dot 
( const Hlt::Selection*  sel    ,
  std::ostream&          stream ) const 
{
  if ( 0 == sel ) 
  { return Error ( "dot: Hlt::Selection* points to NULL" ) ; }
  if ( !stream  ) 
  { return Error ( "dot: the stream is invalid"          ) ; }
  if ( !hasSelection ( sel ) ) 
  { return Error ( "dot: Hlt::Selection* is unknown"     ) ; }

  stream 
    << std::endl << std::endl << std::endl 
    << "## Graph is automatically generated by '" << name() << "'";
  
  std::time_t t;
  std::time( &t );
  tm* localt = std::localtime( &t );
  
  stream << " on "    << std::asctime ( localt )  << std::endl << std::endl ;
  
  const std::string sname = sel->id() ;
  
  // open line 
  stream << "digraph " << sname << " {" << std::endl ;
  
  stream << "## Selection:" << std::endl ;
  stream << selLine ( sel ) << std::endl ;
  
  
  // producer:
  const IAlgorithm* pro = producer ( sel ) ;
  // consumers:
  AlgList con ;
  
  // ==========================================================================
  stream << "## Algorithms: " << std::endl ;  
  if ( 0 != pro ) 
  { stream << algLine ( pro ) << std::endl ; }
  for ( AlgList::const_iterator ic =  con.begin() ;  con.end() != ic ; ++ic ) 
  { stream << algLine ( *ic ) << std::endl ; }
  // ==========================================================================
  
  if  ( 0 != pro ) 
  {
    stream << "## PRODUCER : " << std::endl ;
    stream << arrow ( algNode ( pro ) , selNode ( sel ) ) << std::endl ;
  }
  // ==========================================================================
  stream << "## CONSUMERS :  " << std::endl ;  
  for ( AlgList::const_iterator ic =  con.begin() ;  con.end() != ic ; ++ic ) 
  { stream << arrow ( selNode ( sel ) , algNode ( *ic ) ) << std::endl ; }
  // ==========================================================================
  // closing line 
  stream << "}" << std::endl << std::endl << std::endl ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
namespace 
{
  // ==========================================================================
  struct _AlgCmp 
    : public std::binary_function<const IAlgorithm*,const IAlgorithm*,bool>
  {
    inline bool operator () 
      ( const IAlgorithm* alg1 , 
        const IAlgorithm* alg2 ) const 
    {
      return 
        alg1 == alg2 ? false : 
        0    == alg1 ? true  :
        0    == alg2 ? false : alg1->name() < alg2->name() ;
    }
  };
  // ==========================================================================
}
// ============================================================================
/*  get all algorithms 
 *  @param algs (OUTPUT) the list of algorithms 
 *  @return number of algorithms 
 */
// ============================================================================
size_t Hlt::Service::algorithms ( Hlt::IInspector::AlgList& algs ) const 
{
  algs.clear() ;
  std::set<const IAlgorithm*,_AlgCmp> myalgs ;
  for  ( InputMap::const_iterator ii = m_inputs .begin() ; 
         m_inputs .end() != ii ; ++ii ) { myalgs.insert ( ii -> first ) ; }
  for  ( InputMap::const_iterator io = m_outputs.begin() ; 
         m_outputs.end() != io ; ++io ) { myalgs.insert ( io -> first ) ; }
  // sor
  algs.insert ( algs.end() , myalgs.begin() , myalgs.end() ) ;
  //
  return algs.size () ;
}
// ============================================================================
/*  get all selections 
 *  @param keys (OUTPUT) the list of selections 
 *  @return number of selections  
 */
// ============================================================================
size_t Hlt::Service::selections ( Hlt::IInspector::KeyList& keys ) const 
{
  keys.clear() ;
  for ( SelMap::const_iterator ikey = m_selections.begin() ; 
        m_selections.end () != ikey ; ++ikey ) { keys.push_back ( ikey->first ) ; }
  //
  std::sort( keys.begin() , keys.end() ) ;
  //
  return keys.size() ;
}
// ============================================================================
// The END 
// ============================================================================
