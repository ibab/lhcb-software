#ifndef LBPOWHEG_POWHEGPRODUCTION_H 
#define LBPOWHEG_POWHEGPRODUCTION_H

// Include files.
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"
#include "Pythia.h"
#include "powheg.h"
#include "LbPowheg/PowhegHooks.h"

// Forward declarations.
class IBeamTool;
class GaudiRandomForPowheg;

/** @class PowhegProduction PowhegProduction.h 
 *  
 *  Interface tool to produce events with Pythia8 interfaced with Powheg.
 * 
 *  @author Philip Ilten
 *  @date   2012-11-20
 */
class PowhegProduction : public GaudiTool, virtual public IProductionTool {
  
public:

  /// Constructor.
  typedef std::vector<std::string> CommandVector;
  PowhegProduction(const std::string &type, const std::string &name,
		   const IInterface *parent);
  
  /// Destructor.
  virtual ~PowhegProduction();
  
  /// Initializer.
  virtual StatusCode initialize();
  
  /// Finalize.
  virtual StatusCode finalize();
  
  /// Generate an event.
  virtual StatusCode generateEvent(HepMC::GenEvent *theEvent, 
				   LHCb::GenCollision *theCollision);

  /// Initialization specific to Powheg.
  virtual StatusCode initializeGenerator();
  
  /// Set particle stable.
  virtual void setStable(const LHCb::ParticleProperty *thePP);

  /// Modify particle property.
  virtual void updateParticleProperties(const LHCb::ParticleProperty *thePP);

  /// Turn on fragmentation.
  virtual void turnOnFragmentation();

  /// Turn off fragmentation.
  virtual void turnOffFragmentation();

  /// Hadronize unfragmented event.
  virtual StatusCode hadronize(HepMC::GenEvent *theEvent , 
			       LHCb::GenCollision *theCollision);
  
  /// Not needed in Powheg.
  virtual void savePartonEvent(HepMC::GenEvent *theEvent);

  /// Not needed in Powheg.
  virtual void retrievePartonEvent(HepMC::GenEvent *theEvent);

  /// Print parameters.
  virtual void printRunningConditions();

  /// Return special particle flag.
  virtual bool isSpecialParticle(const LHCb::ParticleProperty *thePP) const;
  
  /// Apply settings for forced fragmentation method.
  virtual StatusCode setupForcedFragmentation(const int thePdgId);
  
protected:
  
  /// Print Pythia parameters.
  void printPythiaParameter() ;
  
  /// Retrieve hard process information.
  void hardProcessInfo(LHCb::GenCollision *theCollision);
  
  /// Pythia -> HEPMC conversion. 
  StatusCode toHepMC (HepMC::GenEvent *theEvent, 
		      LHCb::GenCollision *theCollision);
  
private:

  /// Retrieve the process code.
  int processCode();
    
  /// Default settings.
  CommandVector m_defaultSettings;
  
  /// User settings.
  CommandVector m_userSettings;

  /// Beam tool name.
  std::string m_beamToolName;
  
  /// Energy and IDs of incoming beam particles.
  int m_id1, m_id2;
  double m_cme;
	
  /// Beam tool.
  IBeamTool * m_beamTool;

  /// Pythia generator.
  Pythia8::Pythia *m_pythia;
  Powheg::Powheg  *m_powheg;
  PowhegHooks     *m_hooks;

  /// Pythia event record.
  Pythia8::Event m_event;
  std::vector<int> m_pdtlist;
  int m_nEvents;
  
  /// Random number generator.
  GaudiRandomForPowheg * m_randomEngine ; 

  /// List of particles.
  bool m_listAllParticles;
};
#endif // LBPOWHEG_POWHEGPRODUCTION_H
