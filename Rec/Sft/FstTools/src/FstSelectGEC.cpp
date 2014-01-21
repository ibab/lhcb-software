/**
 * Implementation of an algorithm that performs multiplicity cuts.
 * It requires Patrick Robbe's LLTCaloAlg to have run beforehands.
 *
 * @author: Kevin Dungs
 * @date:   2014-01-19
 */

#include "Event/L0CaloCandidate.h"
#include "GaudiKernel/AlgFactory.h"

#include "FstSelectGEC.h"


DECLARE_ALGORITHM_FACTORY(FstSelectGEC)

FstSelectGEC::FstSelectGEC(const std::string &name, ISvcLocator *loc)
: GaudiHistoAlg(name, loc) {
    declareProperty("MultiplicityCutECAL", m_multiplicityCutECAL=1000);
    declareProperty("MultiplicityCutHCAL", m_multiplicityCutHCAL=1000);
    declareProperty("DoNothing", m_doNothing=false);
}

StatusCode FstSelectGEC::initialize() {
    StatusCode sc = GaudiHistoAlg::initialize();
    if (sc.isFailure()) {
        return sc;
    }

    m_nCaloMult=book(7 , "ECal and HCal multiplicity", 0.,12000.,400);

    if (m_doNothing) {
        info() << "This algorithm is turned off via the DoNothing property."
               << endmsg;
        return StatusCode::SUCCESS;
    }

    info() << "MultiplicityCutECAL = " << m_multiplicityCutECAL << endmsg
           << "MultiplicityCutHCAL = " << m_multiplicityCutHCAL << endmsg;

    return StatusCode::SUCCESS;
}

StatusCode FstSelectGEC::execute() {
    if (m_doNothing) {
        setFilterPassed(true);
        return StatusCode::SUCCESS;
    }

    int multECAL = 0,
        multHCAL = 0;

    LHCb::L0CaloCandidates *cs = get<LHCb::L0CaloCandidates>("LLT/Calo");
    for (
        LHCb::L0CaloCandidates::iterator it = cs->begin();
        it != cs->end();
        ++it
    ) {
        LHCb::L0CaloCandidate *candidate = *it;
        if (candidate->type() == 6) {  // We have a multiplicity cut.
            // Personally, I'm not so very sure if using a double for such a
            // switch is a good idea.
            if (candidate->et() == 0.) {  // ECAL
                multECAL = candidate->etCode();
            } else if (candidate->et() == 1.) {  // HCAL
                multHCAL = candidate->etCode();
            } else { // This should not happen.
                fatal() << "Multiplicity is neither ECAL nor HCAL!" << endmsg;
            }
        }
    }

    m_nCaloMult->fill(multECAL+multHCAL);
    setFilterPassed(
  	    (multECAL+multHCAL) < (m_multiplicityCutECAL+m_multiplicityCutHCAL)   //     multHCAL < m_multiplicityCutHCAL
    );

    return StatusCode::SUCCESS;
}

StatusCode FstSelectGEC::finalize() {
    if (m_doNothing) {
        return GaudiHistoAlg::finalize();
    }

    return GaudiHistoAlg::finalize();
}
