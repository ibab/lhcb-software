// $Id: AlignmentElement.cpp,v 1.23 2009-01-21 16:27:19 wouter Exp $
// Include files

// from STD
#include <iomanip>
#include <algorithm>
#include <functional>

// Gaudi math
#include "GaudiKernel/Vector3DTypes.h"

// from DetDesc
#include "DetDesc/ParamException.h"
#include "DetDesc/GeometryInfoException.h"
#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

// from BOOST
#include "boost/regex.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "AlignmentElement.h"

std::string AlignmentElement::stripElementName(const std::string& name)
{
  return boost::algorithm::erase_all_regex_copy(name, boost::regex("/dd/Structure/LHCb/.*Region/")) ;
}


AlignmentElement::AlignmentElement(const DetectorElement* element, 
                                   const unsigned int index, 
                                   const std::vector<bool>& dofMask,
				   bool useLocalFrame)
  : m_name(stripElementName(element->name())),
    m_basename(element->name()),
    m_elements(1u,element),
    m_index(index),
    m_activeParOffset(-1),
    m_dofMask(dofMask),
    m_useLocalFrame(useLocalFrame)
{
  m_name = description() ;
  validDetectorElement(m_elements.front());
  initAlignmentFrame();
  for( ElementContainer::const_iterator ielem = m_elements.begin();
       ielem != m_elements.end(); ++ielem) 
    addToElementsInTree( *ielem, m_elementsInTree ) ;
}

AlignmentElement::AlignmentElement(const std::string& aname,
				   const std::vector<const DetectorElement*>& elements, 
                                   const unsigned int index, 
                                   const std::vector<bool>& dofMask,
				   bool useLocalFrame)
  : m_name(stripElementName(aname)),
    m_elements(elements),
    m_index(index),
    m_activeParOffset(-1),
    m_dofMask(dofMask),
    m_useLocalFrame(useLocalFrame)
{
  std::for_each(m_elements.begin(), m_elements.end(),
                boost::lambda::bind(&AlignmentElement::validDetectorElement, this, boost::lambda::_1));
  
  size_t pos = aname.find_first_of("(.*") ;
  m_basename = pos == std::string::npos ? aname : aname.substr(0,pos) ;
  
  initAlignmentFrame();
  
  for( ElementContainer::const_iterator ielem = m_elements.begin();
       ielem != m_elements.end(); ++ielem) 
    addToElementsInTree( *ielem, m_elementsInTree ) ;
}

void AlignmentElement::addToElementsInTree( const IDetectorElement* const ielement,
					    ElementContainer& elements ) 
{
  const DetectorElement* element = dynamic_cast<const DetectorElement*>(ielement) ;
  if( element ) elements.push_back(element) ;
  elements.reserve(elements.size() + ielement->childIDetectorElements().size()) ;
  for( IDetectorElement::IDEContainer::const_iterator ichild = ielement->childBegin() ;
       ichild != ielement->childEnd(); ++ichild ) 
    addToElementsInTree(*ichild, elements) ;
}

bool AlignmentElement::isOffspring( const AlignmentElement& dau) const
{
  bool found=false ;
  if( dau.m_elementsInTree.size() < m_elementsInTree.size() ) {
    // check that _all_ elements are there. note that we have a real
    // problem if only a subset is there ...
    found = true ;
    for( ElementContainer::const_iterator ielem =  dau.m_elements.begin() ;
	 ielem != dau.m_elements.end() && found; ++ielem ) 
      found = std::find( m_elementsInTree.begin(), m_elementsInTree.end(), *ielem )!= m_elementsInTree.end() ;
  }
  return found ;
}

void AlignmentElement::validDetectorElement(const DetectorElement* element) const {
  /// Check that it doesn't point to null
  if (!element) throw GeometryInfoException("DetectorElement* points to NULL!");
  /// Check that it has alignment conditions
  if (!(element->geometry()->alignmentCondition())) throw GeometryInfoException("AlignmentCondition* points to NULL!");
}

void AlignmentElement::initAlignmentFrame() {
  // Calculate the center of gravity (the 'local' center)
  Gaudi::XYZVector averageR(0.0, 0.0, 0.0);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    averageR += ((*i)->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0))) - Gaudi::XYZPoint(0.0, 0.0, 0.0);
  }
  averageR /= double(m_elements.size());
  m_centerOfGravity = Gaudi::XYZPoint(0.0, 0.0, 0.0) + averageR;

  // Initialize the alignment frame transform. For now, we'll only do
  // something special for Velo modules. For all other modules the
  // alignment frame is still just a translation to the cog.
  if( m_useLocalFrame ) {
    m_alignmentFrame = m_elements.front()->geometry()->toGlobalMatrix() ;
  } else {
    m_alignmentFrame = Gaudi::Transform3D(averageR) ;
  }
  m_jacobian = AlParameters::jacobian( m_alignmentFrame ) ;
  
  // This is a simple check for velo R, just to make sure Jan separated them correctly:
  if( m_elements.size()==1 && dynamic_cast<const DeVeloRType*>(m_elements.front()) &&
      m_dofMask.isActive(AlParameters::Rz) ) {
    std::cout << "PROBLEM: You are aligning Rz for velo R module with name "
	      << m_elements.front()->name() << std::endl ;
  }
}

