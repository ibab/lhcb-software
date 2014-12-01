// $Id: G4Higgses.cpp,v 1.1 2008-06-24 10:29:45 ibelyaev Exp $
// ============================================================================
// Include files 
//
// GEANT4 
#include "G4ParticleTable.hh"

// GaudiKernel
#include "GaudiKernel/PhysicalConstants.h"

// GiGa
#include "GiGa/GiGaException.h"

// local
#include "G4Higgses.h"

// ============================================================================
/** @file 
 *  The implementation file for class : G4Higgses
 *  @date 2008-06-19 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 */
// ============================================================================

/// initialize the static instance 
G4H_10* G4H_10::s_instance = 0 ;
G4H_20* G4H_20::s_instance = 0 ;
G4H_30* G4H_30::s_instance = 0 ;

// ============================================================================
/*  constructor from the mass and lifetime 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_10::G4H_10 
( const double mass , 
  const double ctau ) 
  : G4ParticleDefinition 
( "H_10"                                  , // the name 
  mass                                    , // the mass   
  Gaudi::Units::hbarc / ctau              , // the width  
  0                                       , // the charge 
  0                                       , // the spin 
  -1                                      , // the parity 
  +1                                      , // the conjugation 
  0                                       , // the isospin 
  0                                       , // the z-projection of isospin
  -1                                      , // the G-parity
  "higgs"                                 , // p-type 
  0                                       , // lepton 
  0                                       , // baryon 
  25                                      , // PDG encoding 
  15 * Gaudi::Units::meter         < ctau , // stable 
  ctau / Gaudi::Units::c_light            , // lifetime 
  NULL                                    , // decay table 
  1.e-3 * Gaudi::Units::micrometer > ctau , // shortlived 
  "higgs"                                 , // subtype    
  25                                        // antiparticle  
  )
{}

// ============================================================================
/*  constructor from the mass and lifetime 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_20::G4H_20 
( const double mass , 
  const double ctau ) 
  : G4ParticleDefinition 
( "H_20"                                  , // the name 
  mass                                    , // the mass   
  Gaudi::Units::hbarc / ctau              , // the width  
  0                                       , // the charge 
  0                                       , // the spin 
  -1                                      , // the parity 
  +1                                      , // the conjugation 
  0                                       , // the isospin 
  0                                       , // the z-projection of isospin
  -1                                      , // the G-parity
  "higgs"                                 , // p-type 
  0                                       , // lepton 
  0                                       , // baryon 
  35                                      , // PDG encoding 
  15 * Gaudi::Units::meter         < ctau , // stable 
  ctau / Gaudi::Units::c_light            , // lifetime 
  NULL                                    , // decay table 
  1.e-3 * Gaudi::Units::micrometer > ctau , // shortlived 
  "higgs"                                 , // subtype    
  35                                        // antiparticle  
  )
{}

// ============================================================================
/*  constructor from the mass and lifetime 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_30::G4H_30 
( const double mass , 
  const double ctau ) 
  : G4ParticleDefinition 
( "H_30"                                  , // the name 
  mass                                    , // the mass   
  Gaudi::Units::hbarc / ctau              , // the width  
  0                                       , // the charge 
  0                                       , // the spin 
  +1                                      , // the parity 
  +1                                      , // the conjugation 
  0                                       , // the isospin 
  0                                       , // the z-projection of isospin
  -1                                      , // the G-parity
  "higgs"                                 , // p-type 
  0                                       , // lepton 
  0                                       , // baryon 
  36                                      , // PDG encoding 
  15 * Gaudi::Units::meter         < ctau , // stable 
  ctau / Gaudi::Units::c_light            , // lifetime 
  NULL                                    , // decay table 
  1.e-3 * Gaudi::Units::micrometer > ctau , // shortlived 
  "higgs"                                 , // subtype    
  36                                        // antiparticle  
  )
{}

// ============================================================================
/*  create the particle definition 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_10* G4H_10::Create 
( const double mass ,                                    // the mass 
  const double ctau )                                    // the lifetime 
{
  if ( 0 != s_instance ) { return s_instance ; }
  // check the resence of the paricle in particle table:
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  if ( 0 == table ) 
  { throw GiGaException ("G4H_10::Create: Invalid G4ParticleTable") ; }
  G4ParticleDefinition* check1 = table->FindParticle("H_10");
  if ( 0 != check1 ) 
  { throw GiGaException ("G4H_10::Create: H_10 already exists!") ; }
  G4ParticleDefinition* check2 = table->FindParticle( 25 );
  if ( 0 != check2 ) 
  { throw GiGaException ("G4H_10::Create: PDGID(25) already exists!") ; }
  // create the particle: 
  s_instance = new G4H_10 ( mass , ctau ) ;
  return s_instance ;
}

// ============================================================================
/*  create the particle definition 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_20* G4H_20::Create 
( const double mass ,                                    // the mass 
  const double ctau )                                    // the lifetime 
{
  if ( 0 != s_instance ) { return s_instance ; }
  // check the resence of the paricle in particle table:
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  if ( 0 == table ) 
  { throw GiGaException ("G4H_20::Create: Invalid G4ParticleTable") ; }
  G4ParticleDefinition* check1 = table->FindParticle("H_20");
  if ( 0 != check1 ) 
  { throw GiGaException ("G4H_20::Create: H_20 already exists!") ; }
  G4ParticleDefinition* check2 = table->FindParticle( 35 );
  if ( 0 != check2 ) 
  { throw GiGaException ("G4H_20::Create: PDGID(35) already exists!") ; }
  // create the particle: 
  s_instance = new G4H_20 ( mass , ctau ) ;
  return s_instance ;
}

// ============================================================================
/*  create the particle definition 
 *  @param mass the mass 
 *  @param ctau the lifetime 
 */
