// $Id: ProcessPhase.h,v 1.2 2004-06-11 13:12:29 cattanem Exp $
#ifndef KERNEL_PROCESSPHASE_H
#define KERNEL_PROCESSPHASE_H

// Include files
#include "GaudiAlg/GaudiSequencer.h"

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


class ProcessPhase : public GaudiSequencer {
public:
	ProcessPhase( const std::string& name, ISvcLocator *svcloc ); ///> Constructor
	virtual ~ProcessPhase();                                      ///> Destructor

	StatusCode initialize(); ///> Create and initialise sequences of this phase

private:
  VectorName m_detList;    ///> List of subdetectors to be processed
};

#endif    // KERNEL_PROCESSPHASE_H
