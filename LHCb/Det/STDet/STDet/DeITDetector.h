// $Id: DeITDetector.h,v 1.3 2006-12-13 13:48:52 mneedham Exp $
#ifndef _DeITDetector_H_
#define _DeITDetector_H_

#include "STDet/DeSTDetector.h"
#include "GaudiKernel/VectorMap.h"

static const CLID& CLID_DeITDetector = 9201;

/** @class DeITDetector DeITDetector.h "STDet/DeITDetector.h"
 *
 *  IT Detector Element class
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

class DeSTSector;

class DeITDetector : public DeSTDetector  {

public:

  /** Constructor */
  DeITDetector ( const std::string& name = "" ) ;

 
  /** Destructor */
  virtual ~DeITDetector(); 
 
  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeITDetector;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const ;

  virtual StatusCode initialize();
  
  /** 
  *  short cut to pick up the wafer corresponding to x,y,z
  * @param point in global frame  
  * @return sector 
  */
  virtual DeSTSector* findSector(const Gaudi::XYZPoint& aPoint); 

  /** 
  *  short cut to pick up the wafer corresponding to a channel
  * @param channel 
  * @return sector 
  */
  // virtual DeSTSector* findSector(const LHCb::STChannelID aChannel); 

private:

  /** make flat list of lowest descendents  and also layers*/
  void flatten();

  //  typedef GaudiUtils::VectorMap<unsigned int,DeSTSector*> SectorMap;
  //SectorMap m_sMap;

};

#endif // _DeITDetector_H








