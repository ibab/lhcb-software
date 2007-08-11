// $Id: GenAlgsDicts.cpp,v 1.3 2007-08-11 20:18:13 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenAlgs.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenAlgsDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for namespace LoKi::Dicts::GenAlgs 
 *  @date 2007-06-03 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
// count the particles in the event 
// ============================================================================
std::size_t 
LoKi::Dicts::GenAlgs::count_if 
( const HepMC::GenEvent*             event , 
  const LoKi::GenTypes::GCuts&       cuts  ) 
{ return LoKi::GenAlgs::count_if ( event , cuts ) ; }
// ============================================================================
// count the particles in the event 
// ============================================================================
std::size_t 
LoKi::Dicts::GenAlgs::count_if 
( const LHCb::HepMCEvent*            event , 
  const LoKi::GenTypes::GCuts&       cuts  ) 
{ return LoKi::GenAlgs::count_if ( event , cuts ) ; }
// ============================================================================
// count the particels in the event 
// ============================================================================
std::size_t 
LoKi::Dicts::GenAlgs::count_if 
( const LHCb::HepMCEvent::Container* event , 
  const LoKi::GenTypes::GCuts&       cuts  ) 
{ return LoKi::GenAlgs::count_if ( event , cuts ) ; }
// ============================================================================
// count the particles in the tree 
// ============================================================================
std::size_t 
LoKi::Dicts::GenAlgs::count_if 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GCuts& cuts     ) 
{ return LoKi::GenAlgs::count_if ( particle , cuts ) ; }
// ============================================================================
// count the particles in the tree 
// ============================================================================
std::size_t
LoKi::Dicts::GenAlgs::count_if 
( const HepMC::GenVertex*      vertex , 
  const LoKi::GenTypes::GCuts& cuts   , 
  HepMC::IteratorRange         range  )
{ return LoKi::GenAlgs::count_if ( vertex ,  cuts , range ) ; }
// ============================================================================
//  check the presence in the event 
// ============================================================================
bool LoKi::Dicts::GenAlgs::found 
( const HepMC::GenEvent*       event , 
  const LoKi::GenTypes::GCuts& cuts  ) 
{ return LoKi::GenAlgs::found ( event , cuts ) ; }  
// ============================================================================
//  check the presence in the event 
// ============================================================================
bool LoKi::Dicts::GenAlgs::found 
( const LHCb::HepMCEvent*      event , 
  const LoKi::GenTypes::GCuts& cuts  ) 
{ return LoKi::GenAlgs::found ( event , cuts ) ; }  
// ============================================================================
//  check the presence in the event 
// ============================================================================
bool LoKi::Dicts::GenAlgs::found 
( const LHCb::HepMCEvent::Container* event , 
  const LoKi::GenTypes::GCuts& cuts  ) 
{ return LoKi::GenAlgs::found ( event , cuts ) ; }  
// ============================================================================
//  check the presence in the tree 
// ============================================================================
bool LoKi::Dicts::GenAlgs::found 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GCuts& cuts     ) 
{ return LoKi::GenAlgs::found ( particle , cuts ) ; }  
// ============================================================================
//  check the presence in the tree 
// ============================================================================
bool LoKi::Dicts::GenAlgs::found 
( const HepMC::GenVertex*      vertex  , 
  const LoKi::GenTypes::GCuts& cuts    , 
  HepMC::IteratorRange         range   )             
{ return LoKi::GenAlgs::found ( vertex , cuts , range ) ; }  
// ============================================================================
// accumulate through the addition 
// ============================================================================
double LoKi::Dicts::GenAlgs::accumulate
( const HepMC::GenEvent*       event ,
  const LoKi::GenTypes::GFunc& fun   ,
  const LoKi::GenTypes::GCuts& cut   ,
  double                       res   ) 
{
  return LoKi::GenAlgs::accumulate 
    ( event , fun , cut , res , std::plus<double>() ) ;
}
// ============================================================================
// accumulate through the addition 
// ============================================================================
double LoKi::Dicts::GenAlgs::accumulate
( const LHCb::HepMCEvent*      event ,
  const LoKi::GenTypes::GFunc& fun   ,
  const LoKi::GenTypes::GCuts& cut   ,
  double                       res   ) 
{
  return LoKi::GenAlgs::accumulate 
    ( event , fun , cut , res , std::plus<double>() ) ;
}
// ============================================================================
// accumulate through the addition 
// ============================================================================
double LoKi::Dicts::GenAlgs::accumulate
( const LHCb::HepMCEvent::Container* event ,
  const LoKi::GenTypes::GFunc&       fun   ,
  const LoKi::GenTypes::GCuts&       cut   ,
  double                             res   ) 
{
  return LoKi::GenAlgs::accumulate 
    ( event , fun , cut , res , std::plus<double>() ) ;
}
// ============================================================================
// accumulate through the addition 
// ============================================================================
double LoKi::Dicts::GenAlgs::accumulate
( const HepMC::GenVertex*      vertex ,
  const LoKi::GenTypes::GFunc& fun    ,
  const LoKi::GenTypes::GCuts& cut    ,
  HepMC::IteratorRange         range  , 
  double                       res    ) 
{
  return LoKi::GenAlgs::accumulate 
    ( vertex , fun , cut , res , range , std::plus<double>() ) ;
}
// ============================================================================
// accumulate through the addition 
// ============================================================================
double LoKi::Dicts::GenAlgs::accumulate
( const HepMC::GenParticle*    particle ,
  const LoKi::GenTypes::GFunc& fun      ,
  const LoKi::GenTypes::GCuts& cut      ,
  const double                 res      ) 
{
  return LoKi::GenAlgs::accumulate 
    ( particle , fun , cut , res , std::plus<double>() ) ;
}
// ============================================================================
// find minimal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::min_value 
( const HepMC::GenEvent*       event    , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::min_value ( event , fun , cut , res ) ; }
// ============================================================================
// find minimal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::min_value 
( const LHCb::HepMCEvent*      event    , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::min_value ( event , fun , cut , res ) ; }
// ============================================================================
// find minimal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::min_value 
( const LHCb::HepMCEvent::Container* event    , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::min_value ( event , fun , cut , res ) ; }
// ============================================================================
// find minimal value over the tree:
// ============================================================================
double LoKi::Dicts::GenAlgs::min_value 
( const HepMC::GenVertex*      vertex   , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  HepMC::IteratorRange         range    , 
  double                       res      ) 
{ return LoKi::GenAlgs::min_value ( vertex , fun , cut , range , res ) ; }
// ============================================================================
// find minimal value over the tree:
// ============================================================================
double LoKi::Dicts::GenAlgs::min_value 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::min_value ( particle , fun , cut , res ) ; }
// ============================================================================
// find the maximal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::max_value 
( const HepMC::GenEvent*       event    , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::max_value ( event , fun , cut , res ) ; }
// ============================================================================
// find the maximal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::max_value 
( const LHCb::HepMCEvent*      event    , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::max_value ( event , fun , cut , res ) ; }
// ============================================================================
// find the maximal value over the event
// ============================================================================
double LoKi::Dicts::GenAlgs::max_value 
( const LHCb::HepMCEvent::Container* event , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::max_value ( event , fun , cut , res ) ; }
// ============================================================================
// find the maximal value over the tree 
// ============================================================================
double LoKi::Dicts::GenAlgs::max_value 
( const HepMC::GenParticle*    particle , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  double                       res      ) 
{ return LoKi::GenAlgs::max_value ( particle , fun , cut , res ) ; }
// ============================================================================
// find the maximal value over the tree:
// ============================================================================
double LoKi::Dicts::GenAlgs::max_value 
( const HepMC::GenVertex*      vertex   , 
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      , 
  HepMC::IteratorRange         range    , 
  double                       res      ) 
{ return LoKi::GenAlgs::max_value ( vertex , fun , cut , range , res ) ; }

