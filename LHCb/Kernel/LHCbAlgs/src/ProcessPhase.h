// $Id: ProcessPhase.h,v 1.1.1.1 2004-06-14 12:40:38 cattanem Exp $
#ifndef PROCESSPHASE_H
#define PROCESSPHASE_H

// Include files
#include "GaudiAlg/GaudiSequencer.h"

// Typedefs
typedef std::vector<std::string> VectorName;

/** @class ProcessPhase ProcessPhase.h LHCbAlgs/ProcessPhase.h
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

#endif    // PROCESSPHASE_H
