// $Id: AlignmentCondition.h,v 1.13 2007-06-13 16:11:46 jpalac Exp $
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
                     std::vector<double>(3, 0.) ) ;
  
  virtual ~AlignmentCondition( ); ///< Destructor

  virtual StatusCode initialize();

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 
  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_AlignmentCondition; };
  /**
   * Return the misaligned -> nominal 3D transformation matrix.
   */
  inline const Gaudi::Transform3D& matrix() const
  {
    return m_matrix;
  }
  /**
   * Return the nominal -> misaligned 3D transformation matrix.
   */
  inline const Gaudi::Transform3D& matrixInv() const
  {
    return m_matrixInv;
  }

  /**
   * Set a new 3D transformation starting directly from from a Transform3D
   * @param newMatrix new full transformation object describing aligned to misalinged 
   * transformation in the frame of the detector element..
   */
  void matrix(const Gaudi::Transform3D& newMatrix);

  /**
   * Set a new 3D transformation starting from the basic set of parameters
   * describing a rotation about Z, Y and X axes in that order.
   * 
   * @param translation vector containing X,Y,Z translation parameters.
   * @param rotation    vector containing rotation angles about Z,Y,X.
   * @param pivot       vector containing X,Y,Z pivot point for rotation.
   * @return            StatusCode
   */
  StatusCode setTransformation( const std::vector<double>& translation,
                                const std::vector<double>& rotation,
                                const std::vector<double>& pivot);

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

private:

  void updateParams();

  DetDesc::Services* m_services;

  Gaudi::Transform3D m_matrix; /// The misaligned to aligned transformation matrix.
  Gaudi::Transform3D m_matrixInv; /// The aligned to misaligned transformation matrix.

  const std::string m_translationString;
  const std::string m_rotationString;
  const std::string m_pivotString;

};
#endif // DETDESC_ALIGNMENTCONDITION_H
