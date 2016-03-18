#ifndef LBPYTHIA8_PYTHIA8PRODUCTION_H 
#define LBPYTHIA8_PYTHIA8PRODUCTION_H 1

// Gaudi.
#include "GaudiAlg/GaudiTool.h"
#include "Generators/IProductionTool.h"

/** 
 * Production tool to generate bound states.
 *  
 * TO-DO.
 * 
 * @class  BoundProduction
 * @file   BoundProduction.h 
 * @author ...
 * @author Philip Ilten
 * @date   2016-03-18
 */
class BoundProduction : public GaudiTool, virtual public IProductionTool {
public:
  typedef std::vector<std::string> CommandVector ;
  
  /// Default constructor.
  BoundProduction(const std::string& type, const std::string& name,
		  const IInterface* parent);
  
  /// Initialize the tool.
  virtual StatusCode initialize();
  
  /// Initialize the generator.
  virtual StatusCode initializeGenerator();

  /// Finalize the tool.
  virtual StatusCode finalize();
  
  /// Generate an event.
  virtual StatusCode generateEvent(HepMC::GenEvent* theEvent, 
				   LHCb::GenCollision* theCollision);
  
  /// Set particle stable.
  virtual void setStable(const LHCb::ParticleProperty* thePP);

  /// Update a particle.
  virtual void updateParticleProperties(const LHCb::ParticleProperty* thePP);

  /// Turn on fragmentation.
  virtual void turnOnFragmentation();

  /// Turn off fragmentation.
  virtual void turnOffFragmentation();

  /// Hadronize an event.
  virtual StatusCode hadronize(HepMC::GenEvent* theEvent, 
			       LHCb::GenCollision* theCollision);
  
  /// Save the event record.
  virtual void savePartonEvent(HepMC::GenEvent* theEvent);

  /// Retrieve the event record.
  virtual void retrievePartonEvent(HepMC::GenEvent* theEvent);

  /// Print the running conditions.
  virtual void printRunningConditions();

  /// Returns whether a particle has special status.
  virtual bool isSpecialParticle(const LHCb::ParticleProperty* thePP) const;

  /// Setup forced fragmentation.
  virtual StatusCode setupForcedFragmentation(const int thePdgId);

protected:

  // Methods.
  StatusCode bindStates(HepMC::GenEvent *theEvent); /// Create the bound states.

  // Properties.
  CommandVector m_userSettings; ///< The user settings vector.
  std::string m_beamToolName;   ///< The beam tool name.
  std::string m_prodToolName;   ///< The production tool name.

  // Members.
  int m_nEvents;                ///< Number of events.
  IProductionTool *m_prodTool;  ///< The production tool.
};

#endif // LBPYTHIA8_PYTHIA8PRODUCTION_H
