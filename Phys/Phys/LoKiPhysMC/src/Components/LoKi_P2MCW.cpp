// $Id: LoKi_P2MCW.cpp,v 1.4 2007-02-23 20:15:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/10/11 15:51:40  ibelyaev
//  remove DaVinciAssociators; use Kernel/Particle2MCMethod.h
//
// Revision 1.2  2006/10/11 15:24:26  ibelyaev
//  use DaVinciMCKernel
//
// ===========================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// ===========================================================================
// GaudiAlg 
// ===========================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ===========================================================================
// Event 
// ===========================================================================
#include "Event/Particle.h"
#include "Event/MCParticle.h"
// ===========================================================================
// Relations 
// ===========================================================================
#include "Relations/IRelationWeighted2D.h"
#include "Relations/RelationWeighted2D.h"
// ===========================================================================
// LoKiPhysMC 
// ===========================================================================
#include "Kernel/Particle2MC.h"
// ===========================================================================
// LinkerEvent 
// ===========================================================================
#include "Linker/LinkedTo.h"
// ===========================================================================
// DaVinciMCKernel
// ===========================================================================
#include "Kernel/Particle2MCMethod.h"
#include "DaVinciAssociators/Particle2MCLinker.h"
// ===========================================================================
/** @file
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table with weight 
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
 *  @date 2006-08-17 
 */
// ============================================================================
/** @class LoKi_P2MCW
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table with weight 
 *
 *  Many thanks to Edwin Bos for kind help!
 *
 *  @todo replace 'include Particle2MCLinker' with 
 *   'include Particle2MCmethod' from new package DaVinciMCTools 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-08-17 
 */
class LoKi_P2MCW : public GaudiAlgorithm 
{
  friend class AlgFactory<LoKi_P2MCW> ;
public:
  typedef Object2MCLinker<LHCb::Particle> MCLinker ;
public:
  /// execution of the algorithm 
  virtual StatusCode execute() ;
  /// initialize the algorithm 
  virtual StatusCode initialize () ;
  /// finalize   the algorithm 
  virtual StatusCode finalize   () ;
protected:
  /// standard constructor 
  LoKi_P2MCW
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_inputs  (   ) 
    , m_method  ( Particle2MCMethod::WithChi2 ) 
    , m_output  (   ) 
    , m_linker  ( 0 )
  {
    declareProperty ( "Inputs"  , m_inputs  ) ;
    declareProperty ( "Method"  , m_method  ) ;
    declareProperty ( "Output"  , m_output  ) ;
  }
  /// virtual protected destructor 
  virtual ~LoKi_P2MCW() {} ;
private:
  // default constructor is disabled 
  LoKi_P2MCW () ;
  // copy  constructor is disabled 
  LoKi_P2MCW ( const LoKi_P2MCW& );
  // assignement operator is disabled 
  LoKi_P2MCW& operator=( const LoKi_P2MCW& );
private:
  typedef std::vector<std::string> Addresses ;
  Addresses   m_inputs  ;
  int         m_method  ;
  std::string m_output  ;
  MCLinker*   m_linker  ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( LoKi_P2MCW );
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
/// initialize the algorithm
// ============================================================================
StatusCode LoKi_P2MCW::initialize () 
{
  StatusCode sc = GaudiAlgorithm::initialize() ;
  if ( sc.isFailure() ) {  return sc ; }
  
  // check the inputs
  if ( m_inputs.empty() ) { Warning ( "Empty list of inputs!") ; }
  
  switch ( m_method ) 
  {
  case Particle2MCMethod::WithChi2  : break ;
  case Particle2MCMethod::Chi2      : 
  case Particle2MCMethod::Links     : 
  case Particle2MCMethod::Composite : 
    Warning ( "Unweighted method is selected" , sc ) ; break ;
  default:
    return Error("Invalid method is choosen!") ;
  }
  //
  if ( 0 != m_linker ) { delete m_linker ; m_linker = 0  ; }
  m_linker = new MCLinker( this , m_method , m_inputs ) ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================
// finalization fo the algorithm
// ============================================================================
StatusCode LoKi_P2MCW::finalize () 
{
  if ( 0 != m_linker ) { delete m_linker ; m_linker = 0  ; }  
  return GaudiAlgorithm::finalize () ; 
} ;
// ============================================================================
/// execution of the algorithm 
// ============================================================================
StatusCode LoKi_P2MCW::execute() 
{
  // avoid long names 
  typedef LHCb::RelationWeighted2D<LHCb::Particle,LHCb::MCParticle,double> Table ;
  // check the inheritance scheme 
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::P2MC2DW));
  
  Assert ( 0 != m_linker , "Linker is invalid!" ) ;
  
  const StatusCode OK = StatusCode::SUCCESS ;
  
  // create the new relation table and register it in TES 
  Table* table = new Table( 100 ) ;
  put ( table , m_output ) ;
  
  // for statistics 
  size_t nParticles = 0 ;
  
  // loop over input linkers 
  for ( Addresses::const_iterator iaddr = m_inputs.begin() ; 
        m_inputs.end() != iaddr ; ++iaddr )
  {
    const LHCb::Particle::Container* particles = 0 ;
    const std::string loc = (*iaddr)  + "/Particles" ;
    if      ( exist<LHCb::Particle::Container>  (  loc   ) ) 
    { particles = get<LHCb::Particle::Container>(  loc   ) ; }
    else if ( exist<LHCb::Particle::Container>  ( *iaddr ) ) 
    { particles = get<LHCb::Particle::Container>( *iaddr ) ; }
    // check the container:
    if ( 0 == particles ) 
    {
      Error ( "No valid LHCb::Particle::Container found for '"+(*iaddr)+"'",OK) ;
      continue ;                                                     // CONTINUE 
    }
    // for statistics 
    nParticles += particles->size() ;
    // loop over all particles 
    for ( LHCb::Particles::const_iterator ipart = particles->begin() ; 
          particles->end() != ipart ; ++ipart )
    {
      const LHCb::Particle* p = *ipart ;
      if ( 0 == p ) { continue ; }                                  // CONTINUE
      // loop over the linked particles 
      double weight = 0 ;
      const LHCb::MCParticle* mcp = m_linker->first ( p , weight ) ;
      while ( 0 != mcp ) 
      {
        // ATTENTION! i-push is in use! 
        table->i_push ( p , mcp , weight ) ;                    // ATTENTION 
        mcp = m_linker->next( weight ) ;
      }
    } // end f the loop over particles in the container
  } // end of the loop over input containers 
  
  // MANDATORY call of i_sprt after i_push!
  table->i_sort() ;                                    // MANDATORY!
  
  // simple check 
  if ( table->i_relations().empty() ) { Warning("Empty relation table!") ; }
  
  // a bit of statistics 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  {
    counter ( "#particles" ) += nParticles ;
    counter ( "#links"     ) += table->i_relations().size() ;
  }
  //
  return StatusCode::SUCCESS;
} ;

// ============================================================================
/// The END 
// ============================================================================
