// $Id: LoKi_P2MC.cpp,v 1.2 2006-10-11 15:24:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/08/29 11:40:47  ibelyaev
//  many fixed to simplify MC-match
// 
// ============================================================================
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
#include "Relations/IRelation2D.h"
#include "Relations/Relation2D.h"
// ===========================================================================
// LoKiPhysMC 
// ===========================================================================
#include "Kernel/Particle2MC.h"
// ===========================================================================
// LinkerEvent 
// ===========================================================================
#include "Linker/LinkedTo.h"
// ===========================================================================
// DaVinciAssociators 
// ===========================================================================
#include "DaVinciAssociators/Particle2MCLinker.h"
// ===========================================================================
/** @file
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table 
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
/** @class LoKi_P2MC
 *
 *  Simple algorithm for conversion of Particle->MC links from 
 *  "linker" form into usable form of relation table 
 *
 *  Many thanks to Edwin Bos for kind help!
 *
 *  @todo replace 'include Particle2MCLinker' with 
 *   'include Particle2MCmethod' from new package DaVinciMCTools 
 *
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-08-17 
 */
class LoKi_P2MC : public GaudiAlgorithm 
{
  friend class AlgFactory<LoKi_P2MC> ;
public:
  /// execution of the algorithm 
  virtual StatusCode execute() ;
protected:
  /// standard constructor 
  LoKi_P2MC
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_linkers () 
    , m_output  () 
  {
    declareProperty ( "Linkers" , m_linkers ) ;
    declareProperty ( "Output"  , m_output  ) ;
  }
  /// virtual protected destructor 
  virtual ~LoKi_P2MC() {} ;
private:
  // default constructor is disabled 
  LoKi_P2MC () ;
  // copy  constructor is disabled 
  LoKi_P2MC ( const LoKi_P2MC& );
  // assignement operator is disabled 
  LoKi_P2MC& operator=( const LoKi_P2MC& );
private:
  typedef std::vector<std::string> Addresses ;
  Addresses   m_linkers ;
  std::string m_output  ;
} ;
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( LoKi_P2MC );
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
/// execution of the algorithm 
// ============================================================================
StatusCode LoKi_P2MC::execute() 
{
  // avoid long names 
  typedef LHCb::Relation2D<LHCb::Particle,LHCb::MCParticle> Table ;
  // check the inheritance scheme 
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::P2MC2D));

  if ( m_linkers.empty() ) 
  { Warning ( "Empty list of input linkers!" ) ; }
  
  // create the new relation table and register it in TES 
  Table* table = new Table( 100 ) ;
  put ( table , m_output ) ;

  // for statistics 
  size_t nParticles = 0 ;
  
  // loop over input linkers 
  for ( Addresses::const_iterator iaddr = m_linkers.begin() ; 
        m_linkers.end() != iaddr ; ++iaddr )
  {
    // get the linker object
    LinkedTo<LHCb::MCParticle,LHCb::Particle> linker ( evtSvc() , msgSvc(), *iaddr );
    if ( linker.notFound() ) 
    {
      Error ( "The linker table '" + (*iaddr) + "' is not found!" ) ;
      continue ;                                                       // CONTINUE 
    }
    
    // get the track container from linked object 
    const LHCb::Particles* particles = 0 ;
    if ( exist<LHCb::Particles>( *iaddr ) ) 
    { particles = get<LHCb::Particles>( *iaddr ) ; }
    
    for ( unsigned int i = Particle2MCMethod::No + 1 ; 
          Particle2MCMethod::Max > i && 0 == particles ; ++i ) 
    {
      std::string::size_type ifind = 
        iaddr->find( Particle2MCMethod::extension[i] ) ;
      if ( 0 != ifind && std::string::npos != ifind ) 
      {
        std::string loc = std::string( *iaddr , 0 , ifind ) ;
        if ( exist<LHCb::Particles>( loc ) )
        { particles = get<LHCb::Particles>( loc ) ; }
      }
    }
    
    // check the container:
    if ( 0 == particles ) 
    {
      Error ( "No valid particle container found for '"+(*iaddr)+"'") ;
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
      // loop over linker
      const LHCb::MCParticle* mcp = linker.first ( p ) ;
      while ( 0 != mcp ) 
      {
        table->i_relate ( p , mcp ) ;
        mcp = linker.next() ;
      }
    } // end f the loop over particles in the container
  } // end of the loop over linkers 
  
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
