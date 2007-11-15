// $Id: FindAlignmentElement.cpp,v 1.2 2007-11-15 15:05:28 jblouw Exp $
// Include files
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from DetDesc
#include "DetDesc/Services.h"

// from detector interfaces
#include "STDet/DeTTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeITBox.h"
#include "OTDet/DeOTDetector.h"

// from BOOST
#include "boost/assign/list_of.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/if.hpp"

// local
#include "FindAlignmentElement.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FindAlignmentElement
//
// 2007-10-29 : Jan Amoraal
//-----------------------------------------------------------------------------

template <typename SUBDETECTOR> 
SUBDETECTOR* getDetector(const std::string& location) {
  
  /// Get a pointer to Detector data service instance
  DetDesc::Services* dds    = DetDesc::Services::services();
  IDataProviderSvc*  detSvc = dds->detSvc(); ///< Throws exception and gives a warning if not available
  
  SmartDataPtr<SUBDETECTOR> detSmartDataPtr(detSvc, location);
  
  SUBDETECTOR* det = detSmartDataPtr;
  
  /// @todo: Need to throw some kind of exception if det points to NULL
  return det;  
}

Alignment::FunctorMap createFunctorMap() {
  
  using namespace LHCb;
  using namespace Alignment;
  using namespace boost;
  using namespace boost::lambda;
  using namespace boost::assign;

  /// These are needed to get a detector element from an LHCBb id.
  /// Get pointers to detector elements
  DeTTDetector* tt = getDetector<DeTTDetector>(DeSTDetLocation::TT);           ///< Pointer to TT Detector
  DeITDetector* it = getDetector<DeITDetector>(DeSTDetLocation::IT);           ///< Pointer to IT Detector
  DeOTDetector* ot = getDetector<DeOTDetector>(DeOTDetectorLocation::Default); ///< Pointer to OT Detector
  
  /// The following methods are overloaded. So we need a fuction pointer to the function we want, i.e.
  /// function pointers to the methods in the detector interfaces that return a detector element for 
  /// a given detector id.
  /// TT & IT
  typedef DeSTStation* (DeSTDetector::*FindSTStation) (const STChannelID);
  FindSTStation findSTStation  = &DeSTDetector::findStation;
  typedef DeSTLayer*   (DeSTDetector::*FindSTLayer) (const STChannelID);
  FindSTLayer findSTLayer      = &DeSTDetector::findLayer;
  typedef DeSTSector* (DeSTDetector::*FindSTSector) (const STChannelID);
  FindSTSector findSTSector    = &DeSTDetector::findSector;

  /// TT specific
  typedef boost::function<const DetectorElement* (const DeSTSector* sensor)> STParentOfSector;
  STParentOfSector findTTHalfModule = &Alignment::findHalfModule;

  /// IT specific
  typedef DeITBox* (DeITDetector::*FindITBox) (const STChannelID);
//  FindITBox findITBox = &DeITDetector::findBox;
  STParentOfSector findITLadder = &Alignment::findLadder;

  /// OT
  typedef const DeOTStation* (DeOTDetector::*FindOTStation) (const OTChannelID&) const;
  FindOTStation findOTStation = &DeOTDetector::findStation;
  typedef const DeOTLayer*   (DeOTDetector::*FindOTLayer) (const OTChannelID&) const;
  FindOTLayer findOTLayer     = &DeOTDetector::findLayer;
  typedef const DeOTModule* (DeOTDetector::*FindOTModule) (const OTChannelID&) const;
  FindOTModule findOTModule   = &DeOTDetector::findModule;
 
  /// I can probably template these (?)
  /// TT find functors
  DetElemFromID ttStation =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1), 
						    bind<const DetectorElement*>(findSTStation, tt, bind<STChannelID>(&LHCbID::stID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID ttLayer   =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1), 
						    bind<const DetectorElement*>(findSTLayer,   tt, bind<STChannelID>(&LHCbID::stID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID ttHalfModule =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1),
						    bind<const DetectorElement*>(findTTHalfModule, 
										 bind<const DeSTSector*>(findSTSector, tt, 
													 bind<STChannelID>(&LHCbID::stID,_1))),
						    static_cast<const DetectorElement*>(0)));
  
  /// IT find functors
  DetElemFromID itStation =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1), 
						    bind<const DetectorElement*>(findSTStation, it, bind<STChannelID>(&LHCbID::stID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID itLayer   =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1), 
						    bind<const DetectorElement*>(findSTLayer,   it, bind<STChannelID>(&LHCbID::stID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
//  DetElemFromID itBox     =  
//    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1), 
//						    bind<const DetectorElement*>(findITBox,     it, bind<STChannelID>(&LHCbID::stID,_1)),
//						    static_cast<const DetectorElement*>(0)));

  DetElemFromID itLadder =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
						    bind<const DetectorElement*>(findITLadder, 
										 bind<const DeSTSector*>(findSTSector, it, 
													 bind<STChannelID>(&LHCbID::stID,_1))),
						    static_cast<const DetectorElement*>(0)));

  /// OT find functors
  DetElemFromID otStation =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1),
						    bind<const DetectorElement*>(findOTStation, ot, bind<OTChannelID>(&LHCbID::otID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID otLayer   =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1), 
						    bind<const DetectorElement*>(findOTLayer,   ot, bind<OTChannelID>(&LHCbID::otID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID otModule  =  
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1), 
						    bind<const DetectorElement*>(findOTModule,  ot, bind<OTChannelID>(&LHCbID::otID,_1)),
						    static_cast<const DetectorElement*>(0)));
  
  FunctorMap map = map_list_of("TTStations"    , ttStation   )
                              ("TTLayers"      , ttLayer     )
                              ("TTHalfModules" , ttHalfModule) 
                              ("ITStations"    , itStation   )
