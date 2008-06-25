// $Id: P2MCBase.h,v 1.1 2008-06-25 17:27:39 ibelyaev Exp $
// ============================================================================
#ifndef LOKIPHYSMC_P2MCBASE_H 
#define LOKIPHYSMC_P2MCBASE_H 1
// ============================================================================
// Include files
// ============================================================================
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
#include "Relations/IsConvertible.h"
#include "Relations/IRelationWeightedBase.h"
// ===========================================================================
// DaVinciMCKernel
// ===========================================================================
#include "Kernel/Particle2MCMethod.h"
#include "Kernel/Particle2MCLinker.h"
// ===========================================================================
namespace LoKi 
{
  // =========================================================================
  /** @class P2MCBase
   *
   *  Helper base class for decoding of Linkers into Relation Tables 
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
   *  @date   2007-02-24
   */
  class P2MCBase : public GaudiAlgorithm 
  {
  public:
    // ========================================================================
    // the linker from Orsay 
    typedef Object2MCLinker<LHCb::Particle> MCLinker ;
    // ========================================================================
  public:
    // ========================================================================
    /// initialize the algorithm 
    virtual StatusCode initialize () ;
    /// finalize   the algorithm 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    P2MCBase
    ( const std::string& name , 
      ISvcLocator*       pSvc ) ;
    /// virtual destructor 
    virtual ~P2MCBase() ;
    // ========================================================================
  public:
    // ========================================================================
    // handler for inputs 
    void inputsHandler ( Property& ) ; ///< handler for inputs 
    // handler for method 
    void methodHandler ( Property& ) ; ///< handler for method 
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled 
    P2MCBase () ;
    // copy  constructor is disabled 
    P2MCBase ( const P2MCBase& );
    // assignement operator is disabled 
    P2MCBase& operator=( const P2MCBase& );
    // ========================================================================
  protected:
    // ========================================================================
    /// copy links from linker to relation table 
    template <class TABLE>
    inline StatusCode fillTable ( TABLE* table ) ;  
    // ========================================================================
  protected:
    // ========================================================================
    typedef std::vector<std::string> Addresses ;
    // list of inputs 
    SimpleProperty<Addresses> m_inputs  ; ///< list of inputs
    // the method
    IntegerProperty           m_method  ; ///<  the method
    // output table 
    std::string               m_output  ; ///<  output Relation table 
    // linker object
    MCLinker*                 m_linker  ; ///< linker object 
    // ========================================================================
 } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
// anonymous namespace to hide some implementation detailes 
// ============================================================================
namespace 
{
  // ==========================================================================
  /// just the forward decalrations 
  template <class TABLE,bool> 
  struct _iGet ;
  // ==========================================================================
  /** @struct _iGet
   *  The helper structure to copy the information from linker 
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   */
  template <class TABLE,bool> 
  struct _iGet 
  {
    void copy 
    ( LoKi::P2MCBase::MCLinker* linker   , 
      const LHCb::Particle*     particle , 
      TABLE*                    table    ) const 
    {
      if ( 0 == linker || 0 == table ) { return ; }
      double weight = 0.0 ;
      const LHCb::MCParticle* mc = linker->first ( particle , weight ) ;
      while ( 0 != mc ) 
      {
        table->i_push ( particle , mc , weight ) ;
        mc = linker->next ( weight ) ;
      }
    }
  } ;
  // ==========================================================================
  /// dispatch for non-weighted relations 
  template <class TABLE> 
  struct _iGet<TABLE,false> 
  {
    void copy 
    ( LoKi::P2MCBase::MCLinker* linker   , 
      const LHCb::Particle*     particle ,
      TABLE*                    table    ) const 
    {
      if ( 0 == linker || 0 == table ) { return ; }
      const LHCb::MCParticle* mc = linker->first ( particle ) ;
      while ( 0 != mc ) 
      {
        table->i_push ( particle , mc ) ;
        mc = linker->next () ;
      }
    }
  } ;
  // ===========================================================================
  /** @struct iGet 
   *  The actual helper structure to copy the information from linker 
   *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
   */
  template <class TABLE> 
  struct iGet : public _iGet<TABLE,INHERITS(TABLE,IRelationWeightedBase)>
  {} ;
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
/// copy links from linker to relation table 
// ============================================================================
template <class TABLE>
inline StatusCode LoKi::P2MCBase::fillTable ( TABLE* table ) 
{
  Assert ( 0 != table , "Invalid Relation Table" ) ;
  // create linker if needed 
  if ( 0 == m_linker ) 
  { m_linker = new MCLinker( this , m_method , m_inputs ) ; }
  // just for convinience 
  const StatusCode OK = StatusCode::SUCCESS ;
  // Helper object to copy links 
  iGet<TABLE> getter ;
  // for statistics 
  size_t nParticles = 0 ;
  // loop over input linkers 
  const Addresses& addresses = m_inputs ;
  for ( Addresses::const_iterator iaddr = addresses.begin() ; 
        addresses.end() != iaddr ; ++iaddr )
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
      // copy the links 
      getter.copy ( m_linker , p , table ) ;                // ATTENTION i_push 
    } // end f the loop over particles in the container
  } // end of the loop over input containers
  // MANDATORY call of i_sort after i_push!
  table->i_sort() ;                                    // MANDATORY i_sort
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
} 
// ============================================================================
// The END 
// ============================================================================
#endif // LoKiPHYSMC_P2MC_H
// ============================================================================
