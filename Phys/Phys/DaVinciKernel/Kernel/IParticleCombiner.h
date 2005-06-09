// $Id: IParticleCombiner.h,v 1.2 2005-06-09 13:10:40 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $; version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.2.1  2005/06/09 11:55:41  pkoppenb
// New vertex interfaces
// 
// ============================================================================
#ifndef VERTEXFITTER_IPARTICLECOMBINER_H 
#define VERTEXFITTER_IPARTICLECOMBINER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
class Particle ;
class Vertex   ;

/** @class IParticleCombiner IParticleCombiner.h 
 * 
 *  The first "basic" abstract interface interface 
 *  for fitting/combiner tools 
 *
 *  @author Vanya BELYAEV belyaev@lapp.lin2p3.fr
 *  @date   2004-12-19
 */
class IParticleCombiner : virtual public IAlgTool 
{
public:
  
  /** the actual type for containe rof daughter particles 
   *  Important: the daughetr are considered to have "const" qualifier!
   */
  typedef std::vector<const Particle*> Daughters ;
  
public:
  
  /** The major method for "combining" the daughter particles 
   *  into "mother" particle.
   *
   *  The container of input particles is "combined" into
   *  "mother" particle and its decay vertex.
   *
   *  The intermediate constrains (mass, vertex, mass-vertex,
   *  etc.. could be applied in the process of "combining", 
   *  dependent on the used implementation)
   * 
   *  @code 
   *
   *  // locate the tool  
   *  const IParticleCombiner* combiner = get<IParticleCombiner>( ... ) 
   * 
   *  // loop over the first daughter particle 
   *  for( ... ipi1 = ... ; ... ; ++ipi1 ) 
   *  {
   *     for( ... ipi2 = ... ; ... ; ++ipi2 ) 
   *     {
   *        const Particle* pi1 = *ipi1 ;
   *        const Particle* pi2 = *ipi2 ;
   *   
   *        IParticleCombiner::Daughters daughters ;
   *        daughters.push_back( pi1 ) ;
   *        daughters.push_back( pi2 ) ;
   *   
   *        Particle K0S  ; 
   *        Vertex   Vrtx ; 
   *   
   *        StatusCode sc = combiner->combine( daughters , K0S , Vrtx ) ;
   *        if( sc.isFailure() ) 
   *        { Warning("Error in K0S fit, skip the pair", sc ); continue ; }  
   * 
   *        .... 
   *     }
   *  }
   *
   *  @endcode 
   *  
   *  @param daughters the vector of daughter particles    (input)
   *  @param mother    the "mother" particle               (output)
   *  @param vertex    the decay vertex of mother particle (output)
   *  @return status code 
   */
  virtual StatusCode combine
  ( const Daughters&  daughters ,  
    Particle&         mother    , 
    Vertex&           vertex    ) const = 0 ;
  
  /** The method which allows to use almost an arbitrary 
   *  sequence of daughter particles, e.g. 
   *  ParticleVector, Particles, SmartRefVector<Particle>, etc.. 
   *
   *  Also this signature allow to make an easy "refit"/"recombine"
   *  of the mother particle, e.g.
   *
   *  @code
   *
   *   // locate the tool  
   *   const IParticleCombiner* combiner = get<IParticleCombiner>( ... ) 
   *
   *   Particle*          mother = ... ;  
   *   
   *   StatusCode sc = combiner->combine
   *   (  mother->vertex->products().begin() , 
   *   (  mother->vertex->products().end()   , 
   *     *mother                             ,
   *     *(mother->endVertex())              ) ;
   *
   *    if( sc.isFailure() ) 
   *      { Warning("Error in fitter", sc ); continue ; }  
   *
   *  @endcode  
   * 
   *  @param begin    begin-iterator for sequence of daughter parricles 
   *  @param end      end-iterator for sequence of daughter parricles 
   *  @param mother   the "mother" particle               (output)
   *  @param vertex   the decay vertex of mother particle (output)
   *  @return status code 
   */  
  template <class DAUGHTER>
  inline StatusCode   combine 
  ( DAUGHTER          begin     ,
    DAUGHTER          end       ,
    Particle&         mother    , 
    Vertex&           vertex    ) const 
  {
    const Daughters children = Daughters( begin , end ) ;
    return combine( children , mother , vertex ) ;
  };
  
public:
  
  /// unique interface identifier
  static const InterfaceID& interfaceID() ; 
  
protected:
  
  // virtual and protected destructor 
  virtual ~IParticleCombiner(); ///< virtual and protected destructor 

};

// ============================================================================
// The END 
// ============================================================================
#endif // VERTEXFITTER_IPARTICLECOMBINER_H
// ============================================================================
