// $Id$
// ============================================================================
#ifndef DAVINCIMCTOOLS_MCPARTICLEMAKERBASE_H 
#define DAVINCIMCTOOLS_MCPARTICLEMAKERBASE_H 1
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

/** @class MCParticleMakerBase MCParticleMakerBase.h
 *
 * This tool fills the particle class with information from (Hep)MCParticles 
 *  and places it in the Transient Event Store. 
 * 
 * It allows several utilities:
 * <ol> 
 * <li> Smear all the MCParticle in the list \b ParticleNames which are 
 *      found in the desktop </li>
 * <li> Smear the information of the first MCParticle of the Decay Channel 
 *      specified in jobOption(e.g. B if B->pipi is chosen)</li>
 * <li> Smear the information of the only stable products of the Decay 
 *      Channel specified in jobOption (e.g. pipi  
 *      if B->pipi is chosen or pi+ pi- mu+ mu- in B->Jpsi(mumu)Ks(pipi)) </li>  
 * </ol>
 *  Different additional selection can be applied to the MCParticles:
 * <ol> 
 * <li> Ask to be reconstructed particles</li>  
 * <li> Ask to be reconstructible particles</li>  
 * <li> Use reconstructed covariance to generate the fake measurements</li>  
 * <li> Use a given parametrization of the covariance matrix 
 *      to generate the fake measurements</li>  
 * </ol>
 *  Moreover some generation options  are  possible: \n
 * <ol> 
 * <li> smearing according a Gaussian (with sigma given by covariance matrix) 
 *      or Double Gaussian distribution </li>  
 * <li> adding BIAS to the measurement and/or ScalingFactors to the covariance
 *      matrix (momentum dependent parametrization) </li> 
 * <li> generate correlated (or not) measurements </li>
 * <li> smear MCTruth information at its origin Vertex or at PointOnTrack </li>
 * </ol>     
 * 
 *   Some  of  the  implemented  properties  to  play  with  in  jobOption:  \n
 * <ol>
 * <li> Selection properties:\n 
 *        \b ParticleNames list of the MCparticle ID to smear\n
 *        \b OnlyReconstructible (Default:false) to chose only reconstructible MCParticles\n
 *        \b OnlyReconstructed (Default:false) to chose only reconstructed MCParticles\n
 *        \b SmearParticle (Default:true)\n
 *        \b OnlyDecayProducts (Default:false) 
 *            to chose only the MCParticle of a given Decay process\n
 *        \b OnlyStableDecayProducts (Default:false) 
 *           to generate Particles only from the stable products
 *           of the decay channel. If true also  OnlyDecayProducts must be set true
 * </li> 
 * <li> Generation properties:\n
 *        \b UseReconstructedCovariance (Default:false) 
 *              to use reconstrucded covariance or a parametrization of it \n
 *        \b IpErrorC0 (Default:0.0173*mm)  
 *        \b IpErrorC1 (Default:0.0265*mm) sets the parametrization 
 *            of the IP error (C0+C1/pt[GeV]) \n
 *        \b SlopeError (Default:0.4*mrad) \n
 *        \b MomError (Default:0.004) parametrize covariance matrix (MomErr*p[GeV])\n
 *        \b SmearATPoT (Default:false) if true smears the MCParticle info at PointOnTrack 
 *           (minimum distance from beam line) otherwise it smears at origin vertex \n
 *        \b rhoVAR1VAR2 (Default=0.0) set the correlation coefficient between 
 *           VAR1&VAR2 (VAR== x,y,z,tx,ty,p): 
 *           typical values in reconstructed particles: xtx=-0.95 yty=-0.95 
 * </li>
 * <li> In case of Double Gaussian generation USE:\n   
 *        \b dualGaussW (Default=0.,0.,0.,0.,0.,0.) 
 *           to set !=0 in case of double gaussian generation: 
 *           second gaussian fraction on (x,y,z,tx,ty,p)\n
 *        \b dualGaussSF (Default=1.,1.,1.,1.,1.,1.) ratios of 
 *           sigma2/sigma1 on (x,y,z,tx,ty,p)  
 * </li>  
 * <li>  To  add  momentum  dependent  Scaling  Factors  on  errors  (SF=1./(C0+C1*p)):\n   
 *        \b ScaleFactorCovarianceC0 (Default=1.,1.,1.,1.,1.,1.) \n
 *        \b ScaleFactorCovarianceC1 (Default=0.,0.,0.,0.,0.,0.) 
 *           Vector of scaling factors on errors (x,y,z,tx,ty,p)
 * </li> 
 * <li> To add a  momentum  dependent  BIAS  to  the  measurement  
 *         (BIAS=q*(C0+C1*p))  q=particle's  charge \n
 *        \b MeasurementBiasC0 (Default=0.,0.,0.,0.,0.,0.)\n
 *        \b MeasurementBiasC1 (Default=0.,0.,0.,0.,0.,0.)
 * </li> 
 * </ol> 
 *   
 *   @author Gerhard Raven with minor contributions from G.Balbi & S.Vecchi
 *   @date   2002-10-08
 * 
 *   @todo implement the smearing for photons 
 */
