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
      // ======================================================================
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
      /** conclassor from LoKi::Point3D
       *  @param point 3D-point 
       */
      MCVertexDistance ( const LoKi::Point3D& point ) ;      
      /** conclassor from MCVertex
       *  @param point 3D-point 
       */
      MCVertexDistance ( const LHCb::MCVertex*   point ) ;      
      /// copy conclassor 
      MCVertexDistance ( const MCVertexDistance& ) ;
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
     *  @see MCParticle
     *  @see MCVertex 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2004-07-07
     */
    class GAUDI_API MCVFunAsMCFun 
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Function
    {
    public:
      // ======================================================================
      /// conclassor from MC Vertex fuction
      MCVFunAsMCFun ( const LoKi::MCTypes::MCVFunc& func   ) ;
      /// copy conclassor 
      MCVFunAsMCFun ( const MCVFunAsMCFun&          right  ) ;
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
      LoKi::MCTypes::MCVFun m_fun ;
      // ======================================================================
    };
    // ========================================================================
  } //                                        end of namespace LoKi::MCVertices 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_MCVERTICES_H
// ============================================================================
