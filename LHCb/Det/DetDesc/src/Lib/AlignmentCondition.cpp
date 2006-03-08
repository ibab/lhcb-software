// $Id: AlignmentCondition.cpp,v 1.11 2006-03-08 11:05:25 jpalac Exp $
// Include files
#include <algorithm>

#include "DetDesc/AlignmentCondition.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
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
  m_services(DetDesc::services()),
  m_translationString("dPosXYZ"),
  m_rotationString("dRotXYZ"),
  m_pivotString("pivotXYZ")
{
}
//=============================================================================
AlignmentCondition::AlignmentCondition(const std::vector<double>& translation,
                                       const std::vector<double>& rotation,
                                       const std::vector<double>& pivot) : 
  Condition(),
  m_services(DetDesc::services()),
  m_translationString("dPosXYZ"),
  m_rotationString("dRotXYZ"),
  m_pivotString("pivotXYZ")
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Constructing AlignmentCondition from transformation parameters. classID " 
      << classID()
      << endmsg;

  setTransformation(translation, rotation, pivot);

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
  return makeMatrices();
}

//=============================================================================
const Gaudi::Transform3D AlignmentCondition::XYZTranslation(const std::vector<double>& coefficients) const
{
  Gaudi::TranslationXYZ trans = (coefficients.size()==3) ? 
    Gaudi::TranslationXYZ(coefficients[0], coefficients[1], coefficients[2]) :
    Gaudi::TranslationXYZ();
  return Gaudi::Transform3D( trans );
}
//=============================================================================
const Gaudi::Transform3D AlignmentCondition::XYZRotation(const std::vector<double>& coefficients) const
{
  if (coefficients.size()!=3) return Gaudi::Transform3D();

  Gaudi::Rotation3D rot = Gaudi::RotationX(coefficients[0])*
    Gaudi::RotationY(coefficients[1]) *
    Gaudi::RotationZ(coefficients[2]);
  return Gaudi::Transform3D(rot);
  
}
//=============================================================================
StatusCode AlignmentCondition::makeMatrices() 
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Making transformation matrix" << endmsg;
  
  std::vector<double> translations = paramAsDoubleVect (m_translationString);
  std::vector<double> rotations    = paramAsDoubleVect (m_rotationString);
  std::vector<double> pivot = (exists(m_pivotString) ) ? 
    paramAsDoubleVect(m_pivotString) : std::vector<double>(3);

  std::transform(pivot.begin(), pivot.end(), 
                 pivot.begin(), std::negate<double>());
  
  if (translations.size()==3  && rotations.size()==3 && pivot.size()==3) {

    m_matrixInv = XYZTranslation( translations ) *
      (   XYZTranslation( pivot ).Inverse() *
          ( XYZRotation( rotations ) * XYZTranslation( pivot ) )  );

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
//=============================================================================
IMessageSvc* AlignmentCondition::msgSvc() const {
  return m_services->msgSvc();
}
//=============================================================================
