// $Id: FindAlignmentElement.cpp,v 1.5 2008-01-07 11:01:16 janos Exp $
// Include files
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from DetDesc
#include "DetDesc/Services.h"

// from detector interfaces
#include "VeloDet/DeVelo.h"
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
  DeVelo*       velo = getDetector<DeVelo>(DeVeloLocation::Default);            ///< Pointer to Velo Detector
  DeTTDetector* tt   = getDetector<DeTTDetector>(DeSTDetLocation::TT);           ///< Pointer to TT Detector
  DeITDetector* it   = getDetector<DeITDetector>(DeSTDetLocation::IT);           ///< Pointer to IT Detector
  DeOTDetector* ot   = getDetector<DeOTDetector>(DeOTDetectorLocation::Default); ///< Pointer to OT Detector

  /// The following methods are overloaded. So we need a fuction pointer to the function we want, i.e.
  /// function pointers to the methods in the detector interfaces that return a detector element for
  /// a given detector id.
  /// Tracking stations
  typedef boost::function<const DetectorElement* (const LHCbID&)> FindTrackingDet;
  FindTrackingDet findTrackingStation = &Alignment::findTrackingStation;

  /// Velo
  typedef const DeVeloSensor* (DeVelo::*FindVeloSensor) (const VeloChannelID) const;
  FindVeloSensor findVeloSensor = &DeVelo::sensor;

  typedef boost::function<const DetectorElement* (const DeVeloSensor*)> VeloParentOfSensor;
  VeloParentOfSensor findVeloBox    = &Alignment::findVeloBox;
  VeloParentOfSensor findVeloModule = &Alignment::findVeloModule;

  /// TT & IT
  typedef DeSTStation* (DeSTDetector::*FindSTStation) (const STChannelID);
  FindSTStation findSTStation  = &DeSTDetector::findStation;
  typedef DeSTLayer*   (DeSTDetector::*FindSTLayer) (const STChannelID);
  FindSTLayer findSTLayer      = &DeSTDetector::findLayer;
  typedef DeSTSector* (DeSTDetector::*FindSTSector) (const STChannelID);
  FindSTSector findSTSector    = &DeSTDetector::findSector;

  /// TT specific
  typedef boost::function<const DetectorElement* (const DeSTSector*)> STParentOfSector;
  STParentOfSector findTTHalfModule = &Alignment::findHalfModule;

  /// IT specific
  typedef DeITBox* (DeITDetector::*FindITBox) (const STChannelID);
  FindITBox findITBox = &DeITDetector::findBox;
  STParentOfSector findITLadder = &Alignment::findLadder;

  /// OT
  typedef const DeOTStation* (DeOTDetector::*FindOTStation) (const OTChannelID&) const;
  FindOTStation findOTStation = &DeOTDetector::findStation;
  typedef const DeOTLayer*   (DeOTDetector::*FindOTLayer) (const OTChannelID&) const;
  FindOTLayer findOTLayer     = &DeOTDetector::findLayer;
  typedef const DeOTModule* (DeOTDetector::*FindOTModule) (const OTChannelID&) const;
  FindOTModule findOTModule   = &DeOTDetector::findModule;

  /// I can probably template these (?)
  /// Velo find functors
  DetElemFromID veloBox =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isVelo,_1),
                                                    bind<const DetectorElement*>(findVeloBox,
                                                                                 bind<const DeVeloSensor*>(findVeloSensor, velo,
                                                                                                           bind<VeloChannelID>(&LHCbID::veloID,_1))),
                                                    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID veloModule =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isVelo,_1),
                                                    bind<const DetectorElement*>(findVeloModule,
                                                                                 bind<const DeVeloSensor*>(findVeloSensor, velo,
                                                                                                           bind<VeloChannelID>(&LHCbID::veloID,_1))),
                                                    static_cast<const DetectorElement*>(0)));
  
  /// TT find functors
  DetElemFromID ttDetector =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1),
                                                    dynamic_cast<const DetectorElement*>(tt),
                                                    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID ttStation =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1),
                                                    bind<const DetectorElement*>(findSTStation, tt, 
                                                                                 bind<STChannelID>(&LHCbID::stID,_1)),
                                                    static_cast<const DetectorElement*>(0)));

  DetElemFromID ttLayer   =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1),
                                                    bind<const DetectorElement*>(findSTLayer, tt, 
                                                                                 bind<STChannelID>(&LHCbID::stID,_1)),
                                                    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID ttHalfModule =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isTT,_1),
                                                    bind<const DetectorElement*>(findTTHalfModule,
                                                                                 bind<const DeSTSector*>(findSTSector, tt,
                                                                                                         bind<STChannelID>(&LHCbID::stID,_1))),
                                                    static_cast<const DetectorElement*>(0)));

  /// IT find functors
  DetElemFromID itDetector =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
                                                    dynamic_cast<const DetectorElement*>(it),
                                                    static_cast<const DetectorElement*>(0)));
  DetElemFromID itStation =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
                                                    bind<const DetectorElement*>(findSTStation, it, 
                                                                                 bind<STChannelID>(&LHCbID::stID,_1)),
                                                    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID itLayer   =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
                                                    bind<const DetectorElement*>(findSTLayer, it, 
                                                                                 bind<STChannelID>(&LHCbID::stID,_1)),
                                                    static_cast<const DetectorElement*>(0)));
  DetElemFromID itBox     =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
                                                    bind<const DetectorElement*>(findITBox, it, 
                                                                                 bind<STChannelID>(&LHCbID::stID,_1)),
                                                    static_cast<const DetectorElement*>(0)));
  
  DetElemFromID itLadder =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isIT,_1),
                                                    bind<const DetectorElement*>(findITLadder,
                                                                                 bind<const DeSTSector*>(findSTSector, it,
                                                                                                         bind<STChannelID>(&LHCbID::stID,_1))),
                                                    static_cast<const DetectorElement*>(0)));

  /// OT find functors
  DetElemFromID otDetector =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1),
                                                    dynamic_cast<const DetectorElement*>(ot),
                                                    static_cast<const DetectorElement*>(0)));

  DetElemFromID otStation =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1),
                                                    bind<const DetectorElement*>(findOTStation, ot, 
                                                                                 bind<OTChannelID>(&LHCbID::otID,_1)),
                                                    static_cast<const DetectorElement*>(0)));

  DetElemFromID otLayer   =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1),
                                                    bind<const DetectorElement*>(findOTLayer,   ot, 
                                                                                 bind<OTChannelID>(&LHCbID::otID,_1)),
                                                    static_cast<const DetectorElement*>(0)));

  DetElemFromID otModule  =
    ret<const DetectorElement*>(if_then_else_return(bind<bool>(&LHCbID::isOT,_1),
                                                    bind<const DetectorElement*>(findOTModule,  ot, 
                                                                                 bind<OTChannelID>(&LHCbID::otID,_1)),
                                                    static_cast<const DetectorElement*>(0)));

  FunctorMap map = map_list_of("TrackingStations", findTrackingStation)
                              ("VeloBoxes"     , veloBox     )
			                        ("VeloModules"   , veloModule  )
			                        ("TTStations"    , ttStation   )
			                        ("TTLayers"      , ttLayer     )
                              ("TTHalfModules" , ttHalfModule)
			                        ("ITStations"    , itStation   )
			                        ("ITBoxes"       , itBox       )
			                        ("ITLayers"      , itLayer     )
			                        ("ITLadders"     , itLadder    )
			                        ("OTDetector"    , otDetector  )
			                        ("OTStations"    , otStation   )
			                        ("OTLayers"      , otLayer     )
			                        ("OTModules"     , otModule    );

  return map;
}

