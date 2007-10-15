// $Id: GetElementsToAlign.cpp,v 1.2 2007-10-15 16:12:27 lnicolas Exp $
// Include files 
// from STD
#include <iomanip>
#include <string>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"

// from DetDesc
#include "DetDesc/GeometryInfoPlus.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from TrackEvent
#include "Event/Track.h"

// from VeloDet
#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTModule.h"

// STDet
#include "STDet/DeITDetector.h"
#include "STDet/DeITStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "STDet/DeITSector.h"
#include "STDet/DeITLadder.h"

// local
#include "GetElementsToAlign.h"

#include "AlignmentFunctions.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetElementsToAlign
//
// 2007-08-09 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace boost;
using namespace boost::lambda;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( GetElementsToAlign );

GetElementsToAlign::GetElementsToAlign( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_elementsToAlign(""),
    m_nElement(0u)
{
  declareProperty("ElementsToAlign", m_elementsToAlign = "/dd/Structure/LHCb");
  declareProperty("Translations"   , m_translations                          );
  declareProperty("Rotations"      , m_rotations                             );

}

GetElementsToAlign::~GetElementsToAlign() {}

StatusCode GetElementsToAlign::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Get pointer to detector svc
  IDataProviderSvc* detectorSvc = svc<IDataProviderSvc>("DetectorDataSvc", true);
  
  /// Retrieve LHCb detector in Transient store
  const std::string lhcbDetectorPath = "/dd/Structure/LHCb";
  DataObject* lhcbObj = 0;
  sc = detectorSvc->retrieveObject(lhcbDetectorPath, lhcbObj);
  if (sc.isFailure()) {
    error() << "==> Couldn't retrieve LHCb Detector" 
            << " from transient store!" << endmsg;
    detectorSvc->release();
    return sc;
  }

  IDetectorElement* lhcb = dynamic_cast<IDetectorElement*>(lhcbObj);
  /// This shouldn't go wrong, but you never know
  if (!lhcb) {
    error() << "==> Object is not a detector element!" << endmsg;
    detectorSvc->release();
    return StatusCode::FAILURE;
  }

  /// Forward slash is the path separator
  Separator sep("/");
  /// Tokens of regular expressions
  Tokenizer regexs(m_elementsToAlign,sep);

  for (Tokenizer::iterator i = regexs.begin(), iEnd = regexs.end(); 
       i != iEnd; ++i) {
    boost::regex ex;
    /// Check if token is a valid regular expression, else catch exception and return statuscode failure.
    /// Better to stop the program and let the user fix the expression than trying to predict what he/she wants.
    try {
      ex.assign((*i), boost::regex_constants::icase);
    } catch (boost::bad_expression& e) {
      error() << "==> " << (*i) << " is not a valid regular expression: " << e.what() << endmsg;
      return StatusCode::FAILURE;
    }
    /// If valid add expression to list of expressions.
    m_regexs.push_back(ex);
  }

  /// Depth is equal to the number regular expressions in the regex list.
  m_depth = m_regexs.size();
  
  /// Print out the list of regular expressions that are going to be used.
  info() << "==> Using the following regular expressions: " << endmsg;
  for (std::list<boost::regex>::const_iterator i = m_regexs.begin(), iEnd = m_regexs.end();
       i != iEnd; ++i) {
    info() << std::left << std::setw(3) << (*i) <<"\n";
  }
  info() << endmsg;

  /// Traverse LHCb detector in transient store and get alignable elements
  getElements(lhcb);

  /// Functions to find to-be-aligned detector element
  std::string name = m_elementsToAlign;
  if (boost::regex_match(name, boost::regex(".*Velo.*"))) { ///< All Velo elements
    DeVelo* velo = getDet<DeVelo>(DeVeloLocation::Default);
    if (!velo) return Error("Failed to retrieve Velo detector element", StatusCode::FAILURE);
    m_isDet = bind<bool>(&LHCb::LHCbID::isVelo,_1);
    if (boost::regex_match(name, boost::regex(".*Velo/VeloRight/Module.{2,4}$"))) { ///< Velo right modules
      typedef const DeVeloSensor* (DeVelo::*memberFunctionP) (LHCb::VeloChannelID) const;
      memberFunctionP fp = &DeVelo::sensor;
      m_detElemFromLHCbID =
        bind<const DetectorElement*>(&GetElementsToAlign::veloModule, this, 
                                     bind<const DeVeloSensor*>(fp, velo, bind<LHCb::VeloChannelID>(&LHCb::LHCbID::veloID, _1)));
    }
  } else if (boost::regex_match(name, boost::regex(".*T/OT/.*"))) {  ///< All OT elements
    DeOTDetector* ot = getDet<DeOTDetector>(DeOTDetectorLocation::Default); ///< Get pointer to OT detector
    if (!ot) return Error("Failed to retrieve OT detector element", StatusCode::FAILURE);
    m_isDet = bind<bool>(&LHCb::LHCbID::isOT,_1);

    if (boost::regex_match(name, boost::regex(".*OT/T.$"))) { ///< OT Stations
      /// DeOTDetector::findStation is overloaded. So we need a function pointer to the function we want to use.
      info() << "Setting find functions for OT stations" << endmsg;
      typedef const DeOTStation* (DeOTDetector::*memberFunctionP) (const LHCb::OTChannelID&) const;
      memberFunctionP fp = &DeOTDetector::findStation;
      m_detElemFromLHCbID = bind<const DetectorElement*>(fp, ot, bind<LHCb::OTChannelID>(&LHCb::LHCbID::otID,_1));
    } else if (boost::regex_match(name, boost::regex(".*OT/T./.{1,2}layer$"))) { ///< OT Layers
      /// DeOTDetector::findLayer is overloaded. So we need a function pointer to the function we want to use.
      typedef const DeOTLayer* (DeOTDetector::*memberFunctionP) (const LHCb::OTChannelID&) const;
      memberFunctionP fp = &DeOTDetector::findLayer;
      m_detElemFromLHCbID = bind<const DetectorElement*>(fp, ot, bind<LHCb::OTChannelID>(&LHCb::LHCbID::otID,_1));
    } 
  } else if (boost::regex_match(name, boost::regex(".*T/IT/.*"))) {  ///< All IT elements
    DeITDetector* it = getDet<DeITDetector>(DeSTDetLocation::IT); ///< Get pointer to IT detector
    if (!it) return Error("Failed to retrieve IT detector element", StatusCode::FAILURE);
    m_isDet = bind<bool>(&LHCb::LHCbID::isIT,_1);

    if (boost::regex_match(name, boost::regex(".*IT/Station.$"))) { ///< IT Stations
      /// DeSTDetector::findStation is overloaded. So we need a function pointer to the function we want to use.
      info() << "Setting find functions for IT stations" << endmsg;
      typedef DeSTStation* (DeSTDetector::*memberFunctionP) (const LHCb::STChannelID);
      memberFunctionP fp = &DeSTDetector::findStation;
      m_detElemFromLHCbID = bind<const DetectorElement*>(fp, it, bind<LHCb::STChannelID>(&LHCb::LHCbID::stID,_1));
    } else if (boost::regex_match(name, boost::regex(".*IT/T./.+Box$"))) { ///< IT Boxes
      /// DeITDetector::findBox is overloaded. So we need a function pointer to the function we want to use.
      typedef DeITBox* (DeITDetector::*memberFunctionP) (const LHCb::STChannelID);
      memberFunctionP fp = &DeITDetector::findBox;
      m_detElemFromLHCbID = bind<const DetectorElement*>(fp, it, bind<LHCb::STChannelID>(&LHCb::LHCbID::stID,_1));
    } else if (boost::regex_match(name, boost::regex(".*IT/T./.+Box/Layer.{1,2}$"))) { ///< IT Layers
      /// DeSTDetector::findLayer is overloaded. So we need a function pointer to the function we want to use.
      typedef DeSTLayer* (DeSTDetector::*memberFunctionP) (const LHCb::STChannelID);
      memberFunctionP fp = &DeSTDetector::findLayer;
      m_detElemFromLHCbID = bind<const DetectorElement*>(fp, it, bind<LHCb::STChannelID>(&LHCb::LHCbID::stID,_1));
    }
  }

  m_findAlignElement = FindAlignmentElement(elementsToAlignBegin(), elementsToAlignEnd(), m_isDet, m_detElemFromLHCbID);

  /// check
  info() << "==> Found " << m_elements.size() << " elements to align: " << endmsg;
  for (Elements::const_iterator i = elementsToAlignBegin(), iEnd = elementsToAlignEnd(); i != iEnd; ++i) {
    info() << std::setw(3) << (*i) << endmsg;
  }
   
  info() << "==> Calling setLocalDeltaParams" << endmsg;
    sc = setLocalDeltaParams(m_translations, m_rotations);
  
  for (Elements::const_iterator i = elementsToAlignBegin(), iEnd = elementsToAlignEnd(); i != iEnd; ++i) {
      info() << std::setw(3) << (*i) << endmsg;
    }
  
  info() << "==> Calling setLocalDeltaMatrix" << endmsg;
  sc = setLocalDeltaMatrix(m_translations, m_rotations);
  
  for (Elements::const_iterator i = elementsToAlignBegin(), iEnd = elementsToAlignEnd(); i != iEnd; ++i) {
    info() << std::setw(3) << (*i) << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode GetElementsToAlign::execute() {
  
  /// Get tracks 
  LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);

  for (LHCb::Tracks::const_iterator i = tracks->begin(), iEnd = tracks->end(); i != iEnd; ++i) {
    std::vector<LHCb::LHCbID> ids = (*i)->lhcbIDs();
    for (std::vector<LHCb::LHCbID>::const_iterator j = ids.begin(), jEnd = ids.end(); j != jEnd; ++j) {
      const AlignmentElement* elem = m_findAlignElement((*j));
      if (elem) {
        info() << "==> LHCbID is " << (*j) << endmsg;
        info() << "==> Alignment Element = " << (*elem) << endmsg;
      }  
    }  
  }
  
  return StatusCode::SUCCESS;
}


void GetElementsToAlign::getElements(const IDetectorElement* parent) {
  
  /// loop over children
  for (IDetIter iC = parent->childBegin(), iCend = parent->childEnd(); iC != iCend; ++iC) {
    
    /// Get path of child
    const std::string childPath = (*iC)->name();
    info() << "Child path = " << childPath << endmsg;
    
    /// forward slash is path seperator
    Separator sep("/");
    /// Tokenise path
    Tokenizer tokenizer(childPath, sep);    
    
    std::list<std::string> paths;
    /// Copy tokens to a temporary list of sub paths. This allows us to determine how deep we are.   
    std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(paths));

    bool match = true;         ///< Can we match a sub path to a regular expression?
    
    /// Begin iterator of regular expressions list 
    std::list<boost::regex>::const_iterator iR = m_regexs.begin();
    /// End iterator of regular expressions list 
    std::list<boost::regex>::const_iterator iRend = m_regexs.end();
    /// Loop over list of sub paths and try to match them
    for (std::list<std::string>::const_iterator i = paths.begin(), iEnd = paths.end(); 
         i != iEnd && iR != iRend; ++i) {
      match = boost::regex_match((*i), (*iR++)); 
      if (!match) break; /// If it doesn't match break loop. No need to try and match the rest 
    }
    
    /// OK we found a detector element
    if (match) {
      if ((paths.size() == m_depth)) {  
        info() << "Adding element " << (*iC)->name() << " with index " << m_nElement << endmsg;
        AlignmentElement elem(dynamic_cast<DetectorElement*>(*iC), m_nElement++);
        m_elements.push_back(elem);
      }
      if (paths.size() < m_depth) debug() << "==> Not deep enough" << endmsg;
      /// Call thyself. Get children recursively.
      /// No need to go any deeper. Somebody might forget $
      if (paths.size() < m_depth) getElements((*iC));
    }
  }
}

