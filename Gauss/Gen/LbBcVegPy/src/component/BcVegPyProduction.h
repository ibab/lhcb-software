#ifndef LBBCVEGPY_BCVEGPYPRODUCTION_H 
#define LBBCVEGPY_BCVEGPYPRODUCTION_H 1

// Pythia.
#include "LbPythia/PythiaProduction.h"

/** 
 *  
 * Production tool to generate events with BcVegPy.
 * 
 * TODO: add further documentation.
 * 
 * @class  BcVegPyProduction
 * @file   BcVegPyProduction.h
 * @author Philip Ilten (original tool by Jibo He and Zhenwei Yang)
 * @date   2014-05-08
 */
class BcVegPyProduction : public GaudiTool, virtual public IProductionTool {
 public:
  typedef std::vector<std::string> CommandVector;

  /// Default constructor.
  BcVegPyProduction(const std::string &type, const std::string &name,
		    const IInterface *parent);
  
  /// Default destructor.
  virtual ~BcVegPyProduction();
  
  /// Initialize the tool.
  virtual StatusCode initialize();
  
  /// Generate an event.
  virtual StatusCode generateEvent(HepMC::GenEvent *theEvent, 
				   LHCb::GenCollision *theCollision);

  /// Initialize the generator.
  virtual StatusCode initializeGenerator() {return StatusCode::SUCCESS;}

  // TODO: migrate relevant methods to IHardProcessProduction.
  virtual void setStable(const LHCb::ParticleProperty *thePP);
  virtual void updateParticleProperties(const LHCb::ParticleProperty *thePP);
  virtual void turnOnFragmentation();
  virtual void turnOffFragmentation();
  virtual StatusCode hadronize(HepMC::GenEvent *theEvent, 
			       LHCb::GenCollision *theCollision); 
  virtual void savePartonEvent(HepMC::GenEvent *theEvent);
  virtual void retrievePartonEvent(HepMC::GenEvent *theEvent);
  virtual void printRunningConditions();
  virtual bool isSpecialParticle(const LHCb::ParticleProperty *thePP) const; 
  virtual StatusCode setupForcedFragmentation(const int thePdgId); 

  /**
   * Writes event to file.
   *
   * Debugging method, writes the HepMC event to the file specified by
   * m_writeEventOutput (specified by the WriteEventOutput Python option). If
   * this variable is an empty string, no HepMC output is written.
   */
  StatusCode writeEvent(HepMC::GenEvent *theEvent);
  
  /// Debugginig method, prints the event to screen.
  StatusCode printEvent(HepMC::GenEvent *theEvent);

  /**
   * Reads an event from file.
   * 
   * Debugging method, reads the HepMC from file and sets this as the event
   * to be passed from the tool. The input file is given by m_readEventInput
   * which is specified by the ReadEventInput Python option.
   */
  StatusCode readEvent(HepMC::GenEvent *theEvent);

protected:
  
  /// Parse BcVegPy commands from a string vector.
  StatusCode parseBcVegPyCommands(const CommandVector &theVector);
  
private:

  // The showering tools.
  PythiaProduction m_pythia;      ///< Pythia 6 showering tool.

  // Additional members.
  CommandVector m_showerSettings;  ///< Shower command vector.
  CommandVector m_defaultSettings; ///< Default command vector.
  CommandVector m_commandSettings; ///< User command vector.

  bool m_printEvent;               ///< Flag to print the event to screen.
  std::string m_writeEventOutput;  ///< File name to write output.
  HepMC::IO_GenEvent *m_hepmcOut;  ///< HepMC output object.
  std::string m_readEventInput;    ///< File name of HepMC event to read.

  // Settings that are needed for Pythia.
  CommandVector m_pygive;     ///< Commands in "Pygive" format.
  std::vector<int> m_pdtlist; ///< List of particles to be printed.
  std::string m_beamToolName; ///< Name of the beam tool to use.
  double m_widthLimit;        ///< Limit to consider a zero lifetime particle.
  std::string m_slhaDecayFile; 
  std::vector<int> m_pdecaylist;
  std::string m_slhaSpectrumFile;
  bool m_validate_HEPEVT;
  std::string m_inconsistencies;
  std::vector<int> m_updatedParticles;
  std::vector<int> m_particlesToAdd;
};

#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
