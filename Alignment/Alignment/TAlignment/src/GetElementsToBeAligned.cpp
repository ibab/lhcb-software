// $Id: GetElementsToBeAligned.cpp,v 1.13 2008-02-13 18:09:12 janos Exp $
// Include files

//from STL
#include <iomanip>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"

// from TrackEvent
#include "Event/Measurement.h"

// from Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "VeloDet/DeVelo.h"
#include "MuonDet/DeMuonDetector.h"

// from Boost
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"

// // local
#include "GetElementsToBeAligned.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetElementsToBeAligned
//
// 2007-10-08 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GetElementsToBeAligned );

GetElementsToBeAligned::GetElementsToBeAligned( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_groupElems(false),
    m_elemsToBeAligned(),
    m_depth(0),
    m_index(0),
    m_elements(),
    m_rangeElements(),
    m_elementMap()
{
  declareInterface<IGetElementsToBeAligned>(this);
  declareProperty("Elements"     , m_elemsToBeAligned);
  declareProperty("GroupElements", m_groupElems      );
}

GetElementsToBeAligned::~GetElementsToBeAligned() {}

StatusCode GetElementsToBeAligned::initialize() {

  StatusCode sc =  GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize tool!", sc);

  if (m_elemsToBeAligned.empty()) return Error("Please specify which elements to align!", sc);

  /// Get pointer to detector svc
  IDataProviderSvc* detectorSvc = svc<IDataProviderSvc>("DetectorDataSvc", true);

  /// Retrieve LHCb detector in Transient store
  const std::string lhcbDetectorPath = "/dd/Structure/LHCb";
  DataObject* lhcbObj = 0;
  sc = detectorSvc->retrieveObject(lhcbDetectorPath, lhcbObj);
  if (sc.isFailure()) {
    error() << "==> Couldn't retrieve LHCb Detector from transient store!" << endmsg;
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

  info() << "===================== GetElementsToAlign =====================" << endmsg;
  info() << "   Using the following regular expressions: " << endmsg;
 
  std::vector<std::string> groupsOfElements = m_elemsToBeAligned.begin()->second;
  for (std::vector<std::string>::const_iterator i = groupsOfElements.begin(), iEnd = groupsOfElements.end(); i != iEnd; ++i) {
    /// Split string into path to det elem regex and dofs to align
    std::string path = "", dofs = "";
    if ((*i).find(":") != std::string::npos) {
      Tokenizer path_DoFs((*i), Separator(":"));
      path = *(path_DoFs.begin());
      dofs = *(++path_DoFs.begin());
    } else path = (*i);
        
    debug() << "Path to detelems: " << path << endmsg;
    debug() << "DoFs to align   : " << dofs << endmsg;
        
    /// Break path into a set of regexs
    /// Forward slash is the path separator
    // Separator pathSep("/");
    /// Tokens of regular expressions
    Tokenizer regexs(path, Separator("/"));//pathSep);

    /// Print the list of regular expressions that are going to be used.
    info() << "       ";
    for (Tokenizer::iterator j = regexs.begin(), jEnd = regexs.end(); j != jEnd; ++j) info() << "\"" << (*j) << "\"" << " ";
    info() << endmsg;

    /// Create list of regular expressions
    for (Tokenizer::iterator j = regexs.begin(), jEnd = regexs.end(); j != jEnd; ++j) {
      boost::regex ex;
      /// Check if token is a valid regular expression, else catch exception and return statuscode failure.
      /// Better to stop the program and let the user fix the expression than trying to predict what he/she wants.
      try {
        ex.assign((*j), boost::regex_constants::icase);
      } catch (boost::bad_expression& exprs) {
        error() << "==> Error: " << (*j) << " is not a valid regular expression: " << exprs.what() << endmsg;
        return StatusCode::FAILURE;
      }
      /// If valid add expression to list of expressions.
      m_regexs.push_back(ex);
    }

    /// Depth is equal to the number regular expressions in the regex list.
    m_depth = m_regexs.size();
    
    /// Traverse LHCb detector in transient store and get alignable elements
    getElements(lhcb);
    
    
    /// Set the dofs we want to align for
    /// First try to find matches
    bool matchTx = false, matchTy = false, matchTz = false;
    bool matchRx = false, matchRy = false, matchRz = false;
    if (dofs.find("Tx") != std::string::npos) matchTx = true;
    if (dofs.find("Ty") != std::string::npos) matchTy = true;
    if (dofs.find("Tz") != std::string::npos) matchTz = true;
    if (dofs.find("Rx") != std::string::npos) matchRx = true;
    if (dofs.find("Ry") != std::string::npos) matchRy = true;
    if (dofs.find("Rz") != std::string::npos) matchRz = true;
    /// create mask
    std::vector<bool> dofMask = boost::assign::list_of(matchTx)(matchTy)(matchTz)(matchRx)(matchRy)(matchRz);
    /// align for all dofs if dofs not specified or couldn't find any match 
    if (dofs.empty() || (!matchTx && !matchTy && !matchTz && !matchRx && !matchRy && !matchRz)) { 
      dofMask.at(Tx) = true; dofMask.at(Ty) = true; dofMask.at(Tz) = true;
      dofMask.at(Rx) = true; dofMask.at(Ry) = true; dofMask.at(Rz) = true;
    }

    /// Loop over elements and create AlignmentElements
    if (m_groupElems) m_alignElements.push_back(AlignmentElement(m_elements, m_index++, dofMask));
    else std::transform(m_elements.begin(), m_elements.end(), std::back_inserter(m_alignElements),
                        boost::lambda::bind(boost::lambda::constructor<AlignmentElement>(), 
                                            boost::lambda::_1, 
                                            boost::lambda::var(m_index)++, 
                                            dofMask));
    
    m_regexs.clear();
    m_depth = 0u;
    m_elements.clear();
  
  }
  
  m_rangeElements = ElementRange(m_alignElements.begin(), m_alignElements.end());

  // Fill the element map
  for (Elements::const_iterator ialignable = m_alignElements.begin(); ialignable != m_alignElements.end(); ++ialignable) {
    AlignmentElement::ElementContainer allelements = (*ialignable).elementsInTree() ;
    for (AlignmentElement::ElementContainer::const_iterator ielement = allelements.begin() ; 
	 ielement != allelements.end(); ++ielement) {
      m_elementMap[*ielement] = &(*ialignable) ;
    }
  }
  info() << "   Number of elements in map: " << m_elementMap.size() << endmsg ;

  /// Print list of detector elements that are going to be aligned
  info() << "   Going to align " << m_rangeElements.size() << " detector elements:" << endmsg;
  for( ElementRange::const_iterator i = m_rangeElements.begin() ; i != m_rangeElements.end(); ++i) info() <<  "        " << (*i) << endmsg;

  info() << "==============================================================" << endmsg;

  return sc;
}

void GetElementsToBeAligned::getElements(const IDetectorElement* parent) {
  /// loop over children
  for (IDetIter iC = parent->childBegin(), iCend = parent->childEnd(); iC != iCend; ++iC) {
    /// Get path of child
    const std::string childPath = (*iC)->name();
    if (msgLevel(MSG::DEBUG)) debug() << "==> Element  = " << childPath << endmsg;

    /// forward slash is path seperator
    Separator sep("/");
    /// Tokenise path
    Tokenizer tokenizer(childPath, sep);

    std::list<std::string> paths;
    /// Copy tokens to a temporary list of sub paths. This allows us to determine how deep we are.
    std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(paths));

    const size_t currentDepth = paths.size();

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
      if (currentDepth == m_depth) {
        m_elements.push_back(dynamic_cast<DetectorElement*>(*iC));
      }
      /// Call thyself. Get children recursively.
      /// No need to go any deeper.
      if (paths.size() < m_depth) getElements((*iC));
    }
  }
}

