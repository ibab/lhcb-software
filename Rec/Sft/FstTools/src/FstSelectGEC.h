/**
 * Defines an algorithm that performs multiplicity cuts.
 * It requires Patrick Robbe's LLTCaloAlg to have run beforehands.
 *
 * @author: Kevin Dungs
 * @date:   2014-01-19
 */

#ifndef __FSTSELECTGEC_H__
#define __FSTSELECTGEC_H__

#include <string>
#include "GaudiAlg/GaudiHistoAlg.h"  //MB                                                                                                                                                                                      


// from AIDA Histogramming  //MB                                                                                                                                                                                               
#include "AIDA/IHistogram1D.h"  //MB                                                                                                                                                                                           
#include "AIDA/IHistogram2D.h"  //MB   
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "TrackInterfaces/ITrackSelector.h"


/** @class FstSelectGEC FstSelectGEC.h
 * An algorithm that performs multiplicity cuts.
 * It depends on LLTCaloAlg having run before.
 *
 * @author Kevin Dungs
 * @date   2014-01-17
 */
class FstSelectGEC : public GaudiHistoAlg {
    public:
        FstSelectGEC(const std::string&, ISvcLocator*);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

    private:
        int m_multiplicityCutECAL,  // above these values, events won't pass
            m_multiplicityCutHCAL;
        bool m_doNothing;  // flag that turns off the algorithm
	AIDA::IHistogram1D * m_nCaloMult;
	
};


#endif /* __FSTSELECTGEC_H__ */
