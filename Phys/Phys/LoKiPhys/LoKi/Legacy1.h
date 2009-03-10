// $Id: Legacy1.h,v 1.2 2009-03-10 22:49:57 spradlin Exp $
// ============================================================================
#ifndef LOKI_LEGACY1_H 
#define LOKI_LEGACY1_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/AuxDesktopBase.h"
#include "LoKi/VertexHolder.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Legacy
   *  the namespace which contains some "legacy" objects
   *  @todo the file LoKi/Legacy1.h and namespace LoKi::Legacy to be removed! 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-06-03
   */
  namespace Legacy 
  {
    // ========================================================================
    /** @class DistanceSignificanceWithBestDV
     *  simple functor which makes the "incorrect" evaluation of the 
     *  significance distance between the particle's vertex anf the best 
     *  related primary vertex. The significance is evaluated (incorrectly) 
     *  as the ratio of distance over the uncertanty of the distance.  
     *  The related primary vertex is extarcted form the desktop. 
     *  @see LoKi::Cuts::BPVVDS_LEGACY
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-06-03
     */
    class DistanceSignificanceWithBestPV
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~DistanceSignificanceWithBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  DistanceSignificanceWithBestPV* clone() const 
      { return new DistanceSignificanceWithBestPV ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "BPVVDS_LEGACY" ; }
    } ;
    // ========================================================================

    // ========================================================================
    /** @class TrgPointingScoreWithPt
     *  Evaluator of the custom 'pointing angle' sometimes used in
     *  trigger applications:
     *  \f[
     *    a = \frac{p \sin\theta}{p \sin\theta + \sum_{\mathrm{daug}} p_{t,i}},
     *  \f]
     *  where \f$p\f$ is the total momentum of the particle,
     *  \f$\sin\theta\f$ is the angle between the particle momentum and
     *  the particle flight direction, and the \f$p_{t,i}\f$ are the
     *  transverse momenta of the particle descendents.
     *
     *  Not exactly a pointing angle, more a weighted comparison of
     *  transverse momenta where the transverse momentum of the parent
     *  is with respect to the flight direction, but the transverse momenta
     *  of the daughters are with respect to the z-axis and are added in
     *  magnitude rather than as vectors.
     *
     *  @see LoKi::Cuts::TRGPOINTINGWPT
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    class TrgPointingScoreWithPt 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
      , public LoKi::Vertices::VertexHolder
    {
    public:
      // ======================================================================
      /// constructor from vertex 
      TrgPointingScoreWithPt ( const LHCb::VertexBase* vertex ) ;
      /// constructor from point 
      TrgPointingScoreWithPt ( const LoKi::Point3D&    point  ) ;
      /// constructor from the holder 
      TrgPointingScoreWithPt ( const LoKi::Vertices::VertexHolder& holder ) ;
      /// copy constructor 
      TrgPointingScoreWithPt ( const TrgPointingScoreWithPt& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~TrgPointingScoreWithPt() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrgPointingScoreWithPt* clone() const 
      { return new LoKi::Legacy::TrgPointingScoreWithPt(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const
      { return pointing ( p ) ; }  
      /// OPTIONAL: the specific printout
      virtual std::ostream& fillStream( std::ostream& s ) const
      { return s << "TRGPOINTINGWPT" ; }
      // ======================================================================
    public:
      // ======================================================================
      result_type pointing ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is private 
      TrgPointingScoreWithPt();
      // ======================================================================
    } ;
    // ========================================================================

    // ========================================================================
    /** @class TrgPointingScoreWithPtWithBestPV
     *  The special version of LoKi::Legacy::TrgPointingScoreWithPt functor,
     *  which gets the related primary vertex from IPhysDesktop tool
     *
     *  @see LoKi::Cuts::BPVTRGPOINTINGWPT
     *  @see IPhysDesktop 
     *  @see LoKi::getPhysDesktop
     *  @see LoKi::Legacy::TrgPointingScoreWithPt
     *
     *  @attention There are no direct needs to use this "Context" 
     *             functor inside the native LoKi-based C++ code, 
     *             there are more efficient, transparent, 
     *             clear and safe analogues...
     * 
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    class TrgPointingScoreWithPtWithBestPV
      : public LoKi::Legacy::TrgPointingScoreWithPt 
      , public virtual LoKi::AuxDesktopBase 
    {
    public:
      /// the default constructor, creates the object in invalid state 
      TrgPointingScoreWithPtWithBestPV () ;
      /// MANDATORY: virtual destructor 
      virtual ~TrgPointingScoreWithPtWithBestPV () {} ;
      /// MANDATORY: the clone method ("virtual constructor")
      virtual  TrgPointingScoreWithPtWithBestPV* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout
      virtual  std::ostream& fillStream ( std::ostream& s ) const 
      { return s << "BPVTRGPOINTINGWPT" ; }
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Legacy
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @var BPVVDS_LEGACY 
     *  Sime functor to evaluate the incorrect distance significance
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2008-06-03 
     */
    const LoKi::Legacy::DistanceSignificanceWithBestPV          BPVVDS_LEGACY ;
    // ========================================================================

    // ========================================================================
    /** @typedef TRGPOINTINGWPT
     *  Simple function that evaluates to the custom 'pointing angle'
     *  sometimes used in trigger applications.
     *
     *  @see LoKi::Legacy::TrgPointingScoreWithPt
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    typedef LoKi::Legacy::TrgPointingScoreWithPt              TRGPOINTINGWPT ;
    // ========================================================================

    // ========================================================================
    /** @var BPVTRGPOINTINGWPT
     *  The special "context-dependent" version of TRGPOINTINGWPT functor.
     *  It takes the best primary vertex from IPhysDesktop tool
     *
     *  @see LoKi::Cuts::TRGPOINTINGWPT
     *  @see LoKi::Legacy::TrgPointingScoreWithPt
     *  @see LoKi::Legacy::TrgPointingScoreWithPtWithBestPV
     *  @author patrick spradlin
     *  @date   10 March 2009
     */
    const LoKi::Legacy::TrgPointingScoreWithPtWithBestPV  BPVTRGPOINTINGWPT ;
    // ========================================================================
  } // end of namespace LoKi::Cuts 
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
#endif // LOKI_LEGACY1_H
// ============================================================================
// The END 
// ============================================================================
