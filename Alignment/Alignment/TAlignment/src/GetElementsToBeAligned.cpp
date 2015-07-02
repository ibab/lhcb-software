// $Id: GetElementsToBeAligned.cpp,v 1.31 2010-08-13 21:03:50 wouter Exp $
// Include files

//from STL
#include <iomanip>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/IDetDataSvc.h"

// from TrackEvent
#include "Event/Measurement.h"
#include "Event/Node.h"

// from Det
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "VeloDet/DeVelo.h"
#include "VPDet/DeVP.h"
#include "STDet/DeUTDetector.h"
#include "FTDet/DeFTDetector.h"
#include "MuonDet/DeMuonDetector.h"
#include "STDet/DeTTSector.h"

// from Boost
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/construct.hpp"
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/regex.hpp"

// // local
#include "GetElementsToBeAligned.h"
#include "AlignKernel/AlEquations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetElementsToBeAligned
//
// 2007-10-08 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GetElementsToBeAligned )

GetElementsToBeAligned::GetElementsToBeAligned( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_useLocalFrame(true),
    m_upgrade(false),
    m_elemsToBeAligned(),
    m_elementMap(),
    m_initTime(0)
{
  declareInterface<IGetElementsToBeAligned>(this);
  declareProperty("Elements"     , m_elemsToBeAligned);
  declareProperty("UseLocalFrame", m_useLocalFrame   );
  declareProperty("Upgrade"      , m_upgrade );
}

GetElementsToBeAligned::~GetElementsToBeAligned() {}

StatusCode GetElementsToBeAligned::initialize() {

  StatusCode sc =  GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize tool!", sc);

  if ( m_elemsToBeAligned.empty() ) {
    warning() << "You haven't specified any elements to align!" << endmsg ;
  }

  info() << "Use local frame = " << m_useLocalFrame << endreq ;
  
  // delay initialization till the first event
  m_initTime = 0 ;

  /// Get pointer to detector svc
  IDataProviderSvc* detectorSvc = 0;
  sc = service("DetectorDataSvc", detectorSvc, true);
  if ( sc.isFailure() ) {
    error() << "Something went wrong with retrieving detector data svc" << endmsg;
    return sc;
  }

  /// Retrieve LHCb detector in Transient store
  const std::string lhcbDetectorPath = "/dd/Structure/LHCb";
  DataObject* lhcbObj = 0;
  sc = detectorSvc->retrieveObject(lhcbDetectorPath, lhcbObj);
  if (sc.isFailure()) {
    error() << "==> Couldn't retrieve LHCb Detector from transient store!" << endmsg;
    detectorSvc->release();
    return sc;
  }

  const IDetectorElement* lhcb = dynamic_cast<const IDetectorElement*>(lhcbObj);
  /// This shouldn't go wrong, but you never know
  if (!lhcb) {
    error() << "==> Object is not a detector element!" << endmsg;
    detectorSvc->release();
    return StatusCode::FAILURE;
  }

  info() << "===================== GetElementsToAlign =====================" << endmsg;
  debug() << "   Using the following regular expressions: " << endmsg;

  size_t index(0) ;
  typedef std::vector<AlignmentElement*> NonConstElements ;
  NonConstElements alignelements ;
  for ( std::vector<std::string>::const_iterator i = m_elemsToBeAligned.begin(), iEnd = m_elemsToBeAligned.end(); 
        i != iEnd; ++i ) {
    /// Split string into path to det elem regex and dofs to align
    bool groupElems = false;
    std::string groupname, path, dofs;

    // first : second : third
    // tokens of strings
    std::vector<std::string> tokens;
    Tokenizer split((*i), Separator(" :"));
    for (Tokenizer::iterator j = split.begin(), jEnd = split.end(); j != jEnd; ++j) tokens.push_back((*j));
    
    if( tokens.size()==2 ) {
      path = tokens.at(0);
      dofs = tokens.at(1);
    } else if (tokens.size()==3) {
      groupElems = true ;
      path = tokens.at(1) ;
      dofs = tokens.at(2) ;
      groupname = tokens.at(0) ;
      if( groupname.find("Group") != std::string::npos) groupname = path ;
    } else {
      error() << "==> There is something wrong with the specified property Elements: " 
              << tokens.size() << std::endl 
              << *i << endmsg;
      return StatusCode::FAILURE;
    }
    
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Group elements   : " << groupElems << endmsg;
      if(groupElems) debug() << "Group name       : " << groupname << endmsg;
      debug() << "Path to detelems : " << path       << endmsg;
      debug() << "DoFs to align    : " << dofs       << endmsg;
    }
    
    // Traverse LHCb detector in transient store and get alignable elements
    std::vector<const DetectorElement*> detelements ;
    // Break path into a set of regexs. Forward slash is the path separator
    Tokenizer elemtokenizer(path, Separator("/"));
    RegExs regexs ;
    
    // Print the list of regular expressions that are going to be used.
    debug() << "       ";
    for (Tokenizer::iterator j = elemtokenizer.begin(), jEnd = elemtokenizer.end(); j != jEnd; ++j) 
      debug() << "\"" << (*j) << "\"" << " ";
    debug() << endmsg;
    
    // Create list of regular expressions
    for (Tokenizer::iterator j = elemtokenizer.begin(), jEnd = elemtokenizer.end(); j != jEnd; ++j) {
      boost::regex ex;
      // Check if token is a valid regular expression, else catch exception and return statuscode failure.
      // Better to stop the program and let the user fix the expression than trying to predict what he/she wants.
      try {
        ex.assign((*j), boost::regex_constants::icase);
      } catch (boost::bad_expression& exprs) {
        error() << "==> Error: " << (*j) << " is not a valid regular expression: " << exprs.what() << endmsg;
        return StatusCode::FAILURE;
      }
      // If valid add expression to list of expressions.
      regexs.push_back(ex);
    }
    
    // Depth is equal to the number regular expressions in the regex list.
    getElements(lhcb, regexs, regexs.size(), detelements) ;

    // Check that we have found elements to align, else exit gracefully
    if ( detelements.empty() ) {
      error() << "\n ==> Couldn't find any elements that matched the given regular expression! \n"
              << " expression=\'" << *i << "\'" << std::endl
              << " ==> The syntax of the property Elements is a list of : \"Group : Regex representing path of det elems : dofs\" \n" 
              << " ==> where group and dofs are optional.\n" 
              << " ==> Check the regular expression and also check if there are no whitespaces.\n" << endmsg;
      return StatusCode::FAILURE;
    }

    // Loop over elements and create AlignmentElements
    if (groupElems) {
      // first check that there isn't already a group with this name. if there is, add the elements.
      NonConstElements::iterator ielem = alignelements.begin() ;
      while( ielem != alignelements.end() && (*ielem)->name() != groupname) ++ielem ;
      if( ielem != alignelements.end() ) {
	(*ielem)->addElements( detelements ) ;
	(*ielem)->addDofs( dofs ) ;
      }
      else alignelements.push_back(new AlignmentElement(groupname,
                                                        detelements, index++, dofs,info(),m_useLocalFrame));
    } else {
      for(std::vector<const DetectorElement*>::iterator ielem = detelements.begin() ;
          ielem != detelements.end(); ++ielem) {
	//check that there isn't already a group with this name. if there is, only set the dofs
	std::string name = AlignmentElement::stripElementName( (*ielem)->name()) ;
	NonConstElements::iterator jelem = alignelements.begin() ;
	while( jelem != alignelements.end() && (*jelem)->name() != name) ++jelem ;
	if( jelem != alignelements.end() ) {
	  warning() << "Multiple specifications of dofs for alignable "
		    << name << ". Using '" << dofs << "'." << endreq ;
	  (*jelem)->setDofs( dofs ) ;
	} else {
	  alignelements.push_back(new AlignmentElement(**ielem, index++, dofs,info(),m_useLocalFrame));
	}
      }
    }
  }
  
  // sort the elements by hierarchy. currently, this just follows the
  // name of the first associated detector element.
  std::stable_sort( alignelements.begin(), alignelements.end(), ElementSorter()) ;
  
  // make sure to reset the indices after sorting
  index = 0 ;
  for (NonConstElements::iterator ielem = alignelements.begin(); 
       ielem != alignelements.end(); ++ielem, ++index) 
    (*ielem)->setIndex(index) ;
  
  // set up the mother-daughter relations. This only works once the
  // elements are sorted. We start with the lowest level elements, and
  // work our way up the tree.
  for (NonConstElements::reverse_iterator ielem = alignelements.rbegin(); 
       alignelements.rend() != ielem ; ++ielem) {
    bool found = false ;
    NonConstElements::reverse_iterator jelem = ielem ;
    for (++jelem ; jelem != alignelements.rend()&&!found; ++jelem) 
      // is 'i' a daughter of 'j'
      if( (*jelem)->isOffspring( **ielem ) ) {
        (const_cast<AlignmentElement*>((*jelem)))->addDaughter( **ielem) ;
        found = true ;
      }
  }
  
  // now fill the element map. this map is used to assign a certain
  // hit to a certain alignment element. again, it is important that
  // alignment elements are sorted: we add the hit only to the lowest
  // level element in the tree.
  for (NonConstElements::const_iterator ielem = alignelements.begin(); 
       ielem != alignelements.end(); ++ielem) {
    AlignmentElement::ElementContainer allelements = (*ielem)->elementsInTree() ;
    for (AlignmentElement::ElementContainer::const_iterator idetelem = allelements.begin() ; 
         idetelem != allelements.end(); ++idetelem)
      m_elementMap[*idetelem] = *ielem ;
  }

  // copy the non-const elements into the local container.
  m_elements.insert(m_elements.end(),alignelements.begin(),alignelements.end()) ;

  /// Print list of detector elements that are going to be aligned
  
  info() << "   Going to align " << m_elements.size() << " detector elements:" << endmsg;
  info() << "   Number of elements in map: " << m_elementMap.size() << endmsg ;
  
  info() << "==============================================================" << endmsg;

  return sc;
}

