// $Id: GiGaCnvFunctors.h,v 1.9 2006-01-31 10:34:15 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2005/01/25 15:52:10  gcorti
// adapt to CLHEP 19.0
//
// Revision 1.7  2002/05/20 13:36:16  ibelyaev
//  add conversion of primary vertices
//
// ============================================================================
#ifndef      GIGACNV_GIGACNVFUNCTORS_H
#define      GIGACNV_GIGACNVFUNCTORS_H 1 
// ============================================================================
/// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SmartIF.h"
// LHCb
#include "Kernel/IParticlePropertySvc.h"
// Event
#include "Event/MCVertex.h"
#include "Event/MCParticle.h"
// GiGa 
#include "GiGa/GiGaTrajectoryPoint.h"
// local 

/// forward declarations 
class GiGaKineRefTable;
class G4VTrajectory;

/** @namespace GiGaCnvFunctors GiGaCnvFunctors.h
 *
 *  Collection of auxillary functors/methods to simplify 
 *  conversion procedures.
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date  xx/xx/xxxx
 */

namespace GiGaCnvFunctors
{
  
  /** comparison operator for 3D-Vector objects
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/07/2001
   *
   *  @param vec1 const reference to the first vector
   *  @param vec2 const reference to the second vector
   *  @return true if first vector is "less" than the second vector 
   */
  inline bool operator< 
  ( const Gaudi::XYZPoint& vec1 ,
    const Gaudi::XYZPoint& vec2 )
  {
    if( &vec1 == &vec2 ) { return false ; } ///< the same object
    ///
    return
      vec1.z() < vec2.z() ? true  :
      vec1.z() > vec2.z() ? false :
      vec1.x() < vec2.x() ? true  : 
      vec1.x() > vec2.x() ? false :
      vec1.y() < vec2.y() ? true  : false ;
    ///  
  };

  /** @class MCVerticessLess 
   *
   * "Less" operator for MCVertex class, using
   *  Time-of-Flight and the vertex position as 
   *  ordering criteria.  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  class MCVerticesLess:  
    public std::binary_function<const LHCb::MCVertex*, const LHCb::MCVertex* , bool>
  {
  public:
    
    /** comparison between 2 MCVertex objects
     *  @param v1 pointer to the first vertex 
     *  @param v2 pointer to the second vertex 
     *  @return true if verst vertes is "less" than the second one
     */ 
    inline bool operator() 
      ( const LHCb::MCVertex* v1 , 
        const LHCb::MCVertex* v2 ) const 
    {      
      return  
        ( v1 == v2                                   ) ? false :
        ( 0  == v1                                   ) ? true  :
        ( 0  == v2                                   ) ? false :
        ( v1->time() <  v2->time() ) ? true  : 
        ( v1->time() >  v2->time() ) ? false : 
        ( v1->position     () <  v2->position     () ) ? true  : false ;
    }
  };
  
  /** @class MCVerticesEqual
   * 
   * "Equality" operator for MCVertex class. 
   *  Vertices are "equal" if their time-of-Flight 
   *  the vertex position data are equal
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   xx/xx/xxxx
   */
  class MCVerticesEqual  : 
    public std::binary_function<const LHCb::MCVertex*, const LHCb::MCVertex*,bool>
  {
  public:
    
    /** "equality" operator for 2 vertices 
     *  @param v1  pointer to the first vertex 
     *  @param v2  pointer to the second vertes 
     *  @return true if vertices have the same Time-of-Flight and Position
     */
    inline bool operator() 
      ( const LHCb::MCVertex* v1 , 
        const LHCb::MCVertex* v2 ) const 
    {
      return 
        ( v1 == v2                                   ) ? true  :
        ( 0  == v1 || 0 == v2                        ) ? false :
        ( v1->time() == v2->time() &&  
          v1->position     () == v2->position     () ) ? true  : false ;
    };
  };
  
  /** @class MCVertexResetRefs
   * 
   *  Reset the references  for given MCVertex object  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   22/07/2001
   */
  class MCVertexResetRefs:
    public std::unary_function<LHCb::MCVertex*,LHCb::MCVertex*>
  {
  public:
    
    /** reset relations for given MCVertex object 
     *  @param vertex reference to MCVertex object
     *  @return pointer to MCVertex object 
     */
    inline LHCb::MCVertex* operator() 
      ( LHCb::MCVertex* vertex ) const 
    {
      if( 0 == vertex ) { return 0 ; }  // skip NULLs
      vertex->clearProducts ()    ;     // remove all daughter particles 
      vertex->setMother( (LHCb::MCParticle*) 0  ) ; // unset the mother particle
      ///
      return vertex ;
    };
  };
  
  /** @class MCParticleResetRefs
   * 
   *  Reset the references  for given MCParticle object  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   22/07/2001
   */
  class MCParticleResetRefs:
    public std::unary_function<LHCb::MCParticle*,LHCb::MCParticle*>
  {
  public:
    
    /** reset relations for given MCParticle object 
     *  @param particle reference to MCParticle object
     *  @return pointer to MCParticle object 
     */
    inline LHCb::MCParticle* operator() 
      ( LHCb::MCParticle* particle ) const 
    {
      if( 0 == particle ) { return 0 ; }  ///< skip NULLs
      particle->clearEndVertices();   ///< remove all decay vertices  
      particle->setOriginVertex( (LHCb::MCVertex*)0 );   ///< remove origin vertex 
      ///
      return particle;
    };
  };
  
}; ///< end of namespace 


// ============================================================================
// End 
// ============================================================================
#endif  ///<   GIGACNV_GIGACNVFUNCTORS_H
// ============================================================================















