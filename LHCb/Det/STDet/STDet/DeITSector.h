#ifndef _DeITSector_H_
#define _DeITSector_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTSector.h"

class DeITLayer;

/** @class DeITSector DeITSector.h "STDet/DeITSector.h"
 *
 *  Class representing a IT Ladder  --> sensor part
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

static const CLID CLID_DeITSector = 9206;

class DeITSector : public DeSTSector  {

public:

  /** parent type */
  typedef STDetTraits<DeITSector>::parent parent_type;

  /** Constructor */
  DeITSector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeITSector();

  virtual unsigned int prodID() const;

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

  std::string conditionsPathName() const;

private:

  StatusCode updateProdIDCondition();

  parent_type* m_parent;
  unsigned int m_prodID;
  std::string m_prodIDString;

};


#endif // _DeITSector_H








