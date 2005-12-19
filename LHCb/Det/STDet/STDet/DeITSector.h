#ifndef _DeITSector_H_
#define _DeITSector_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSector.h"

class DeITLayer;

/** @class DeITSector DeITSector.h "STDet/DeITSector.h"
 *
 *  Class representing a TT Half module (7 sensors ganged together)
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

static const CLID& CLID_DeITSector = 9210;

class DeITSector : public DeSTSector  {

public:

  /** parent type */
  typedef STDetTraits<DeITSector>::parent parent_type;

  /** Constructor */
  DeITSector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeITSector(); 
 
  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeITSector;}

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


#endif // _DeITSector_H








