// $Id: GenMCParticles.cpp,v 1.5 2007-04-16 16:16:45 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2007/01/23 13:29:18  ibelyaev
//  fix for Ex/LoKiExample
//
// Revision 1.3  2006/10/27 13:38:22  ibelyaev
//  fix for SLC4 platform
//
// Revision 1.2  2006/02/18 18:15:44  ibelyaev
//  fix a typo
//
// Revision 1.1.1.1  2006/01/26 17:35:17  ibelyaev
// New import: Generator/MC related part of LoKi project 
// 
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelation2D.h"
// ============================================================================
// Kernel
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
  , m_mcps  () 
  , m_table ( table  )
{ 
  add ( mc ) ; 
}
// ============================================================================
/** constructor from the particle and relation table 
 *  @param mc   pointer to MC particle 
 *  @param table relation table  
 */
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::IsAMotherForMC 
( const LHCb::MCParticle::ConstVector& mc   , 
  const LHCb::HepMC2MC*   table ) 
  : LoKi::Predicate<const HepMC::GenParticle*> ()
  , m_mcps  () 
  , m_table ( table  )
{ 
  add ( mc ) ; 
}
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
  , m_mcps  () 
  , m_table ( table  )
{ 
  add ( mc ) ; 
}
// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::IsAMotherForMC 
( const LoKi::GenParticles::IsAMotherForMC& right )
  : LoKi::AuxFunBase                           ( right ) 
  , LoKi::Predicate<const HepMC::GenParticle*> ( right )
  , m_mcps  ( right.m_mcps  ) 
  , m_table ( right.m_table )
{}  
// ============================================================================
/// MANDATORY: virtual destructor
// ============================================================================
LoKi::GenParticles::IsAMotherForMC::~IsAMotherForMC(){}
// ============================================================================
/// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::GenParticles::IsAMotherForMC*
LoKi::GenParticles::IsAMotherForMC::clone() const
{ return new LoKi::GenParticles::IsAMotherForMC(*this) ; }
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
  if ( !m_table.validPointer() ) 
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
      const bool found = std::binary_search 
        ( m_mcps.begin () , m_mcps.end () , mc ) ;
      // found ?
      if ( found )          { return true ; }                   // RETURN
    } 
  }
  //
  return false ;  
};
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::GenParticles::IsAMotherForMC::fillStream
( std::ostream& s ) const { return s << "GMCMOTH" ; }
// ============================================================================
LoKi::GenParticles::IsAMotherForMC& 
LoKi::GenParticles::IsAMotherForMC::add ( const LHCb::MCParticle* mc ) 
{
  while ( 0 != mc ) 
  {
    m_mcps.insert ( mc ) ;
    const LHCb::MCVertex* mv = mc->originVertex() ;
    if ( 0 == mv ) { break ; }             // BREAK 
    mc = mv->mother() ;
  }
  return *this ;;
}
// ============================================================================
LoKi::GenParticles::IsAMotherForMC& 
LoKi::GenParticles::IsAMotherForMC::add
( const LHCb::MCParticle::ConstVector& mc ) 
{ return add ( mc.begin() , mc.end() ) ; }
// ============================================================================
LoKi::GenParticles::IsAMotherForMC& 
LoKi::GenParticles::IsAMotherForMC::add
( const LoKi::Types::MCRange&          mc ) 
{ return add ( mc.begin() , mc.end() ) ; }
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
  , m_gps   (       ) 
  , m_table ( table ) 
{ 
  add ( p ) ; 
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const HepMC::GenParticle* p     , 
  const LHCb::HepMC2MC2D*   table ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (   ) 
  , m_table ( 0 ) 
{ 
  add ( p     ) ; 
  set ( table ) ;
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const HepMC::GenParticle* p     , 
  const LHCb::HepMC2MC*     table ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (   ) 
  , m_table ( 0 ) 
{ 
  add ( p ) ; 
  set ( table ) ;
}
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
  add ( vertex ) ; 
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::GenTypes::GenContainer& range , 
  const LHCb::MC2HepMC*   table  ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (       ) 
  , m_table ( table ) 
{ 
  add ( range ) ; 
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::Types::GRange& range , 
  const LHCb::MC2HepMC*      table  ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (       ) 
  , m_table ( table ) 
{ 
  add ( range ) ; 
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::Types::GRange& range , 
  const LHCb::HepMC2MC2D*    table  ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (   ) 
  , m_table ( 0 ) 
{ 
  add ( range ) ; 
  set ( table ) ;
}
// ============================================================================
/// constructor from particle and a table 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::Types::GRange& range , 
  const LHCb::HepMC2MC*      table  ) 
  : LoKi::Predicate<const LHCb::MCParticle*> ()
  , m_gps   (   ) 
  , m_table ( 0 ) 
{ 
  add ( range ) ; 
  set ( table ) ;
}
// ============================================================================
/** copy constructor
 *  @param rigth object to be copied 
 */
// ============================================================================
LoKi::MCParticles::IsFromHepMC::IsFromHepMC 
( const LoKi::MCParticles::IsFromHepMC& right ) 
  : LoKi::AuxFunBase                         ( right ) 
  , LoKi::Predicate<const LHCb::MCParticle*> ( right ) 
  , m_gps   ( right.m_gps   ) 
  , m_table ( right.m_table ) 
{}
// ============================================================================
/// MANDATORY: virtual destructor 
// ============================================================================
LoKi::MCParticles::IsFromHepMC::~IsFromHepMC(){}
// ============================================================================
/// MANDATORY: clone method("virtual constructor")
// ============================================================================
LoKi::MCParticles::IsFromHepMC*
LoKi::MCParticles::IsFromHepMC::clone() const 
{ return new LoKi::MCParticles::IsFromHepMC(*this) ; }
// ============================================================================
/// "SHORT" representation, @see LoKi::AuxFunBase 
// ============================================================================
std::ostream& 
LoKi::MCParticles::IsFromHepMC::fillStream
( std::ostream& s ) const { return s << "FROMGTREE" ; }
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
  if ( !m_table.validPointer() ) 
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
  mcps.reserve ( 10 ) ;
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
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::add ( const HepMC::GenParticle* p ) 
{
  if ( 0 == p ) { return *this ;} 
  m_gps.insert( p ) ; 
  return add ( p->end_vertex () ) ;
}
// ============================================================================
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::add ( const HepMC::GenVertex* v ) 
{
  if ( 0 == v ) { return *this ; } 
  HepMC::GenVertex* _v = const_cast<HepMC::GenVertex*>( v ) ;  
  return add ( _v -> particles_begin ( HepMC::children ) , 
               _v -> particles_end   ( HepMC::children ) ) ;
}
// ============================================================================
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::add ( const LoKi::Types::GRange& r )  
{ return add ( r.begin() , r.end() ) ; }
// ============================================================================
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::set 
( const LHCb::MC2HepMC*   table ) 
{
  m_table = table ;
  if ( !m_table.validPointer() ) 
  { Warning ( "LHCb::MC2HepMC* points toNULL" ) ; }  
  return *this ;
}
// ============================================================================
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::set
( const LHCb::HepMC2MC2D* table ) 
{
  if ( 0 != table ) { set ( table->inverse() ) ; }
  return *this ;
}
// ============================================================================
LoKi::MCParticles::IsFromHepMC& 
LoKi::MCParticles::IsFromHepMC::set
( const LHCb::HepMC2MC*   table ) 
{ return set ( dynamic_cast<const LHCb::HepMC2MC2D*>( table ) ) ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
