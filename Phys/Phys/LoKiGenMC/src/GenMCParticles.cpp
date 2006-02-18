// $Id: GenMCParticles.cpp,v 1.2 2006-02-18 18:15:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/01/26 17:35:17  ibelyaev
// New import: Generator/MC related part of LoKi project 
// 
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
// ============================================================================
// KErnel
// ============================================================================
#include "Kernel/HepMC2MC.h"
// ============================================================================
// HepMC 
// ============================================================================
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
// ============================================================================
#include "LoKi/GenExtract.h"
#include "LoKi/GenMCParticles.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  Implementation file for the functions from the file LoKi/GenMCParticles.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** constructor from the particle and relation table 
 *  @param mc   pointer to MC particle 
 *  @param table relation table  
 */
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::IsAMotherForMC 
( const LHCb::MCParticle* mc   , 
  const LHCb::HepMC2MC*   table ) 
  : LoKi::Predicate<const HepMC::GenParticle*> ()
  , m_mcps  ( 1 , mc ) 
  , m_table ( table  )
{ getMC()  ; }
// ============================================================================

// ============================================================================
/** constructor from the particles and relation table 
 *  @param mc   range of MC particles 
 *  @param table relation table  
 */
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::IsAMotherForMC 
( const LoKi::MCTypes::MCRange& mc   , 
  const LHCb::HepMC2MC*         table ) 
  : LoKi::Predicate<const HepMC::GenParticle*> ()
  , m_mcps  ( mc.begin() , mc.end() ) 
  , m_table ( table  )
{ getMC()  ; }
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::IsAMotherForMC 
( const LoKi::GenParticles::IsAMotherForMC& right )
  : LoKi::Predicate<const HepMC::GenParticle*> ( right )
  , m_mcps  ( right.m_mcps  ) 
  , m_table ( right.m_table )
{}  
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::~IsAMotherForMC(){}
// ============================================================================

// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::IsAMotherForMC*
LoKi::GenParticles::IsAMotherForMC::clone() const
{ return new LoKi::GenParticles::IsAMotherForMC(*this) ; }
// ============================================================================

// ============================================================================
LoKi::GenParticles::IsAMotherForMC::result_type 
LoKi::GenParticles::IsAMotherForMC::operator() 
  ( LoKi::GenParticles::IsAMotherForMC::argument p ) const 
{
  //
  if ( 0 == p ) 
  {
    Error ( " HepMC::GenParticle* points to NULL, return 'false' " ) ;
    return false ;                                                  // RETURN 
  }
  //
  if ( 0 == m_table ) 
  {
    Error ( " HepMC2MC* table  points to NULL, return 'false' " ) ;
    return false ;                                                  // RETURN 
  }
  if ( m_mcps.empty() ) 
  {
    Warning ( " MCPs container is empty! return 'false'") ;
    return false ;                                                  // RETURN 
  }
  //   // get all descendent 
  typedef std::vector<const HepMC::GenParticle*> GPs ;
  GPs gps ( 1 , p ) ;
  HepMC::GenVertex* vertex = p->end_vertex() ;
  if ( 0 != vertex ) 
  {
    gps.insert ( gps.end() , 
                 vertex -> particles_begin ( HepMC::descendants ) ,
                 vertex -> particles_end   ( HepMC::descendants ) ) ;
  }
  // loop over all descendants 
  for ( GPs::const_iterator igp = gps.begin() ; 
        gps.end() != igp ; ++igp ) 
  {
    const HepMC::GenParticle* gp = *igp ;
    if ( 0 == gp       ) { continue ; }
    // get all links 
    LHCb::HepMC2MC::Range links = m_table->relations ( gp ) ;
    if ( links.empty() ) { continue ; }
    // loop over all links 
    for ( LHCb::HepMC2MC::iterator ilink = links.begin() ; 
          links.end() != ilink ; ++ilink ) 
    {
      const LHCb::MCParticle* mc = ilink->to() ;
      if ( 0 == mc ) { continue ; }
      const bool found = 
        std::binary_search ( m_mcps.begin () , m_mcps.end   () , mc ) ;
      // found ?
      if ( found )          { return true ; }                   // RETURN
    } 
  }
  //
  return false ;  
};
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::IsAMotherForMC::fillStream
( std::ostream& s ) const { return s << "GMCMOTH" ; }
// ============================================================================

