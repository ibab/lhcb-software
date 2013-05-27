// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel
#include "Kernel/VPChannelID.h"
// Event
#include "Event/VPDigit.h"
// Local
#include "VPSpilloverSubtraction.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

//-------------------------------------------------------------
// Implementation file for class : VPSpilloverSubtraction
//
// 19/10/2009 : Marcin Kucharczyk
//-------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPSpilloverSubtraction);

//=============================================================================
// Constructor
//=============================================================================
VPSpilloverSubtraction::VPSpilloverSubtraction(
                             const std::string& name,
                             ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/")("/PrevPrev/");
  declareProperty("SpillVector", m_spillNames = tmp);   
  declareProperty("InputLocation", m_inputLocation =  
                  "VP/Digits");
  declareProperty("SwitchOn", m_switchOn = true);
}

//=============================================================================
// Destructor
//=============================================================================
VPSpilloverSubtraction::~VPSpilloverSubtraction() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPSpilloverSubtraction::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPSpilloverSubtraction::execute()
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
    VPDigits* digitContCentr = get<VPDigits>(m_spillPaths[0]);
    VPDigits* digitContPrev = get<VPDigits>(m_spillPaths[1]);
    VPDigits* digitContPrevPrev = get<VPDigits>(m_spillPaths[2]);
    // Identify spill affected digits
    std::vector<LHCb::VPDigit*> selDigits;
    mvDigits(digitContCentr,digitContPrev,digitContPrevPrev,selDigits);
    // Remove selected digits from input - central
    std::vector<LHCb::VPDigit*>::reverse_iterator iD = selDigits.rbegin();
    for(; iD != selDigits.rend(); ++iD) {
      digitContCentr->erase(*iD);
    }
  }

  return StatusCode::SUCCESS;
}

//============================================================================
// Remove selected digits
//============================================================================ 
void VPSpilloverSubtraction::mvDigits(VPDigits* digitContCentr,
                                  VPDigits* digitContPrev,
                                  VPDigits* digitContPrevPrev,
                                  std::vector<LHCb::VPDigit*>& selDigits)
{
  for(LHCb::VPDigits::iterator ic = digitContCentr->begin(); 
      ic != digitContCentr->end(); ic++) {
    LHCb::VPDigit* dCentr = *ic;
    bool rmPrev = false;
    for(LHCb::VPDigits::iterator ip = digitContPrev->begin(); 
        ip != digitContPrev->end(); ip++) {
      LHCb::VPDigit* dPrev = *ip;
      if(dCentr->channelID() == dPrev->channelID()) {
        rmPrev = true;
        break;
      }
    }
    if(rmPrev) {
      selDigits.push_back(dCentr);
    } else {
      bool rmPrevPrev = false;
      for(LHCb::VPDigits::iterator ipp = digitContPrevPrev->begin(); 
          ipp != digitContPrevPrev->end(); ipp++) {
        LHCb::VPDigit* dPrevPrev = *ipp;
        if(dCentr->channelID() == dPrevPrev->channelID()) {
          rmPrevPrev = true;
          break;
	}
      }
      if(rmPrevPrev) selDigits.push_back(dCentr);
    }
  }
}
