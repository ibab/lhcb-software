// $Id: Vertices.h,v 1.1.1.1 2006-02-17 19:17:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_VERTICES_H 
#define LOKI_VERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/PhysRangeTypes.h"
#include "LoKi/VertexHolder.h"
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
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-16 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace  Vertices Vertices.h LoKi/Vertices.h
   *  
   *  Namespace with collection of "Particle" functions for LoKi
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-16
   */
  namespace  Vertices 
  {    
    /** @struct IsPrimary
     *
     *  trivial predicate whcih evaluates to truw for primary vertices 
     *  it relies on th emethod LHCb::Vertex::isPrimary
     *
     *  @see LoKi::Cuts::PRIMARY
     *  @see LoKi::Cuts::ISPRIMARY
     *  @see LHCb::Vertex 
     *  @see LHCb::PrimVertex 
     *  
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct IsPrimary : 
      public LoKi::Predicate<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual IsPrimary* clone() const  ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
    { 
      /// MANDATORY: clone method ("virtual constructor")
      virtual Technique* clone() const ;
      /// MANDATOY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexChi2* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual VertexDoF* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionX* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionY* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual PositionZ* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfOutgoing* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
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
      : public LoKi::Predicate<const LHCb::Vertex*>
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
      result_type operator() ( argument p ) const ;
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
      : public LoKi::Function<const LHCb::Vertex*>
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
      result_type operator() ( argument p ) const ;
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
     *  It relies on the method LHCb::PrimVertex::outgoingParticles
     *  
     *  @see LoKi::Cuts::NTRACKS
     *  @see LHCb::PrimVertex
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2002-07-15
     */
    struct NumberOfTracks
      : public LoKi::Function<const LHCb::Vertex*>
    {      
      /// MANDATORY: clone method ("virtual constructor")
      virtual NumberOfTracks* clone() const ;
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    };
    
    /** @class VertexDistance
     *  evaluator of the distance
     *  between vertex and the 'vertex'
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexDistance : 
      public    LoKi::Function<const LHCb::Vertex*> , 
      public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexDistance 
      ( const LHCb::Vertex*  vertex ) ;
      /// constructor 
      VertexDistance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexDistance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// copy constructor 
      VertexDistance ( const VertexDistance& right ) ;
      /// destructor 
      virtual ~VertexDistance();
      /// clone method (mandatory!)
      virtual VertexDistance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         distance   ( argument v ) const ;
    private:
      /// default constructor is private 
      VertexDistance();
    };
    
    /** @class VertexChi2Distance
     *  evaluator of the distance in chi2 units  
     *  between vertex and the 'vertex'
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class VertexChi2Distance : 
      public    LoKi::Function<const LHCb::Vertex*> , 
      public    LoKi::Vertices::VertexHolder 
    {
    public:
      /// constructor 
      VertexChi2Distance 
      ( const LHCb::Vertex*  vertex ) ;
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Point3D& vertex ) ;      
      /// constructor 
      VertexChi2Distance 
      ( const LoKi::Vertices::VertexHolder& base ) ;
      /// copy constructor 
      VertexChi2Distance ( const VertexChi2Distance& right ) ;
      /// destructor 
      virtual ~VertexChi2Distance();
      /// clone method (mandatory!)
      virtual VertexChi2Distance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      /** the only one essential method 
       *  @param p pointer to the vertex 
       *  @return the estimate of the distance between vertex and the 'vertex'
       */
      result_type         chi2       ( argument v ) const ;
    private:
      /// default constructor is private 
      VertexChi2Distance();
    };
    
    /** @class MinVertexChi2Distance
     *  class for evaluation of minimal chi2 distance 
     *  between the vertex and other vertices 
     *
     *  @author Vanya ELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-08
     */
    class MinVertexChi2Distance :
      public    LoKi::Function<const LHCb::Vertex*> 
    {
    protected:
      typedef std::vector<const LHCb::Vertex*> Vrtxs    ;
      typedef FunB::vector_result              Results  ;
    public:
      /** constructor from only vertex 
       *  @param vertex the vertex 
       */
      MinVertexChi2Distance 
      ( const LHCb::Vertex* vertex ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::Vertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::Vertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::PrimVertex::Vector& vs ) ;      
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::PrimVertex::ConstVector& vs ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LHCb::PrimVertices*   vs  ) ;
      /** constructor from container of vertices 
       *  @param vs container of primary vertices 
       */
      MinVertexChi2Distance 
      ( const LoKi::PhysTypes::VRange& vs ) ;
      /** templated constructor from arbitrary sequence 
       *  of vertices and helper object 
       *  @param first begin iterator of arbitrary sequence of primary vertices
       *  @param last  end iterator of arbitrary sequence of primary vertices
       */
      template <class VERTEX>
      MinVertexChi2Distance 
      ( VERTEX                          first , 
        VERTEX                          last  )
        : LoKi::Function<const LHCb::Vertex*> () 
        , m_vertices  ( first , last    )
        , m_evaluator ( LoKi::Point3D() )
        , m_results   ( last - first    )
      {}
      /// copy  constructor
      MinVertexChi2Distance 
      ( const MinVertexChi2Distance& right ) ;
      // destructor 
      virtual ~MinVertexChi2Distance();
      /// clone method (mandatory!)
      virtual  MinVertexChi2Distance* clone() const ;
      /** the only one essential method 
       *  @param p pointer to the particle 
       *  @return evaluation of impact parameters 
       *          with respect to point/vertex 
       */
      virtual result_type operator() ( argument v ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default constructor is disabled 
      MinVertexChi2Distance () ;
    private:
      Vrtxs                                      m_vertices  ;
      mutable LoKi::Vertices::VertexChi2Distance m_evaluator ;
      mutable Results                            m_results   ;
    };

  } ; // end of namespace LoKi::Vertices 
} ; // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_VERTICES_H
// ============================================================================