// ============================================================================
// prepare an appropriate MC information
// ============================================================================
StatusCode LoKi::GenParticles::IsAMotherForMC::getMC() 
{
  MCPs _tmp ( m_mcps ) ;
  if ( _tmp.empty() ) 
  { return Warning ( "'getMC()': container is empty") ; }
  //
  m_mcps.clear() ;
  for ( MCPs::const_iterator imc = _tmp.begin() ; 
        _tmp.end() != imc ; ++imc ) 
  {
    const LHCb::MCParticle* mc = *imc ;
    while ( 0 != mc ) 
    {
      m_mcps.push_back( mc ) ;
      mc = mc->mother() ;
    }
  }
  // sort and eliminate duplicates
  std::sort ( m_mcps.begin() , m_mcps.end() ) ;
  MCPs::iterator iend = std::unique ( m_mcps.begin() , m_mcps.end() ) ;
  if ( m_mcps.end() != iend ) 
  { m_mcps.erase ( iend , m_mcps.end() ) ; }
  // final check
  if ( m_mcps.empty() ) 
  { return Warning ( "'getMC()': container is empty") ; }
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** constructor from particle and a table 
 *  @param p pointer to the particle
 *  @param table pointer to table 
 */
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const HepMC::GenParticle* p     , 
  const LHCb::MC2HepMC*     table ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   ( 1 , p ) 
  , m_table ( table ) 
{ getHepMC() ; }
// ============================================================================

// ============================================================================
/** constructor from particle and a table 
 *  @param vertex pointer to the vertex 
 *  @param table  pointer to table 
 */
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const HepMC::GenVertex* vertex , 
  const LHCb::MC2HepMC*   table  ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (       ) 
  , m_table ( table ) 
{
  if ( 0 != vertex ) 
  {
    HepMC::GenVertex* v = const_cast<HepMC::GenVertex*>( vertex ) ;
    if ( 0 != v ) 
    {
      m_gps.insert
        (  m_gps.end() , 
           v -> particles_begin ( HepMC::descendants ) , 
           v -> particles_end   ( HepMC::descendants ) ) ;
    }
  }
  //
  getHepMC() ; 
} ;
// ============================================================================

// ============================================================================
/** copy constructor
 *  @param rigth object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::MCParticles::IsFromHepMC& right ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ( right ) 
  , m_gps   ( right.m_gps   ) 
  , m_table ( right.m_table ) 
{}
// ============================================================================

// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::~IsFromHepMC(){}
// ============================================================================

// ============================================================================
/// MANDATORY: clone method("virtual constructor")
// ============================================================================
LoKi::MCParticles::IsFromHepMC*
LoKi::MCParticles::IsFromHepMC::clone() const 
{ return new LoKi::MCParticles::IsFromHepMC(*this) ; }
// ============================================================================

// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::IsFromHepMC::fillStream
( std::ostream& s ) const { return s << "FROMGTREE" ; }
// ============================================================================

// ============================================================================
/// MANDATORY: the only one essential method 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::result_type
LoKi::MCParticles::IsFromHepMC::operator() 
  ( LoKi::MCParticles::IsFromHepMC::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error    ( "MCParticle* points to NULL, return 'false'" ) ;
    return false ;
  }
  //
  if ( 0 == m_table ) 
  {
    Error    ( "Table* points to NULL, return 'false'"      ) ;
    return false ;
  }
  if ( m_gps.empty() ) 
  {
    Warning  ( "GPs container is empty, return 'false'"     ) ;
    return false ;
  }
  //
  typedef std::vector<const LHCb::MCParticle*> MCPs ;
  MCPs mcps ( 1 , p ) ;
  // get all "parents" 
  const LHCb::MCParticle* mother = p->mother() ;
  while ( 0 != mother ) 
  {
    mcps.push_back( mother ) ;
    mother = mother->mother() ;
  }
  // loop over all parents in reverse order 
  for ( MCPs::reverse_iterator imc = mcps.rbegin() ; 
        mcps.rend() != imc ; ++imc ) 
  {
    const LHCb::MCParticle* mc = *imc ;
    if ( 0 == mc       ) { continue ; }                         // CONTINUE 
    // get all links
    LHCb::MC2HepMC::Range links = m_table->relations ( mc ) ;
    if ( links.empty() ) { continue ; }                         // CONTINUE 
    // loop over all links 
    for ( LHCb::MC2HepMC::iterator ilink = links.begin() ; 
          links.end() != ilink ; ++ilink ) 
    {
      const HepMC::GenParticle* gp = ilink->to() ;
      if ( 0 == gp ) { continue ; }                  // CONTINUE 
      const bool found = 
        std::binary_search ( m_gps.begin() ,  m_gps.end  () , gp ) ;
      // found ?
      if ( found )          { return true ; }                   // RETURN
    }
  }
  //
  return false ;
  //
};
// ============================================================================

// ============================================================================
StatusCode LoKi::MCParticles::IsFromHepMC::getHepMC() 
{
  GPs _tmp ( m_gps ) ;
  if ( _tmp.empty() ) { return Warning ( "GPs container is empty!" ) ; }
  //
  m_gps.clear() ;
  for ( GPs::const_iterator ip = _tmp.begin() ; 
        _tmp.end() != ip ; ++ip )
  {
    const HepMC::GenParticle* p      = *ip ;
    if ( 0 == p      ) { continue ; }                           // CONTINUE 
    HepMC::GenVertex*   vertex = p->end_vertex () ;
    if ( 0 == vertex ) { continue ; }
    m_gps.insert ( m_gps.end() , 
                   vertex -> particles_begin ( HepMC::descendants ) , 
                   vertex -> particles_end   ( HepMC::descendants ) ) ;
  }
  std::sort ( m_gps.begin() , m_gps.end() ) ;
  GPs::iterator iend = std::unique ( m_gps.begin() , m_gps.end() ) ; 
  if ( iend != m_gps.end() ) { m_gps.erase ( iend , m_gps.end() ) ; }
  //
  if ( m_gps.empty() ) { return Warning ( "GPs container is empty!" ) ; }
  // 
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
