// $Id: LbAppInit.h,v 1.1 2004-06-28 11:35:06 cattanem Exp $
#ifndef LHCBALGS_LBAPPINIT_H
#define LHCBALGS_LPAPPINIT_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class IRndmEngine;
class IHistogram1D;

/** @class LbAppInit LbAppInit.h
 * Generic initialisation for LHCb data processing applications
 *
 * @author  Marco Cattaneo
 * @date    24th June 2004
 *
 * Based on similar BrunelInitialisation, BooleInit, GaussInitialisation classes
 */

class LbAppInit : public GaudiAlgorithm {
public:
	LbAppInit( const std::string& name, ISvcLocator *svcloc ); ///< Constructor
	virtual ~LbAppInit();                                      ///< Destructor

	// Implement IAlgorithm
	virtual StatusCode LbAppInit::initialize(); ///< Initialization
	virtual StatusCode LbAppInit::execute();    ///< Event loop
	virtual StatusCode LbAppInit::finalize();   ///< Finalization

  StatusCode LbAppInit::createProcStatus(); ///< Creates an empty ProcStatus

  /** Sets random number seed (hashed string made from name(), evt, run)
   *  @param evt first  number for hash string, usually event number
   *  @param run second number for hash string, usually run   number
   *  @return    the seed used
   */
  long LbAppInit::initRndmNum( unsigned long evt, unsigned long run); 

private:
  /// Property to set the first event number, if event header is missing
  unsigned int m_firstEvent;

  /// Property to set the run number, if event header is missing
  unsigned int m_runNumber;

  /// Property to enable creation and filling of memory monitoring histogram
  bool m_doHistos;

  // Member data
  IRndmEngine*  m_engine;       ///< Pointer to random number engine
  unsigned int  m_eventCounter; ///< Number of events processed
  IHistogram1D* m_hMemVirtual;  ///< Pointer to virtual memory usage histogram
};

#endif    // LHCBALGS_LBAPPINIT_H
