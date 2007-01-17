// $Id: AlignmentCondition.h,v 1.9 2007-01-17 12:10:07 cattanem Exp $
#ifndef DETDESC_ALIGNMENTCONDITION_H 
#define DETDESC_ALIGNMENTCONDITION_H 1

// Include files
#include "DetDesc/Condition.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/CLIDAlignmentCondition.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Transform3DTypes.h"
/** @class AlignmentCondition AlignmentCondition.h DetDesc/AlignmentCondition.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2005-04-12
 */

class MsgStream;
class IMessageSvc;

class AlignmentCondition : public Condition {

public: 
  /// Standard constructor
  AlignmentCondition( );
  ///
  AlignmentCondition(const std::vector<double>& translation,
                     const std::vector<double>& rotation,
                     const std::vector<double>& pivot =
                     std::vector<double>(3) ) ;
  
  virtual ~AlignmentCondition( ); ///< Destructor

  virtual StatusCode initialize();

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 
  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_AlignmentCondition; };
  
  inline const Gaudi::Transform3D& matrix() const
  {
    return m_matrix;
  }

  inline const Gaudi::Transform3D& matrixInv() const
  {
    return m_matrixInv;
  }

  inline void matrix(const Gaudi::Transform3D& newMatrix) 
  {
    m_matrix=newMatrix;
    m_matrixInv=m_matrix.Inverse();
  }

  inline StatusCode setTransformation( const std::vector<double>& translation,
                                       const std::vector<double>& rotation,
                                       const std::vector<double>& pivot ) 
  {
    loadParams(translation, rotation, pivot);
    return makeMatrices();
  }
  

protected:

  IMessageSvc*       msgSvc  () const;

private:

  inline void loadParams(const std::vector<double>& translation,
                         const std::vector<double>& rotation,
                         const std::vector<double>& pivot) 
  {
    this->addParam(m_translationString, translation );
    this->addParam(m_rotationString,    rotation    );
    this->addParam(m_pivotString,       pivot       );
  }
  

  StatusCode makeMatrices();
  
  const Gaudi::Transform3D XYZRotation(const std::vector<double>& coefficients) const;
  const Gaudi::Transform3D XYZTranslation(const std::vector<double>& coefficients) const;
private:

  DetDesc::Services* m_services;
  Gaudi::Transform3D m_matrix;
  Gaudi::Transform3D m_matrixInv;

  const std::string m_translationString;
  const std::string m_rotationString;
  const std::string m_pivotString;

};
#endif // DETDESC_ALIGNMENTCONDITION_H
