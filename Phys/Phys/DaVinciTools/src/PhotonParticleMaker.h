// $Id: PhotonParticleMaker.h,v 1.5 2004-05-11 16:01:25 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/04/21 02:29:30  ibelyaev
//  PhotonParticleMaker is modified
//
// Revision 1.3  2004/03/11 13:02:14  pkoppenb
// Split DaVinciTools into DaVinciTools and DaVinciKernel
//
// Revision 1.2  2003/04/08 17:22:28  ibelyaev
//  CnvPhotonParticleMaker: new creator of converted photons
//
// Revision 1.1  2003/01/22 16:43:24  ibelyaev
//  new tools for Photons
// 
// ============================================================================
#ifndef PHOTONPARTICLEMAKER_H 
#define PHOTONPARTICLEMAKER_H 1
// Include files
// from STL
#include <string>
// From Event 
#include "Event/Particle.h"
// FromDaVinciTools 
#include "DaVinciTools/IParticleMaker.h"
// from Gaudi
#include "CaloKernel/CaloTool.h"
class IPhotonParams ;  // DaVinciTools 
class ProtoParticle ;  // PhysEvent 
class CaloHypo      ;

/** @class PhotonParticleMaker PhotonParticleMaker.h
 *  
 *  The specialized producer of photons 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */

class PhotonParticleMaker : 
  public virtual IParticleMaker , 
  public               CaloTool 
{
  /// friend class for instantiation 
  friend class ToolFactory<PhotonParticleMaker>;
public:
  
  /** Make the particles 
   *  @see IParticleMaker 
   *  @param particles  vector of particles  
   */
  virtual StatusCode makeParticles ( ParticleVector & particles ) ;
  
  /** standard initialization of tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize    () ;   
  
  /** standard finalization of tool 
   *  @see CaloTool
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize      () ;

protected:
  
  /** Standard constructor
   *  @param type   tool type
   *  @param name   tool name 
   *  @param parent tool parent
   */
  PhotonParticleMaker( const std::string& type   , 
                       const std::string& name   ,
                       const IInterface*  parent ) ;
  
  /// virtual detructor
   virtual ~PhotonParticleMaker() ;

protected:
  
  /** accessor to the specialized tool for estimation of
   *  photon parameters 
   */
  const IPhotonParams* photPars  () const { return m_photPars ; }
  
  /// input data location 
  const std::string&   inputData () const { return m_input    ; }  
  
  /**  evaluate Photon's "confidence level" from following quantities 
   * 
   *  <li>  <c> CaloTrMatch     </c> 
   *  <li>  <c> CaloDepositID   </c>
   *  <li>  <c> ShowerShape     </c>
   *  <li>  <c> ClusterMass     </c>
   *  <li>  <c> PhotonID        </c>
   *
   */
  double confLevel        ( const ProtoParticle* pp ) const ;

  /// change a flag for converted photons
  void   setConverted     ( const bool value ) { m_converted = value  ; }
  /// change a flag for converted photons
  void   setUseAllPhotons ( const bool value ) { m_useAll    = value  ; }

private:

  typedef std::vector<double>        Params             ;

  // input data (ProtoParticles)
  std::string                        m_input            ;

  // evaluator of photon parameters at some nominal vertex
  std::string                        m_photParsName     ;
  IPhotonParams*                     m_photPars         ;

  // nominal production vertex
  Params                             m_pointVector      ;
  HepPoint3D                         m_point            ;

  // covarinace matrix for nominal production vertex
  HepSymMatrix                       m_pointErr         ;


  // square roots from diagonal elements   (3 values)
  Params                             m_diagonal         ;

  // off-diagonal correlation coefficients (3 values)
  Params                             m_offDiagonal      ;

  // techniques for CL evaluation
  bool                               m_useCaloTrMatch   ;
  bool                               m_useCaloDepositID ;
  bool                               m_useShowerShape   ;
  bool                               m_useClusterMass   ;
  bool                               m_usePhotonID      ;

  // cut on confidence level
  double                             m_cut              ;

  // flag for converted photons
  bool                               m_converted        ;
  // flag for ALL photons
  bool                               m_useAll           ;

};

// ============================================================================
// The END
// ============================================================================
#endif // PHOTONPARTICLEMAKER_H
// ============================================================================