// ============================================================================
// find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::min_element 
( const HepMC::GenEvent*       event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   )
{ return LoKi::GenAlgs::min_element ( event , fun , cut ) ; }
// ============================================================================
// find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::min_element 
( const LHCb::HepMCEvent*      event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   ) 
{ return LoKi::GenAlgs::min_element ( event , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::min_element 
( const LHCb::HepMCEvent::Container* event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   ) 
{ return LoKi::GenAlgs::min_element ( event , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the tree
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::min_element 
( const HepMC::GenParticle*    particle ,
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      ) 
{ return LoKi::GenAlgs::min_element ( particle , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the treec
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::min_element 
( const HepMC::GenVertex*      vertex ,
  const LoKi::GenTypes::GFunc& fun    ,
  const LoKi::GenTypes::GCuts& cut    ,
  HepMC::IteratorRange         range  ) 
{ return LoKi::GenAlgs::min_element ( vertex  , fun , cut , range ) ; }


// ============================================================================
// find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::max_element 
( const HepMC::GenEvent*       event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   )
{ return LoKi::GenAlgs::max_element ( event , fun , cut ) ; }
// ============================================================================
// find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::max_element 
( const LHCb::HepMCEvent*      event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   ) 
{ return LoKi::GenAlgs::max_element ( event , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the event 
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::max_element 
( const LHCb::HepMCEvent::Container* event ,
  const LoKi::GenTypes::GFunc& fun   , 
  const LoKi::GenTypes::GCuts& cut   ) 
{ return LoKi::GenAlgs::max_element ( event , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the tree
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::max_element 
( const HepMC::GenParticle*    particle ,
  const LoKi::GenTypes::GFunc& fun      , 
  const LoKi::GenTypes::GCuts& cut      ) 
{ return LoKi::GenAlgs::max_element ( particle , fun , cut ) ; }
// ============================================================================
//  find the minimum element through the treec
// ============================================================================
const HepMC::GenParticle* 
LoKi::Dicts::GenAlgs::max_element 
( const HepMC::GenVertex*      vertex ,
  const LoKi::GenTypes::GFunc& fun    ,
  const LoKi::GenTypes::GCuts& cut    ,
  HepMC::IteratorRange         range  ) 
{ return LoKi::GenAlgs::max_element ( vertex  , fun , cut , range ) ; }

// ============================================================================
// The END 
// ============================================================================
