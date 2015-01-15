// $Id$
// ============================================================================
#ifndef LOKI_MCVERTICES_H
#define LOKI_MCVERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// STL & STD
// ============================================================================
#include <string>
#include <math.h>
// ============================================================================
// Event
// ============================================================================
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Functions.h"
#include "LoKi/Operators.h"
#include "LoKi/Constants.h"
#include "LoKi/MCTypes.h"
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
 *  @date 2001-01-23
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace  LoKi::MCVertices MCVertices.h LoKi/MCVertices.h
   *
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-07-07
   */
  namespace MCVertices
  {
    // ========================================================================
    /** @class TypeOfMCVertex
     *  evaluator of MC verte type
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API TypeOfMCVertex
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      TypeOfMCVertex() {}
      /// destructor
      virtual ~TypeOfMCVertex();
      /// clone method (mandatory)
      virtual TypeOfMCVertex* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TimeOfFlight
     *  evaluator of MC vertex TOF
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API TimeOfFlight
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      TimeOfFlight() {}
      /// destructor
      virtual ~TimeOfFlight();
      /// clone method (mandatory)
      virtual TimeOfFlight* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexPositionX
     *  evaluator of x-position of MC vertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API VertexPositionX
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      VertexPositionX() {}
      /// destructor
      virtual ~VertexPositionX();
      /// clone method (mandatory)
      virtual VertexPositionX* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexPositionY
     *  evaluator of y-position of MC vertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API VertexPositionY
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      VertexPositionY() {}
      /// destructor
      virtual ~VertexPositionY();
      /// clone method (mandatory)
      virtual VertexPositionY* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexPositionZ
     *  evaluator of z-position of MC vertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API VertexPositionZ
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      VertexPositionZ() {}
      /// destructor
      virtual ~VertexPositionZ();
      /// clone method (mandatory)
      virtual VertexPositionZ* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class VertexTime
     *  evaluator of "vertex time" (?)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    class GAUDI_API VertexTime
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      VertexTime() {}
      /// destructor
      virtual ~VertexTime();
      /// clone method (mandatory)
      virtual VertexTime* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Primary
     *  return true for 'primary' vertices
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    class GAUDI_API Primary
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Predicate
    {
    public:
      // ======================================================================
      /// default constructor
      Primary() {}
      /// destructor
      virtual ~Primary();
      /// clone method (mandatory)
      virtual Primary* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Decay
     *  return true for 'decay' vertices
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    class GAUDI_API Decay
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Predicate
    {
    public:
      // ======================================================================
      /// default constructor
      Decay() {}
      /// destructor
      virtual ~Decay();
      /// clone method (mandatory)
      virtual Decay* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MCVertexDistance
     *  evaluator of distance from MC verted to 3D-point
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API MCVertexDistance
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /** constructor from LoKi::Point3D
       *  @param point 3D-point
       */
      MCVertexDistance ( const LoKi::Point3D& point ) ;
      /** constructor from MCVertex
       *  @param point 3D-point
       */
      MCVertexDistance ( const LHCb::MCVertex*   point ) ;
      /// virtual descructor
      virtual ~MCVertexDistance() ;
      /// clone method (mandatory!)
      virtual  MCVertexDistance* clone() const ;
      /// the only one essential method
      virtual  result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      MCVertexDistance() ;
      // ======================================================================
    private:
      // ======================================================================
      LoKi::Point3D m_point ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MCVFunAsMCFun
     *  simple adapter function which allows to use MC Veretx function as
     *  MC particle function
     *  Adapter makes use of MCParticle::originVertex function
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex
     *  @see LoKi::Cuts::MCVXFUN
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API MCVFunAsMCFun
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Function
    {
    public:
      // ======================================================================
      /// constructor from MC Vertex fuction
      MCVFunAsMCFun ( const LoKi::MCTypes::MCVFunc& func   ) ;
      /// virtual descructor
      virtual ~MCVFunAsMCFun() ;
      /// clone method (mandatory!)
      virtual  MCVFunAsMCFun* clone() const ;
      /// the only one essential method
      virtual  result_type    operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      MCVFunAsMCFun () ;
      // ======================================================================
    protected:
      // ======================================================================
      const LoKi::MCTypes::MCVFunc& func() const { return m_fun.func() ; }
      // ======================================================================
    private:
      // ======================================================================
      LoKi::MCTypes::MCVFun m_fun ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MCVFunction
     *  simple adapter function which allows to use MC Veretx function as
     *  MC particle function
     *  Adapter makes use of MCParticle::originVertex function
     *  @see LHCb::MCParticle
     *  @see LHCb::MCVertex
     *  @see LoKi::Cuts::MCVFUN
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API MCVFunction : public LoKi::MCVertices::MCVFunAsMCFun
    {
    public:
      // ======================================================================
      enum {
        Regular  =  0 ,
        Last     = -1 ,
        Origin   = -2 ,
        Primary  = -3
      } ;
      // ======================================================================
    public:
      // ======================================================================
      /** constructor from MC Vertex fuction
       *  @param func vertex function tobe applied
       *  @param index vertex index :
       *         -1   - the last vertex  in the list
       *         -2   - the origin vertex
       *         -3   - the primary ertex
       */
      MCVFunction ( const LoKi::MCTypes::MCVFunc& func  ,
                    const int                     index ) ;
      /// constructor from MC Vertex fuction and vertex selection
      MCVFunction ( const LoKi::MCTypes::MCVFunc& func  ,
                    const LoKi::MCTypes::MCVCuts& cuts  ) ;
      /// constructor from MC Vertex fuction and vertex selection
      MCVFunction ( const LoKi::MCTypes::MCVCuts& cuts  ,
                    const LoKi::MCTypes::MCVFunc& func  ) ;
      /// virtual descructor
      virtual ~MCVFunction () ;
      /// clone method (mandatory!)
      virtual  MCVFunction* clone() const ;
      /// the only one essential method
      virtual  result_type    operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      MCVFunction () ;
      // ======================================================================
    private:
      // ======================================================================
      bool                  m_case   ;
      int                   m_index  ;
      LoKi::MCTypes::MCVCut m_cut    ;
      // ======================================================================
    };
    // ========================================================================
    /** @class Key
     *  get the key of MC-vertex
     *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
     *  @date   2011-02-14
     */
    class GAUDI_API Key
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      // ======================================================================
      /// default constructor
      Key() {}
      /// destructor
      virtual ~Key();
      /// clone method (mandatory)
      virtual Key* clone() const ;
      /// the only one essential method
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    };
    // ========================================================================
    /** @class MCVPFunAsMCFun
     *  simple adapter function which allows to use MC Veretx function as
     *  MC particle function
     *  Adapter makes use of MCParticle::primaryVertex function
     *  @see MCParticle
     *  @see MCVertex
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API MCVPFunAsMCFun : public MCVFunAsMCFun
    {
    public:
      // ======================================================================
      /// constructor from MC Vertex fuction
      MCVPFunAsMCFun ( const LoKi::MCTypes::MCVFunc& func   ) ;
      /// virtual descructor
      virtual ~MCVPFunAsMCFun() ;
      /// clone method (mandatory!)
      virtual  MCVPFunAsMCFun* clone() const ;
      /// the only one essential method
      virtual  result_type    operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      MCVPFunAsMCFun () ;
      // ======================================================================
    };
  } //                                        end of namespace LoKi::MCVertices
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_MCVERTICES_H
// ============================================================================
