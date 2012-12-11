// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
// LHCbKernel includes
#include "Kernel/STChannelID.h"
#include "Kernel/STHistoAlgBase.h"
// Event
#include "Event/STDigit.h"
// Local
#include "STSpilloverSubtrMonitor.h"
// Boost
#include "boost/assign/list_of.hpp"
#include <boost/foreach.hpp>

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY(STSpilloverSubtrMonitor);

STSpilloverSubtrMonitor::STSpilloverSubtrMonitor(const std::string& name,
                                     ISvcLocator* pSvcLocator):
  ST::HistoAlgBase(name, pSvcLocator)
{
  std::vector<std::string> tmp = boost::assign::list_of("/")("/Prev/");
  declareProperty("SpillVector", m_spillNames = tmp);   
  declareSTConfigProperty("InputLocation", m_inputLocation, 
                                           STDigitLocation::TTDigits);
}

STSpilloverSubtrMonitor::~STSpilloverSubtrMonitor() {}

StatusCode STSpilloverSubtrMonitor::initialize()
{
  StatusCode sc = ST::HistoAlgBase::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  // Set the top directory to IT or TT.
  if( "" == histoTopDir() ) setHistoTopDir(detType()+"/");

  m_spillPaths.clear();
  // Load spill paths
  std::vector<std::string>::const_iterator iSpillName = m_spillNames.begin();
  while (iSpillName != m_spillNames.end()) {
    m_spillPaths.push_back("/Event"+(*iSpillName)+m_inputLocation);
    ++iSpillName;
  }

  return StatusCode::SUCCESS;
}

StatusCode STSpilloverSubtrMonitor::execute()
{
    // Get input containers
    if( !exist<STDigits>(m_spillPaths[0]) ){
      return Error(" No STDigits for current sampling time ",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContCentr = get<STDigits>(m_spillPaths[0]);
    if( !exist<STDigits>(m_spillPaths[1]) ){
      return Error(" No STDigits for previous sampling time ",StatusCode::SUCCESS,100);
    }    
    STDigits* digitContPrev  = get<STDigits>(m_spillPaths[1]);

    for(LHCb::STDigits::iterator ic = digitContCentr->begin(); 
        ic != digitContCentr->end(); ic++) {
      LHCb::STDigit* dCentr = *ic;
      double q = dCentr->depositedCharge();
      LHCb::STDigit* dPrev = digitContPrev->object(dCentr->key());
      double q_prev =0.0;      
      if( dPrev ){
        q_prev = dPrev->depositedCharge();
      }
      plot2D( q, q_prev, 1, " q_prev vs q ", -20., 130., -20., 130.,150,150);      
    }
    
  return StatusCode::SUCCESS;
}