const IGetElementsToBeAligned::ElementRange& GetElementsToBeAligned::rangeElements() const {
  return m_rangeElements;
}

const AlignmentElement* GetElementsToBeAligned::findElement(const LHCb::Measurement& meas) const {
  // We'll do this differently with new brunel version
  return findElement(meas.lhcbID()) ;
}

const AlignmentElement* GetElementsToBeAligned::findElement( LHCb::LHCbID id) const {
  // first get the detector element from the measurement. this is no
  // longer needed if we have a new version of brunel.
  const DetectorElement* element(0) ; // = meas.detectorElement() ;
  {
    static DeVelo*       velo = getDet<DeVelo>(DeVeloLocation::Default); 
    static DeSTDetector* tt   = getDet<DeSTDetector>(DeSTDetLocation::TT);
    static DeSTDetector* it   = getDet<DeSTDetector>(DeSTDetLocation::IT);
    static DeOTDetector* ot   = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
    static DeMuonDetector* muon = getDet<DeMuonDetector>(DeMuonLocation::Default);

    switch( id.detectorType() ) {
    case LHCb::LHCbID::Velo:
      element = id.isVeloR() ? 
	static_cast<const DetectorElement*>(velo->rSensor( id.veloID() )) :
	static_cast<const DetectorElement*>(velo->phiSensor( id.veloID() )) ;
	break ;
    case LHCb::LHCbID::TT:
      element = tt->findSector( id.stID() );
      break ;
    case LHCb::LHCbID::IT:
      element = it->findSector( id.stID() );
      break ;
    case LHCb::LHCbID::OT:
      element = ot->findModule( id.otID() ) ;
      break ;
    case LHCb::LHCbID::Muon:
      element = muon->getChmbPtr(id.muonID().station(), id.muonID().region(), id.muonID().quarter() ) ;
      break ;
    default:
      error() << "LHCbID from non-tracking detector"  << endmsg ;
    }
  }
  
  // see if that element is in the map
  ElementMap::const_iterator it = m_elementMap.find( element ) ;
  return it == m_elementMap.end() ? 0 : it->second ;
}
