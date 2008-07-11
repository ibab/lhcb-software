// $Id: VeloAlignCond.cpp,v 1.1 2008-07-11 16:35:58 marcocle Exp $
#include "VeloDet/VeloAlignCond.h"

#include "DetDesc/3DTransformationFunctions.h"

#include "GaudiKernel/IUpdateManagerSvc.h"
#include "GaudiKernel/IRegistry.h"

#include <boost/regex.hpp>

//-----------------------------------------------------------------------------
// Costructor of VeloAlignCond::PositionPaths
//-----------------------------------------------------------------------------
// Converts two strings in the format "path/in/TS[param]" to two pairs of string
VeloAlignCond::PositionPaths::PositionPaths(const std::string &_x,const std::string &_y){
  // prepare the reg. exp. parser
  const std::string rgxp("^(.*)\\[(.*)\\]$"); 
  boost::regex e(rgxp);
  boost::smatch what;
  // check X
  if ( ! _x.empty() ) {
    if (boost::regex_match(_x,what,e)){
      x.first = what[1];
      x.second = what[2];
    } else {
      throw GaudiException
        ("Bad syntax for location of stepping motor reading (x): " + _x,
         "VeloAlignCond::PositionPaths" , StatusCode::FAILURE);
    }
  } else { // set values to empty if the string is empty
    x.first = "";
    x.second = "";
  }
  // check Y
  if ( ! _y.empty() ) {
    if (boost::regex_match(_y,what,e)){
      y.first = what[1];
      y.second = what[2];
    } else {
      throw GaudiException
        ("Bad syntax for location of stepping motor reading (y): " + _y,
         "VeloAlignCond::PositionPaths" , StatusCode::FAILURE);
    }
  } else { // set values to empty if the string is empty
    y.first = "";
    y.second = "";
  }
}

//-----------------------------------------------------------------------------
// Default costructor
//-----------------------------------------------------------------------------
VeloAlignCond::VeloAlignCond():
  AlignmentCondition(),
  m_paths(),
  m_xOffCond(0),m_yOffCond(0),
  m_firstInit(true) {}

