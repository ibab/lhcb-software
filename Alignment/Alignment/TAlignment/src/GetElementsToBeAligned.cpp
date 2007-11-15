// $Id: GetElementsToBeAligned.cpp,v 1.1 2007-11-15 11:12:05 janos Exp $
// Include files 
//from STL
#include <iomanip>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataObject.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

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
    m_elemsToBeAligned(),
    m_depth(0),
    m_index(0),
    m_elements(),
    m_rangeElements(),
    m_constraints(0),
    m_findElement()
{
  declareInterface<IGetElementsToBeAligned>(this);
  declareProperty("Elements", m_elemsToBeAligned);
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
  
  /// Forward slash is the path separator
  Separator sep("/");
  /// Tokens of regular expressions
  Tokenizer regexs(m_elemsToBeAligned.begin()->second, sep);

  /// Print the list of regular expressions that are going to be used.
  info() << "===================== GetElementsToAlign =====================" << endmsg;
  info() << "   Using the following regular expressions: " << endmsg;
  info() << "       ";
  for (Tokenizer::iterator i = regexs.begin(), iEnd = regexs.end(); i != iEnd; ++i) {
    info() << std::left << std::setw(3) << (*i) << " ";
  }
  info() << endmsg;
  
  /// Create list of regular expressions
  for (Tokenizer::iterator i = regexs.begin(), iEnd = regexs.end(); i != iEnd; ++i) {
    boost::regex ex;
    /// Check if token is a valid regular expression, else catch exception and return statuscode failure.
    /// Better to stop the program and let the user fix the expression than trying to predict what he/she wants.
    try {
      ex.assign((*i), boost::regex_constants::icase);
    } catch (boost::bad_expression& exprs) {
      error() << "==> Error: " << (*i) << " is not a valid regular expression: " << exprs.what() << endmsg;
      return StatusCode::FAILURE;
    }
    /// If valid add expression to list of expressions.
    m_regexs.push_back(ex);
  }  

  /// Depth is equal to the number regular expressions in the regex list.
  m_depth = m_regexs.size();  

  /// Traverse LHCb detector in transient store and get alignable elements
  getElements(lhcb);

  m_rangeElements = std::make_pair(m_elements.begin(), m_elements.end());
  m_findElement   = FindAlignmentElement(Alignment::FindElement(m_elemsToBeAligned.begin()->first), m_rangeElements);  

  /// Print list of detector elements that are going to be aligned
  info() << "   Going to align " << std::distance(m_rangeElements.first, m_rangeElements.second) << " detector elements:" << endmsg;
  typedef std::vector<AlignmentElement>::const_iterator ElemIter;
  for (ElemIter i = m_rangeElements.first, iEnd = m_rangeElements.second; i != iEnd; ++i) {
    info()<<  "        " << "Element " << i->name() << " with index " << i->index() << " and pivot " << i->pivotXYZPoint() << endmsg;
  }
  info() << "   With " << m_constraints.size() << " constraints:" << endmsg;
  unsigned nC = 0u;
  bool clear = false;
  for (IGetElementsToBeAligned::Constraints::const_iterator i = m_constraints.begin(), iEnd = m_constraints.end(); i != iEnd; 
       ++i, ++nC) {
    info() << "       " << nC << ": f = [ ";
    for (std::vector<double>::const_iterator j = i->begin(), jEnd = i->end()-1u; j != jEnd; ++j) {
      info()  << (*j) << " ";
    }
    info() << " ] ";
    info() << " and f0 = " << i->back() << endmsg;
    if (i->size()-1u !=  std::distance(m_rangeElements.first, m_rangeElements.second)*6u) {
      warning() << "Number of elements do not much total number of degrees of freedom!" << endmsg;
      warning() << "Setting constraints to ZERO!" << endmsg;
      clear = true;
      break;
    }
  }
  if (clear) m_constraints.clear();
  
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
        if (msgLevel(MSG::DEBUG)) debug() << "Adding element " << (*iC)->name() << " with index " << m_index << endmsg;
        AlignmentElement elem(dynamic_cast<DetectorElement*>(*iC), m_index++);
        m_elements.push_back(elem);
      }
      /// Call thyself. Get children recursively.
      /// No need to go any deeper.
      if (paths.size() < m_depth) getElements((*iC));
    }
  }
}

const IGetElementsToBeAligned::Range& GetElementsToBeAligned::rangeElements() const {
  return m_rangeElements;
}

const IGetElementsToBeAligned::Constraints& GetElementsToBeAligned::constraints() const {
  return m_constraints;
}

const AlignmentElement* GetElementsToBeAligned::findElement(LHCb::LHCbID anLHCbID) const {  
  return m_findElement(anLHCbID);
}
