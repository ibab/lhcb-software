// $Id: VeloAlignCond.h,v 1.1 2008-07-11 16:35:58 marcocle Exp $
#ifndef VELODET_VELOALIGNCOND_H 
#define VELODET_VELOALIGNCOND_H 1

// Include files
#include "DetDesc/AlignmentCondition.h"
#include "VeloDet/CLIDVeloAlignCond.h"
/** @class VeloAlignCond VeloAlignCond.h VeloDet/VeloAlignCond.h
 *  
 *
 *  @author Marco Clemencic
 *  @date   2008-06-23
 */

class MsgStream;
class IMessageSvc;

class VeloAlignCond : public AlignmentCondition {

public: 
  /**
   * Helper class to handle the two strings indicating the location in the TS
   * of the stepping motors readings. 
   */
  struct PositionPaths {
    PositionPaths(): x(), y() {}
    PositionPaths(const std::string &_x,const std::string &_y);
    std::pair<std::string,std::string> x,y;
    bool operator!=(const PositionPaths &rhs) const {
      return x != rhs.x || y != rhs.y;
    }
  };
  
  /// Standard constructor
  VeloAlignCond( );
  ///
  VeloAlignCond(const std::vector<double>& translation,
                const std::vector<double>& rotation,
                const std::vector<double>& pivot = std::vector<double>(3, 0.),
                const std::string &xOffsetLoc = "",
                const std::string &yOffsetLoc = "") ;
  
  virtual ~VeloAlignCond(); ///< Destructor

  virtual StatusCode initialize();

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 
  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_VeloAlignCond; };
  
protected:

  virtual StatusCode makeMatrices();
  
  /// Initialize m_paths and register to the UpdateManagerSvc
  void initializePosPath();
  
  /// Update m_paths and the registration to the UpdateManagerSvc
  void updatePosPath();
  
  PositionPaths m_paths;
  
  Condition *m_xOffCond;
  Condition *m_yOffCond;
  
  bool m_firstInit;
  
};
#endif // VELODET_VELOALIGNCOND_H