StatusCode GetElementsToBeAligned::finalize()
{
  for (Elements::const_iterator ialignable = m_elements.begin(); 
       ialignable != m_elements.end(); ++ialignable) 
    delete *ialignable ;
  m_elements.clear() ;
  return GaudiTool::finalize() ;
}


void GetElementsToBeAligned::getElements(const IDetectorElement* parent, const RegExs& regexs,
                                         size_t depth, std::vector<const DetectorElement*>& detelements) const {
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
    std::list<boost::regex>::const_iterator iR = regexs.begin();
    /// End iterator of regular expressions list
    std::list<boost::regex>::const_iterator iRend = regexs.end();
    /// Loop over list of sub paths and try to match them
    for (std::list<std::string>::const_iterator i = paths.begin(), iEnd = paths.end();
         i != iEnd && iR != iRend; ++i) {
      match = boost::regex_match((*i), (*iR++));
      if (!match) break; /// If it doesn't match break loop. No need to try and match the rest
    }

    /// OK we found a detector element
    if (match) {
      if (currentDepth == depth) {
        detelements.push_back(dynamic_cast<DetectorElement*>(*iC));
      }
      /// Call thyself. Get children recursively.
      /// No need to go any deeper.
      if (paths.size() < depth) getElements((*iC),regexs,depth,detelements);
    }
  }
}