Alignment::DetElemFromID Alignment::FindElement(const std::string& findWhat) {

  static std::map<std::string, DetElemFromID> s_map = createFunctorMap();

  std::map<std::string, DetElemFromID>::const_iterator i = s_map.find(findWhat);
  Alignment::DetElemFromID functor = (i != s_map.end() ? i->second : 0);
  
  if (functor.empty()) {
    std::cout << "Warning (FindElement): No functor for " + findWhat + "!!!!" << std::endl;
    std::cout << "Warning (FindElement): Available functors are: ";
    std::map<std::string, DetElemFromID>::const_iterator last = --s_map.end();
    for (std::map<std::string, DetElemFromID>::const_iterator i = s_map.begin(), iEnd = s_map.end(); i != iEnd; ++i) {
      std::cout << i->first << (i != last ? ", " : ".");
    }
    std::cout << std::endl;
  }
  
  return functor;
}

const DetectorElement* Alignment::findTrackingStation(const LHCb::LHCbID& anLHCbID) {
  DetectorElement* elem = 0;
  if (anLHCbID.isVelo()) elem = getDetector<DetectorElement>(DeVeloLocation::Default);
  if (anLHCbID.isIT())   elem = getDetector<DetectorElement>(DeSTDetLocation::IT);
  if (anLHCbID.isOT())   elem = getDetector<DetectorElement>(DeOTDetectorLocation::Default);
  return elem;
}

const DetectorElement* Alignment::findVeloBox(const DeVeloSensor* sensor) {
  IDetectorElement* iElem = sensor->parentIDetectorElement();
  ///std::cout << dynamic_cast<const DetectorElement*>(iElem)->name() << std::endl;
  IDetectorElement* mod   = (dynamic_cast<const DetectorElement*>(iElem))->parentIDetectorElement();
  ///std::cout << dynamic_cast<const DetectorElement*>(mod)->name() << std::endl;
  IDetectorElement* box   = (dynamic_cast<const DetectorElement*>(mod))->parentIDetectorElement();
  ///std::cout << dynamic_cast<const DetectorElement*>(box)->name() << std::endl;
  return dynamic_cast<const DetectorElement*>(box);
}

const DetectorElement* Alignment::findVeloModule(const DeVeloSensor* sensor) {
  IDetectorElement* iElem = sensor->parentIDetectorElement();
  ///std::cout << dynamic_cast<const DetectorElement*>(iElem)->name() << std::endl;
  IDetectorElement* mod   = (dynamic_cast<const DetectorElement*>(iElem))->parentIDetectorElement();
  ///std::cout << dynamic_cast<const DetectorElement*>(mod)->name() << std::endl;
  return dynamic_cast<const DetectorElement*>(mod);
}

const DetectorElement* findOTDetector(const DeOTStation* station) {
  IDetectorElement* ot = station->parentIDetectorElement();
  return dynamic_cast<const DetectorElement*>(ot);
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

  const DetectorElement* elem = 0;
  try {
    elem = m_elemFromLHCbID(anLHCbID);
  } catch (boost::bad_function_call) {
    std::cout << "Warning (FindAlignmentElement): Calling empty functor!!!! See previous warning from FindElement." << std::endl;
    throw std::exception();
  }
  
  if (!elem ) return static_cast<const AlignmentElement*>(0);
  AlignElemIter AlignElem = find_if(m_rangeElements.first, m_rangeElements.second,
                                    boost::lambda::bind(&AlignmentElement::operator==,
                                                        boost::lambda::_1,
                                                        elem));
  return (AlignElem != m_rangeElements.second) ? &(*AlignElem) : static_cast<const AlignmentElement*>(0);
}