class MCParticleMakerBase : public GaudiTool
{
public:
  /// Initialize
  virtual StatusCode initialize() ;
  /// Finalize
  virtual StatusCode finalize  () ;
protected:
  /// internal method
  StatusCode fillParticle
  ( const Gaudi::LorentzVector&  mom      ,
    const Gaudi::XYZPoint&       point    ,
    const LHCb::ParticleID&      pid      ,
    const Gaudi::SymMatrix7x7&   cov      , 
    LHCb::Particle&              particle ) ;
  /// Generate covariance according realistic parametrization  
  StatusCode generateCovariance 
  ( const Gaudi::LorentzVector&  momentum ,
    Gaudi::SymMatrix7x7&         ccc      ) ;
  /// get correlation matrix 
  inline const Gaudi::SymMatrix6x6& rho() const { return m_rho ; }
  /// generate a vector of correlated random numbers according cov matrix
  StatusCode correlatedRandomVectorGenerator
  ( const  Gaudi::SymMatrix7x7& cov, Gaudi::Vector7& vector );
protected:  
  /// (optional) location of output relation table
  const std::string& outputTable() const { return m_outputTable ; }
  /// set (optional) location of output relation table
  void setOutputTable( const std::string& value ) { m_outputTable = value ; }
public:
  /// Standard constructor
  MCParticleMakerBase
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  /// protected and virtual destructor 
  virtual ~MCParticleMakerBase(){}; ///< Destructor
public:
  enum 
    { 
      _X  = 0 , 
      _Y  = 1 , 
      _Z  = 2 , 
      _TX = 3 , 
      _TY = 4 , 
      _P  = 5 
    } ;
private:
  // default constructor is disabled 
  MCParticleMakerBase () ;
  // copy constructor is disabled 
  MCParticleMakerBase ( const MCParticleMakerBase& ) ;
  // assignement operator is disabled 
  MCParticleMakerBase& operator=( const MCParticleMakerBase& ) ;
private:
  bool    m_smearParticle; ///< flag to Smear Particles
  bool    m_smearATPoT;    ///< flag to smeat particle at PointOnTrack (minimum distance to the beam line)
  double  m_ipErrorC0;     ///< C0 constant for IP error parametrization 
  double  m_ipErrorC1;     ///< C1 constant for IP error parametrization 
  double  m_ipErrorZ;      ///< Error on Z 
  double  m_slopeError;     ///< constant for Slope error parametrization
  double  m_momError;       ///< constant for momentum error parametrization
  //
  // (optional) location of output relation table
  std::string m_outputTable ; ///< (optional) location of output relation table
  //
  Gaudi::SymMatrix6x6 m_rho;
  //
  std::vector< double >  m_covSFsC0, m_covSFsC1; ///< vector for SF parametrization of Scaling Factors on errors 1./(C0+C1*p[GeV])
  std::vector< double >  m_BIASsC0,  m_BIASsC1; ///< vector for BIAS parametrization q*(C0+C1*p[GeV])
  std::vector< double > m_dualGaussSF; ///<vector of Second Gaussian relative sigma (sigma2/sigma1) FOR Double Gaussian Generation
  std::vector< double > m_dualGaussWeight;      ///<  vector of Second Gaussian Weights FOR Double Gaussian Generation
  //
  Rndm::Numbers m_ranGauss;   
  Rndm::Numbers m_ranFlat;  
};

// ============================================================================
// The END 
// ============================================================================
#endif // MCPARTICLEMAKERBASE_H
// ============================================================================

