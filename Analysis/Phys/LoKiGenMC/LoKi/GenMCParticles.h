// $Id$
// ============================================================================
#ifndef LOKI_GENMCPARTICLES_H 
#define LOKI_GENMCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
#include "Kernel/HepMC2MC.h"
// ============================================================================
// LoKi
// ============================================================================
#include "Relations/IRelation.h"
#include "LoKi/UniqueKeeper.h"
#include "LoKi/MCTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenExtract.h"
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
 *  @date 2001-01-23 
 */
// ============================================================================
namespace LoKi
{
  namespace GenMCParticles 
  {   
    // ========================================================================
    /** @class IsAMotherForMC
     *  Trivial predicate which evaluates to 'true'
     *  for (HepMC)particles which are "origins" for 
     *  given certain MCParticles 
     * 
     *  @see LHCb::MCParticle
     *  @see HepMC::GenParticle
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class IsAMotherForMC 
      : public LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate
    {
    public:
      /** constructor from the particle and relation table 
       *  @see IHepMC2MC
       *  @param mc   pointer to MC particle 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LHCb::MCParticle*  mc   , 
        const LHCb::HepMC2MC*    table ) ;
      /** constructor from the particles and relation table 
       *  @see IHepMC2MC
       *  @param mc   range of MC particles 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LHCb::MCParticle::ConstVector&  mc    , 
        const LHCb::HepMC2MC*          table ) ;
      /** constructor from the particles and relation table 
       *  @see IHepMC2MC
       *  @param mc   range of MC particles 
       *  @param table relation table  
       */
      IsAMotherForMC 
      ( const LoKi::MCTypes::MCRange&  mc    , 
        const LHCb::HepMC2MC*          table ) ;
      /** templated constructor from the particles relation table 
       *  @see IHepMC2MC
       *  @param first "begin"-iterator for sequence of MC particles 
       *  @param last  "end"-iterator for sequence of MC particles 
       *  @param table relation table  
       */
      template <class MCPARTICLE>
      IsAMotherForMC 
      ( MCPARTICLE            first ,
        MCPARTICLE            last  , 
        const LHCb::HepMC2MC* table ) 
        : LoKi::BasicFunctors<const HepMC::GenParticle*>::Predicate () 
        , m_mcps  () 
        , m_table ( table )
      { 
        add ( first , last ) ;
      }
      /** copy constructor 
       *  @param right object to be copied 
       */
      IsAMotherForMC 
      ( const IsAMotherForMC& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~IsAMotherForMC() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsAMotherForMC* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;      
    public:
      IsAMotherForMC& add    ( const LHCb::MCParticle*              mc ) ;
      IsAMotherForMC& add    ( const LHCb::MCParticle::ConstVector& mc ) ;
      IsAMotherForMC& add    ( const LoKi::Types::MCRange&          mc ) ;
      template <class OBJECT>
      IsAMotherForMC& add    ( OBJECT first , OBJECT last  ) 
      {
        for ( ; first != last ; ++first ) { add ( *first ) ; } 
        return *this ;
      }
    private:
      // default constructor is disabled 
      IsAMotherForMC() ;
      // assignement operator is disabled 
      IsAMotherForMC& operator=
      ( const IsAMotherForMC& ) ;
    private:
      typedef LoKi::UniqueKeeper<LHCb::MCParticle>  MCPs    ;
      MCPs                                          m_mcps  ;
      LoKi::Interface<LHCb::HepMC2MC>               m_table ;
    };
    // =========================================================================
    /** @class IsFromHepMC
     *  trivial predicate which 
     *  evaluates to 'true' for MC-particles,
     *  which originates from certain HepMC particle 
     *  @see MCParticle
     *  @see IHepMC2MC
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
     *  @date 2005-05-16
     */
    class IsFromHepMC
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate
    {
    public:
      /** constructor from the particle and a table 
       *  @see LoKi::Relations::HepMC2MC
       *  @see LoKi::Relations::MC2HepMC
       *  @see LoKi::Relations::HepMC2MC2D
       *  @param p pointer to the particle
       *  @param table pointer to table 
       */
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from the particle and a table 
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::HepMC2MC*     table ) ;
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::HepMC2MC2D*   table ) ;
      /// constructor from vertex and a tool
      IsFromHepMC
      ( const HepMC::GenVertex*   v     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from vector of particles 
      IsFromHepMC 
      ( const LoKi::GenTypes::GenContainer& p , 
        const LHCb::MC2HepMC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::MC2HepMC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::HepMC2MC* table ) ;
      /// constructor from range of particles 
      IsFromHepMC 
      ( const LoKi::Types::GRange& p , 
        const LHCb::HepMC2MC2D* table ) ;
      /// constructor from range of objects
      template <class GENOBJECT>
      IsFromHepMC
      ( GENOBJECT             first , 
        GENOBJECT             last  , 
        const LHCb::MC2HepMC* table  ) 
        : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
        , m_gps   () 
        , m_table ( table  )
      { 
        add ( first , last ) ; 
      } 
      /// constructor from range of objects
      template <class GENOBJECT>
      IsFromHepMC
      ( const LHCb::MC2HepMC* table , 
        GENOBJECT             first , 
        GENOBJECT             last  ) 
        : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate()
        , m_gps () 
        , m_table ( table )
      { 
        add ( first , last ) ; 
      } 
      /// copy constructor
      IsFromHepMC 
      ( const IsFromHepMC& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~IsFromHepMC() ;
      /// MANDATORY: clone method("virtual constructor")
      virtual  IsFromHepMC* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    public:
      // add a genparticle 
      IsFromHepMC& add ( const HepMC::GenParticle*  p ) ;
      // add a genparticle 
      IsFromHepMC& add ( const HepMC::GenVertex*    p ) ;      
      // add set of genparticles 
      IsFromHepMC& add ( const LoKi::Types::GRange& r )  ;
      // add a sequence
      template <class OBJECT>
      IsFromHepMC& add ( OBJECT first , OBJECT last  ) 
      { 
        for ( ; first != last ; ++first ) { add (*first ) ; } 
        return *this ;
      }
    public:
      IsFromHepMC& set ( const LHCb::MC2HepMC*   table ) ;
      IsFromHepMC& set ( const LHCb::HepMC2MC*   table ) ;      
      IsFromHepMC& set ( const LHCb::HepMC2MC2D* table ) ;      
    private:
      // default constructor is disabled
      IsFromHepMC();
      // assignement operator is disabled 
      IsFromHepMC& operator=
      ( const IsFromHepMC& ) ;
    private:
      typedef LoKi::UniqueKeeper<HepMC::GenParticle> GPs     ;
      GPs                                            m_gps   ;
      LoKi::Interface<LHCb::MC2HepMC>                m_table ;
    } ;
    // ========================================================================
    namespace MCParticles
    {
      /// import a type into consistent namespace 
      typedef LoKi::GenMCParticles::IsFromHepMC        IsFromHepMC    ;
    }
    // ========================================================================
    namespace GenParticles
    {
      /// import a type into consistent namespace 
      typedef LoKi::GenMCParticles::IsAMotherForMC     IsAMotherForMC ;
    }
    // ========================================================================    
  }  // end of namespace GenMCParticles
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENMCPARTICLES_H
// ============================================================================
