// $Id: Vertices0.h,v 1.4 2007-04-16 16:16:27 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/11/27 12:01:32  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/08/15 15:13:25  ibelyaev
//  update for new inheritance scheme Vertex<--VertexBase-->RecVertex
//
// Revision 1.1  2006/02/19 21:49:12  ibelyaev
//  restructirisation + new funtions
//
// Revision 1.1.1.1  2006/02/17 19:17:26  ibelyaev
// New package: "Phys"-part of restructurized LoKi project  
//
// ============================================================================
#ifndef LOKI_VERTICES0_H 
#define LOKI_VERTICES0_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================

namespace LoKi 
{
  namespace  Vertices 
  {    
    /** @struct IsPrimary
     *
     *  trivial predicate whcih evaluates to true for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
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
    struct IsPrimary : 
      public LoKi::Predicate<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsPrimary* clone() const  ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct  Technique
     *  evaluator of the "technique" used for the vertex 
     *
     *  It relies on the method LHCb::Vertex::technique
     *
     *  @see LoKi::Cuts::TECHNIQUE
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    class Technique 
      : public LoKi::Function<const LHCb::VertexBase*>
    { 
      /// MANDATORY: clone method ("virtual constructor")
      virtual Technique* clone() const ;
      /// MANDATOY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    
    /** @struct VertexChi2
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::chi2
     *
     *  @see LoKi::Cuts::VCHI2
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct VertexChi2 : 
      public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };


    /** @struct VertexDoF
     *  evaluator of the Chi2 of the vertex  
     *  
     *  It relies on the method LHCb::Vertex::nDoF
     *
     *  @see LoKi::Cuts::VXDOF
     *  @see LoKi::Cuts::VXNDOF
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct VertexDoF : 
      public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDoF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct PositionX 
     *  the trivial evaluator of X-position of the vertex 
     *
     *  @see LoKi::Cuts::VX
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PositionX 
      : public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionX* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct PositionY 
     *  the trivial evaluator of Y-position of the vertex 
     *
     *  @see LoKi::Cuts::VY
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PositionY 
      : public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionY* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };

    /** @struct PositionZ 
     *  the trivial evaluator of Z-position of the vertex 
     *
     *  @see LoKi::Cuts::VZ
     *  @see LHCb::Vertex 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct PositionZ
      : public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionZ* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @struct  NumberOfOutgoing 
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::Vertex::outgoingParticles
     *  
     *  @see LoKi::Cuts::NPRONGS
     *  @see LHCb::Vertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct NumberOfOutgoing 
      : public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfOutgoing* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::Vertex::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Vertex::hasInfo
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::HASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasInfo
      : public LoKi::Predicate<const LHCb::VertexBase*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// copy constructor 
      HasInfo ( const HasInfo& right ) ;
      /// destructor
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
    
    /** @class Info
     *  Trivial function which evaluates LHCb::Vertex::info
     *  
     *  It relies on the method LHCb::Vertex::info
     *
     *  @see LHCb::Vertex
     *  @see LoKi::Cuts::INFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Info
      : public LoKi::Function<const LHCb::VertexBase*>
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
      /// destructor
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
    
    /** @struct  NumberOfTracks
     *  The trivial function which evalautes to 
     *  number of outgoing particles 
     *  It relies on the method LHCb::RecVertex::outgoingParticles
     *  
     *  @see LoKi::Cuts::NTRACKS
     *  @see LHCb::RecVertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct NumberOfTracks
      : public LoKi::Function<const LHCb::VertexBase*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfTracks* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
  }  // end of namespace LoKi::Vertices 
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES0_H
// ============================================================================
