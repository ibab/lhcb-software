// $Id: AlignmentCondition.cpp,v 1.13 2007-05-31 13:55:34 jpalac Exp $
// Include files
#include <algorithm>

#include "DetDesc/AlignmentCondition.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Vector3DTypes.h"
///@todo put RotationXYZ.h in GaudiKernel when it is released in MathCore.
#include "RotationZYX.h"
#include "3DConversions.h"
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
void AlignmentCondition::matrix(const Gaudi::Transform3D& newMatrix) 
{
  m_matrix=newMatrix.Inverse();
  m_matrixInv=m_matrix;
  updateParams();
}
//=============================================================================
StatusCode 
AlignmentCondition::setTransformation( const std::vector<double>& translation,
                                       const std::vector<double>& rotation,
                                       const std::vector<double>& pivot) 
{
  loadParams(translation, rotation, pivot);
  return makeMatrices();
}
//=============================================================================
const Gaudi::Transform3D AlignmentCondition::XYZTranslation(const std::vector<double>& coefficients) const
{
  // Calculate the inverse translation directly.
  Gaudi::TranslationXYZ trans = (coefficients.size()==3) ? 
    Gaudi::TranslationXYZ(-1.*coefficients[0],
                          -1.*coefficients[1],
                          -1.*coefficients[2]) :
    Gaudi::TranslationXYZ();
  return Gaudi::Transform3D( trans );
}
//=============================================================================
const Gaudi::Transform3D AlignmentCondition::XYZRotation(const std::vector<double>& coefficients) const
{
  if (coefficients.size()!=3) return Gaudi::Transform3D();
  // calculate the inverse matrix directly. This means each angle *=-1 and
  // swap Z for X.
  Gaudi::RotationZYX rotZYX(-1.*coefficients[0],
                            -1.*coefficients[1],
                            -1.*coefficients[2]);

//   Gaudi::Rotation3D rot = Gaudi::RotationZ(-1.*coefficients[2])*
//     Gaudi::RotationY(-1.*coefficients[1]) *
//     Gaudi::RotationX(-1.*coefficients[0]);
  Gaudi::Rotation3D tmp;
  Gaudi::Math::convert(tmp, rotZYX);
  return Gaudi::Transform3D(tmp);
  
}
//=============================================================================
StatusCode AlignmentCondition::makeMatrices() 
{
  MsgStream log(msgSvc(), "AlignmentCondition");
  log << MSG::VERBOSE << "Making transformation matrix" << endmsg;
  
  std::vector<double> translations = paramAsDoubleVect (m_translationString);
  std::vector<double> rotations    = paramAsDoubleVect (m_rotationString);
  std::vector<double> pivot = (exists(m_pivotString) ) ? 
    paramAsDoubleVect(m_pivotString) : std::vector<double>(3, 0);

  if (translations.size()==3  && rotations.size()==3 && pivot.size()==3) {

    m_matrix = ( XYZTranslation( pivot ).Inverse() 
                 * XYZRotation( rotations ) 
                 * XYZTranslation( pivot )         ) 
      * XYZTranslation( translations );

    m_matrixInv = m_matrix.Inverse();
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
void AlignmentCondition::updateParams() 
{
  std::vector<double> newTrans(3,0);
  std::vector<double> newRot(3,0);
  const std::vector<double> pivot = (exists(m_pivotString) ) ? 
    paramAsDoubleVect(m_pivotString) : std::vector<double>(3, 0);
  recalculateParameters( m_matrix, newTrans, newRot, pivot );
  
  loadParams( newTrans, newRot, pivot );

  return;
  
}
//=============================================================================
void AlignmentCondition::recalculateParameters(const Gaudi::Transform3D& newMatrix,
                                               std::vector<double>& translation,
                                               std::vector<double>& rotation,
                                               const std::vector<double>& pivot) 
{

  using namespace Gaudi;

  Rotation3D newRot;
  TranslationXYZ newTrans;
  newMatrix.GetDecomposition(newRot, newTrans);
  const Transform3D pivotTrans = XYZTranslation(pivot);
  
  // Take the pivot out of the rotation.
  const Transform3D newRotPart = pivotTrans * Transform3D(newRot) * pivotTrans.Inverse();
  TranslationXYZ tmp;
  newRotPart.GetDecomposition(newRot, tmp);

  getRotationParameters(newRot, rotation);

  getTranslationParameters(newTrans, translation);

  return;
}
//=============================================================================
void AlignmentCondition::getRotationParameters(const Gaudi::Rotation3D& rot,
                                               std::vector<double>& newRot)
{
  using namespace Gaudi;

  RotationZYX invRotation(rot);
  // rot is the inverse matrix, so angles are swapped.
  invRotation.GetComponents(newRot[0], newRot[1], newRot[2]);
  // and angles are in opposite direction
  // why does this not work??
  // std::transform( newRot.begin(), newRot.end(), std::negate<double>() );
  newRot[0] *= -1;
  newRot[1] *= -1;
  newRot[2] *= -1;
//   std::vector<double> components(12,0.);
  
//   rot.GetComponents(components.begin(), components.end());
  
//   const double sinY     = -1.*components[Transform3D::kZX];
//   const double Y = -1*std::asin(sinY);
//   if (std::fabs(sinY)-1. < std::numeric_limits<double>::epsilon() ) {
//     // One degree of freedom lost, set X transformation to 0.
//     newRot[0] = 0.;
//     newRot[2] = std::atan2(components[Transform3D::kYZ],
//                            components[Transform3D::kYY]);
//   } else {
//     const double cosYsinX =     components[Transform3D::kZY];
//     const double cosZcosY =     components[Transform3D::kXX];
//     newRot[0]= -1*std::asin(cosYsinX)/std::cos(Y);
//     newRot[2] = -1*std::acos(cosZcosY)/std::cos(Y);
//   }
  
//   newRot[1] = Y;

  return;
}
//=============================================================================
void AlignmentCondition::getTranslationParameters(const Gaudi::TranslationXYZ& newTrans,
                                                  std::vector<double>& translation)
{

  translation[0]=newTrans.X();
  translation[1]=newTrans.Y();
  translation[2]=newTrans.Z();
  
  return;
}
//=============================================================================
IMessageSvc* AlignmentCondition::msgSvc() const {
  return m_services->msgSvc();
}
//=============================================================================