StatusCode GetElementsToBeAligned::findElements(const std::string& path,
                                                std::vector<const AlignmentElement*>& alignelements) const 
{
  alignelements.clear() ;
  boost::regex ex ;
  ex.assign( path, boost::regex_constants::icase) ;
  for( Elements::const_iterator ialelem = m_elements.begin() ; 
       ialelem != m_elements.end() ; ++ialelem) {
    bool match = (*ialelem)->name() == path || boost::regex_match((*ialelem)->name(),ex) ;
    if(!match)
      // if any detector element matches, this is also fine
      for( AlignmentElement::ElementContainer::const_iterator idetelem = (*ialelem)->detelements().begin() ;
           idetelem != (*ialelem)->detelements().end() && !match ; ++idetelem) 
        match = boost::regex_match((*idetelem)->name(),ex) ;
    if(match) alignelements.push_back( *ialelem ) ;
  }
  return StatusCode::SUCCESS ;
}

const AlignmentElement* GetElementsToBeAligned::findElement(const LHCb::Node& node) const
{
  const AlignmentElement* elem(0) ;
  if( node.hasMeasurement() ) {
    const LHCb::Measurement& meas = node.measurement() ;
    const DetectorElement* detelem = meas.detectorElement() ;
    if( detelem ) {
      // for TT locate the sensor using the estimated position of the track
      if( meas.lhcbID().isTT() ) {
	const DeTTSector* ladder = dynamic_cast<const DeTTSector*>(detelem) ;
	if( ladder ) detelem = ladder->findSensor( node.state().position() ) ;
      }
      elem = findElement( *detelem ) ;
    } else {
      elem = findElement(meas.lhcbID()) ;
    }
  }
  return elem ;
}

const AlignmentElement* GetElementsToBeAligned::findElement(const LHCb::Measurement& meas) const {
  const DetectorElement* detelem = meas.detectorElement() ;
  return detelem ?  findElement( *detelem ) : findElement(meas.lhcbID()) ;
}

const AlignmentElement* GetElementsToBeAligned::findElement(const DetectorElement& element) const 
{  
  ElementMap::const_iterator it = m_elementMap.find( &element ) ;
  return it == m_elementMap.end() ? 0 : it->second ;
}

