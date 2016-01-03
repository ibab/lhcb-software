// $Id: VeloAlignCond.h,v 1.4 2009-07-27 10:36:15 jonrob Exp $
#ifndef VELODET_VELOALIGNCOND_H
#define VELODET_VELOALIGNCOND_H 1


// Include files
#include "GaudiKernel/MsgStream.h"
#include "DetDesc/AlignmentCondition.h"
#include "VeloDet/CLIDVeloAlignCond.h"
/** @class VeloAlignCond VeloAlignCond.h VeloDet/VeloAlignCond.h
 *
 *
 *  @author Marco Clemencic
 *  @date   2008-06-23
 */

class IMessageSvc;

#ifdef __INTEL_COMPILER        // Disable ICC remark from ROOT GenVector classes
  #pragma warning(disable:1572) // floating-point equality and inequality comparisons are unreliable
#pragma warning(push)
#endif

class VeloAlignCond : public AlignmentCondition {

public:
  /**
   * Helper class to handle the two strings indicating the location in the TS
   * of the stepping motors readings.
   */
  struct PositionPaths {
    typedef std::pair<std::string,std::string> ValueType;
    PositionPaths(): x(), y() {}
    PositionPaths(const std::string &_x,const std::string &_y);
    ValueType x,y;
    bool operator!=(const PositionPaths &rhs) const {
      return x != rhs.x || y != rhs.y;
    }
  };

  /// Standard constructor
  VeloAlignCond( );
  /// Constructor
  VeloAlignCond(const std::vector<double>& translation,
                const std::vector<double>& rotation,
                const std::vector<double>& pivot = std::vector<double>(3, 0.),
                const std::string &xOffsetLoc = "",
                const std::string &yOffsetLoc = "") ;

  virtual ~VeloAlignCond(); ///< Destructor

  /// Initialize the internal structures (transformation matrices)
  virtual StatusCode initialize();

  using AlignmentCondition::update;
  /// Update internal data from another condition.
  virtual void update(ValidDataObject& obj);

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); }
  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_VeloAlignCond; };

  /// Creates the transformation from the motion system
  Gaudi::Transform3D motionSystemTransform() const ;

protected:

  /// @see AlignmentCondition
  virtual StatusCode makeMatrices();

  /// @see AlignmentCondition
  virtual void updateParams(const Gaudi::Transform3D& matrixInv);

  /// Commodity function to register to the UpdateMgrSvc for an offset condition.
  virtual void i_registerOffsetCond(const PositionPaths::ValueType &offsetCond, Condition *&cond,
                                    PositionPaths::ValueType &oldOffsetCond);

  /// Variable to store the location and parameters of the offset conditions.
  PositionPaths m_paths;

  /// Pointer to the condition used for the offset along x (MotionSystem).
  Condition *m_xOffCond;
  /// Pointer to the condition used for the offset along y (MotionSystem).
  Condition *m_yOffCond;

  /// Flag to remember if the condition registered itself to the UpdateManagerSvc.
  bool m_inUpdMgrSvc;

private:

  /// cached Message Stream object
  mutable MsgStream * m_msgStream;

  /// On demand access to MsgStream object
  inline MsgStream & msg() const
  {
    if ( !m_msgStream ) m_msgStream = new MsgStream( this->msgSvc(), "VeloAlignCond" );
    return *m_msgStream;
  }

};
#ifdef __INTEL_COMPILER // End disable ICC remark from ROOT GenVector classes
  #pragma warning(pop)
#endif

#endif // VELODET_VELOALIGNCOND_H