//-----------------------------------------------------------------------------
// Main costructor
//-----------------------------------------------------------------------------
VeloAlignCond::VeloAlignCond(const std::vector<double>& translation,
                             const std::vector<double>& rotation,
                             const std::vector<double>& pivot,
                             const std::string &xOffsetLoc,
                             const std::string &yOffsetLoc):
  AlignmentCondition(translation,rotation,pivot),
  m_paths(),
  m_xOffCond(0),m_yOffCond(0),
  m_firstInit(true)
{
  if ( !xOffsetLoc.empty() ) this->addParam("XOffset",xOffsetLoc);
  if ( !yOffsetLoc.empty() ) this->addParam("YOffset",yOffsetLoc);
}
//=============================================================================
// Destructor
//=============================================================================
VeloAlignCond::~VeloAlignCond(){
  // De-register from the UMS (if needed)
  if ( (! m_paths.x.first.empty()) || (! m_paths.y.first.empty()) ) {
    m_services->updMgrSvc()->unregister(this);
  }
}
//=============================================================================
// Initialize the position path data member (after instantiation)
//=============================================================================
void VeloAlignCond::initializePosPath() {
  // Prepare new PositionPaths object
  std::string xOff, yOff;
  if(exists("XOffset")) xOff = param<std::string>("XOffset");
  if(exists("YOffset")) yOff = param<std::string>("YOffset");
  m_paths = PositionPaths(xOff,yOff);

  // Register the condition to the UMS for the positions of the stepping motors
  if ( ! m_paths.x.first.empty() ) {
    m_services->updMgrSvc()->registerCondition(this, m_paths.x.first,
        &VeloAlignCond::makeMatrices,
        m_xOffCond);
  }
  if ( ! m_paths.y.first.empty() ) {
    m_services->updMgrSvc()->registerCondition(this, m_paths.y.first,
        &VeloAlignCond::makeMatrices,
        m_yOffCond);
  }
}
//=============================================================================
// Update the position path data member (after update)
//=============================================================================
void VeloAlignCond::updatePosPath() {
  
  std::string xOff, yOff;
  if(exists("XOffset")) xOff = param<std::string>("XOffset");
  if(exists("YOffset")) yOff = param<std::string>("YOffset");
  PositionPaths paths(xOff,yOff);
  
  // check if it has changed
  if ( m_paths != paths ) {
    MsgStream log(this->msgSvc(), "VeloAlignCond");
    if (this->registry()) {
      log << MSG::WARNING << "In condition " << this->registry()->identifier() << ":" << endmsg;
    }
    log << MSG::WARNING << "The location of the X and Y offsets has changed, but they will be ignored." << endmsg;
  }
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloAlignCond::initialize() {
  
  if (m_firstInit) {
    // This should not be done only on the first load of the condition
    initializePosPath();
    m_firstInit = false;
  } else {
    updatePosPath();
  }
  if ( m_paths.x.first.empty() && m_paths.y.first.empty() ) {
    // the UMS doesn't know about us, so we have to call makeMatrices ourselves
    return makeMatrices();
  }
  // Load the conditions we depend on and trigger the function to make the matrices
  return m_services->updMgrSvc()->update(this);
}

#include <numeric>
namespace {
  /// helper class used by correctOffset to calculate a polynomial function as
  /// $(\ldots{}((c_n x + c_{n-1}) x + c_{n-2})\ldots{}) x + c_0$
  struct poly: public std::binary_function<double,double,double>{
    poly(double _x):x(_x){}
    inline double operator()(first_argument_type a, second_argument_type b){
      return a*x+b;
    }
    double x;
  };
  /// Correct the offset using a polinomial function that has the element of the
  /// vector coeffs as coefficients.
  /// The result is equivalent to:
  ///    $\sum_{i=0}^{n} c_i x^i$
  inline
  double correctOffset(double offset, const std::vector<double> &coeffs) {
    return std::accumulate(coeffs.rbegin()+1,coeffs.rend(),
                           *(coeffs.rbegin()),poly(offset));
  }
}

//=============================================================================
StatusCode VeloAlignCond::makeMatrices() 
{
  MsgStream log(this->msgSvc(), "VeloAlignCond");
  log << MSG::VERBOSE << "Making transformation matrix" << endmsg;

  std::vector<double> translations = paramAsDoubleVect (m_translationString);
  std::vector<double> rotations    = paramAsDoubleVect (m_rotationString);
  std::vector<double> pivot = (exists(m_pivotString) ) ? 
      paramAsDoubleVect(m_pivotString) : std::vector<double>(3, 0);

  if (translations.size()==3  && rotations.size()==3 && pivot.size()==3) {

    // Apply the offsets given by the stepping motors
    if ( m_xOffCond ) {
      double offset = m_xOffCond->param<double>(m_paths.x.second);
      if (exists("XOffsetCoeffs")) {
        offset = correctOffset(offset,param<std::vector<double> >("XOffsetCoeffs"));
      }
      translations[0] += offset;
    }
    if ( m_yOffCond ) {
      double offset = m_yOffCond->param<double>(m_paths.y.second);
      if (exists("YOffsetCoeffs")) {
        offset = correctOffset(offset,param<std::vector<double> >("YOffsetCoeffs"));
      }
      translations[1] += offset;
    }

    m_matrixInv = DetDesc::localToGlobalTransformation(translations,
                                                       rotations,
                                                       pivot);

    m_matrix = m_matrixInv.Inverse();

    return StatusCode::SUCCESS;
  } else {
    log << MSG::ERROR << "Translations vector has funny size: "
    << translations.size() << ". Assigning identity matrices" << endmsg;
    m_matrixInv=Gaudi::Transform3D();
    m_matrix=m_matrixInv;
    return StatusCode::FAILURE;
  }

}
