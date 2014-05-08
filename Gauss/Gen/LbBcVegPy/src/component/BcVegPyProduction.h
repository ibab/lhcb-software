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
class BcVegPyProduction : public PythiaProduction {
 public:

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

  CommandVector m_defaultBcVegPySettings;
  CommandVector m_commandBcVegPyVector;

  bool m_printEvent;               ///< Flag to print the event to screen.
  std::string m_writeEventOutput;  ///< File name to write output.
  HepMC::IO_GenEvent *m_hepmcOut;  ///< HepMC output object.
  std::string m_readEventInput;    ///< File name of HepMC event to read.
};

#endif // LBBCVEGPY_BCVEGPYPRODUCTION_H
