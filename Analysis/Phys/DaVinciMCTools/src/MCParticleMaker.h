// $Id: MCParticleMaker.h,v 1.6 2005-12-15 14:19:50 pkoppenb Exp $
#ifndef MCPARTICLEMAKER_H 
#define MCPARTICLEMAKER_H 1

// Include files
// from STL
#include <vector>
#include <string>
#include <utility>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"
#include "CLHEP/Matrix/SymMatrix.h"

// from DaVinciTools
#include "Kernel/IParticleMaker.h"
#include "DaVinciMCTools/IDebugTool.h"
// Forward declarations
class IParticlePropertySvc;
class IRndmGenSvc;
class IMCDecayFinder;

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
 * <li> Ask to be reconstructable particles</li>  
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
 *                  \b OnlyReconstructable (Default:false) to chose only reconstructable MCParticles\n
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
class MCParticleMaker : public GaudiTool,
                        virtual public IParticleMaker {
public:
  /// Standard constructor
  MCParticleMaker( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MCParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// Functional method to make particles.
  virtual StatusCode makeParticles( ParticleVector & parts );
  
  // Finalize
  //  StatusCode finalize();
 
protected:
  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
  std::vector<const MCParticle*> getFinalState(const MCParticle& m);

private:

  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< int > m_ids;                  ///< PDGid of Particles to make
  std::string m_input;
  bool    m_onlyDecayProducts;               ///< flag to select only specific decay products 
  bool    m_onlyStableDecayProducts;        ///<flag to select only STABLE products of a given Decay Channel(defined in JobOption)
  bool    m_onlyReconstructable;             ///< flag to use only Reconstructable MCparticles
  bool    m_onlyReconstructed;               ///< flag to use only Reconstruced MCparticles
  bool    m_useReconstructedCovariance;      ///< flag to use only Reconstructed Covariance matrix
  bool    m_smearParticle;                   ///< flag to Smear PArticles
  double  m_ipErrorC0;                       ///< C0 constant for Impact parameter error parametrization 
  double  m_ipErrorC1;                       ///< C1 constant for Impact parameter error parametrization 
  double  m_slopeError;                      ///< constant for Slope error parametrization
  double  m_momError;                        ///< constant for momentum error parametrization
  bool    m_smearATPoT;                      ///< flag to smeat particle at PointOnTrack (minimum distance to the beam line)
  double m_rhoxy;                            ///< correlation coefficient x-y 
  double m_rhoxz;                            ///< correlation coefficient x-z
  double m_rhoyz;                            ///< correlation coefficient y-z
  double m_rhoxtx;                           ///< correlation coefficient x-tx
  double m_rhoxty;                           ///< correlation coefficient x-ty
  double m_rhoxp;                            ///< correlation coefficient x-p
  double m_rhoytx;                           ///< correlation coefficient y-tx
  double m_rhoyty;                           ///< correlation coefficient y-ty
  double m_rhoyp;                            ///< correlation coefficient y-p
  double m_rhoztx;                           ///< correlation coefficient z-tx
  double m_rhozty;                           ///< correlation coefficient z-ty
  double m_rhozp;                            ///< correlation coefficient z-p
  double m_rhotxty;                          ///< correlation coefficient tx-ty
  double m_rhotxp;                           ///< correlation coefficient tx-p
  double m_rhotyp;                           ///< correlation coefficient ty-p

  std::vector< double >  m_covSFsC0, m_covSFsC1; ///< vector for SF parametrization of Scaling Factors on errors 1./(C0+C1*p[GeV])
  std::vector< double >  m_BIASsC0,  m_BIASsC1; ///< vector for BIAS parametrization q*(C0+C1*p[GeV])
  std::vector< double > m_dualGaussSF; ///<vector of Second Gaussian relative sigma (sigma2/sigma1) FOR Double Gaussian Generation
  std::vector< double > m_dualGaussWeight;      ///<  vector of Second Gaussian Weights FOR Double Gaussian Generation

  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IMCDecayFinder* m_pMCDecFinder;
  Rndm::Numbers m_ranGauss;   
  Rndm::Numbers m_ranFlat;


  /// internal method
  StatusCode fillParticle( const MCParticle& mc, 
                           Particle& particle,
                           const HepSymMatrix& cov);  ///< fill Particle according generation criteria
  bool reconstructable(const MCParticle& icand) const; 
  const Particle *reconstructed(const MCParticle& icand) const;
  HepSymMatrix * fetchCovariance(const Particle& p ) const;
  HepSymMatrix * generateCovariance(const HepLorentzVector& p) const;///< Generate covariance according realistic parametrization

  
};
#endif // MCPARTICLEMAKER_H
