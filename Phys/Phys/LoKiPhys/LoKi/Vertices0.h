// $Id: Vertices0.h,v 1.8 2008-03-30 13:43:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_VERTICES0_H 
#define LOKI_VERTICES0_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
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
 *  @date 2006-02-16 
 */
// ============================================================================
namespace LoKi 
{
  namespace  Vertices 
  {    
    // ========================================================================
    /** @class IsPrimary
     *
     *  trivial predicate whcih evaluates to true for primary vertices 
     *  it relies on th emethod LHCb::VertexBase::isPrimary
     *
     *  @see LoKi::Cuts::PRIMARY
     *  @see LoKi::Cuts::ISPRIMARY
     *  @see LHCb::VertexBase  
     *  @see LHCb::RecVertex 
     *  @see LHCb::Vertex 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class IsPrimary 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsPrimary* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class  Technique
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::VertexBase::technique
     *
     *  @see LoKi::Cuts::TECHNIQUE
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Technique
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    { 
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual Technique* clone() const ;
      /// MANDATOY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================    
    /** @class VertexChi2
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::VertexBase::chi2
     *
     *  @see LoKi::Cuts::VCHI2
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexChi2
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class VertexDoF
     *  Evaluator of the number of degrees of freedom for the vertex 
     *  
     *  It relies on the method LHCb::VertexBase::nDoF
     *
     *  @see LoKi::Cuts::VXDOF
     *  @see LoKi::Cuts::VXNDOF
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class VertexDoF
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDoF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class PositionX 
     *  the trivial evaluator of X-position of the vertex 
     *
     *  @see LoKi::Cuts::VX
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PositionX
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionX* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class PositionY 
     *  the trivial evaluator of Y-position of the vertex 
     *
     *  @see LoKi::Cuts::VY
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PositionY
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionY* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class PositionZ 
     *  the trivial evaluator of Z-position of the vertex 
     *
     *  @see LoKi::Cuts::VZ
     *  @see LHCb::VertexBase
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class PositionZ 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionZ* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class  NumberOfOutgoing 
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::Vertex::outgoingParticles
     *  
     *  @see LoKi::Cuts::NPRONGS
     *  @see LHCb::VertexBase
     *  @see LHCb::Vertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class NumberOfOutgoing 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfOutgoing* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::VertexBase::hasInfo
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::HASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasInfo
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// copy constructor 
      HasInfo ( const HasInfo& right ) ;
      /// declassor
      virtual ~HasInfo();
      /// clone method (mandatory!)
      virtual HasInfo* clone() const ;
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasInfo();
    private:
      int m_info ;
    };
    // ========================================================================    
    /** @class Info
     *  Trivial function which evaluates LHCb::Vertex::info
     *  
     *  It relies on the method LHCb::VertexBase::info
     *
     *  @see LHCb::VertexBase
     *  @see LoKi::Cuts::INFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Info
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       *  @param bad bad value to be retured for invalid particle 
       */
      Info 
      ( const int    key , 
        const double def , 
        const double bad ) ;
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       */
      Info 
      ( const int    key , 
        const double def ) ;
      /// copy constructor 
      Info ( const Info& right ) ;
      /// declassor
      virtual ~Info();
      /// clone method (mandatory!)
      virtual Info* clone() const ;
      /// the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      Info();
    private:
      int    m_key ;
      double m_def ;
      double m_bad ;
    };
    // ========================================================================    
    /** @class  NumberOfTracks
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::RecVertex::tracks
     *  
     *  @see LoKi::Cuts::NTRACKS
     *  @see LHCb::VertexBase
     *  @see LHCb::RecVertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class NumberOfTracks 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {    
    public:
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfTracks* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    // ========================================================================
    /** @class Chi2Prob 
     *
     *  The trival functor with evaluated the chi2 probability for certain 
     *  \f$\chi^2\f$ values
     *
     *  The GSL routine <b>gsl_cdf_chisq_Q</b> is used for evaluation
     *
     *  @see LoKi::Cuts::VPCHI2N
     *  @see LoKi::Cuts::VPCHI2
     *  @see LHCb::VertexBase::chi2 
     *  @see LHCb::VertexBase::nDoF 
     *  @see LHCb::VertexBase
     *
     *  @attention: The "unphysical" values mean: 
     *  <b>"get number of DoFs from the vertex itself"</b>
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-02-28
     */  
    class Chi2Prob 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      /// MANDATORY: virtual destructor:
      virtual ~Chi2Prob() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Chi2Prob* clone() const ;
      /// MANDATORY: the only one essential method:
      virtual  result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
     } ;
    // ========================================================================
  } // end of namespace LoKi::Vertices 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES0_H
// ============================================================================
