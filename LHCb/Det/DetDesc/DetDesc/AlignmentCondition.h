// $Id: AlignmentCondition.h,v 1.19 2010-01-11 15:57:16 wouter Exp $
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
   * Return the nominal -> off-nominal 3D transformation.
   */
  inline const Gaudi::Transform3D& offNominalMatrix() const
  {
    return m_matrixInv;
  }
  /**
   * Return the off-nominal -> nominal 3D transformation.
   */
  inline const Gaudi::Transform3D& toNominalMatrix() const
  {
    return m_matrix;
  }

  /**
   * Set a nominal -> off-nomunal 3D transformation starting directly 
   * from from a Transform3D
   * @param newMatrix new full transformation object describing nominal to 
   *        off-nominal transformation in the frame of the detector element..
   */
  void offNominalMatrix(const Gaudi::Transform3D& newMatrix);

  /**
   * Set a new nominal -> off-nominal 3D transformation starting 
   * from the basic set of parameters describing a 
   * rotation about the Z, Y' and X" axes in that order.
   * 
   * @param translation vector containing X,Y,Z translation parameters.
   * @param rotation    vector containing rotation angles about Z,Y,X.
   * @param pivot       vector containing X,Y,Z pivot point for rotation.
   * @return            StatusCode
   */
  StatusCode setOffNominalTransformation( const std::vector<double>& translation,
                                          const std::vector<double>& rotation,
                                          const std::vector<double>& pivot);

  /**
   * Set the pivot point. Update pivot point used when writing
   * conditions to xml. We actually don't use 'non-trivial' pivot
   * points anymore.
   * @param pivot       pivot point
   */
  void setPivotPoint( const Gaudi::XYZPoint& pivot ) ;
  
  std::ostream &fillStream(std::ostream &s) const;
  
protected:

  IMessageSvc*       msgSvc  () const;

  void loadParams(const std::vector<double>& translation,
		  const std::vector<double>& rotation,
		  const std::vector<double>& pivot);
  
  virtual StatusCode makeMatrices();

  virtual void updateParams(const Gaudi::Transform3D& matrixInv);

  DetDesc::Services* m_services;

  Gaudi::Transform3D m_matrix; /// The misaligned to aligned transformation matrix.
  Gaudi::Transform3D m_matrixInv; /// The aligned to misaligned transformation matrix.

  const std::string m_translationString;
  const std::string m_rotationString;
  const std::string m_pivotString;

};
#endif // DETDESC_ALIGNMENTCONDITION_H
