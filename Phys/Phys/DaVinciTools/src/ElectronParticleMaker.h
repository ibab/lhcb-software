// $Id: ElectronParticleMaker.h,v 1.2 2002-10-22 17:41:30 gcorti Exp $
#ifndef ElectronParticleMAKER_H 
#define ElectronParticleMAKER_H 1

// Include files
// from STL
#include <string>
#include <utility>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// ASSOCIATORS
//#include "DaVinciAssociators/ProtoParticle2MCAsct.h"

// Forward declarations
class IParticlePropertySvc;
//class IHistogram1D;

/** @class ElectronParticleMaker ElectronParticleMaker.h
 *  
 *
 *  @author Gloria Corti
 *  @date   2002-07-11
 */
class ElectronParticleMaker : public AlgTool,
                             virtual public IParticleMaker {
public:
  /// Standard constructor
  ElectronParticleMaker( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~ElectronParticleMaker( ); ///< Destructor

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
			
  
protected:
 
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
  
  /// electron particleID
  int m_eID;
  
  
};
#endif // ElectronParticleMAKER_H
