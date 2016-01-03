#ifndef _DeITSensor_H_
#define _DeITSensor_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSensor.h"

/** @class DeITSensor DeITSensor.h "STDet/DeITSensor.h"
 *
 *  Class representing a IT Sensor
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

class DeSTSector;

static const CLID CLID_DeITSensor = 9231;

class DeITSensor : public DeSTSensor  {

public:

  /** parent type */
  typedef STDetTraits<DeITSensor>::parent parent_type;

  /** Constructor */
  DeITSensor ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeITSensor();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeITSensor;}

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


#endif // _DeITSensor_H








