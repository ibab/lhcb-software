// $Id:
#ifndef FTCOMBINEDPARTICLEMAKER_H 
#define FTCOMBINEDPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>
#include <utility>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// Forward declarations
class IParticlePropertySvc;

/** @class FTCombinedParticleMaker FTCombinedParticleMaker.h
 *  
 *
 *  @author Marco Musy, merging various makers
 *  @date   2002-15-11
 */
class FTCombinedParticleMaker : public AlgTool,
			      virtual public IParticleMaker {
public:
  /// Standard constructor
  FTCombinedParticleMaker( const std::string& type, 
			 const std::string& name,
			 const IInterface* parent);

  virtual ~FTCombinedParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;

  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
  
  /// Functional method to make particles.
  virtual StatusCode makeParticles( ParticleVector & parts );
  
  /// Finalize
  StatusCode finalize();

  /// internal method
  StatusCode fillParticle( const ProtoParticle* protoCand, 
                           int protoID, double protoCL,
                           Particle* particle, 
                           HepLorentzVector  m_momentum );
private:
  std::string m_chargedinput;

  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service

  /// cut on chi2 of CaloTrMatch
  double m_CutOnChi2CaloTrMatch    ;
  
  /// cut on chi2 of CaloEMatch
  double m_CutOnChi2CaloEMatch     ;
  
  /// cut on chi2 of BremMatch
  double m_CutOnChi2BremMatch      ;
  
  /// cut on RICH probability that the ProtoParticle is an e+/e-
  double m_CutOnRichProb           ;
  
  /// type of id wanted (see job options for more details)
  int    m_TypeOfIdentification    ;
  
  /// if m_choiceOnBremPhot=1 brem photon are added neither not
  int    m_choiceOnBremPhot        ;
  
  ///cut on the distance between phot cluster and extrapolation of track to ECAL
  double m_cutOnClusterDist        ; 

  /// cut on de of combined PID
  double m_deCut;
  int    m_flag;

  /// number of electron created
  int    m_nTotalElecPosiMade           ; /// number of elec/posi created
    
};
#endif // FTCOMBINEDPARTICLEMAKER_H
