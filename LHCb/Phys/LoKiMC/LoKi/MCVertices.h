// $Id: MCVertices.h,v 1.7 2007-11-28 14:13:59 ibelyaev Exp $
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
  /** @namespace  LoKi::MCVertices MCVertices.h LoKi/MCVertices.h
   *  
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-07-07
   */
  namespace MCVertices 
  {
    // ========================================================================
    /** @struct TypeOfMCVertex 
     *  evaluator of MC verte type 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    struct TypeOfMCVertex 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual TypeOfMCVertex* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================    
    /** @struct TimeOfFlight
     *  evaluator of MC vertex TOF 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    struct TimeOfFlight 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual TimeOfFlight* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================    
    /** @struct VertexPositionX
     *  evaluator of x-position of MC vertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    struct VertexPositionX 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual VertexPositionX* clone() const ; 
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================
    /** @struct VertexPositionY
     *  evaluator of y-position of MC vertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    struct VertexPositionY 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual VertexPositionY* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================
    /** @struct VertexPositionZ
     *  evaluator of z-position of MC vertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    struct VertexPositionZ 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual VertexPositionZ* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================    
    /** @struct VertexTime 
     *  evaluator of "vertex time" (?)
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    struct VertexTime 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
      /// clone method (mandatory)
      virtual VertexTime* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================
    /** @struct Primary
     *  return true for 'primary' vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    struct Primary
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Predicate
    {
      /// clone method (mandatory)
      virtual Primary* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================
    /** @struct Decay
     *  return true for 'decay' vertices 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-06
     */
    struct Decay
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Predicate
    {
      /// clone method (mandatory)
      virtual Decay* clone() const ;
      /// the only one essential method 
      result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    };
    // ========================================================================    
    /** @class MCVertexDistance
     *  evaluator of distance from MC verted to 3D-point 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class MCVertexDistance 
      : public LoKi::BasicFunctors<const LHCb::MCVertex*>::Function
    {
    public:
      /** constructor from LoKi::Point3D
       *  @param point 3D-point 
       */
      MCVertexDistance ( const LoKi::Point3D& point ) ;      
      /** constructor from MCVertex
       *  @param point 3D-point 
       */
      MCVertexDistance ( const LHCb::MCVertex*   point ) ;      
      /// copy constructor 
      MCVertexDistance ( const MCVertexDistance& ) ;
      /// virtual destructor 
      virtual ~MCVertexDistance() ;  
      /// clone method (mandatory!)
      virtual  MCVertexDistance* clone() const ;
      /// the only one essential method 
      virtual  result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    private:
      MCVertexDistance() ;
    private:
      LoKi::Point3D m_point ;
    };
    // ========================================================================    
    /** @class MCVFunAsMCFun
     *  simple adapter function which allows to use MC Veretx function as 
     *  MC particle function
     *  Adapter makes use of MCParticle::originVertex function 
     *  @see MCParticle
     *  @see MCVertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class MCVFunAsMCFun 
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Function
    {
    public:
      /// constructor from MC Vertex fuction
      MCVFunAsMCFun ( const LoKi::MCTypes::MCVFunc& func   ) ;
      /// copy constructor 
      MCVFunAsMCFun ( const MCVFunAsMCFun&          right  ) ;
      /// virtual destructor 
      virtual ~MCVFunAsMCFun() ;
      /// clone method (mandatory!)
      virtual  MCVFunAsMCFun* clone() const ;
      /// the only one essential method 
      virtual  result_type    operator() ( argument p ) const ;      
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;    
    private:
      LoKi::MCTypes::MCVFun m_fun ;
    };
    // ========================================================================
  } // end of namespace LoKi::MCVertices 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCVERTICES_H
// ============================================================================
