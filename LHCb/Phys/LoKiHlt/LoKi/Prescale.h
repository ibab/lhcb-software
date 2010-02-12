// $Id: Prescale.h,v 1.1 2010-02-12 14:17:27 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PRESCALE_H 
#define LOKI_PRESCALE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
// DAQEvent
// ============================================================================
#include "Event/ODIN.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Odin 
  {
    // ========================================================================
    /** @class Prescale 
     *  Simple "functor"-implementation of Deterministic Prescaler
     *  @see LoKi::Cuts::ODIN_PRESCALE
     *  @see DeterministicPrescaler 
     *  The idea & code belongs to Gerhard The Great Raven
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2010-02-12
     */
    class Prescale : public LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor from the accept fraction and the seed 
      Prescale ( const double       accept      , 
                 const std::string& seed   = "" ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Prescale() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Prescale* clone() const ;
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument o ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the defautl constructor is disabled 
      Prescale() ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the accept fraction 
      double       m_accept  ;                           // the accept fraction 
      /// the "seed" 
      std::string  m_seed    ;                           //          the "seed" 
      /// the initial value 
      unsigned int m_initial ;                           //   the initial value 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                              end of namepsace LoKi::Odin
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PRESCALE_H
// ============================================================================
