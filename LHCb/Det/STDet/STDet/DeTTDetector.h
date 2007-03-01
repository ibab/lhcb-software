// $Id: DeTTDetector.h,v 1.6 2007-03-01 11:08:00 cattanem Exp $
#ifndef _DeTTDetector_H_
#define _DeTTDetector_H_

#include "STDet/DeSTDetector.h"
#include "Kernel/STChannelID.h"

class DeSTSector;

/** @class DeTTDetector DeTTDetector.h "STDet/DeTTDetector.h"
 *
 *  TT Detector Element class
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

static const CLID CLID_DeTTDetector = 9101;

class DeTTDetector : public DeSTDetector  {

public:

  /** Constructor */
  DeTTDetector ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeTTDetector(); 

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID(){return CLID_DeTTDetector;}
 
  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /** initialization method 
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** 
  *  short cut to pick up the wafer corresponding to x,y,z
  * @param  aPoint point in global frame  
  * @return sector 
  */
  virtual DeSTSector* findSector(const Gaudi::XYZPoint& aPoint); 

  /** 
  *  short cut to pick up the wafer corresponding to a channel
  * @param  aChannel channel 
  * @return sector 
  */
  //virtual DeSTSector* findSector(const LHCb::STChannelID aChannel); 
  
private:

  /** make flat list of lowest descendents  and also layers */
  void flatten();

};



#endif // _DeTTDetector_H











