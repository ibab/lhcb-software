// $Id: PhotonFromMergedParameters.h,v 1.2 2004-03-11 13:02:14 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/25 18:17:33  gcorti
// tools for merged pi0
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
#include "DaVinciTools/IPhotonFromMergedParams.h"
// forward declarations 
class CaloHypo          ;
class Particle          ;
class Vertex            ;
class PrimVertex        ;
class HepPoint3D        ;
class HepLorentzVector  ;
class HepMatrix         ;
class HepSymMatrix      ;
 
/** @class PhotonFromMergedParameters PhotonFromMergedParameters.h
 *  
 *  An concrete tool 
 * (implementation of IPhotonFromMergedParams abstract interface) 
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

class PhotonFromMergedParameters : 
  public virtual IPhotonFromMergedParams , 
  public              CaloTool 
{
  // friend factory for instantiation 
  friend class ToolFactory<PhotonFromMergedParameters>;
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
   *  @see IPhotonFromMergedParams 
   *  @see Particle 
   *  @see PrimaryVertex 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the primary vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*         photon , 
                               const CaloHypo*   thehypo,
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
   *  @see IPhotonFromMergedParams 
   *  @see Particle 
   *  @see Vertex 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  vertex  pointer to the ptoton's production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const Vertex*        vertex ) const ;
  
  /** (Re)evaluate the photon's parameters using the knowledge 
   *  of photon production vertex  
   *
   *  Error codes:
   *  <li> <b>400</b> invalid (NULL) photon  </li> 
   *
   *  @see IPhotonFromMergedParams 
   *  @see Particle 
   *  @param  photon  pointer to the photon to be (re)evaluated
   *  @param  point   position of the photon production vertex 
   *  @param  error   covariance matrix of photon production vertex 
   *  @return status code 
   */  
  virtual StatusCode process ( Particle*            photon , 
                               const CaloHypo*      thehypo,
                               const HepPoint3D&    point  , 
                               const HepSymMatrix&  cov    ) const ;
  
protected :
  
  /** Standard constructor
   *  @param type tool type 
   *  @param name tool name 
   *  @param parent pointer to tool's parent 
   */
  PhotonFromMergedParameters ( const std::string&  type   , 
                     const std::string&  name   ,
                     const IInterface*   parent ) ;
  
  /// virtual destructor 
  virtual ~PhotonFromMergedParameters() ;
  
private:
  
  /// default constructor  is private 
  PhotonFromMergedParameters ();
  /// copy constructor     is private 
  PhotonFromMergedParameters           ( const PhotonFromMergedParameters & );
  /// assignement operator is private 
  PhotonFromMergedParameters& operator=( const PhotonFromMergedParameters & );
  
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
