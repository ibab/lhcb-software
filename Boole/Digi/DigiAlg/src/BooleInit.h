// $Id: BooleInit.h,v 1.5 2004-06-23 12:39:12 cattanem Exp $
#ifndef DIGIALG_BOOLEINIT_H
#define DIGIALG_BOOLEINIT_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

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

class BooleInit : public GaudiAlgorithm {
public:
	BooleInit( const std::string& name, ISvcLocator *svcloc ); ///< Constructor
	virtual ~BooleInit();                                      ///< Destructor

	// Implement IAlgorithm
	virtual StatusCode BooleInit::initialize(); ///< Initialization
	virtual StatusCode BooleInit::execute();    ///< Event loop
	virtual StatusCode BooleInit::finalize();   ///< Finalization

  StatusCode BooleInit::createL1Buffer();  ///< Creates an empty L1 buffer
  StatusCode BooleInit::createRawBuffer(); ///< Creates an empty Raw buffer

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
