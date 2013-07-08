// $Id$
// ============================================================================
#ifndef LOKI_RECVERTICES_H 
#define LOKI_RECVERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ExtraInfo.h"
#include "LoKi/TriggerTypes.h"
// ============================================================================
/** @file  
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::RecVertices
   *  Collection of  functions specific for class LHCb::RecVertex
   *  @see LHCb::RecVertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  namespace RecVertices
  {
    // ========================================================================
    /** @class MaxDOCA
     *  Get the maximal value of DOCA for rec-tracks in vertex 
     *  @see LHCb::RecVertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */
    class MaxDOCA 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      MaxDOCA() { }
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCA () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCA* clone ()  const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate DOCA for two tracks
      double     doca     ( const LHCb::Track* t1   , 
                            const LHCb::Track* t2   ) const ;
      /// evaluate DOCA for two tracks
      double     docachi2 ( const LHCb::Track* t1   , 
                            const LHCb::Track* t2   ) const ;      
      /// evaluate DOCA & chi2(DOCA) for two tracks
      StatusCode distance ( const LHCb::Track* t1   , 
                            const LHCb::Track* t2   , 
                            double&            doca , 
                            double&            chi2 ) const ;
      /// check DOCA & chi2(DOCA) for two tracks
      bool       check    ( const LHCb::Track* t1      , 
                            const LHCb::Track* t2      , 
                            const double       docamax , 
                            const double       chi2max ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MaxDOCAChi2
     *  Get the maximal value of chi2(DOCA) for rec-tracks in vertex 
     *  @see LHCb::RecVertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07
     */
    class MaxDOCAChi2 : public LoKi::RecVertices::MaxDOCA 
    {
    public:
      // ======================================================================
      /// Default Constructor
      MaxDOCAChi2() { }
      /// MANDATORY: virtual destructor 
      virtual ~MaxDOCAChi2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  MaxDOCAChi2* clone ()  const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    class DOCACut 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
    public:
      // ======================================================================
      /** constructor with cuts on max(DOCA) and max(chi2(DOCA))
       *  @param docamax  max(DOCA)       (negative: no cut)
       *  @param docamax  max(chi2(DOCA)) (negative: no cut)
       */
      DOCACut ( const double docamax ,   // max(DOCA)       (negative: no cut)
                const double chi2max ) ; // max(chi2(DOCA)) (negative: no cut)
      /// MANDATORY: virtual destructor 
      virtual ~DOCACut () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DOCACut* clone ()  const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// check DOCA & chi2(DOCA) for two tracks
      bool       check    ( const LHCb::Track* t1      , 
                            const LHCb::Track* t2      ) const 
      {
        if ( 0  == t1 || 0 == t2 ) { return false ; }
        if ( t1 == t2 )            { return true  ; }
        return m_doca.check ( t1 , t2 , m_docamax , m_chi2max ) ; 
      }
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DOCACut () ;                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// evaluator 
      LoKi::RecVertices::MaxDOCA m_doca    ; // evaluator 
      /// maximal DOCA 
      double                     m_docamax ; // maximal DOCA 
      /// maximal chi2(DOCA)
      double                     m_chi2max ; // maximal chi2(DOCA)
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                       end of namespace LoKi::RecVertices 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================    
    // Suppress Intel compiler warnings about missing default constructor
    // In this case the compiler generated constructor is fine, since there are
    // no member data to be initialised
#ifdef __INTEL_COMPILER
#pragma warning(disable:854)
#pragma warning(push)
#endif
    // ========================================================================
    /** @var RV_MAXDOCA 
     *  Simple evaluator for "DOCA-max" between all tracks in 
     *  recontructed vertex 
     * 
     *  @code 
     * 
     *   const LHCb::VertexBase* v = ... ;
     *
     *   const double docamax = RV_MAXDOCA ( v ) ;
     *
     *  @endcode 
     *  
     *  @see LHCb::RecVertex
     *  @see LoKi::RecVertices::MaxDOCA 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07     
     */
    const LoKi::RecVertices::MaxDOCA                               RV_MAXDOCA ;
    // ========================================================================
    /** @var RV_MAXDOCACHI2  
     *  Simple evaluator for "chi2(DOCA)-max" between all tracks in 
     *  recontructed vertex 
     * 
     *  @code 
     * 
     *   const LHCb::VertexBase* v = ... ;
     *
     *   const double chi2max = RV_MAXDOCACHI2 ( v ) ;
     *
     *  @endcode 
     *  
     *  @see LHCb::RecVertex
     *  @see LoKi::RecVertices::MaxDOCAChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07     
     */
    const LoKi::RecVertices::MaxDOCAChi2                       RV_MAXDOCACHI2 ;
    // ========================================================================
    /** @typedef RV_DOCACUT 
     *  check if all 2-track combinations satisfy DOCA/chi2(DOCA) cuts 
     * 
     *  @code 
     * 
     *   const RV_DOCACUT good = RV_DOCACUT ( 0.1 * mm , 36 ) ;
     *
     *   const LHCb::VertexBase* v = ... ;
     *
     *   const bool ok = good ( v )
     *
     *  @endcode 
     *  
     *  @see LHCb::RecVertex
     *  @see LoKi::RecVertices::MaxDOCAChi2
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-07     
     */
    typedef LoKi::RecVertices::DOCACut                             RV_DOCACUT ;
    // ========================================================================
#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_RECVERTICES_H
// ============================================================================
