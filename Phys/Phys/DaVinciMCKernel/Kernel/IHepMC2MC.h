// $Id: IHepMC2MC.h,v 1.2 2008-06-03 15:24:14 cattanem Exp $
// ============================================================================
#ifndef MCTOOLS_IHEPMC2MC_H 
#define MCTOOLS_IHEPMC2MC_H 1
// ============================================================================
// incldue files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// Kernel ?
// ============================================================================
#include "Kernel/HepMC2MC.h"
// ============================================================================

// ============================================================================
/** @file
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
 *  @date 2006-01-23 
 */
// ============================================================================

/** @class IHepMC2MC IHepMC2MC.h MCTools/IHepMC2MC.h
 *  
 *  An abstract interface for links between 
 *  HepMC (generator) and "MC" structures 
 *
 *  @see HepMC::GenParticle 
 *  @see LHCb::MCParticle 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date   2005-05-12
   */
class IHepMC2MC 
  : virtual public IAlgTool 
{
public:
  
  /// the actual type for HepMC ->    MC relations 
  typedef LHCb::HepMC2MC  HepMC2MC ;
  /// the actual type for    MC -> HepMC relations 
  typedef LHCb::MC2HepMC  MC2HepMC ;
  
public:
  /** return a relation table for HepMC::GenParticle -> LHCb::MCParticle 
   *  relations 
   *  
   *  @code 
   *  typedef IHepMC2MC::HepMC2MC Table ;
   *  typedef Table::Range        Range ;
   * 
   *  // tool itself 
   *  const IHepMC2MC*  imc = ... ;
   *  // get the relation table 
   *  const Table* table = imc->hepMC2MC() ;
   *  if ( 0 == table ) { ... } ; // error here!!
   * 
   *  // 
   *  const HepMC::GenParticle* hepMC = ... ;
   *  // get MC particles associated with given HepMC::GenParticle
   *  Range links = table->relations( hepMC ) ; 
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-12 
   */  
  virtual const HepMC2MC* hepMC2MC () const = 0 ;
  /** return a relation table for LHcb::MCParticle -> HepMC::GenParticle 
   *  relations 
   *  
   *  @code 
   *
   *  typedef IHepMC2MC::MC2HepMC Table ;
   *  typedef Table::Range        Range ;
   * 
   *  // tool itself 
   *  const IHepMC2MC*  imc = ... ;
   *  // get the relation table 
   *  const Table* table = imc->mc2HepMC() ;
   *  if ( 0 == table ) { ... } ; // error here!!
   * 
   *  // 
   *  const LHcb::MCParticle* mc = ... ;
   *  // get HepMC particles associated with given MCParticle
   *  Range links = table->relations( mc ) ; 
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-12 
   */  
  virtual const MC2HepMC* mc2HepMC () const = 0 ;
  //
public:  
  /// Return the unique interface identifier 
  static const InterfaceID& interfaceID() ;
protected:
  // virtual and protected destructor 
  virtual ~IHepMC2MC() ; ///< virtual and protected destructor 
} ;

// ============================================================================
// The END 
// ============================================================================
#endif // MCTOOLS_IHEPMC2MC_H
// ============================================================================
