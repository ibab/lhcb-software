// $Id: AlignmentCondition.h,v 1.2 2005-06-29 13:46:40 jpalac Exp $
#ifndef DETDESC_ALIGNMENTCONDITION_H 
#define DETDESC_ALIGNMENTCONDITION_H 1

// Include files
#include "DetDesc/Condition.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/CLIDAlignmentCondition.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
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
                     const std::vector<double>& rotation );
  
  /// Constructor
  AlignmentCondition (const ITime& since, const ITime& till);
  /// Copy constructor 
  AlignmentCondition (Condition& obj);

  virtual ~AlignmentCondition( ); ///< Destructor

  virtual StatusCode initialize();

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 
  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_AlignmentCondition; };
  
  inline const HepTransform3D& matrix() const
  {
    return m_matrix;
  }

  inline const HepTransform3D& matrixInv() const
  {
    return m_matrixInv;
  }

  inline void matrix(const HepTransform3D& newMatrix) 
  {
    m_matrix=newMatrix;
    m_matrixInv=m_matrix.inverse();
  }

  inline StatusCode setTransformation( const std::vector<double>& translation,
                                       const std::vector<double>& rotation) 
  {
    loadParams(translation, rotation);
    return makeMatrices();
  }
  

protected:

  IMessageSvc*       msgSvc  () const;

private:

  inline void loadParams(const std::vector<double>& translation,
                         const std::vector<double>& rotation     ) 
  {
    this->addParam(m_translationString, translation);
    this->addParam(m_rotationString, rotation);
  }
  

  StatusCode makeMatrices();
  
  const HepTransform3D* XYZRotation(const std::vector<double>& coefficients) const;
  const HepTranslate3D* XYZTranslation(const std::vector<double>& coefficients) const;
private:

  DetDesc::Services* m_services;
  HepTransform3D m_transMatrix;
  HepTransform3D m_rotMatrix;
  HepTransform3D m_matrix;
  HepTransform3D m_matrixInv;

  const std::string m_translationString;
  const std::string m_rotationString;

};
#endif // DETDESC_ALIGNMENTCONDITION_H
