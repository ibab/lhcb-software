// $Id: ElectronParticleMaker_bbincl.h,v 1.1 2003-04-30 17:01:07 gcorti Exp $
#ifndef ElectronParticleMAKER_BBINCL_H 
#define ElectronParticleMAKER_BBINCL_H 1

// Include files
// from STL
#include <string>
#include <utility>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"

// TDR ID
// LHCBKernel/Relations
#include "Relations/Associator.h"
#include "Relations/AssociatorWeighted.h"

// Forward declarations
class IParticlePropertySvc;
/** @class ElectronParticleMaker_bbincl ElectronParticleMaker_bbincl.h
 *  
 *
 *  @author Gloria Corti
 *  @date   2002-07-11
 */
class ElectronParticleMaker_bbincl : public AlgTool,
                              virtual public IParticleMaker{
public:
  /// Standard constructor
  ElectronParticleMaker_bbincl( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~ElectronParticleMaker_bbincl( ); ///< Destructor

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
			
  ///other internal methods
  double chi2BremMatch( ProtoParticle * proto );
  HepLorentzVector MomOfBremPhBremMatch(ProtoParticle* proto);
  double XClusterBremMatch(ProtoParticle* proto);
  double YClusterBremMatch(ProtoParticle* proto);
  
  double TDRCombinedPID( ProtoParticle* proto );

protected:
 
private:

  ///<  Reference to Particle Property Service
  IParticlePropertySvc* m_ppSvc;  
  
  ///<  Reference to Event Data Service
  IDataProviderSvc* m_EDS;        

  /// pid of elec (11)
  int m_ePID;     

  /// total number of protoparticles retrieved
  int m_nProtoP;

  // number of e+e- identified
  int m_nTotalElecPosiMade;

  // location of charged protoparticles
  std::string m_chargedinput;
  
  /// if m_choiceOnBremPhot=1 brem photon are added neither not
  std::string m_choiceOnBremPhot;

  /// cut on chi2 of BremMatch
  double  m_CutOnChi2BremMatchIP;
  double  m_CutOnChi2BremMatchMP;
  double  m_CutOnChi2BremMatchOP;

  /// cut on delta log-likelihood
  double  m_DLLCut;

  // TDR IDENTIFICATION
  typedef IAssociator<TrStoredTrack,float>   IAsctTr   ;
  typedef IAsctTr::DirectType                TableTr;
  typedef TableTr::Range                     RangeTr;
  
  std::string         m_dll_EcalPIDe_Type ;
  std::string         m_dll_EcalPIDe_Name ;
  IAsctTr*            m_dll_EcalPIDe      ;
  
  std::string         m_dll_PrsPIDe_Type  ;
  std::string         m_dll_PrsPIDe_Name  ;
  IAsctTr*            m_dll_PrsPIDe       ;

  std::string         m_dll_BremPIDe_Type ;
  std::string         m_dll_BremPIDe_Name ;
  IAsctTr*            m_dll_BremPIDe      ;

  std::string         m_dll_HcalPIDe_Type ;
  std::string         m_dll_HcalPIDe_Name ;
  IAsctTr*            m_dll_HcalPIDe      ;

};
#endif // ElectronParticleMAKER_BBINCL_H
