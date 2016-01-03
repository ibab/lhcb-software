#ifndef _DeTTSensor_H_
#define _DeTTSensor_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSensor.h"

/** @class DeTTSensor DeTTSensor.h "STDet/DeTTSensor.h"
 *
 *  Class representing a TT Sensor
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 *   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

class DeSTSector;

static const CLID CLID_DeTTSensor = 9130;

class DeTTSensor : public DeSTSensor  {

public:

  /** parent type */
  typedef STDetTraits<DeTTSensor>::parent parent_type;

  /** Constructor */
  DeTTSensor ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeTTSensor();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeTTSensor;}

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


#endif // _DeTTSensor_H