std::string AlignmentElement::description() const {
  std::string begin = (m_elements.size() > 1u ? "Group = { " : "");
  
  std::string middle;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::string nameElem = stripElementName( (*i)->name() )
      + (i != (iEnd - 1u)?", ":"");
    middle += nameElem;
  }
  
  std::string end = (m_elements.size() > 1u ? "}" : "");
  
  return begin+middle+end;
}

const std::vector<double> AlignmentElement::deltaTranslations() const {
  std::vector<double> averageDeltaTranslations(3u, 0.0);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::vector<double> elemDeltaTranslations = (*i)->geometry()->alignmentCondition()->paramVect<double>("dPosXYZ");
    std::transform(elemDeltaTranslations.begin(), elemDeltaTranslations.end(), averageDeltaTranslations.begin(),
                   averageDeltaTranslations.begin(), std::plus<double>());
  }
  std::transform(averageDeltaTranslations.begin(), averageDeltaTranslations.end(),
                 averageDeltaTranslations.begin(), boost::lambda::bind(&AlignmentElement::average, this, boost::lambda::_1));
  return averageDeltaTranslations;
}

const std::vector<double> AlignmentElement::deltaRotations() const {  
  std::vector<double> averageDeltaRotations(3u, 0.0);
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    std::vector<double> elemDeltaRotations = (*i)->geometry()->alignmentCondition()->paramVect<double>("dRotXYZ");
    std::transform(elemDeltaRotations.begin(), elemDeltaRotations.end(), averageDeltaRotations.begin(),
                   averageDeltaRotations.begin(), std::plus<double>());
  }
  std::transform(averageDeltaRotations.begin(), averageDeltaRotations.end(),
                 averageDeltaRotations.begin(), boost::lambda::bind(&AlignmentElement::average, this, boost::lambda::_1));
  
  return averageDeltaRotations;
}

AlParameters AlignmentElement::currentTotalDelta() const {
  double par[6] = {0,0,0,0,0,0} ;
  Gaudi::Transform3D alignmentFrameInv = alignmentFrame().Inverse() ;
  for (ElemIter ielem = m_elements.begin(); ielem != m_elements.end(); ++ielem) {
    //Gaudi::Transform3D globalDeltaMatrix = DetDesc::globalDelta(*ielem) ;
    Gaudi::Transform3D global        = ((*ielem)->geometry())->toGlobalMatrix();
    Gaudi::Transform3D globalNominal = ((*ielem)->geometry())->toGlobalMatrixNominal();
    Gaudi::Transform3D globalDelta   = global*globalNominal.Inverse();
    //Gaudi::Transform3D alignDeltaMatrix = alignmentFrameInv * globalDeltaMatrix * alignmentFrame();
    Gaudi::Transform3D alignDeltaMatrix = alignmentFrameInv * globalDelta * alignmentFrame();
    std::vector<double> translations(3,0.0), rotations(3,0.0);
    DetDesc::getZYXTransformParameters(alignDeltaMatrix, translations, rotations);//, it->pivot());
    for(size_t i=0; i<3; ++i) {
      par[i]   += translations[i] ;
      par[i+3] += rotations[i] ;
    }
  }
  for(size_t i=0; i<6; ++i) par[i] /= m_elements.size() ;
  return AlParameters(par) ;
}

AlParameters AlignmentElement::currentActiveTotalDelta() const 
{
  return AlParameters(currentTotalDelta().parameterArray(),m_dofMask ) ;
}

