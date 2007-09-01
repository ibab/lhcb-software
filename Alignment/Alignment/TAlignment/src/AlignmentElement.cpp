// $Id: AlignmentElement.cpp,v 1.1 2007-09-01 17:34:43 janos Exp $
// Include files

// from STD
#include <iomanip>

// from DetDesc
#include "DetDesc/ParamException.h"
#include "DetDesc/GeometryInfoException.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"

// from BOOST
#include "boost/regex.hpp"
#include "boost/algorithm/string_regex.hpp"

// local
#include "AlignmentElement.h"

AlignmentElement::AlignmentElement(const DetectorElement* element, const unsigned int index) : 
  m_element           ( element                                   ),
  m_index             ( index                                     ) 
{
  
  if (0 == element) {
    throw GeometryInfoException("DetectorElement* points to NULL!");
  }
  
  if (0 == element->geometry()->alignmentCondition()) {
    throw GeometryInfoException("AlignmentCondition* points to NULL!");
  }

  grabPivotPoint();
}

const std::string AlignmentElement::name() const {

  return m_element->name(); 
}

void AlignmentElement::grabPivotPoint() {

  try { /// If pivot is defined for this detector elememt get it
    std::cout << "Getting pivot point from conditions database" << std::endl;
    std::vector<double> pivot = m_element->geometry()->alignmentCondition()->paramVect<double>("PivotXYZ");
    m_pivot = Gaudi::XYZPoint(pivot[0], pivot[1], pivot[2]);
    std::cout << "Pivot is " << m_pivot << std::endl;
  } catch (ParamException& p) { /// else use default, i.e. global centre
    m_pivot = m_element->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0));
    std::cout << "Using default pivot point " <<  m_pivot << std::endl;
  } 
}

std::ostream& AlignmentElement::fillStream(std::ostream& lhs) const {
  const std::string s = boost::algorithm::erase_all_regex_copy(name(), boost::regex("/dd/Structure/LHCb/.*Region"));
  const std::vector<double>& t = m_element->geometry()->alignmentCondition()->paramVect<double>("dPosXYZ");
  const std::vector<double>& r = m_element->geometry()->alignmentCondition()->paramVect<double>("dRotXYZ");
  lhs << std::endl;
  lhs << std::left << std::setw(s.size()+30u) << std::setfill('*') << "" << std::endl;
  lhs << "* Element  : " << s << "\n"
      << "* Index    : " << index() << "\n"
      << "* dPosXYZ  : " << Gaudi::XYZPoint(t[0], t[1], t[2]) << "\n" 
      << "* dRotXYZ  : " << Gaudi::XYZPoint(r[0], r[1], r[2]) << "\n" 
      << "* pivotXYZ : " << pivotXYZPoint() << std::endl;
  lhs << std::left << std::setw(s.size()+30u) << std::setfill('*') << "" << std::endl;
  
  return lhs;
}
