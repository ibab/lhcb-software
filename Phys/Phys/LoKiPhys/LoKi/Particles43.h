// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES43_H 
#define LOKI_PARTICLES43_H 1
// ============================================================================
// Include files
// ============================================================================
/** @file LoKi/Particles43.h
 *  
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2012-07-25
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
namespace LoKi
{
  // ==========================================================================
  namespace Particles
  {
    // ========================================================================
    /** @class SumInR2Cone
     *  sum some function over R2-cone
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-07-25
     * 
     */
    class SumInR2Cone 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::UniqueKeeper<LHCb::Particle>
    {
      // ======================================================================
    public:
      // ======================================================================
      /** constructor form value of \f$\left(\Delta R\right)^2\f$
       *  @param dr2 the DR2-value
       *  @param fun the functor for summation 
       *  @param parts the particles 
       *  @param init  the initial value 
       */
      SumInR2Cone
      ( const double                                                dr2 ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun , 
        const LHCb::Particle::Range& parts  = LHCb::Particle::Range()   , 
        const double                 init   = 0                         ) ;
      // ======================================================================
      /// MANDATORY: virtual destructor 
      virtual ~SumInR2Cone () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SumInR2Cone* clone() const ;
      /// MANDATORY: the only essential method
      virtual  result_type operator() ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SumInR2Cone () ;                  // the default constructor is disabled 
      // ======================================================================
    protected:
      // ======================================================================
      /// calcualte the sum 
      double sum ( const LHCb::Particle* p ) const ; // calcualte the sum 
      double dr2  () const { return m_dr2  ; }
      double init () const { return m_init ; }      
      // ======================================================================
    private:
      // ======================================================================
      /// the function to be summed 
      LoKi::Types::Fun  m_fun  ;                   // the function to be summed 
      /// the DR2 value 
      double            m_dr2  ;                           //     the DR2 value 
      /// the initial value 
      double            m_init ;                           // the initial value 
      // ======================================================================      
    } ;
    // ========================================================================
    /** @class SumInR2ConeWithSource
     *  sum some function over R2-cone 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-07-25
     * 
     */
    class SumInR2ConeWithSource : public SumInR2Cone 
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the source 
      SumInR2ConeWithSource
      ( const double                                                 dr2      ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Function&  fun      , 
        const LoKi::BasicFunctors<const LHCb::Particle*>::Source&    source   ,
        const double                                                 init   = 0 ) ;
      /// constructor from the TES location
      SumInR2ConeWithSource
      ( const double                                                 dr2      ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Function&  fun      , 
        const std::string&                                           location ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut     = 
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true )   , 
        const double                                                 init    = 0 ) ;
      /// constructor from the TES locations 
      SumInR2ConeWithSource
      ( const double                                                dr2       ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Function& fun       , 
        const std::vector<std::string>&                             locations ,
        const LoKi::BasicFunctors<const LHCb::Particle*>::Predicate& cut     =
        LoKi::BasicFunctors<const LHCb::Particle*>::BooleanConstant( true )   , 
        const double                                                 init    = 0 ) ;
      /// MANDATORY : virtual destructor 
      virtual ~SumInR2ConeWithSource () ;
      /// MANDATORY : clone method ("virtual constructor")
      virtual  SumInR2ConeWithSource* clone () const ;
      /// MANDATORY: the only essential method
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================      
    protected:
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::Particle*>::Source   iSource ;
      typedef LoKi::Assignable<iSource>::Type                       Source ;
      // ======================================================================
      const iSource& source() const { return m_source ; }
      // ======================================================================      
    private:
      // ======================================================================
      /// the source 
      Source m_source ;                                           // the source 
      // ======================================================================      
      // 
    } ;  
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef SUMINR2CONE
     *  @see LoKi::Particles::SumInR2Cone
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-07-25
     */
    typedef LoKi::Particles::SumInR2Cone                          SUMINR2CONE ;
    // ========================================================================
    /** @typedef SUMINCONE
     *  @see LoKi::Particles::SumInR2ConeWithSource
     * 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-07-25
     */
    typedef LoKi::Particles::SumInR2ConeWithSource                    SUMCONE ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES43_H
// ============================================================================