// ============================================================================
G4H_30* G4H_30::Create 
( const double mass ,                                    // the mass 
  const double ctau )                                    // the lifetime 
{
  if ( 0 != s_instance ) { return s_instance ; }
  // check the resence of the paricle in particle table:
  G4ParticleTable* table = G4ParticleTable::GetParticleTable();
  if ( 0 == table ) 
  { throw GiGaException ("G4H_30::Create: Invalid G4ParticleTable") ; }
  G4ParticleDefinition* check1 = table->FindParticle("H_30");
  if ( 0 != check1 ) 
  { throw GiGaException ("G4H_30::Create: H_30 already exists!") ; }
  G4ParticleDefinition* check2 = table->FindParticle( 36 );
  if ( 0 != check2 ) 
  { throw GiGaException ("G4H_30::Create: PDGID(36) already exists!") ; }
  // create the particle: 
  s_instance = new G4H_30 ( mass , ctau ) ;
  return s_instance ;
}

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_10 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_10* G4H_10::Definition  () { return H_10Definition() ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_10 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_10* G4H_10::H_10Definition () { return s_instance ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_10 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_10* G4H_10::H_10            () { return H_10Definition() ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_20 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_20* G4H_20::Definition () { return H_20Definition() ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_20 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_20* G4H_20::H_20Definition () { return s_instance ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_20 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_20* G4H_20::H_20           () { return H_20Definition() ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_30 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_30* G4H_30::Definition () { return H_30Definition() ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_30 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_30* G4H_30::H_30Definition () { return s_instance ; }

// ============================================================================
/*  get the definition (Access to the static instance)
 *  @return G4H_30 particle definition 
 *  @attention this method *COULD* (on purpose!) return NULL pointer!!
 */
// ============================================================================
G4H_30* G4H_30::H_30           () { return H_30Definition() ; }

// ============================================================================
// virtual destructor 
// ============================================================================
G4H_10::~G4H_10(){}   // virtual desctructor 

// ============================================================================
// virtual destructor 
// ============================================================================
G4H_20::~G4H_20(){}   // virtual desctructor 

// ============================================================================
// virtual destructor 
// ============================================================================
G4H_30::~G4H_30(){}   // virtual desctructor 


// ============================================================================
// The END 
// ============================================================================
