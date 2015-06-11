// $Id: AlignmentElement.cpp,v 1.32 2010-08-13 21:03:14 wouter Exp $
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
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/AlignmentCondition.h"
#include "VeloDet/VeloAlignCond.h"

// from BOOST
#include "boost/regex.hpp"
#include "boost/algorithm/string_regex.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"

// local
#include "AlignmentElement.h"

Gaudi::Transform3D AlignmentElement::localDelta( const DetectorElement& element)
{
  Gaudi::Transform3D delta = element.geometry()->ownToOffNominalMatrix() ;
  const VeloAlignCond* velocond = dynamic_cast<const VeloAlignCond*>(element.geometry()->alignmentCondition()) ;
  if(velocond) {
    // ok, this is tricky. this is a velo half and we need to subtract
    // the transform of the motion system.
    // from the VeloAlignCond code, I think: total = ( motion-systym-transform * alignmenttransform )
    Gaudi::Transform3D mstransform = velocond->motionSystemTransform() ;
    Gaudi::Transform3D alignmenttransform = mstransform.Inverse() * delta ;
    delta = alignmenttransform ;
  }
  return delta ;
}

Gaudi::Transform3D AlignmentElement::toGlobalMatrixMinusDelta( const DetectorElement& element)
{
  Gaudi::Transform3D localdelta = localDelta( element ) ;
  const Gaudi::Transform3D& global       = element.geometry()->toGlobalMatrix();
  return global * localdelta.Inverse() ;
}

std::string AlignmentElement::stripElementName(const std::string& name)
{
  return boost::algorithm::erase_all_regex_copy(name, boost::regex("/dd/Structure/LHCb/.*Region(/T/|/)")) ;
}

namespace {
  template<class T> struct SortByName
  {
    bool operator()( const T* lhs, const T* rhs ) { return lhs->name() < rhs->name() ; }
  } ;
}
  
AlignmentElement::AlignmentElement(const DetectorElement& element, 
                                   const unsigned int index, 
				   const std::string& dofs,
				   MsgStream& msgstream,
				   bool useLocalFrame)
  : m_name(stripElementName(element.name())),
    m_basename(element.name()),
    m_mother(0),
    m_index(index),
    m_activeParOffset(-1),
    m_dofMask(AlParameters::NumPars,false),
    m_useLocalFrame(useLocalFrame),
    m_msgstream(&msgstream)
{
  //m_name = description() ;
  std::vector<const DetectorElement*> elements(1u,&element) ;
  addElements( elements ) ;
  addDofs(dofs) ;
}

AlignmentElement::AlignmentElement(const std::string& aname,
				   const std::vector<const DetectorElement*>& elements, 
                                   const unsigned int index,
				   const std::string& dofs,
				   MsgStream& msgstream,
				   bool useLocalFrame)
  : m_name(aname),
    m_mother(0),
    m_index(index),
    m_activeParOffset(-1),
    m_dofMask(AlParameters::NumPars,false),
    m_useLocalFrame(useLocalFrame),
    m_msgstream(&msgstream)
{
  size_t pos = aname.find_first_of("(.*") ;
  m_basename = pos == std::string::npos ? aname : aname.substr(0,pos) ;
  addElements(elements) ;
  addDofs(dofs) ;
}

void AlignmentElement::addDofs( const std::string& dofs )
{
  for(size_t ipar = 0; ipar<AlParameters::NumPars; ++ipar) 
    if( dofs.find( AlParameters::parName(ipar ) ) != std::string::npos )
      m_dofMask.setActive(ipar,true) ;
  m_lastDeltaDelta = AlParameters( m_dofMask ) ;
}

void AlignmentElement::setDofs( const std::string& dofs )
{
  for(size_t ipar = 0; ipar<AlParameters::NumPars; ++ipar) 
    m_dofMask.setActive(ipar,dofs.find( AlParameters::parName(ipar ) ) != std::string::npos ) ;
  m_lastDeltaDelta = AlParameters( m_dofMask ) ;
}

