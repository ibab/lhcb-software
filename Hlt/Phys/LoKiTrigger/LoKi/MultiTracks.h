// $Id$
// ============================================================================
#ifndef LOKI_MULTITRACKS_H 
#define LOKI_MULTITRACKS_H 1
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/HltMultiTrack.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file  
 *  Couple of functions to deal with multi-tracks
 *  @see Hlt::MultiTrack
 *
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-03-27
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace LoKi 
{
  // ==========================================================================
  namespace Candidates
  {
    // ========================================================================
    class GAUDI_API MtFun : public LoKi::BasicFunctors<const Hlt::Candidate*>::Function
    {
    public:
      // ======================================================================
      /// constructor form the functor, index  and bad-value 
      MtFun ( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun   ,
              const unsigned int                                       index , 
              const double                                             bad   ) ;
      /// constructor form the functor, index  and bad-value 
      MtFun ( const LoKi::BasicFunctors<const LHCb::Track*>::Function& fun   ,
              const unsigned int                                       index ) ;
      /// MANDATORY: virtual destructor 
      virtual ~MtFun () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MtFun* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument c ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      MtFun() ;                          // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual function
      LoKi::FunctorFromFunctor<const LHCb::Track*,double> m_fun  ; // the cut
      /// the index 
      unsigned int                                       m_index ; // the index 
      /// the bad-value 
      const double m_bad ; // the bad-value 
      // ======================================================================
   } ;   
    // ========================================================================
  } //                                        end of namespace LoKi::Candidates 
  // ==========================================================================    
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MULTITRACKS_H
// ============================================================================
