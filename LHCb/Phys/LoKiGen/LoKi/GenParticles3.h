// $Id$
// ============================================================================
#ifndef LOKI_GENPARTICLES3_H 
#define LOKI_GENPARTICLES3_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/GenTypes.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace GenParticles
  {
    // ========================================================================
    /** @class Count
     *  simple class which counts how many particles 
     *  satisfy the certain criteria
     *
     *  @see LoKi::Cuts::GCOUNT
     *  @see HepMC::GenParticle
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@itep.ru
     *  @date   2012-01-28
     */
    class GAUDI_API Count : public LoKi::GenTypes::GFunc 
    {
    public:
      // ======================================================================
      /** constructor from the criteria and "range"
       *  @param cut the criteria
       *  @param range search region 
       *  @see HepMC::IteratorRange 
       */
      Count
      ( const LoKi::Types::GCuts& cut   , 
        HepMC::IteratorRange      range ) ;
      /** constructor from the criteria and "range"
       *  @param range search region 
       *  @param cut the criteria
       *  @see HepMC::IteratorRange 
       */
      Count
      ( HepMC::IteratorRange      range ,
        const LoKi::Types::GCuts& cut   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Count ();
      /// MANDATORY: clone method ("virtual contructor")
      virtual Count * clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Count () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Types::GCut    m_cut   ;
      HepMC::IteratorRange m_range ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Has
     *  simple class which check the presence of particles 
     *  satisfy the certain criteria
     *
     *  @see LoKi::Cuts::GHAS
     *  @see HepMC::GenParticle
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@itep.ru
     *  @date   2012-01-28
     */
    class GAUDI_API Has : public LoKi::GenTypes::GCuts
    {
    public:
      // ======================================================================
      /** constructor from the criteria and "range"
       *  @param cut the criteria
       *  @param range search region 
       *  @see HepMC::IteratorRange 
       */
      Has
      ( const LoKi::Types::GCuts& cut   , 
        HepMC::IteratorRange      range ) ;
      /** constructor from the criteria and "range"
       *  @param range search region 
       *  @param cut the criteria
       *  @see HepMC::IteratorRange 
       */
      Has
      ( HepMC::IteratorRange      range ,
        const LoKi::Types::GCuts& cut   ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor
      virtual ~Has ();
      /// MANDATORY: clone method ("virtual contructor")
      virtual  Has* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Has () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Types::GCut    m_cut   ;
      HepMC::IteratorRange m_range ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                      end of namespace LoKi::GenParticles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef GCOUNT 
     *  simple functor to count certain particles in HepMC-graph 
     * 
     *  @code 
     * 
     *   const GCOUNT cnt = GCOUNT ( "pi+" == GABSID , HepMC.descendents ) ;
     *
     *   const HepMC::GenParticle* p = ... ;
     *
     *   const double numPi = cnt ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi:GenParticles::Count 
     *  @see LoKi:GenVertices::CountIF 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::Count                                  GCOUNT ;
    // ========================================================================
    /** @typedef GHAS
     *  simple functor to check certain particles in HepMC-graph 
     *  @code 
     * 
     *   const GHAS chk = GHAS ( "pi+" == GABSID , HepMC.descendents ) ;
     *
     *   const HepMC::GenParticle* p = ... ;
     *
     *   const bool hasPi = chk ( p ) ;
     *
     *  @endcode 
     *
     *  @see LoKi:GenParticles::Has
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-01-28
     */
    typedef LoKi::GenParticles::Has                                      GHAS ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GENPARTICLES3_H
// ============================================================================
