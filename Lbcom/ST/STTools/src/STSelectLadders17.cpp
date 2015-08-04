// Event
#include "Kernel/STChannelID.h"

#include "STSelectLadders17.h"
#include "STDet/DeSTDetector.h"

//boost
#include <boost/foreach.hpp>


DECLARE_TOOL_FACTORY( STSelectLadders17)
 
STSelectLadders17::STSelectLadders17( const std::string& type, 
                                    const std::string& name,
                                    const IInterface* parent ) :
  ST::ToolBase(type, name, parent)
{

  //declareProperty("elementNames", m_elementNames);
  setForcedInit();
  declareInterface<ISTChannelIDSelector>(this);
}

/*
StatusCode STSelectLadders17::initialize() {

  StatusCode sc = ST::ToolBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  BOOST_FOREACH(std::string name, m_elementNames) {
    DeSTBaseElement* detElement = tracker()->findTopLevelElement(name);
    if (detElement == 0){
      return Error("Failed to find detector element",StatusCode::FAILURE); 
    }
    else {
      info () << "Adding " <<  name << " to element list" << endmsg;     
      m_detElements.push_back(detElement);
    }
  } // for each

  return StatusCode::SUCCESS;
}
*/
STSelectLadders17::~STSelectLadders17()
{
  //destructer
}

bool STSelectLadders17::select( const LHCb::STChannelID& id ) const{
  return (*this) (id);  
}
  
bool STSelectLadders17::operator()( const LHCb::STChannelID& id ) const{

  unsigned int sector = id.sector();
  return (sector == 1 || sector == 7 ? true:false);

  /*
  std::vector<DeSTBaseElement*>::const_iterator iterElem = m_detElements.begin();
  for ( ; iterElem != m_detElements.end() ; ++iterElem){
    if ((*iterElem)->contains(id) == true) break;
  } 
  return (iterElem != m_detElements.end() ? true : false);
*/
}
