// $Id: PhotonParticleMaker.h,v 1.1 2003-01-22 16:43:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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

/** @class PhotonParticleMaker PhotonParticleMaker.h
 *  
 *  The specialized producer of photons 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
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

  // cut on confidence level 
  double                             m_cut              ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // PHOTONPARTICLEMAKER_H
// ============================================================================
