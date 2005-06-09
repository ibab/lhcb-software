// $Id: IPhotonParams.h,v 1.4 2005-06-09 13:11:30 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/06/09 13:10:40  pkoppenb
// Joined with v3r3b1 branch
//
// Revision 1.2.6.1  2005/06/02 16:45:41  pkoppenb
// CheckOverlap - another implementation
//
// Revision 1.2  2005/02/09 17:20:43  pkoppenb
// CLHEP
//
// Revision 1.1  2005/01/06 10:30:43  pkoppenb
// *.h moved from DaVinciTools to Kernel
//
// Revision 1.2  2004/10/27 14:09:58  pkoppenb
// IPVLocator
//
// Revision 1.1.1.1  2004/08/24 06:59:45  pkoppenb
// DaVinci Kerel and Interfaces extracted from DaVinciTools
//
// Revision 1.5  2004/07/28 14:15:23  pkoppenb
// untag
//
// Revision 1.4  2004/05/11 16:01:24  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.3  2004/03/11 16:12:34  pkoppenb
// Restored DaVinciTools. Better to have two libraries here than a DaVinciKernel
//
// Revision 1.1  2003/01/22 16:43:22  ibelyaev
//  new tools for Photons
// 
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
// forward declarations 
class Particle     ;
class PrimVertex   ;
class Vertex       ;

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
  virtual StatusCode process ( Particle*            photon , 
                               const PrimVertex*    vertex ) const = 0 ;
  
  /** (Re)evaluate the photon's paramters using the knowledge 
   *  of photon production  vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const Vertex*        vertex ) const = 0 ;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const HepPoint3D&    point  , 
                               const HepSymMatrix&  error  ) const = 0 ;
  
  
  /** useful templated shortcuts 
   *  it just applies standard virtual method to any 
   *  sequence of the particles
   *
   *  @code 
   * 
   *   IPhotonParams*    photPar = ... ;
   *   const PrimVertex* vertex  = ... ;
   *   ParticleVector    photons = ... ;
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
                              const PrimVertex*  vertex ) const 
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
   *   const Vertex*     vertex  = ... ;
   *   ParticleVector    photons = ... ;
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
                              const Vertex*         vertex ) const 
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
   *   const HepPoint3D   point   = ... ;
   *   const HepSymMatrix error   = ... ; 
   *   ParticleVector     photons = ... ;
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
                              const HepPoint3D&     point  , 
                              const HepSymMatrix&   error  ) const
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
