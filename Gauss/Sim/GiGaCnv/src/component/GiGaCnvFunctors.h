// $Id: GiGaCnvFunctors.h,v 1.5 2002-01-22 18:24:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/08/12 17:24:51  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.3  2001/07/30 14:13:35  ibelyaev
// update in GiGaCnvFunctors
//
// Revision 1.2  2001/07/24 11:13:55  ibelyaev
// package restructurization(III) and update for newer GiGa
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
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/SmartIF.h"
// LHCbEvent 
#include "LHCbEvent/MCVertex.h"
#include "LHCbEvent/MCParticle.h"

// GiGa 
#include "GiGa/GiGaTrajectoryPoint.h"

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
  
  /** comparison oprator for 3D-Vector objects
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   30/07/2001
   *
   *  @param vec1 const reference to the first vector
   *  @param vec2 const reference to the second vector
   *  @return true if first vector is "less" than the second vector 
   */
  inline bool operator< ( const Hep3Vector& vec1 ,
                          const Hep3Vector& vec2 )
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
    public std::binary_function<const MCVertex*, const MCVertex* , bool>
  {
  public:
    
    /** comparison between 2 MCVertex objects
     *  @param v1 pointer to the first vertex 
     *  @param v2 pointer to the second vertex 
     *  @return true if verst vertes is "less" than the second one
     */ 
    inline bool operator() ( const MCVertex* v1 , 
                             const MCVertex* v2 ) const 
    {      
      return  
        ( v1 == v2                                   ) ? false :
        ( 0  == v1                                   ) ? true  :
        ( 0  == v2                                   ) ? false :
        ( v1->timeOfFlight () <  v2->timeOfFlight () ) ? true  : 
        ( v1->timeOfFlight () >  v2->timeOfFlight () ) ? false : 
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
    public std::binary_function<const MCVertex*, const MCVertex*,bool>
  {
  public:
    
    /** "equality" operator for 2 vertices 
     *  @param v1  pointer to the first vertex 
     *  @param v2  pointer to the second vertes 
     *  @return true if vertices have the same Time-of-Flight and Position
     */
    inline bool operator() ( const MCVertex* v1 , 
                             const MCVertex* v2 ) const 
    {
      return 
        ( v1 == v2                                 ) ? true  :
        ( 0  == v1 || 0 == v2                      ) ? false :
        ( v1->timeOfFlight() == v2->timeOfFlight() &&  
          v1->position    () == v2->position    () ) ? true  : false ; 
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
    public std::unary_function<MCVertex*,MCVertex*>
  {
  public:
    
    /** reset relations for given MCVertex object 
     *  @param vertex reference to MCVertex object
     *  @return pointer to MCVertex object 
     */
    inline MCVertex* operator() ( MCVertex* vertex ) const 
    {
      if( 0 == vertex ) { return 0 ; }      ///< skip NULLs
      vertex->clearDaughterMCParticles();  ///< remove all daughter particles 
      vertex->setMotherMCParticle( 0 );     ///< unset the mother particle 
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
    public std::unary_function<MCParticle*,MCParticle*>
  {
  public:
    
    /** reset relations for given MCParticle object 
     *  @param particle reference to MCParticle object
     *  @return pointer to MCParticle object 
     */
    inline MCParticle* operator() ( MCParticle* particle ) const 
    {
      if( 0 == particle ) { return 0 ; }  ///< skip NULLs
      particle->clearDecayMCVertices();   ///< remove all decay vertices  
      particle->setOriginMCVertex( 0 );   ///< remove origin vertex 
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















