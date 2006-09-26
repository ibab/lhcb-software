// $Id: MCParticleMaker.h,v 1.13 2006-09-26 10:45:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.13 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef MCPARTICLEMAKER_H 
#define MCPARTICLEMAKER_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <vector>
#include <string>
#include <utility>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
// ============================================================================
// from DaVinciTools
// ============================================================================
#include "Kernel/IParticleMaker.h"
#include "Kernel/IMCDecayFinder.h"
#include "DaVinciMCTools/IDebugTool.h"
// ============================================================================
// local 
// ============================================================================
#include "DaVinciMCTools/MCParticleMakerBase.h"
// Forward declarations
class IParticlePropertySvc;
class IRndmGenSvc;
class IMCDecayFinder;
class IMCReconstructible ;

class ProtoParticle;

/** @class MCParticleMaker MCParticleMaker.h
 * This tool fills the particle class with information from MCParticles and places it in the Transient Event Store. 
 * 
 * It allows several utilities:
 * <ol> 
 * <li> Smear all the MCParticle in the list \b ParticleNames which are found in the desktop </li>
 * <li>Smear the information of the first MCParticle of the Decay Channel specified in jobOption(e.g. B if B->pipi is chosen)</li>
 * <li> Smear the information of the only stable products of the Decay Channel specified in jobOption (e.g. pipi  
 * if B->pipi is chosen or pi+ pi- mu+ mu- in B->Jpsi(mumu)Ks(pipi)) </li>  
 * </ol>
 *  Different additional selection can be applied to the MCParticles:
 * <ol> 
 * <li> Ask to be reconstructed particles</li>  
 * <li> Ask to be reconstructible particles</li>  
 * <li> Use reconstructed covariance to generate the fake measurements</li>  
 * <li> Use a given parametrization of the covariance matrix to generate the fake measurements</li>  
 * </ol>
 Moreover some generation options  are  possible: \n
 * <ol> 
 * <li> smearing according a Gaussian (with sigma given by covariance matrix) or Double Gaussian distribution </li>  
 * <li> adding BIAS to the measurement and/or ScalingFactors to the covariance matrix (momentum dependent parametrization) </li> 
 * <li> generate correlated (or not) measurements </li>
 * <li> smear MCTruth information at its origin Vertex or at PointOnTrack </li>
 * </ol>     
 * 
 *   Some  of  the  implemented  properties  to  play  with  in  jobOption:  \n
 * <ol>
 * <li> Selection properties:\n 
 *                  \b ParticleNames list of the MCparticle ID to smear\n
 *                  \b OnlyReconstructible (Default:false) to chose only reconstructible MCParticles\n
 *                  \b OnlyReconstructed (Default:false) to chose only reconstructed MCParticles\n
 *                  \b SmearParticle (Default:true)\n
 *                  \b OnlyDecayProducts (Default:false) to chose only the MCParticle of a given Decay process\n
 *                  \b OnlyStableDecayProducts (Default:false) to generate Particles only from the stable products
 *                     of the decay channel. If true also  OnlyDecayProducts must be set true
 * </li> 
 * <li> Generation properties:\n
 *                  \b UseReconstructedCovariance (Default:false) to use reconstrucded covariance or a parametrization of it \n
 *                  \b IpErrorC0 (Default:0.0173*mm)  \b IpErrorC1 (Default:0.0265*mm) sets the parametrization 
 of the IP error (C0+C1/pt[GeV]) \n
 *                  \b SlopeError (Default:0.4*mrad) \n
 *                  \b MomError (Default:0.004) parametrize covariance matrix (MomErr*p[GeV])\n
 *                  \b SmearATPoT (Default:false) if true smears the MCParticle info at PointOnTrack 
 *                    (minimum distance from beam line) otherwise it smears at origin vertex \n
 *                  \b rhoVAR1VAR2 (Default=0.0) set the correlation coefficient between VAR1&VAR2 (VAR== x,y,z,tx,ty,p): 
 typical values in reconstructed particles: xtx=-0.95 yty=-0.95 
 * </li>
 * <li> In case of Double Gaussian generation USE:\n   
 *                  \b dualGaussW (Default=0.,0.,0.,0.,0.,0.) to set !=0 in case of double gaussian generation: 
 *                                                                   second gaussian fraction on (x,y,z,tx,ty,p)\n
 *                  \b dualGaussSF (Default=1.,1.,1.,1.,1.,1.) ratios of sigma2/sigma1 on (x,y,z,tx,ty,p)  
 * </li>  
 * <li>  To  add  momentum  dependent  Scaling  Factors  on  errors  (SF=1./(C0+C1*p)):\n   
 *                             \b ScaleFactorCovarianceC0 (Default=1.,1.,1.,1.,1.,1.) \n
 *                             \b ScaleFactorCovarianceC1 (Default=0.,0.,0.,0.,0.,0.) Vector of scaling factors on errors 
 (x,y,z,tx,ty,p)
 * </li> 
 * <li> To add a  momentum  dependent  BIAS  to  the  measurement  (BIAS=q*(C0+C1*p))  q=particle's  charge \n
 *                             \b MeasurementBiasC0 (Default=0.,0.,0.,0.,0.,0.)\n
 *                             \b MeasurementBiasC1 (Default=0.,0.,0.,0.,0.,0.)
 * </li> 
 * </ol> 
 *   
 *   @author Gerhard Raven with minor contributions from G.Balbi & S.Vecchi
 *   @date   2002-10-08
 * 
 **/


class MCParticleMaker 
  : public MCParticleMakerBase
  , public virtual IParticleMaker 
{
public:
  /// Standard constructor
  MCParticleMaker( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MCParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// Functional method to make particles.
  virtual StatusCode makeParticles( LHCb::Particle::ConstVector & parts );
  
 
protected:
  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
  std::vector<const LHCb::MCParticle*> getFinalState(const LHCb::MCParticle& m);
  
private:
  
  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< int > m_ids;                  ///< PDGid of Particles to make
  std::string m_input;
  bool    m_onlyDecayProducts;               ///< flag to select only specific decay products 
  bool    m_onlyStableDecayProducts;        ///<flag to select only STABLE products of a given Decay Channel(defined in JobOption)
  bool    m_onlyReconstructible;             ///< flag to use only Reconstructible MCparticles
  bool    m_onlyReconstructed;               ///< flag to use only Reconstruced MCparticles
  bool    m_useReconstructedCovariance;      ///< flag to use only Reconstructed Covariance matrix

  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IMCDecayFinder* m_pMCDecFinder;
  
  /// internal method
  StatusCode fillParticle( const LHCb::MCParticle& mc, 
                           LHCb::Particle& particle,
                           const Gaudi::SymMatrix7x7& cov);  ///< fill Particle according generation criteria
  bool reconstructible(const LHCb::MCParticle& icand) const; 
  const LHCb::Particle *reconstructed(const LHCb::MCParticle& icand) const;
  
  IMCReconstructible* m_reco ; ///< Reconstructible?
  bool m_requireReco ; ///< Require reconstructible
//=============================================================================
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // MCPARTICLEMAKER_H
// ============================================================================