void AlignmentElement::addElements( const std::vector<const DetectorElement*>& newelements )
{
  // first check that the element hasn't been used yet
  std::vector<const DetectorElement*> elements ;
  elements.reserve( newelements.size() ) ;
  for( ElementContainer::const_iterator ielem = newelements.begin();
       ielem != newelements.end(); ++ielem) 
    if( std::find( m_elements.begin(), m_elements.end(), *ielem) == m_elements.end() )
      elements.push_back( *ielem ) ;
  
  std::for_each(elements.begin(), elements.end(),
                boost::lambda::bind(&AlignmentElement::validDetectorElement, this, boost::lambda::_1));
  for( ElementContainer::const_iterator ielem = elements.begin();
       ielem != elements.end(); ++ielem) 
    addToElementsInTree( *ielem, m_elementsInTree ) ;
  std::sort( m_elementsInTree.begin(), m_elementsInTree.end() ) ;

  m_elements.insert(m_elements.end(), elements.begin(), elements.end()) ;
  std::sort( m_elements.begin(), m_elements.end(),SortByName<DetectorElement>() ) ;
 
  // loop over all elements in tree and find the largest name they have in common
  m_basename = m_elements.front()->name() ;
  for( ElementContainer::const_iterator ielem =  m_elements.begin() ;
       ielem != m_elements.end(); ++ielem ) {
    size_t ipos(0) ;
    std::string thisname = (*ielem)->name() ;
    while( ipos < m_basename.size() && ipos < thisname.size() &&
	   m_basename[ipos] == thisname[ipos] ) ++ipos ;
    if( ipos < m_basename.size() ) m_basename.resize(ipos) ;
  }

  // initialize the frame and jacobian
  initAlignmentFrame();
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
  double xmin(1e10),xmax(-1e10),ymin(1e10),ymax(-1e10),zmin(1e10),zmax(-1e10) ;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd; ++i) {
    Gaudi::XYZPoint R =  toGlobalMatrixMinusDelta( **i ) * Gaudi::XYZPoint(0,0,0) ;
    xmin = std::min(xmin,R.x()) ; 
    xmax = std::max(xmax,R.x()) ;
    ymin = std::min(ymin,R.y()) ; 
    ymax = std::max(ymax,R.y()) ;
    zmin = std::min(zmin,R.z()) ; 
    zmax = std::max(zmax,R.z()) ;
  }
  Gaudi::XYZVector averageR(0.5*(xmin+xmax),0.5*(ymin+ymax),0.5*(zmin+zmax)) ;
  m_centerOfGravity = Gaudi::XYZPoint(0.0, 0.0, 0.0) + averageR;
  
  // Initialize the alignment frame transform. For now, we'll only do
  // something special for Velo modules. For all other modules the
  // alignment frame is still just a translation to the cog.
  if( m_useLocalFrame ) {
    if( m_elements.size() == 1 )
      // if there is only only element, takes its frame
      //m_alignmentFrame = m_elements.front()->geometry()->toGlobalMatrix() ;
      m_alignmentFrame = toGlobalMatrixMinusDelta( *m_elements.front() ) ;
    else {
      // if there is more than one element, we'll use the center of
      // gravity for the translation, and the frame of the first
      // element for the rotation.
      //ROOT::Math::Rotation3D rotation =  m_elements.front()->geometry()->toGlobalMatrix().Rotation() ;
      ROOT::Math::Rotation3D rotation = toGlobalMatrixMinusDelta( *m_elements.front() ).Rotation() ;

      m_alignmentFrame = Gaudi::Transform3D(averageR) * rotation ;
    }
  } else {
    m_alignmentFrame = Gaudi::Transform3D(averageR) ;
  }

  // We do something special for the 'Tracker': choose its pivot point
  // in the center of the magnet, such that we can align for magnetic
  // field rotations
  if( m_name == std::string("Tracker") ) {
    m_centerOfGravity = Gaudi::XYZPoint(0.0,0.0,5020.);
    m_alignmentFrame = Gaudi::Transform3D(Gaudi::XYZVector(0,0,5020.)) ;
  }
  
  // Set the Jacobian
  m_jacobian = AlParameters::jacobian( m_alignmentFrame ) ;
  
  // This is a simple check for velo R, just to make sure Jan separated them correctly:
  if( m_elements.size()==1 && dynamic_cast<const DeVeloRType*>(m_elements.front()) &&
      m_dofMask.isActive(AlParameters::Rz) ) {
    msg( MSG::ERROR ) << "PROBLEM: You are aligning Rz for velo R module with name "
		      << m_elements.front()->name() << endmsg ;
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
  return AlParameters(currentTotalDelta().transformParameters(),m_dofMask ) ;
}