StatusCode GetElementsToAlign::setLocalDeltaParams(const Translations& translations,
                                                   const Rotations&    rotations   ) const {
  if (m_elements.size() == translations.size() && m_elements.size() == rotations.size() ) {
    for (Elements::const_iterator i = elementsToAlignBegin(), iEnd = elementsToAlignEnd(); i != iEnd; ++i) {
      const unsigned& index = i->index();
      StatusCode sc = i->setLocalDeltaParams(translations.at(index), rotations.at(index));
      if (sc.isFailure()) {
        info() << "==> Failed to set alignment conditions! Are you sure the element has alignment conditions?" << endmsg;
        break;
      }
    }
  } else {
    info() << "Number of parameters don't match number of detector elements. Not setting alignment conditions." << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode GetElementsToAlign::setLocalDeltaMatrix(const Translations& translations,
                                                   const Rotations&    rotations   ) const {
  if (m_elements.size() == translations.size() && m_elements.size() == rotations.size() ) {
    for (Elements::const_iterator i = elementsToAlignBegin(), iEnd = elementsToAlignEnd(); i != iEnd; ++i) {
      const unsigned& index = i->index();
      StatusCode sc = i->setLocalDeltaMatrix(translations.at(index), rotations.at(index));
      if (sc.isFailure()) {
        info() << "==> Failed to set alignment conditions! Are you sure the element has alignment conditions?" << endmsg;
        break;
      }
    }
  } else {
    info() << "Number of parameters don't match number of detector elements. Not setting alignment conditions." << endmsg;
  }

  return StatusCode::SUCCESS;
}
