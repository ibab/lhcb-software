// $Id: IParticleReFitter.h,v 1.4 2007-02-06 09:57:56 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $; version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2006/03/15 13:34:02  pkoppenb
// Head is now DC06
//
// Revision 1.2.4.2  2006/01/24 09:56:05  pkoppenb
// small changes
//
// Revision 1.2.4.1  2006/01/17 08:54:45  pkoppenb
// First DC06 version
//
// Revision 1.2  2005/06/09 13:10:40  pkoppenb
// Joined with v3r3b1 branch
//
// Revision 1.1.2.1  2005/06/09 11:55:41  pkoppenb
// New vertex interfaces
// 
// ============================================================================
#ifndef VERTEXFITTER_IPARTICLEREFITTER_H 
#define VERTEXFITTER_IPARTICLEREFITTER_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations 
// ============================================================================
class LHCb::Particle ;
// ============================================================================

/** @class IParticleReFitter IParticleReFitter.h 
 *  
 *  The second "basic" abstract interface for (re)fitter tools.
 *
 *  According to my (biased) vision, *ALL* concrete 
 *  tools could(should?) implement this interface. 
 *  E.g. the typical implementation for any concrete tool 
 *  could be :
 *    - an extraction of all information from the particle 
 *    - invoke the basic appropriate methdod for concrete tool 
 *      (e.g. vertex fit, or mass-vertex fit, or lifetiem fit...)
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-12-19
 */
class IParticleReFitter : virtual public IAlgTool 
{
public:
  
  /** The basic method for "refit" of the particle
   *
   *  @code 
   *
   *  // locate the tool 
   *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
   * 
   *  // particle to be refit 
   *  LHCb::Particle* p = ... ;
   *
   *  StatusCode sc = refitter->reFit ( *p ) ;
   *  if ( sc.isFailure() ) { Warning("Error in reFit" , sc ) ; }
   *
   *  @endcode 
   *
   *  @see Particle
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param particle reference to the particle 
   *  @return statsu code 
   */
  virtual StatusCode reFit ( LHCb::Particle& particle ) const = 0 ;
  
  /** The method which allows to 
   *  refit a sequence of particles. The actual tyep of sequence
   *  is irrelevant, e.g. it could be 
   *  KeyedContainer<Particle>, std::vetctor<Particle> , 
   *  SmartRef<Particle>, std::vector<LHCb::Particle*>, etc..
   *  Elements of the vector shoudl be convertible 
   *  either to Parrticle& or LHCb::Particle* 
   * 
   *  @code 
   *  
   *  // locate the tool 
   *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
   * 
   *  // refit *ALL* K0S  
   *  Particles* allK0S = get<Particle>( "/Event/Phys/K0S" ) ;
   * 
   *  StatusCode sc = refitter->reFit( allK0S->begin() , allK0S.end() ) ;
   *
   *  @endcode 
   * 
   *  @see LHCb::Particle 
   *  @see GaudiAlgorithm::get
   *  @see GaudiAlgorithm::tool
   * 
   *  @param begin  begin-iterator for sequence of particles
   *  @param end    end-iterator for sequence of particles
   *  @return status code 
   */
  template <class PARTICLE>
  inline StatusCode reFit 
  ( PARTICLE        begin , 
    PARTICLE        end   ) const 
  {
    StatusCode code = StatusCode::SUCCESS ;
    for( ; begin != end ; ++begin ) 
    {
      StatusCode sc = reFit( *begin ) ;
      if ( sc.isFailure() ) { code = sc ; }
    }
    return code ;
  };
  
public:
  
  /// Return the unique interface identifier 
  static const InterfaceID& interfaceID() ;
  
protected:

  // virtual and protected destructor 
  virtual ~IParticleReFitter() ; ///< virtual and protected destructor 

};

// ============================================================================
// The END 
// ============================================================================
#endif // VERTEXFITTER_IPARTICLEREFITTER_H
// ============================================================================
