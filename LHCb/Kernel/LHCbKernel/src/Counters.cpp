// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/StatEntity.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/Counters.h"
// ============================================================================
/** @file
 *  Implementaton for simpel TES-counters
 *
 *  @see Gaudi::Counter
 *  @see Gaudi::Counters
 *  @see Gaudi::Numbers 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2010-11-09
 *  
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 *
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var CLID_Counter 
   *  The uinque indentifier/CLID for class Gaudi::Counter 
   *  @see Gaudi::Counter 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2010-11-09
   */  
  const CLID CLID_Counter   = 150 ;
  // ==========================================================================
  /** @var CLID_Counters 
   *  The uinque indentifier/CLID for class Gaudi::Counters 
   *  @see Gaudi::Counters 
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2010-11-09
   */  
  const CLID CLID_Counters  = 151 ;
  // ==========================================================================
  /** @var CLID_Numbers 
   *  The uinque indentifier/CLID for class Gaudi::Numbers
   *  @see Gaudi::Numbers
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2010-11-09
   */  
  const CLID CLID_Numbers    = 152 ;
  // ==========================================================================
}
// ============================================================================
// constructor from counter
// ============================================================================
Gaudi::Counter::Counter
( const StatEntity&  counter     , 
  const std::string& description ) 
  : DataObject    (             ) 
  , m_counter     ( counter     ) 
  , m_description ( description ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
Gaudi::Counter::~Counter(){}
// ============================================================================
// retrieve the unique object identifier  (virtual) 
// ============================================================================
const CLID& Gaudi::Counter::clID() const { return Gaudi::Counter::classID() ; }
// ============================================================================
// retrieve the unique object identifier  (static) 
// ============================================================================
const CLID& Gaudi::Counter::classID() { return CLID_Counter ; }
// ============================================================================
// printout  to std::ostream
// ============================================================================
std::ostream& Gaudi::Counter::fillStream ( std::ostream& o ) const 
{ return o ; }
// ============================================================================


// ============================================================================
// constructor from counters
// ============================================================================
Gaudi::Counters::Counters
( const Gaudi::Counters::Map&  counters ) 
  : DataObject (          ) 
  , m_counters ( counters ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
Gaudi::Counters::~Counters(){}
// ============================================================================
// retrieve the unique object identifier  (virtual) 
// ============================================================================
const CLID& Gaudi::Counters::clID() const { return Gaudi::Counters::classID() ; }
// ============================================================================
// retrieve the unique object identifier  (static) 
// ============================================================================
const CLID& Gaudi::Counters::classID() { return CLID_Counters ; }
// ============================================================================
// printout  to std::ostream
// ============================================================================
std::ostream& Gaudi::Counters::fillStream ( std::ostream& o ) const 
{ return o ; }

// ============================================================================
// constructor from counters
// ============================================================================
Gaudi::Numbers::Numbers
( const Gaudi::Numbers::Map&  counters ) 
  : DataObject (          ) 
  , m_counters ( counters ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
Gaudi::Numbers::~Numbers(){}
// ============================================================================
// retrieve the unique object identifier  (virtual) 
// ============================================================================
const CLID& Gaudi::Numbers::clID() const { return Gaudi::Counters::classID() ; }
// ============================================================================
// retrieve the unique object identifier  (static) 
// ============================================================================
const CLID& Gaudi::Numbers::classID() { return CLID_Numbers ; }
// ============================================================================
// printout  to std::ostream
// ============================================================================
std::ostream& Gaudi::Numbers::fillStream ( std::ostream& o ) const 
{ return o ; }





// ============================================================================
// The END 
// ============================================================================

