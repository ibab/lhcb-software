// $Id: AlignmentCondition.cpp,v 1.2 2005-06-20 12:23:43 jpalac Exp $
// Include files

#include "DetDesc/AlignmentCondition.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"
//-----------------------------------------------------------------------------
// Implementation file for class : AlignmentCondition
//
// 2005-04-12 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Default constructor
//=============================================================================
AlignmentCondition::AlignmentCondition(  ) : 
  Condition(),
  m_services(DetDesc::services())
{
  //  m_services = DetDesc::services();
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing default AlignmentCondition, classID " 
      << classID()
      << endmsg;
}
//=============================================================================
// Constructor
//=============================================================================
AlignmentCondition::AlignmentCondition(const ITime& since, const ITime& till )
  :
  Condition (since, till),
  m_services(DetDesc::services())
{
  //  m_services=DetDesc::services();
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing AlignmentCondition classID " << classID()
      << endmsg;
  
}

//=============================================================================
// Destructor
//=============================================================================
AlignmentCondition::~AlignmentCondition() {}; 

//=============================================================================
/// initialize
StatusCode AlignmentCondition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  makeMatrices();
  
  return StatusCode::SUCCESS;
}

//=============================================================================
const HepTranslate3D* AlignmentCondition::XYZTranslation(const std::vector<double>& coefficients) const
{
  return (coefficients.size()==3) ? new HepTranslate3D(coefficients[0],
                                                       coefficients[1],
                                                       coefficients[2]) :
    new HepTranslate3D();
}
//=============================================================================
const HepTransform3D* AlignmentCondition::XYZRotation(const std::vector<double>& coefficients) const
{
  if (coefficients.size()!=3) return new HepTransform3D();
  
  HepTransform3D* result = new HepRotateX3D(coefficients[0]);
  *result = *result * HepRotateY3D(coefficients[1]);
  *result = *result * HepRotateZ3D(coefficients[2]);
  return result;
  
}
//=============================================================================
void AlignmentCondition::makeMatrices() 
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Making transformation matrix" << endmsg;
  
//   std::vector<double> translations =  userParameterVectorAsDouble("dPosXYZ");
//   std::vector<double> rotations =  userParameterVectorAsDouble("dRotXYZ");
  std::vector<double> translations = paramAsDoubleVect ("dPosXYZ");
  std::vector<double> rotations    = paramAsDoubleVect ("dRotXYZ");
  if (translations.size()==3) {
    log <<  MSG::VERBOSE << "Translations " 
        << " X " << translations[0]
        << " Y " << translations[1]
        << " z " << translations[2]
        << endmsg;
  } else {
    log << MSG::VERBOSE << "Translations vector has funny size: "
        << translations.size() << endmsg;
    
  }
  
   
  m_matrixInv =  (*XYZTranslation(translations)) * (*XYZRotation(rotations));
  m_matrix = m_matrixInv.inverse();
}

//=============================================================================
IMessageSvc* AlignmentCondition::msgSvc() const {
  return m_services->msgSvc();
}
//=============================================================================
