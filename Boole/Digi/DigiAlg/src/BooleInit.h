// $Id: BooleInit.h,v 1.1 2003-10-06 16:16:39 cattanem Exp $
#ifndef DIGIALG_BOOLEINIT_H
#define DIGIALG_BOOLEINIT_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IRndmEngine;

/** @class BooleInit BooleInit.h
 * Generic Boole initialisation, independent of phase
 *
 * @author  Marco Cattaneo
 * @date    30th September 2003
 *
 * Based on similar BrunelInitialisation class
 */

class BooleInit : public Algorithm {
public:
	BooleInit( const std::string& name, ISvcLocator *svcloc ); ///< Constructor
	virtual ~BooleInit();                                      ///< Destructor

	// Implement IAlgorithm
	virtual StatusCode BooleInit::initialize();
	virtual StatusCode BooleInit::execute();
	virtual StatusCode BooleInit::finalize();

  StatusCode BooleInit::createL1Buffer();  ///< Creates an empty L1 buffer
  StatusCode BooleInit::createHLTBuffer(); ///< Creates an empty HLT buffer

private:
  IRndmEngine* m_engine; ///< Interface to random number engine
  bool m_initRndm;       ///< If true, initialise random engine with m_theSeed
  long m_theSeed;        ///< Random number seed. Default is run number
  bool m_firstEvent;     ///< For initialisations using the run number
};

#endif    // DIGIALG_BOOLEINIT_H
