#ifndef GAUSS_GAUSSINITIALISATION_H
#define GAUSS_GAUSSINITIALISATION_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IRndmEngine;

/** @class GaussInitialisation GaussInitialisation.h
 * Generic Gauss initialisation, independent of phase
 *
 * @author  Witold Pokorski
 * @date    16th December 2002
 */

class GaussInitialisation : public Algorithm {
public:
	// Construtor
	GaussInitialisation( const std::string& name, ISvcLocator *svcloc );
	// Destructor
	virtual ~GaussInitialisation();

	// Extend the default behaviour
	virtual StatusCode GaussInitialisation::initialize();

	virtual StatusCode GaussInitialisation::execute();

	virtual StatusCode GaussInitialisation::finalize();
private:
  IRndmEngine* m_engine; ///> Interface to random number engine
  bool m_initRndm;       ///> If true, initialise random engine with m_theSeed
                         ///> at the begining of the job, if false initialize
                         ///> random engine at the begining of each event
                         ///> with m_theSeed = 100000 * (m_eventNumb % 20000) 
                         ///>                         + (m_runNumb % 100000)
  long m_theSeed;        ///> Random number seed. Default is run number
  long m_eventNumb;      ///> Counter for events
  long m_runNumb;        ///> Run number 
  long m_firstEvent;     ///> First event number
};

#endif    // GAUSS_GAUSSITIALISATION_H
