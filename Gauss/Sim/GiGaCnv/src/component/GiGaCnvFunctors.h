/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef      GIGACNV_GIGACNVFUNCTORS_H
#define      GIGACNV_GIGACNVFUNCTORS_H 1 
/// ===========================================================================
/// STD & STL 
#include <functional> 

/// forward declarations 
class IParticlePropertySvc;
class MCVertex;
class MCParticle;
class GiGaKineRefTable;
class G4VTrajectory;

#include "LHCbEvent/MCVertex.h"
#include "GiGa/GiGaTrajectoryPoint.h"

/** @namespace GiGaCnvFunctors GiGaCnvFunctors.h
 *
 *  Collection of auxillary functors/methods to simplify 
 *  conversion procedures.
 *  
 *  @author Vanya Belyaev 
 *  @date  xx/xx/xxxx
 */

namespace GiGaCnvFunctors
{
  
  /** @class MCVerticessLess 
   *
   * "Less" operator for MCVertex class, using
   *  Time-of-Flight and the vertex position as 
   *  ordering criteria.  
   *
   *  @author Vanya Belyaev 
   *  @date   xx/xx/xxxx
   */
  class MCVerticesLess:  
    public std::binary_function<const MCVertex*, const MCVertex* , bool>
  {
  public:
    
    /** comparison between 2 MCVertex objects
     *  @param v1 pointer to first vertex 
     *  @param v2 pointer to second vertex 
     *  @return true if verst vertes is "less" than the second one
     */ 
    inline bool operator() ( const MCVertex* v1 , 
                             const MCVertex* v2 ) const 
    {
      return 
        ( 0  == v1                                 ) ? true  :
        ( 0  == v2                                 ) ? false :
        ( v1 == v2                                 ) ? false :
        ( v1->timeOfFlight() <  v2->timeOfFlight() ) ? true  : 
        ( v1->timeOfFlight() >  v2->timeOfFlight() ) ? false : 
        ( v1->position().z() <  v2->position().z() ) ? true  : 
        ( v1->position().z() >  v2->position().z() ) ? false : 
        ( v1->position().x() <  v2->position().x() ) ? true  : 
        ( v1->position().x() >  v2->position().x() ) ? false : 
        ( v1->position().y() <  v2->position().y() ) ? true  : false ; 
    }
  };
  
  /** @class MCVerticesEqual
   * 
   * "Equality" operator for MCVertex class. 
   *  Vertices are "equal" if their time-of-Flight 
   *  the vertex position data are equal
   *
   *  @author Vanya Belyaev 
   *  @date   xx/xx/xxxx
   */
  class MCVerticesEqual  : 
    public std::binary_function<const MCVertex*, const MCVertex*,bool>
  {
  public:
    
    /** "equality" operator for 2 vertices 
     *  @param v1  pointer to the first vertex 
     *  @param v2  pointer to teh second vertes 
     *  @return true if vertices have the same Time-of-Flight and Position
     */
    inline bool operator() ( const MCVertex* v1 , 
                             const MCVertex* v2 ) const 
    {
      return 
        ( 0  == v1 || 0 == v2                      ) ? false :
        ( v1 == v2                                 ) ? true  :
        ( v1->timeOfFlight() != v2->timeOfFlight() ) ? false : 
        ( v1->position    () != v2->position    () ) ? false : true ; 
    };
  };
  
  /** @class MCVertexResetRefs
   * 
   *  Reset the references  for given MCVertex object  
   *
   *  @author Vanya Belyaev 
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
      /// skip NULLs
      if( 0 == vertex ) { return 0 ; }
      /// remove all daughter particles 
      vertex->removeDaughterMCParticles();
      /// remove mother particle 
      vertex->setMotherMCParticle( 0 );
      ///
      return vertex ;
    };
  };
  
  /** @class MCParticleResetRefs
   * 
   *  Reset the references  for given MCParticle object  
   *
   *  @author Vanya Belyaev 
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
      /// skip NULLs
      if( 0 == particle ) { return 0 ; }
      /// remove all decay vertices  
      particle->removeDecayMCVertices();
      /// remove origin vertex 
      particle->setOriginMCVertex( 0 );
      ///
      return particle;
    };
  };

};


/// ===========================================================================
#endif  ///<   GIGACNV_GIGACNVFUNCTORS_H
/// ===========================================================================















