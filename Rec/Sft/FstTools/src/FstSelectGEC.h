/**
 * Defines an algorithm that performs multiplicity cuts.
 * Based on LLTCaloAlg by Patrick Robbe.
 *
 * @author: Kevin Dungs
 * @date:   2014-01-10
 */

#ifndef __FSTSELECTGEC_H__
#define __FSTSELECTGEC_H__

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/ITrackSelector.h"


/** @class FstSelectGEC FstSelectGEC.h
 * An algorithm that performs multiplicity cuts.
 *
 * @author Kevin Dungs
 * @date   2014-01-10
 */
class FstSelectGEC : public GaudiAlgorithm {
    public:
        FstSelectGEC(const std::string&, ISvcLocator*);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

    private:
        bool m_doNothing;  // flag that turns off the algorithm
};


#endif /* __FSTSELECTGEC_H__ */