AlParameters AlignmentElement::currentDelta() const {
  AlParameters d = currentDelta( alignmentFrame() ) ;
  return AlParameters( d.transformParameters(), m_lastDeltaDelta.transformCovariance() ) ;
}

AlParameters AlignmentElement::currentLocalDelta() const {
  AlParameters d = currentLocalDelta( alignmentFrame() ) ;
  return AlParameters( d.transformParameters(), m_lastDeltaDelta.transformCovariance() ) ;
}


AlParameters AlignmentElement::currentDelta(const Gaudi::Transform3D& frame) const 
{
  // cache the inverse of the frame
  Gaudi::Transform3D frameInv = frame.Inverse() ;
  double par[6] = {0,0,0,0,0,0} ;
  for (ElemIter ielem = m_elements.begin(); ielem != m_elements.end(); ++ielem) {
    // here I want just the _local_ delta.
    Gaudi::Transform3D localdelta = localDelta( **ielem ) ;
    // this one includes the localDelta, I think.
    const Gaudi::Transform3D& global        = ((*ielem)->geometry())->toGlobalMatrix();
    // so we subtract the delta to get the transformation matrix to global
    // Gaudi::Transform3D globalMinusDelta = global * localDelta.Inverse() ;
    // transform the local delta into the global frame
    // Gaudi::Transform3D localDeltaInGlobal = globalMinusDelta * localDelta * globalMinusDelta.Inverse() ;
    // which could be shorter written like this (really!)
    Gaudi::Transform3D localDeltaInGlobal = global * localdelta * global.Inverse() ;
    // now transform it into the alignment frame
    Gaudi::Transform3D alignDeltaMatrix = frameInv * localDeltaInGlobal * frame;
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

AlParameters AlignmentElement::currentLocalDelta(const Gaudi::Transform3D& frame) const
{
  // if our mother uses the same conditions as we, then we want to
  // subtract the delta of the mother. this is important for the
  // survey constraints.
  AlParameters rc = currentDelta(frame) ;
  if(m_mother &&
     std::includes( m_mother->m_elements.begin(), m_mother->m_elements.end(),
		    m_elements.begin(), m_elements.end(), SortByName<DetectorElement>() ) ) {
    AlParameters motherDelta = m_mother->currentActiveDelta() ;
    Gaudi::Transform3D frameInv = frame.Inverse() ;
    Gaudi::Transform3D localMotherDelta = frameInv * m_mother->alignmentFrame() * motherDelta.transform() * m_mother->alignmentFrame().Inverse() * frame ;
    // does the order matter here? I hope not ...
    Gaudi::Transform3D localCorrectedDelta = localMotherDelta.Inverse() * rc.transform() ;
    rc = AlParameters( localCorrectedDelta ) ;
  }
  return rc ;
}

AlParameters AlignmentElement::currentActiveDelta() const 
{
  // this one has correct covariance _and_ weight matrix.
  AlParameters rc = m_lastDeltaDelta ;
  rc.setParameters( currentDelta().transformParameters() ) ; 
  return rc ;
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
  StatusCode sc = StatusCode::SUCCESS ;
  for (ElemIter i = m_elements.begin(), iEnd = m_elements.end(); i != iEnd && sc.isSuccess() ; ++i) {
    /// make sure this element has an alignment condition
    if( !(*i)->geometry()->alignmentCondition() ) {
      msg(MSG::FATAL) << "Updating element without alignment condition: " << (*i)->name() << endmsg ;
      sc = StatusCode::FAILURE ;
    } else {
      /// Transform global delta matrix to new local delta matrix (takes current local delta matrix into account!!)
      const Gaudi::Transform3D localDeltaMatrix  = DetDesc::localDeltaMatrix((*i), globalDeltaMatrix);
      /// Update position of detector element with new local delta matrix
      sc = const_cast<IGeometryInfo*>((*i)->geometry())->ownToOffNominalMatrix(localDeltaMatrix);
      if (sc.isFailure()) {
	msg(MSG::FATAL) << "Failed to update local delta matrix of detector element " + (*i)->name() << endmsg;
      }
    }
  }
  // Update the transform of the alignment frame
  initAlignmentFrame() ;
  m_lastDeltaDelta = parameters ;
  return sc;
}

std::ostream& AlignmentElement::fillStream(std::ostream& lhs) const {
  const std::vector<double> t = deltaTranslations();
  const std::vector<double> r = deltaRotations();
  static const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");

  std::vector<double> aft(3,0.0), afr(3,0.0);
  DetDesc::getZYXTransformParameters(alignmentFrame(),aft,afr);//, it->pivot());

  lhs << std::left << std::setw(80u) << std::setfill('*') << "" << std::endl;
  lhs << "* Alignable: " << name() << "\n" 
      << "* Element  : " << description() << std::endl
      << "* Num elements in tree: " << m_elementsInTree.size() << std::endl 
      << "* Basename : " << basename() << std::endl
      << "* Index    : " << index() << "\n"
      << "* dPosXYZ  : " << Gaudi::XYZPoint(t[0], t[1], t[2]) << "\n"
      << "* dRotXYZ  : " << Gaudi::XYZPoint(r[0], r[1], r[2]) << "\n"
      << "* CogXYZ   : " << centerOfGravity() << "\n"
      << "* Alignment frame: " <<  Gaudi::XYZPoint(aft[0], aft[1], aft[2]) << " " << Gaudi::XYZPoint(afr[0], afr[1], afr[2]) << std::endl
      << "* First elem nominal: " 
      << m_elements.front()->geometry()->toGlobalMatrixNominal()* Gaudi::XYZPoint(0,0,0) << std::endl
      << "* First elem aligned: " 
      << m_elements.front()->geometry()->toGlobalMatrix()*Gaudi::XYZPoint(0,0,0) << std::endl
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

std::vector<int> AlignmentElement::redundantDofs() const
{
  // first check that all elements of this alignable also appear as its daughters
  std::vector<int> rc ;
  ElementContainer  dauelements ;
  for( DaughterContainer::const_iterator idau = daughters().begin() ;
       idau != daughters().end() ; ++idau) 
    dauelements.insert( dauelements.end(), (*idau)->detelements().begin(), (*idau)->detelements().end()) ;
  std::sort( dauelements.begin(), dauelements.end() ) ;
  ElementContainer myelements = detelements() ;
  std::sort( myelements.begin(), myelements.end()) ;
  if( dauelements == myelements ) 
    for(int i=0; i<6; ++i) 
      if(dofMask().isActive(i)) {
	bool foundall(true) ;
	for( DaughterContainer::const_iterator idau = daughters().begin() ;
	     idau != daughters().end()&&foundall ; ++idau)
	  foundall = foundall && (*idau)->dofMask().isActive(i) ;
	if(foundall) rc.push_back(i) ;
      }
  return rc ;
}

