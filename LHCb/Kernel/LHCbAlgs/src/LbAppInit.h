// $Id: LbAppInit.h,v 1.4 2005-06-24 06:45:54 cattanem Exp $
#ifndef LHCBALGS_LBAPPINIT_H
#define LHCBALGS_LPAPPINIT_H 1

// Include files
#include "GaudiAlg/GaudiHistoAlg.h"

// Forward declarations
class IRndmEngine;

/** @class LbAppInit LbAppInit.h
 * Generic initialisation for LHCb data processing applications
 *
 * @author  Marco Cattaneo
 * @date    24th June 2004
 *
 * Based on similar BrunelInitialisation, BooleInit, GaussInitialisation classes
 */

class LbAppInit : public GaudiHistoAlg {
public:
	LbAppInit( const std::string& name, ISvcLocator *svcloc ); ///< Constructor
	virtual ~LbAppInit();                                      ///< Destructor

	// Implement IAlgorithm
	virtual StatusCode LbAppInit::initialize(); ///< Initialization
	virtual StatusCode LbAppInit::execute();    ///< Event loop
	virtual StatusCode LbAppInit::finalize();   ///< Finalization

  StatusCode LbAppInit::createProcStatus(); ///< Creates an empty ProcStatus

  /** Sets random number seeds (hashed string made from name(), evt, run)
   *  @param evt first  number for hash string, usually event number
   *  @param run second number for hash string, usually run   number
   */
  void LbAppInit::initRndmNum( unsigned long evt, unsigned long run); 

private:
  /// Property to set the first event number, if event header is missing
  unsigned int m_firstEvent;

  /// Property to set the run number, if event header is missing
  unsigned int m_runNumber;

  /// Property to skip some random numbers (default is zero)
  int  m_skipFactor;

  /// Property to use only one seed (default is false)
  bool m_singleSeed;
  
  /// Property to preload the detector geometry (default is false)
  bool m_preloadGeom;

  /// Property to define number of bins in memory histo
  unsigned int m_bins;
 
  // Member data
  IRndmEngine*  m_engine;       ///< Pointer to random number engine
  unsigned int  m_eventCounter; ///< Number of events processed
};

#endif    // LHCBALGS_LBAPPINIT_H
