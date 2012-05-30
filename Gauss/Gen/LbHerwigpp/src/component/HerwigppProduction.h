#ifndef LBHERWIGPP_HERWIGPPPRODUCTION_H 
#define LBHERWIGPP_HERWIGPPPRODUCTION_H 1

// Beamtool declaration.
class IBeamTool;

/** 
 * 
 * Wrapper for ThePEG::EventGenerator class to provide RandomGenerator access.
 *
 * Random generators in ThePEG should be accessed using ThePEG::UseRandom
 * methods, but with the current version of ThePEG and Herwig++ this causes
 * segmentation violations (the reason has not been fully investigated but has
 * been reported). Consequently, it is necessary to access the RandomGenerator
 * through the ThePEG::EventGenerator::random() method which is protected.
 * This class inherets from ThePEG::EventGenerator and makes the protected
 * random() method available through the accessRandom() method.
 * 
 * @class  HerwigppGenerator
 * @file   HerwigppProduction.h 
 * @author Philip Ilten
 * @date   2011-10-10
 */
class HerwigppGenerator : public ThePEG::EventGenerator {

public:
  
  /// Default constructor.
  HerwigppGenerator() {};

  /// Constructor which clones the passed EventGenerator.
  HerwigppGenerator(ThePEG::EventGenerator *gen) :
    ThePEG::EventGenerator(*gen) {};

  /// Default destructor.
  ~HerwigppGenerator() {};
  
  /// Returns the random generator for the EventGenerator.
  ThePEG::RandomGenerator *accessRandom() {return &this->random();}
};

/**
 * 
 * Production tool to generate events with Herwig++.
 * 
 * Production of events using the Herwig++ Monte Carlo event generator is
 * provided by this class. Herwig++, while the C++ successor of the Fortran
 * Herwig generator, is a complete rewrite and incorporates many new features
 * including full helicity correlations, cluster hadronization, and NLO ME
 * implementations using the the POWHEG matching method.
 * 
 * @class  HerwigppProduction
 * @file   HerwigppProduction.h 
 * @author Philip Ilten
 * @date   2010-03-30
 */
class HerwigppProduction : public GaudiTool, virtual public IProductionTool {
 public:
  typedef std::vector<std::string> CommandVector;
  
  /// Default constructor.
  HerwigppProduction(const std::string &type, const std::string &name,
		     const IInterface *parent);
  
  /// Default destructor.
  virtual ~HerwigppProduction();
  
  /**
   * Initialize the tool.
   *
   * Initializes the tool, but leaves Herwig++ specifics
   * to initializeGenerator(). The Gaudi tool, random number service,
   * beam tool, and particle property services are initialized. Additionally,
   * the beam tool is used to set the Herwig++ generator beam parameters.
   */
  virtual StatusCode initialize();
  
  /// Finalize the tool.
  virtual StatusCode finalize(); 
  
  /// Initialize Herwig++ generator.
  virtual StatusCode initializeGenerator();
  
  /// Generate an event.
  virtual StatusCode generateEvent(HepMC::GenEvent *theEvent, 
				   LHCb::GenCollision *theCollision);

  /**
   * Convert Herwig++ event pointer to HepMC format.
   * 
   * This method converts the native output of Herwig++ to the HepMC format
   * through several steps. First, the vertex positions of the particles are
   * modified so that the correct ordering is passed to GEANT. This is
   * necessary because of a bug in some vertex positions produced by Herwig++
   * (ticket 152). A fix has been introduced through the hadronization method
   * but this does not work in the rare cases that hadronization is switched
   * off.
   * 
   * Second, the status codes of the particles are converted from the Herwig++
   * format to the Gauss format. Specifically, all generator specific particles
   * are set to status 3, and all non-stable particles to status 2. Finally,
   * initial parton information (including PDF's used) is added to the event
   * record, although currently this information is not used.
   */
  virtual StatusCode toHepMC (HepMC::GenEvent *theEvent);

  /**
   * Set a particle stable.
   * 
   * Sets a particle as stable by calling ThePEG repository and executing the
   * "set /Herwig/Particles/ParticleName:Stable Stable" command. Note that this
   * method will have no affect if it is called after the initializeGenerator()
   * method as changes to the repository do not influence the already created
   * event generator object.
   */
  virtual void setStable(const LHCb::ParticleProperty *thePP);

  /**
   * Update a particle.
   *
   * Updates the properties for a particle by calling ThePEG repository and
   * executing the "set /Herwig/Particles/ParticleName:ParticleProperty Value"
   * command. Note that this method will have no affect if it is called after
   * the initializeGenerator() methad as changes to the repository do not
   * influence the already created event generator object.
   */
  virtual void updateParticleProperties(const LHCb::ParticleProperty * thePP);

  /// Sets Herwig++'s default hadronization handler (ClusterHandler).
  virtual void turnOnFragmentation();

  /// Sets Herwig++'s hadronization handler to NULL.
  virtual void turnOffFragmentation();

  /// Hadronize an event (this method is not implemented).
  virtual StatusCode hadronize(HepMC::GenEvent * theEvent, 
			       LHCb::GenCollision * theCollision);
  
  /// This method is not implemented.
  virtual void savePartonEvent(HepMC::GenEvent *theEvent);

  /// This method is not implemented.
  virtual void retrievePartonEvent( HepMC::GenEvent *theEvent);
  
  /// This method is not implemented.
  virtual void printRunningConditions();

  /// This methid is not implemented.
  virtual bool isSpecialParticle(const LHCb::ParticleProperty *thePP) const;

  /// This methid is not implemented.
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

  /// Convert a number to a string.
  std::string numToString(double num);

  /**
   * Replace special characters in the command string.
   *
   * Decay channels within Herwig++ must be semicolon-delimited (page 117 of
   * the Herwig++ manual). Unfortunately, the strings imported from the options
   * file must also be semicolon-delimited and so consequently individual decay
   * channels cannot be set! To work around this issue we use the special
   * character "\s" to denote a semicolon in the options file, and replace
   * this with an actual ";" in the following code. Additionally, tab
   * characters are correctly converted to spaces.
   */
  std::string replaceSpecialCharacters(std::string str);
  
private:

  bool m_printEvent;               ///< Flag to print the event to screen.

  CommandVector m_defaultSettings; ///< Herwig++ default settings.
  CommandVector m_minbiasSettings; ///< Herwig++ minbias settings.
  CommandVector m_userSettings;    ///< Herwig++ user settings.
  
  std::string m_beamToolName;      ///< Name of beam tool.
  std::string m_writeEventOutput;  ///< File name to write output.
  HepMC::IO_GenEvent *m_hepmcOut;  ///< HepMC output object.
  std::string m_readEventInput;    ///< File name of HepMC event to read.
  
  int m_id1 , m_id2;               ///< PDG ID's for the beams. 
  double m_cme;                    ///< Center of mass energy.
  
  IBeamTool *m_beamTool;           ///< Beam tool.
  
  HerwigppGenerator *m_herwigpp;   ///< Herwig++ engine.
  ThePEG::EventPtr m_event;        ///< Generated event.

  int m_nEvents;                   ///< Number of events to generate.
  
  Rndm::Numbers m_random;          ///< Random number generator.
  double m_seed;                   ///< Random number generator seed.
};

#endif // LBHERWIGPP_HERWIGPPPRODUCTION_H
