// $Id: PhotonParameters.h,v 1.2 2005-01-06 10:46:54 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/08/24 06:47:48  pkoppenb
// ParticleMaker extracted from DaVinciTools
//
// Revision 1.4  2004/07/28 14:15:24  pkoppenb
// untag
//
// Revision 1.3  2004/05/11 16:01:25  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.2  2004/03/11 13:02:14  pkoppenb
// Split DaVinciTools into DaVinciTools and DaVinciKernel
//
// Revision 1.1  2003/01/22 16:43:23  ibelyaev
//  new tools for Photons
// 
// ============================================================================
#ifndef DAVINCITOOLS_PHOTONPARAMETERS_H 
#define DAVINCITOOLS_PHOTONPARAMETERS_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from DaVinciTool 
#include "Kernel/IPhotonParams.h"
// forward declarations 
class Particle          ;
class Vertex            ;
class PrimVertex        ;
class HepPoint3D        ;
class HepLorentzVector  ;
class HepMatrix         ;
class HepSymMatrix      ;
 
/** @class PhotonParameters PhotonParameters.h
 *  
 *  An concrete tool (implementation of IPhotonParams abstract interface) 
 *  for (re)evaluation of photon parameters using the knowledge of 
 *  photon production vertex 
 *
 *  An obvious clients are "PhotonParticleMaker" and 
 *  any algorithm wihc explicitely deals with photons 
 *
 *  The code is based on F.Machefert's (machefer@in2p3.fr) lines.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */

class PhotonParameters : 
  public virtual IPhotonParams , 
  public              CaloTool 
{
  // friend factory for instantiation 
  friend class ToolFactory<PhotonParameters>;
public:
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of the primary vertex
   * 
   *  Scale factor could be applied to the covariance matrix 
   *
   *  Error codes:
   *  <li> <b>401</b> invalid (NULL) primary vertex </li> 
   *  Other error codes from method with <c>Vertex</c> argument 
   *
   *  @see IPhotonParams 
   *  @see Particle 
   *  @see PrimaryVertex 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the primary vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*         photon , 
                               const PrimVertex* vertex ) const ;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production  vertex  
   *
   *  Scale factor could be applied to the covariance matrix 
   *
   *  Error codes:
   *  <li> <b>402</b> invalid (NULL) vertex </li> 
   *  Other error codes from method with <c>HepPoint3D</c> argument 
   *
   *  @see IPhotonParams 
   *  @see Particle 
   *  @see Vertex 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const Vertex*        vertex ) const ;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *
   *  Error codes:
   *  <li> <b>400</b> invalid (NULL) photon  </li> 
   *
   *  @see IPhotonParams 
   *  @see Particle 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const HepPoint3D&    point  , 
                               const HepSymMatrix&  cov    ) const ;
  
protected :
  
  /** Standard constructor
   *  @param type tool type 
   *  @param name tool name 
   *  @param parent pointer to tool's parent 
   */
  PhotonParameters ( const std::string&  type   , 
                     const std::string&  name   ,
                     const IInterface*   parent ) ;
  
  /// virtual destructor 
  virtual ~PhotonParameters() ;
  
private:
  
  /// default constructor  is private 
  PhotonParameters ();
  /// copy constructor     is private 
  PhotonParameters           ( const PhotonParameters & );
  /// assignement operator is private 
  PhotonParameters& operator=( const PhotonParameters & );
  
private:
  
  /** auxillary method for actual evaluation of 
   *  intermediate matrices and parameters 
   *  @param calo  pointer to CaloPosition object 
   *  @param point position of primary vertex 
   *  @param vxcov covarinace matrix for position of primary vertex 
   *  @return status code 
   */
  StatusCode calculate 
  ( const CaloPosition* calo  , 
    const HepPoint3D&   point , 
    const HepSymMatrix& vxcov ) const ;
  
  // get the calculated covariance matrix for photon's 4-momentum 
  const HepSymMatrix&     c11      () const { return  m_C11      ; }
  // get the calculated covariance matrix for photon's point on track 
  const HepSymMatrix&     c22      () const { return *m_C22      ; }
  // get teh calculated covarinace matrix between 4-momentum and the point 
  const HepMatrix&        c12      () const { return  m_C12      ; }
  // get the calculated photon's momentum
  const HepLorentzVector& momentum () const { return  m_momentum ; }
  // get the calculated point on the track 
  const HepPoint3D&       position () const { return  m_position ; }
  
private:
  
  /// the scale parameter to be applied for vertex's covariance matrix 
  double                          m_scale    ;
  
  mutable       HepLorentzVector  m_momentum ;
  mutable       HepPoint3D        m_position ;
  
  mutable       HepMatrix         m_F11      ;
  mutable       HepMatrix         m_F12      ;
  
  mutable       HepSymMatrix      m_S11      ;
  
  mutable       HepSymMatrix      m_C11      ;
  mutable       HepMatrix         m_C12      ;
  mutable const HepSymMatrix*     m_C22      ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCITOOLS_PHOTONPARAMETERS_H
// ============================================================================
