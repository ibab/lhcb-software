/**
 * Implementation of an algorithm that performs multiplicity cuts.
 * At this point, however, it only prints 'Hello, world!'
 *
 * @author: Kevin Dungs
 * @date:   2014-01-10
 */

#include "GaudiKernel/AlgFactory.h"

#include "FstSelectGEC.h"


DECLARE_ALGORITHM_FACTORY(FstSelectGEC)

FstSelectGEC::FstSelectGEC(const std::string &name, ISvcLocator *loc)
    : GaudiAlgorithm(name, loc) {
    declareProperty("DoNothing", m_doNothing = false);
}

StatusCode FstSelectGEC::initialize() {
    StatusCode sc = GaudiAlgorithm::initialize();
    if (sc.isFailure()) {
        return sc;
    }

    if (m_doNothing) {
        info() << "This algorithm is turned off via the DoNothing property."
               << endmsg;
        return StatusCode::SUCCESS;
    }

    return StatusCode::SUCCESS;
}

StatusCode FstSelectGEC::execute() {
    if (m_doNothing) {
        info() << "This algorithm is turned off via the DoNothing property."
               << endmsg;
        setFilterPassed(true);
        return StatusCode::SUCCESS;
    }

    always() << "Hello, world!" << endmsg;
    setFilterPassed(true);
    return StatusCode::SUCCESS;
}

StatusCode FstSelectGEC::finalize() {
    if (m_doNothing) {
        info() << "This algorithm is turned off via the DoNothing property."
               << endmsg;
        return GaudiAlgorithm::finalize();
    }

    return GaudiAlgorithm::finalize();
}
