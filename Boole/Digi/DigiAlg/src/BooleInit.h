// $Id: BooleInit.h,v 1.3 2003-11-03 16:54:27 cattanem Exp $
#ifndef DIGIALG_BOOLEINIT_H
#define DIGIALG_BOOLEINIT_H 1

// Include files
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IRndmEngine;
class IHistogram1D;

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
  StatusCode BooleInit::createHltBuffer(); ///< Creates an empty Hlt buffer

private:
  // Job options
  bool m_initRndm;       ///< If true, initialise random engine with m_theSeed
                         ///< Default is false, use run+event number hash
  long m_theSeed;        ///< Random number seed if m_initRndm is true
  bool m_doHistos;       ///< If true, fill monitoring histograms

  // Member data
  IRndmEngine*  m_engine;       ///< Pointer to random number engine
  unsigned int  m_eventCounter; ///< Number of events processed
  IHistogram1D* m_hMemMapped;   ///< Pointer to mapped memory usage histogram
  IHistogram1D* m_hMemVirtual;  ///< Pointer to virtual memory usage histogram
  
};

#endif    // DIGIALG_BOOLEINIT_H
