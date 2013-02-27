// $Id$
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "Service.h"
// ============================================================================
/** @file 
 *  Implementation file for class Hlt::Service
 *  The methods from Hlt::IRegister are implemented here 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-18
 */
// ============================================================================
/*  start the transaction/lock the registrator 
 *  @param alg the algorithm which starts the transaction
 *  @param status code
 */
// ============================================================================
StatusCode Hlt::Service::lock   ( const IAlgorithm* alg ) 
{
  // invalid algoritm ? 
  if ( 0 == alg      ) 
  { return Error ( "lock : IAlgorithm* points to NULL"  , 
                   Lock_Invalid_Algorithm     )  ; }                 // RETURN 
  // all transactions are disabled ?
  else if ( m_frozen      ) 
  { return Error ( "lock : all transactions are frozen" , 
                   Lock_Transactions_Disabled )  ; }                 // RETURN 
  else if ( !m_lockers.empty() && m_lockers.back() == alg ) 
  {  
    return Error ( "lock : the service is already locked'" , 
                   Lock_Invalid_Lock          )  ; }                 // RETURN
  //else if ( !(!m_locker)  ) 
  //{ return Error ( "lock : the service is already locked by '"  + 
  //                 m_locker->name() + "'" , 
  //                 Lock_Invalid_Lock          )  ; }                 // RETURN 
  // the algorithm has already been locked previously 
  else if ( m_locked.end()  
            != std::find ( m_locked.begin() , m_locked.end() , alg ) )    
  { return Error ( "lock : illegal attempt to re-register for '"  + 
                   alg->name() + "'" , 
                   Lock_Double_Registration    )  ; }                // RETURN 
  // ==========================================================================
  // lock it! 
  m_lockers.push_back ( alg ) ;
  // ==========================================================================
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  end the transaction/unlock the registrator 
 *  @param alg the algorithm which ends the transaction
 *  @param status code
 */
// ============================================================================
StatusCode Hlt::Service::unlock ( const IAlgorithm* alg ) 
{
  // invalid algoritm ? 
  if      ( 0 == alg       ) 
  { return Error ( "unlock : IAlgorithm* points to NULL" , 
                   Lock_Invalid_Algorithm     )  ; }                 // RETURN 
  else if ( m_lockers.empty()  ) 
  { return Error ( "unlock : lock/unlock mismatch "      , 
                   Lock_Invalid_Lock          )  ; }                 // RETURN 
  else if ( m_lockers.back() != alg  ) 
  { return Error ( "unlock : lock/unlock mismatch "      , 
                   Lock_Invalid_Lock          )  ; }                 // RETURN 
  // ==========================================================================
  // lock it! 
  m_lockers.pop_back () ;
  m_locked.push_back ( alg ) ;
  // ==========================================================================
  return StatusCode::SUCCESS ;  
}

// ============================================================================
/*  register the output selection during the allowed transaction for 
 *  the locked service 
 *  @param sel the selection to be registered 
 *  @param alg the algorithm/producer
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::registerOutput 
( Hlt::Selection*         selection ,               // the output selection 
  const IAlgorithm*       producer  )               //             producer 
{
  // ==========================================================================
  if      ( 0 == producer  ) 
  { return Error ( "registerOutput: invald producer" , 
                   Register_Invalid_Producer          ) ; }           // RETURN
  else if ( 0 == selection ) 
  { return Error ( "registerOutput: invalid seelction for producer '" + 
                   producer->name() + "'" , 
                   Register_Invalid_Selection         ) ;  }          // RETURN  
  else if ( m_lockers.empty()  ) 
  { return Error ( "registerOutput: the service is not locked" ,   
                   Register_Invalid_Lock              ) ;  }          // RETURN
  else if ( m_lockers.back () != producer ) 
  { return Error ( "registerOutput: the service is locked by '" 
                   + m_lockers.back()->name() + "'"   , 
                   Register_Invalid_Lock              ) ;  }          // RETURN
  // ==========================================================================
  { // check for the double registration 
    if      ( inMap ( selection -> id()       ) )
    { return Error ( "registerOutput: the selection with ID '" 
                     + selection->id().str() + 
                     "' is already registered (1) " ,
                     Register_Double_Registration ) ; }                // RETURN
    else if ( inMap ( selection  ) ) 
    { return Error ( "registerOutput: the selection '" 
                     + selection->id().str() +
                     "' is already registered (2) " ,
                     Register_Double_Registration ) ; }              // RETURN
  }
  // ==========================================================================
  // finally insert it!
  OutputMap::iterator ifind = m_outputs.find ( producer ) ;
  if ( m_outputs.end() == ifind ) 
  {
    m_outputs.insert ( producer , SelMap() ) ;
    ifind = m_outputs.find ( producer ) ;
  }
  // insert it! 
  SelMap sels = ifind->second ;
  sels.insert ( selection -> id()       , selection ) ;
  m_outputs.update ( producer , sels ) ;
  // register it also in global map:
  m_selections.insert ( selection -> id ()       , selection ) ;
  //
  debug() << "Register OUTPUT" 
          << " selection '" << std::string( selection -> id () ) << "'" 
          << " for algorithm '"  << producer  -> name () << "'" << endmsg ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  register the input selection  dirung the allowed transactions for 
 *  locked service 
 *  @param sel the selection to be registered 
 *  @param alg the algorithm/consumer
 *  @return status code 
 */
// ============================================================================
StatusCode Hlt::Service::registerInput 
( const Key&              selection ,                // the input selection 
  const IAlgorithm*       consumer  )               //            consumer 
{
  // ==========================================================================
  if      ( 0 == consumer  ) 
  { return Error ( "registerInput: invalid consumer"  , 
                   Register_Invalid_Consumer          ) ; }           // RETURN
  else if ( m_lockers.empty()  ) 
  { return Error ( "registerInput: the service is not locked" ,   
                   Register_Invalid_Lock              ) ;  }          // RETURN
  else if ( m_lockers.back() != consumer ) 
  { return Error ( "registerInput: the service is locked by '" 
                   + m_lockers.back()->name() + "'"   , 
                   Register_Invalid_Lock              ) ;  }          // RETURN
  // ==========================================================================
  // check the existence of the selection:
  if ( !inMap ( selection ) ) 
  { return Error ( "registerInput: the selection '" 
                   + selection.str() + "' is unknown " ,
                   Register_Unknown_Selection ) ; }             // RETURN
  // ==========================================================================
  //   { // check that it is not in the list of own output selection 
  //     OutputMap::iterator iout = m_outputs.find ( consumer ) ;
  //     if ( m_outputs.end() != iout ) 
  //     {
  //       SelMap::iterator i = iout->second.find ( selection ) ;
  //       if ( iout->second.end() != i ) 
  //       { return Error ( "registerInput: the input selection '" 
  //                        + selection + 
  //                        "' is output selection "   ,
  //                        Register_Invalid_Selection ) ; }             // RETURN
  //     }
  //   }
  { // check it if is not in the list of own input selections 
    InputMap::iterator iin = m_inputs.find ( consumer ) ;
    if ( m_inputs.end() == iin ) 
    {
      m_inputs.insert ( consumer , SelMap() );
      iin = m_inputs.find ( consumer ) ;
    }
    SelMap::iterator i = iin->second.find ( selection ) ;
    if ( iin->second.end() != i && m_pedantic ) 
    { Warning ( "registerInput: the input selection '" 
                + selection.str() + 
                "' is already input selection " ,
                Register_Double_Registration ) ; }
    else 
    {
      // insert it !
      SelMap inp = iin->second ;
      Hlt::Selection* sel = m_selections[ selection ] ;
      inp.update ( selection , sel ) ;
      m_inputs.update ( consumer , inp ) ;
    } 
  }
  // debug printout here 
  debug() << "Register  INPUT " 
          << " selection '"      << std::string(selection) << "'" 
          << " for algorithm '"  << consumer  -> name ()   << "'" << endmsg ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  register the requies to TES-selection 
 *  @attention the service much be locked properly during the transaction!
 *  @param location TES location to be registered
 *  @param consumer algorithm/consumer 
 *  @return Status Code 
 */
// ============================================================================
StatusCode Hlt::Service::registerTESInput
( const Key&              location  ,                 //        TES location 
  const IAlgorithm*       consumer  )                 //            consumer   
{
  // ==========================================================================
  if      ( 0 == consumer     ) 
  { return Error ( "registerTESInput: invalid consumer"  , 
                   Register_Invalid_Consumer          ) ; }           // RETURN
  else if ( m_lockers.empty() ) 
  { return Error ( "registerTESInput: the service is not locked" ,   
                   Register_Invalid_Lock              ) ;  }          // RETURN
  else if ( m_lockers.back () != consumer ) 
  { return Error ( "registerTESInput: the service is locked by '" 
                   + m_lockers.back()->name() + "'" , 
                   Register_Invalid_Lock              ) ;  }          // RETURN
  //
  TESMap::const_iterator intes = m_tesmap.find ( consumer ) ;
  if ( m_tesmap.end() == intes ) 
  {
    m_tesmap.insert ( consumer , TESLocs() ) ;
    intes = m_tesmap.find ( consumer ) ;
  }
  TESLocs::const_iterator iin = intes->second.find ( location ) ;
  if ( intes->second.end() != iin )
  { Warning ( "registerTESInput: the input location '" 
              + location.str() + 
              "' is already input location " ,
              Register_Double_Registration ) ; }
  else 
  {
    // Insert it!!!
    TESLocs locs = intes->second ;
    locs.insert ( location ) ;
    m_tesmap.update ( consumer , locs ) ;
  }
  // debug printout here 
  if (msgLevel(MSG::DEBUG)) debug() << "Register  INPUT-TES " 
                                    << " location '"       << std::string(location)  << "'" 
                                    << " for algorithm '"  << consumer  -> name ()   << "'" << endmsg ;
  //
  return StatusCode::SUCCESS ;                                        // RETURN 
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