AlParameters AlignmentElement::currentDelta() const {
  double par[6] = {0,0,0,0,0,0} ;
  Gaudi::Transform3D alignmentFrameInv = alignmentFrame().Inverse() ;
  for (ElemIter ielem = m_elements.begin(); ielem != m_elements.end(); ++ielem) {
    // here I want just the _local_ delta.
    const Gaudi::Transform3D& localDelta   = ((*ielem)->geometry())->ownToOffNominalMatrix() ;
    // this one includes the localDelta, I think.
    const Gaudi::Transform3D& global        = ((*ielem)->geometry())->toGlobalMatrix();
    // so we subtract the delta to get the transformation matrix to global
    // Gaudi::Transform3D globalMinusDelta = global * localDelta.Inverse() ;
    // transform the local delta into the global frame
    // Gaudi::Transform3D localDeltaInGlobal = globalMinusDelta * localDelta * globalMinusDelta.Inverse() ;
    // which could be shorter written like this (really!)
    Gaudi::Transform3D localDeltaInGlobal = global * localDelta * global.Inverse() ;
    // now transform it into the alignment frame
    Gaudi::Transform3D alignDeltaMatrix = alignmentFrameInv * localDeltaInGlobal * alignmentFrame();
    std::vector<double> translations(3,0.0), rotations(3,0.0);
    DetDesc::getZYXTransformParameters(alignDeltaMatrix, translations, rotations);//, it->pivot());
    for(size_t i=0; i<3; ++i) {
      par[i]   += translations[i] ;
      par[i+3] += rotations[i] ;
    }
  }
  for(size_t i=0; i<6; ++i) par[i] /= m_elements.size() ;
  return AlParameters(par) ;
}

AlParameters AlignmentElement::currentActiveDelta() const 
{
  return AlParameters(currentDelta().parameterArray(),m_dofMask ) ;
}

bool AlignmentElement::operator==(const DetectorElement* rhs) const {
  ElemIter match = std::find(m_elements.begin(), m_elements.end(), rhs);
  return (match != m_elements.end() ? true : false);
}

StatusCode AlignmentElement::updateGeometry( const AlParameters& parameters) 
{
  /// Construct global delta matrix from deltas
  // transform the delta to the global frame
  const Gaudi::Transform3D globalDeltaMatrix = m_alignmentFrame * parameters.transform() * m_alignmentFrame.Inverse() ;
  StatusCode sc;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    /// Transform global delta matrix to new local delta matrix (takes current local delta matrix into account!!)
    const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*i), globalDeltaMatrix);
    /// Update position of detector element with new local delta matrix
    sc = const_cast<IGeometryInfo*>((*i)->geometry())->ownToOffNominalMatrix(localDeltaMatrix);
    if (sc.isFailure()) {
      std::cout << "Failed to update local delta matrix of detector element " + (*i)->name() <<  std::endl;
      break; ///< Break loop if sc is failure
    }
  }
  // Update the transform of the alignment frame
  initAlignmentFrame() ;
  return sc;
}

// inline const Gaudi::Transform3D AlignmentElement::localDeltaMatrix() const {
//   return m_element->geometry()->ownToOffNominalMatrix();
// }

std::ostream& AlignmentElement::fillStream(std::ostream& lhs) const {
  const std::vector<double> t = deltaTranslations();
  const std::vector<double> r = deltaRotations();
  static const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
  
  lhs << std::endl;
  lhs << std::left << std::setw(80u) << std::setfill('*') << "" << std::endl;
  lhs << "* Alignable: " << name() << "\n" 
      << "* Element  : " << description() << std::endl
      << "* Index    : " << index() << "\n"
      << "* dPosXYZ  : " << Gaudi::XYZPoint(t[0], t[1], t[2]) << "\n"
      << "* dRotXYZ  : " << Gaudi::XYZPoint(r[0], r[1], r[2]) << "\n"
      << "* PivotXYZ : " << centerOfGravity() << "\n"
      << "* DoFs     : ";
  for (DofMask::const_iterator j = m_dofMask.begin(), jEnd = m_dofMask.end(); j != jEnd; ++j) {
    if ((*j)) lhs << dofs.at(std::distance(m_dofMask.begin(), j)) + " ";
  }
  lhs << std::endl;
  if( !m_daughters.empty() ) {
    lhs << "* Daughters: ";
    for(DaughterContainer::const_iterator idau = m_daughters.begin() ;
	idau != m_daughters.end(); ++idau)
      lhs << (*idau)->index() << " " ;
    lhs << std::endl;
  }
  lhs << std::left << std::setw(80u) << std::setfill('*') << "" << std::endl;

  return lhs;
}

double AlignmentElement::histoResidualMax() const
{
  // in the end, I would like to do this by inheritance. for now, just
  // very ugly:
  double resmax[] = { 0.1,0.1,0.5,0.5,5.0,10 } ;
  size_t dettype(0) ; // [veloR,veloPhi,,tt,it,ot,muon]
  if(     name().find("Muon",0) != std::string::npos)
    dettype = 5 ;
  else if(name().find("OT",0) != std::string::npos)
    dettype = 4 ;
  else if(name().find("IT",0) != std::string::npos)
    dettype = 3 ;
  else if(name().find("TT",0) != std::string::npos)
    dettype = 2 ;
  return resmax[dettype] ;
}
