// $Id: IPhotonParams.h,v 1.5 2006-03-15 13:34:03 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// ============================================================================
#ifndef DAVINCIKERNEL_IPHOTONPARAMS_H 
#define DAVINCIKERNEL_IPHOTONPARAMS_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
// DaVinciTools 
#include "Kernel/IIDIPhotonParams.h"
#include "Event/Particle.h"
#include "Event/PrimVertex.h"

/** @class IPhotonParams IPhotonParams.h Kernel/IPhotonParams.h
 *  
 *  An abstract interface for (re)evaluation of photon parameters 
 *  using the knowledge of the photon production vertex 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */
class IPhotonParams : public virtual IAlgTool                          
{  
public:
  
  /** static interface identification 
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IPhotonParams ; }
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of the primary vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the primary vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const PrimLHCb::Vertex*    vertex ) const = 0 ;
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of photon production  vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const LHCb::Vertex*        vertex ) const = 0 ;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( LHCb::Particle*            photon , 
                               const Gaudi::XYZPoint&    point  , 
                               const Gaudi::Matrix3x3&  error  ) const = 0 ;
  
  
  /** useful templated shortcuts 
   *  it just applies standard virtual method to any 
   *  sequence of the particles
   *
   *  @code 
   * 
   *   IPhotonParams*    photPar = ... ;
   *   const PrimLHCb::Vertex* vertex  = ... ;
   *   LHCb::Particle::ConstVector    photons = ... ;
   * 
   *   StatusCode sc = photPars -> 
   *             process ( photons.begin() , photons.end() , vertex );
   *
   *  @endcode 
   *  
   *  @param begin 'begin' iterator of the sequence 
   *  @param end   'end'   iterator of the sequance 
   *  @param vertex pointer to primary vertex
   *  @return status code 
   */
  template <class PHOT>
  StatusCode         process( PHOT               begin  , 
                              PHOT               end    , 
                              const PrimLHCb::Vertex*  vertex ) const 
  {
    StatusCode sc( StatusCode::SUCCESS );
    // loop over all particles 
    for( ; begin != end && sc.isSuccess() ; ++begin ) 
      { 
        sc = process( *begin , vertex ); 
      }
    return sc ;
  };


  /** useful templated shortcuts 
   *  it just applies standard virtual method to any 
   *  sequence of the particles
   *
   *  @code 
   * 
   *   IPhotonParams*    photPar = ... ;
   *   const LHCb::Vertex*     vertex  = ... ;
   *   LHCb::Particle::ConstVector    photons = ... ;
   * 
   *   StatusCode sc = photPars -> 
   *            process( photons.begin() , photons.end() , vertex );
   *
   *  @endcode 
   *
   *  @param begin 'begin' iterator of the sequence 
   *  @param end   'end'   iterator of the sequance 
   *  @param vertex pointer to photon production vertex
   *  @return status code 
   */
  template <class PHOT>
  StatusCode         process( PHOT                  begin  , 
                              PHOT                  end    , 
                              const LHCb::Vertex*         vertex ) const 
  {
    StatusCode sc( StatusCode::SUCCESS );
    // loop over all particles 
    for( ; begin != end && sc.isSuccess() ; ++begin ) 
      { 
        sc = process( *begin , vertex ); 
      }
    return sc ;
  };

  /** useful templated shortcuts 
   *  it just applies standard virtual method to any 
   *  sequence of the particles
   *
   *  @code 
   * 
   *   IPhotonParams*     photPar = ... ;
   *   const Gaudi::XYZPoint   point   = ... ;
   *   const Gaudi::Matrix3x3 error   = ... ; 
   *   LHCb::Particle::ConstVector     photons = ... ;
   * 
   *   StatusCode sc = photPars -> 
   *          process( photons.begin() , photons.end() , point , error );
   *
   *  @endcode 
   *
   *  @param begin 'begin' iterator of the sequence 
   *  @param end   'end'   iterator of the sequance 
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */
  template <class PHOT>
  StatusCode         process( PHOT                  begin  , 
                              PHOT                  end    , 
                              const Gaudi::XYZPoint&     point  , 
                              const Gaudi::Matrix3x3&   error  ) const
  {
    StatusCode sc( StatusCode::SUCCESS );
    // loop over all particles 
    for( ; begin != end && sc.isSuccess() ; ++begin ) 
      { 
        sc = process( *begin , point , error ); 
      }
    return sc ;
  };

protected:
  
  /// destructor 
  virtual ~IPhotonParams(){};

};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IPHOTONPARAMS_H
// ============================================================================