const AlignmentElement* GetElementsToBeAligned::findElement(const LHCb::LHCbID& id) const {
  // first get the detector element from the measurement. this is no
  // longer needed if we have a new version of brunel.
  const DetectorElement* element(0) ; // = meas.detectorElement() ;
  {
    static DeVP*         vp;
    static DeSTDetector* ut;
    static DeFTDetector* ft;
    static DeVelo*       velo;
    static DeSTDetector* tt;
    static DeSTDetector* it;
    static DeOTDetector* ot;
    
    if (m_upgrade) 
    {
      vp   = getDet<DeVP>(  DeVPLocation::Default ) ;
      ut   = getDet<DeUTDetector>(DeSTDetLocation::UT);
      ft   = getDet<DeFTDetector>( DeFTDetectorLocation::Default );
    }
    else
    {
      velo = getDet<DeVelo>(DeVeloLocation::Default); 
      tt   = getDet<DeSTDetector>(DeSTDetLocation::TT);
      it   = getDet<DeSTDetector>(DeSTDetLocation::IT);
      ot   = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
    }
    static DeMuonDetector* muon = getDet<DeMuonDetector>(DeMuonLocation::Default);

    switch( id.detectorType() ) {
    case LHCb::LHCbID::VP:
      {
        LHCb::VPChannelID chID = id.vpID();
        element = vp->sensorOfChannel(chID); 
      }
      break ;
    case LHCb::LHCbID::UT:
      element = ut->findSector( id.stID() ) ;
      break;
    case LHCb::LHCbID::FT:
      element = ft->findFibreMat( id.ftID() );
      break;  
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
      element = muon->getChmbPtr(id.muonID().station(), id.muonID().region(), (muon->Tile2Chamber(id.muonID()))[0]->chamberNumber() ) ;
      break ;
    default:
      error() << "LHCbID from non-tracking detector"  << endmsg ;
    }
  }
  
  // see if that element is in the map
  ElementMap::const_iterator it = m_elementMap.find( element ) ;
  return it == m_elementMap.end() ? 0 : it->second ;
}

void GetElementsToBeAligned::initEquations( Al::Equations& equations ) const
{
  equations = Al::Equations( m_elements.size(), initTime() ) ;
  for(Elements::const_iterator ielem = m_elements.begin(); ielem!= m_elements.end(); ++ielem) {
    // get the current delta (called alpha in Al::Equations)
    Gaudi::Vector6 alpha = (*ielem)->currentTotalDelta().transformParameters() ;
    equations.element( (*ielem)->index() ).m_alpha = alpha ;
    equations.element( (*ielem)->index() ).m_alphaIsSet = true ;
  }
}

StatusCode GetElementsToBeAligned::initAlignmentFrame( Gaudi::Time now )
{
  StatusCode sc = StatusCode::SUCCESS ;
  if( m_initTime != 0 ) {
    error() << "Severe ERROR: alignment frame already initialized: "
	    << m_initTime << " " << now << endreq ;
    sc = Warning("Severe ERROR: alignment frame already initialized", StatusCode::FAILURE) ;
  } else {
    std::stringstream message ;
    message << std::left << std::setw(80u) << std::setfill('*') << std::endl ;
    IDetDataSvc* detDataSvc(0) ;
    sc = service("DetectorDataSvc",detDataSvc, false);
    if ( sc.isFailure() ) {
      error() << "Could not retrieve DetectorDataSvc" << endmsg ;
      return sc;
    }
    if( now == 0 ) {
      now = detDataSvc->eventTime() ;
    } else {
      message << "Updating detector data svc and update manager svc with time: " << m_initTime.ns() << std::endl ;
      IUpdateManagerSvc* updateMgrSvc(0) ;
      sc = service("UpdateManagerSvc",updateMgrSvc, false);
      if ( sc.isFailure() ) {
	error() << "Could not retrieve UpdateManagerSvc" << endmsg ;
	return sc;
      }
      detDataSvc->setEventTime( now ) ;
      updateMgrSvc->newEvent() ;
    }

    m_initTime = now ;
    message << "Initializing alignment frames with time: " << m_initTime.ns()
	    << " --> " 
	    << m_initTime.format(true,"%F %r") << std::endl ;
    // update all elements with this time
    for (Elements::iterator ielem = m_elements.begin(); 
	 ielem != m_elements.end(); ++ielem) 
      (const_cast<AlignmentElement*>(*ielem))->initAlignmentFrame();
    
    // now print the positions, just for debugging  
    for( Elements::const_iterator i = m_elements.begin() ; i != m_elements.end(); ++i) 
      message << **i  ;
    
    message << std::left << std::setw(80u) << std::setfill('*') << "" ;
    info() << message.str() << endreq ;
  }
  return sc ;
}

