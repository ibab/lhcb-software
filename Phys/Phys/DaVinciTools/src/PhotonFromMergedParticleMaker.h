// $Id: PhotonFromMergedParticleMaker.h,v 1.1 2003-04-25 18:17:34 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//  new tools for Photons from Merged pi0
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
#include "DaVinciTools/IPhysDesktop.h"
#include "DaVinciTools/IParticleMaker.h"
// from Gaudi
#include "CaloKernel/CaloTool.h"
class IPhotonFromMergedParams ;  // DaVinciTools 
class ProtoParticle ;  // PhysEvent 

/** @class PhotonFromMergedParticleMaker PhotonFromMergedParticleMaker.h
 *  
 *  The specialized producer of photons from Merged Pi0
 *
 *  @author Olivier Deschamps from Vanya's PhotoParticleMaker v1.1
 *  @date   2003-03-11
 */

class PhotonFromMergedParticleMaker : 
  public virtual IParticleMaker , 
  public               CaloTool 
{
  /// friend class for instantiation 
  friend class ToolFactory<PhotonFromMergedParticleMaker>;
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
  PhotonFromMergedParticleMaker( const std::string& type   , 
                       const std::string& name   ,
                       const IInterface*  parent ) ;
  
  /// virtual detructor
   virtual ~PhotonFromMergedParticleMaker() ;

protected:
  /** accessor to the specialized tool for estimation of
   *  photon parameters 
   */
  const IPhotonFromMergedParams* photPars  () const { return m_photPars ; }
  
  /// acessor to event data provider service 
  IDataProviderSvc*    evtSvc    () const { return m_evtSvc   ; }
  
  /// input data location 
  const std::string&   inputData () const { return m_input    ; }
  
  
  /**  evaluate Photon's "confidence level" from following quantities 
   * 
   *  <li>  <c> CaloTrMatch     </c> 
   *  <li>  <c> CaloDepositID   </c>
   *  <li>  <c> ShowerShape     </c>
   *  <li>  <c> ClusterMass     </c> 
   *
   */
  double confLevel( const ProtoParticle* pp ) const ;
  
private:


  typedef std::vector<double>        Params             ;

  // input data (ProtoParticles)  
  std::string                        m_input            ;
  
  // event data service 
  IDataProviderSvc*                  m_evtSvc           ;
  
  // evaluator of photon parameters at some nominal vertex 
  std::string                        m_photParsName     ;
  IPhotonFromMergedParams*           m_photPars         ;
  
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

  // cut on confidence level 
  double                             m_cut              ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // PHOTONPARTICLEMAKER_H
// ============================================================================
