// $Id: ProcessPhase.h,v 1.1 2003-05-30 12:01:58 cattanem Exp $
#ifndef KERNEL_PROCESSPHASE_H
#define KERNEL_PROCESSPHASE_H

// Include files
#include "GaudiKernel/Algorithm.h"

// Typedefs
typedef std::vector<std::string> VectorName;

/** @class ProcessPhase ProcessPhase.h LHCbKernel/ProcessPhase.h
 *
 * Processing phase of LHCb application
 *
 * Creates and invokes subdetector processing algorithms
 * Convention: algorithm name = <phase name> + <detector name> 
 *
 * @author: Marco Cattaneo
 * @date:   17th December 1999
 */


class ProcessPhase : public Algorithm {
public:
	ProcessPhase( const std::string& name, ISvcLocator *svcloc ); ///> Constructor
	virtual ~ProcessPhase();                                      ///> Destructor

	// Implement IAlgorithm
	StatusCode initialize(); ///> Create and initialise sequences of this phase
	StatusCode execute();    ///> Execute the sequences of this phase
	StatusCode finalize();   ///> Finalise the sequences of this phase

private:
  VectorName m_detList;    ///> List of subdetectors to be processed
};

#endif    // KERNEL_PROCESSPHASE_H
