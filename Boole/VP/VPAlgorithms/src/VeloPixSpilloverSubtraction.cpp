// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel
#include "Kernel/VeloPixChannelID.h"
// Event
#include "Event/VeloPixDigit.h"
// Local
#include "VeloPixSpilloverSubtraction.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

//-------------------------------------------------------------
// Implementation file for class : VeloPixSpilloverSubtraction
//
// 19/10/2009 : Marcin Kucharczyk
//-------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VeloPixSpilloverSubtraction);

//=============================================================================
// Constructor
//=============================================================================
VeloPixSpilloverSubtraction::VeloPixSpilloverSubtraction(
                             const std::string& name,
                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("SpillVector", m_spillNames = 
		              boost::assign::list_of("/")("/Prev/")("/PrevPrev/"));   
  declareProperty("InputLocation", m_inputLocation =  
                  "VP/Digits");
  declareProperty("SwitchOn", m_switchOn = true);
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixSpilloverSubtraction::~VeloPixSpilloverSubtraction() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixSpilloverSubtraction::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixSpilloverSubtraction::execute()
{
  if(m_switchOn) {
    m_spillPaths.clear();
    // Load spill paths
    std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
    while (iSpillName != m_spillNames.end()) {
      m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
      ++iSpillName;
    }
    // Get input containers
    VeloPixDigits* digitContCentr = get<VeloPixDigits>(m_spillPaths[0]);
    VeloPixDigits* digitContPrev = get<VeloPixDigits>(m_spillPaths[1]);
    VeloPixDigits* digitContPrevPrev = get<VeloPixDigits>(m_spillPaths[2]);
    // Identify spill affected digits
    std::vector<LHCb::VeloPixDigit*> selDigits;
    mvDigits(digitContCentr,digitContPrev,digitContPrevPrev,selDigits);
    // Remove selected digits from input - central
    std::vector<LHCb::VeloPixDigit*>::reverse_iterator iD = selDigits.rbegin();
    for(; iD != selDigits.rend(); ++iD) {
      digitContCentr->erase(*iD);
    }
  }

  return StatusCode::SUCCESS;
}

//============================================================================
// Remove selected digits
//============================================================================ 
void VeloPixSpilloverSubtraction::mvDigits(VeloPixDigits* digitContCentr,
                                  VeloPixDigits* digitContPrev,
                                  VeloPixDigits* digitContPrevPrev,
                                  std::vector<LHCb::VeloPixDigit*>& selDigits)
{
  for(LHCb::VeloPixDigits::iterator ic = digitContCentr->begin(); 
      ic != digitContCentr->end(); ic++) {
    LHCb::VeloPixDigit* dCentr = *ic;
    bool rmPrev = false;
    for(LHCb::VeloPixDigits::iterator ip = digitContPrev->begin(); 
        ip != digitContPrev->end(); ip++) {
      LHCb::VeloPixDigit* dPrev = *ip;
      if(dCentr->channelID() == dPrev->channelID()) {
        rmPrev = true;
        break;
      }
    }
    if(rmPrev) {
      selDigits.push_back(dCentr);
    } else {
      bool rmPrevPrev = false;
      for(LHCb::VeloPixDigits::iterator ipp = digitContPrevPrev->begin(); 
          ipp != digitContPrevPrev->end(); ipp++) {
        LHCb::VeloPixDigit* dPrevPrev = *ipp;
        if(dCentr->channelID() == dPrevPrev->channelID()) {
          rmPrevPrev = true;
          break;
	}
      }
      if(rmPrevPrev) selDigits.push_back(dCentr);
    }
  }
}


//============================================================================
StatusCode VeloPixSpilloverSubtraction::finalize() {

  return GaudiAlgorithm::finalize();

}
