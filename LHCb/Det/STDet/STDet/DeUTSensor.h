#ifndef _DEUTSENSOR_H_
#define _DEUTSENSOR_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSensor.h"

/** @class DeUTSensor DeUTSensor.h STDet/DeUTSensor.h
 *
 *  Class representing a UT Sensor
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

class DeSTSector;

static const CLID CLID_DeUTSensor = 9330;

class DeUTSensor : public DeSTSensor  {

public:

  /** parent type */
  typedef STDetTraits<DeUTSensor>::parent parent_type;

  /** Constructor */
  DeUTSensor ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeUTSensor();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeUTSensor;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

private:

  parent_type* m_parent;

};


#endif // _DEUTSENSOR_H
