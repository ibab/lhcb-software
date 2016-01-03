#ifndef _DEUTSECTOR_H_
#define _DEUTSECTOR_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSector.h"

/** @class DeUTSector DeUTSector.h STDet/DeUTSector.h
 *
 *  Class representing a UT Sector
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

class DeUTModule;

static const CLID CLID_DeUTSector = 9320;

class DeUTSector : public DeSTSector  {

public:

  /** parent type */
  typedef STDetTraits<DeUTSector>::parent parent_type;

  /** Constructor */
  DeUTSector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeUTSector();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeUTSector;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /**
   * Retrives the hybrid type
   * @return the hybrid type
   */
  std::string hybridType() const;

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** column number */
  unsigned int column() const;

  /** row Number.... */
  unsigned int row() const;

  /** production ID --> in fact parent ID */
  virtual unsigned int prodID() const;

  std::string conditionsPathName() const;

  /** module type */
  std::string moduleType() const;

private:

  parent_type* m_parent;
  unsigned int m_row;
  std::string m_hybridType;
  std::string m_conditionPathName;

  std::string moduleNumber(const unsigned int& chan,
                           const unsigned int& reg) const;
};

#include "STDet/DeUTModule.h"

inline unsigned int DeUTSector::column() const {
  return m_parent->column();
}

inline unsigned int DeUTSector::row() const {
  return m_row;
}

inline std::string DeUTSector::moduleType() const {
  return m_parent->type();
}


#endif // _DEUTSECTOR_H
