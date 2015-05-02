#ifndef LBHARD_HARDPPRODUCTION_H 
#define LBHARD_HARDPPRODUCTION_H 1

// Generators.
#include "Generators/IProductionTool.h"

// Parton showers packages.
#include "LbPythia/PythiaProduction.h"
#include "LbPythia8/Pythia8Production.h"

/**
 * Production tool base class to generate a hard process and then
 * shower the event.
 * 
 * @class  HardProduction
 * @file   HardProduction.h 
 * @author Philip Ilten
 * @date   2015-05-01
 */
class HardProduction : public GaudiTool, virtual public IProductionTool {
 public:
  typedef std::vector<std::string> CommandVector;
  
  /// Default constructor.
  HardProduction(const std::string &type, const std::string &name,
		     const IInterface *parent);
  
  /// Initialize the tool.
  StatusCode initialize();

  /// Initialize the generator.
  StatusCode initializeGenerator();

  /// Finalize the tool.
  StatusCode finalize(); 
  
  /// Generate an event.
  StatusCode generateEvent(HepMC::GenEvent *theEvent, 
			   LHCb::GenCollision *theCollision);

  /// Set a particle stable.
  void setStable(const LHCb::ParticleProperty *thePP);

  /// Update a particle.
  void updateParticleProperties(const LHCb::ParticleProperty *thePP);

  /// Turn off fragmentation.
  void turnOnFragmentation();

  /// Turn on fragmentation.
  void turnOffFragmentation();

  /// Hadronize an event.
  StatusCode hadronize(HepMC::GenEvent *theEvent, 
		       LHCb::GenCollision *theCollision);
  
  /// Save the event record.
  void savePartonEvent(HepMC::GenEvent* theEvent);

  /// Retrieve the event record.
  void retrievePartonEvent( HepMC::GenEvent* theEvent);
  
  /// Print the running conditions.
  void printRunningConditions();

  /// Returns whether a particle has special status.
  bool isSpecialParticle(const LHCb::ParticleProperty* thePP) const;

  /// Setup forced fragmentation.
  StatusCode setupForcedFragmentation(const int thePdgId);
  
  /**
   * Initialize the hard process tool. Called before the shower tool
   * is initialized.
   */
  virtual StatusCode hardInitialize();

  /**
   * Initialize the hard process generator. Called before the shower
   * generator is initialized.
   */
  virtual StatusCode hardInitializeGenerator();
  
  /// Finalize the hard process tool.
  virtual StatusCode hardFinalize();

  /// Set a particle stable for the hard process generator.
  virtual void hardSetStable(const LHCb::ParticleProperty *thePP);

  /// Update a particle for the hard process generator.
  virtual void hardUpdateParticleProperties(const LHCb::ParticleProperty	
					    *thePP);

  /// Print the running conditions for the hard process generator.
  virtual void hardPrintRunningConditions();

  /**
   * Returns whether a particle has special status for the hard
   * process generator.
   */
  virtual bool hardIsSpecialParticle(const LHCb::ParticleProperty* thePP) const;

  // Members needed externally.
  PythiaProduction *m_pythia;   ///< The Pythia tool (may be null).
  Pythia8Production *m_pythia8; ///< The Pythia 8 tool (may be null).

protected:

  // Properties.
  CommandVector m_userSettings; ///< The user settings vector.
  std::string m_beamToolName;   ///< The beam tool name.
  std::string m_showerToolName; ///< The shower generator name.

  // Members.
  int m_nEvents;                ///< Number of events.
  IProductionTool *m_shower;    ///< The shower tool.
  IProductionTool *m_hard;      ///< The hard process tool (may be null).
  IBeamTool *m_beamTool;        ///< The Gaudi beam tool.

  /**
   * Pythia 8 user hooks pointer.
   *
   * The Pythia 8 tool deletes this object, so must be malloced with
   * new and should not be deleted by this tool.
   */
  Pythia8::UserHooks *m_hooks;

  /**
   * Pythia 8 Les Houches Accord user process pointer.
   *
   * The Pythia 8 tool deletes this object, so must be malloced with
   * new and should not be deleted by this tool.
   */
  Pythia8::LHAup *m_lhaup;
};

#endif // LBHARD_HARDPRODUCTION_H