//                              ("ITBoxes"       , itBox       )
                              ("ITLayers"      , itLayer     )
                              ("ITLadders"     , itLadder    )
                              ("OTStations"    , otStation   )
                              ("OTLayers"      , otLayer     )
                              ("OTModules"     , otModule    );
  

  
  return map;
}

Alignment::DetElemFromID Alignment::FindElement(const std::string& findWhat) {
  
  static std::map<std::string, DetElemFromID> s_map = createFunctorMap();
  
  std::map<std::string, DetElemFromID>::const_iterator i = s_map.find(findWhat);
  
  Alignment::DetElemFromID emptyFunctor = 0;
  return ( i != s_map.end() ? i->second : emptyFunctor);
  ///@todo throw exception if functor is empty, i.e. when the user is trying to align something we don't know.
}

const DetectorElement* Alignment::findHalfModule(const DeSTSector* sensor) {
  IDetectorElement* module = sensor->parentIDetectorElement();
  return dynamic_cast<const DetectorElement*>(module);
}

const DetectorElement* Alignment::findLadder(const DeSTSector* sensor) {
  IDetectorElement* ladder = sensor->parentIDetectorElement();
  return dynamic_cast<const DetectorElement*>(ladder);
}

FindAlignmentElement::FindAlignmentElement(){}
 
FindAlignmentElement::FindAlignmentElement(Alignment::DetElemFromID elemFromLHCbID, 
					   Range rangeElements): m_elemFromLHCbID(elemFromLHCbID),
								 m_rangeElements(rangeElements){}

    

const AlignmentElement* FindAlignmentElement::operator()(const LHCb::LHCbID& anLHCbID) {
  const DetectorElement* elem = m_elemFromLHCbID(anLHCbID);
  if (!elem ) return static_cast<const AlignmentElement*>(0);
  AlignElemIter AlignElem = find_if(m_rangeElements.first, m_rangeElements.second, 
				    boost::lambda::bind(&AlignmentElement::operator==, 
							boost::lambda::_1, 
							elem)
				    );
  return (AlignElem != m_rangeElements.second) ? &(*AlignElem) : static_cast<const AlignmentElement*>(0);
}
