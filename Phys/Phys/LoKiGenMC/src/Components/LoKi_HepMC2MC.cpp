// $Id: LoKi_HepMC2MC.cpp,v 1.2 2006-02-09 17:52:35 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/IIncidentSvc.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/IRelation2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/select.h"
#include "LoKi/MCParticleCuts.h"
#include "LoKi/MCVertexCuts.h"
#include "LoKi/GenParticleCuts.h"
// ============================================================================
// MCTools 
// ============================================================================
#include "Kernel/IHepMC2MC.h"
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot at al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================

// ============================================================================
/** @class LoKi_HEpMC2MC
 *  The most simplest implementation of IHepMC2MC interface
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2001-01-23 
 */
// ============================================================================
class LoKi_HepMC2MC : 
  public virtual IHepMC2MC         ,
  public virtual IIncidentListener ,
  public         GaudiTool  
{
  // friend factory for instantiation
  friend class ToolFactory<LoKi_HepMC2MC> ;
  //
public:

  /** return a relation table for HepMC::GenParticle -> LHCb::MCParticle 
   *  relations 
   *  
   *  @code 
   *  typedef IHepMC2MC::HepMC2MC Table ;
   *  typedef Table::Range        Range ;
   * 
   *  // tool itself 
   *  const LoKi::IHepMC2MC*  imc = ... ;
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
  virtual const HepMC2MC* hepMC2MC () const 
  {
    //
    if ( 0 != m_table ) { return m_table->direct() ; }
    m_table = get<Table>( m_location ) ;
    if ( 0 == m_table ) 
    {
      Error ( "Could not load input data from '" + m_location + "'" ) ;
      return 0 ;
    }
    return m_table->direct() ;
    //
  } ;
  
  /** return a relation table for LHCb::MCParticle -> HepMC::GenParticle 
   *  relations 
   *  
   *  @code 
   *  typedef IHepMC2MC::MC2HepMC Table ;
   *  typedef Table::Range        Range ;
   * 
   *  // tool itself 
   *  const LoKi::IHepMC2MC*  imc = ... ;
   *  // get the relation table 
   *  const Table* table = imc->mc2HepMC() ;
   *  if ( 0 == table ) { ... } ; // error here!!
   * 
   *  // 
   *  const LHCb::MCParticle* mc = ... ;
   *  // get HepMC particles associated with given MCParticle
   *  Range links = table->relations( mc ) ; 
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-12 
   */  
  virtual const MC2HepMC* mc2HepMC () const 
  {
    //
    if ( 0 != m_table ) { return m_table->inverse() ; }
    m_table = get<Table>( m_location ) ;
    if ( 0 == m_table ) 
    {
      Error ( "Could not load input data from '" + m_location + "'" ) ;
      return 0 ;
    }
    return m_table->inverse() ;
    //
  };
  //
public:
  
  /// standard initialization of the tool 
  virtual StatusCode initialize () 
  {
    StatusCode sc = GaudiTool::initialize () ;
    if ( sc.isFailure() ) { return sc ; }
    
    IIncidentSvc* isvc = incSvc() ;
    Assert ( 0 != isvc , "IIncidentSvc* points to NULL" );
    isvc -> addListener ( this , IncidentType::EndEvent , 10 ) ;
    
    return StatusCode::SUCCESS ;
  };
  /// handle new incident 
  virtual void handle ( const Incident& ) { m_table = 0 ; }
  
protected:
  
  /** standart constructor 
   *  @param type tool type 
   *  @param name tool name 
   *  @param parent parent object
   */
  LoKi_HepMC2MC 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent )
    : GaudiTool ( type, name , parent )
    ///
    , m_table    ( 0                               ) 
    , m_location ( LHCb::HepMC2MCLocation::Default ) 
    ///
  {
    declareInterface<IIncidentListener> ( this ) ; 
    declareInterface<IHepMC2MC>         ( this ) ; 
    // 
    declareProperty  ( "Location"    , m_location  ) ;
  };
  /// destructor (virual and protected)
  virtual ~LoKi_HepMC2MC () {} ;
private:
  // default constructor  is disabled 
  LoKi_HepMC2MC ()  ;
  // copy constructor     is disabled 
  LoKi_HepMC2MC 
  ( const LoKi_HepMC2MC& ) ;
  // assignement operator is disabled 
  LoKi_HepMC2MC& operator=
  ( const LoKi_HepMC2MC& ) ;
  //
private:
  // the actual interface class (not exported) 
  typedef const IRelation2D<HepMC::GenParticle*,LHCb::MCParticle> Table ;
  // the relation table itself
  mutable Table* m_table     ;
  // 
private:
  // table location 
  std::string    m_location  ;
};
// ============================================================================

// ============================================================================
/// anonymous namespace to hide a concrete tool factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  local (Concrete) tool factory for instantiaton of 
   *  LoKi_HepMC2MC tools 
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-12 
   */
  // ==========================================================================
  const  ToolFactory<LoKi_HepMC2MC>         s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var LoKi_HepMC2MCFactory
 *  exported (abstract) tool factory for instantiaton of 
 *  LoKi_HepMC2MC tools 
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-05-12 
 */
// ============================================================================
const   IToolFactory&LoKi_HepMC2MCFactory = s_Factory ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
