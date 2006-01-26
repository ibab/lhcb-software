// $Id: GenMCParticles.h,v 1.1.1.1 2006-01-26 17:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
#include "LoKi/MCTypes.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenExtract.h"
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
namespace LoKi
{
  namespace GenParticles 
  {   
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
    class IsAMotherForMC :
      public LoKi::Predicate<const HepMC::GenParticle*> 
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
      ( const LoKi::MCTypes::MCRange&  mc    , 
        const LHCb::HepMC2MC*          table ) ;
      /** templated constructor from the particles relation table 
       *  @see IHepMC2MC
       *  @param first "begin"-iterator for sequence of MC particles 
       *  @param end   "end"-iterator for sequence of MC particles 
       *  @param table relation table  
       */
      template <class MCPARTICLE>
      IsAMotherForMC 
      ( MCPARTICLE            first ,
        MCPARTICLE            last  , 
        const LHCb::HepMC2MC* table ) 
        : LoKi::Predicate<const HepMC::GenParticle*> ()
        , m_mcps  ( first , last ) 
        , m_table ( table )
      { getMC() ; }
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
    protected:
      // prepare an appropriate MC information
      StatusCode getMC() ;
    private:
      // default constructor is disabled 
      IsAMotherForMC() ;
      // assignement operator is disabled 
      IsAMotherForMC& operator=
      ( const IsAMotherForMC& ) ;
    private:
      typedef std::vector<const LHCb::MCParticle*>  MCPs ;
      MCPs                  m_mcps  ;
      const LHCb::HepMC2MC* m_table ;
    };
    
  } ;  // end of namespace GenParticles
  
  namespace MCParticles 
  {
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
    class IsFromHepMC :
      public LoKi::Predicate<const LHCb::MCParticle*>
    {
    public:
      /** constructor from particle and a table 
       *  @see LoKi::Relations::HepMC2MC
       *  @see LoKi::Relations::MC2HepMC
       *  @see LoKi::Relations::HepMC2MC2D
       *  @param p pointer to the particle
       *  @param table pointer to table 
       */
      IsFromHepMC 
      ( const HepMC::GenParticle* p     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from vertex and a tool
      IsFromHepMC
      ( const HepMC::GenVertex*   v     , 
        const LHCb::MC2HepMC*     table ) ;
      /// constructor from range of particles
      template <class GENPARTICLE>
      IsFromHepMC
      ( GENPARTICLE first , 
        GENPARTICLE last  , 
        const LHCb::MC2HepMC* table  ) 
        : LoKi::Predicate<const LHCb::MCParticle*> ()
        , m_gps   ( first , last ) 
        , m_table ( table  )
      { getHepMC() ; } ;
      /// constructor from range of vertices
      template <class GENVERTEX>
      IsFromHepMC
      ( const LHCb::MC2HepMC* table , 
        GENVERTEX             first , 
        GENVERTEX             last  ) 
        : LoKi::Predicate<const LHCb::MCParticle*> () 
        , m_gps   (       ) 
        , m_table ( table )
      {
        for ( ; first != last ; ++first ) 
        {
          const HepMC::GenVertex* v = *first ;
          if ( 0 == v      ) { continue ; }
          HepMC::GenVertex* vertex = const_cast<HepMC::GenVertex*>( v ) ;
          if ( 0 == vertex ) { continue ; }
          m_gps.insert ( m_gps.end() , 
                         vertex->particles_begin ( HepMC::descendants ) , 
                         vertex->particles_end   ( HepMC::descendants ) ) ; 
        }
        getHepMC() ;
      };
      /** copy constructor
       *  @param rigth object to be copied 
       */
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
    protected:
      StatusCode getHepMC() ;
    private:
      // default constructor is disabled
      IsFromHepMC();
      // assignement operator is disabled 
      IsFromHepMC& operator=
      ( const IsFromHepMC& ) ;
    private:
      typedef std::vector<const HepMC::GenParticle*> GPs ;
      GPs                   m_gps   ;
      const LHCb::MC2HepMC* m_table ;
    } ;
    
  } ; // end of namespace MCParticles
  
} ; // end of namespace LoKi

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_GENMCPARTICLES_H
// ============================================================================
