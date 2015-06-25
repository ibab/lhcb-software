// $Id$
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 *
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
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
    class GAUDI_API IsPrimary 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {      
    public:
      // ======================================================================
      /// Default Constructor
      IsPrimary() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsPrimary* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API Technique
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    { 
    public:
      // ======================================================================
      /// Default Constructor
      Technique() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual Technique* clone() const ;
      /// MANDATOY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API VertexChi2
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      VertexChi2() { }
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      double chi2( const LHCb::VertexBase& v ) const { return v.chi2() ; }
      // ======================================================================
    };
    // ========================================================================    
    /** @class VertexChi2PerDof
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::VertexBase::chi2
     *
     *  @see LoKi::Cuts::VCHI2PDOF
     *  @see LHCb::VertexBase 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2008-06-20
     */
    class GAUDI_API VertexChi2PerDoF : public VertexChi2 
    {      
    public:
      // ======================================================================
      /// Default Constructor
      VertexChi2PerDoF() { }
      /// MANDATORY: virtual destructor 
      virtual ~VertexChi2PerDoF() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2PerDoF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      inline double chi2PerDoF( const LHCb::VertexBase& v ) const 
      { return v.nDoF() != 0 ? v.chi2() / v.nDoF() : 0 ; }
      // ======================================================================
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
    class GAUDI_API VertexDoF
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      VertexDoF() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDoF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API PositionX
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      PositionX() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionX* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API PositionY
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      PositionY() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionY* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API PositionZ 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      PositionZ() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionZ* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class PositionRho
     *  the trivial evaluator of Rho-position of the vertex 
     *
     *  @see LoKi::Cuts::VRHO
     *  @see LHCb::VertexBase
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2015-06-25
     */
    class GAUDI_API PositionRho
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      PositionRho() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionRho* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API NumberOfOutgoing 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {      
    public:
      // ======================================================================
      /// Default Constructor
      NumberOfOutgoing() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfOutgoing* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
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
    class GAUDI_API HasInfo
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      HasInfo();
      // ======================================================================
    private:
      // ======================================================================
      int m_info ;
      // ======================================================================
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
    class GAUDI_API Info
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      // ======================================================================
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
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      Info();
      // ======================================================================
    private:
      // ======================================================================
      int    m_key ;
      double m_def ;
      double m_bad ;
      // ======================================================================
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
    class GAUDI_API NumberOfTracks 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {    
    public:
      /// Default Constructor
      NumberOfTracks() { }
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
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2008-02-28
     */  
    class GAUDI_API Chi2Prob 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      // ======================================================================
      /// Default Constructor
      Chi2Prob() { }
      /// MANDATORY: virtual destructor:
      virtual ~Chi2Prob() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Chi2Prob* clone() const ;
      /// MANDATORY: the only one essential method:
      virtual  result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;      
      // ======================================================================
     } ;
    // ========================================================================
    /** @class IsVertex 
     *  simple  checker of vertex type 
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::ISVERTEX 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */
    class IsVertex 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsVertex() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsVertex* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class IsRecVertex 
     *  simple  checker of vertex type 
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::ISRECVERTEX 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */
    class IsRecVertex 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Predicate
    {
    public:
      // ======================================================================
      /// Default Constructor
      IsRecVertex() { }
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsRecVertex* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Cov2 
     *  accessor to covariance matrix 
     *  @see LHCb::VertexBase
     *  @see LHCb::VertexBase::covMatrix 
     *  @see LoKi::Cuts::VCOV2 
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date 2010-12-04
     */     
    class Cov2 : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Function
    {
    public:
      // ======================================================================
      /// constructor from the indices 
      Cov2 ( const std::size_t i , 
             const std::size_t j ) ;
      /// MANDATORY: virtual destructor
      virtual ~Cov2 () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Cov2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constriuctor is disabled 
      Cov2 () ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the indices 
      std::size_t m_i ; // the indices 
      /// the indices 
      std::size_t m_j ; // the indices 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace LoKi::Vertices 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_VERTICES0_H
// ============================================================================
