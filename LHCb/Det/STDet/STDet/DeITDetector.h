// $Id: DeITDetector.h,v 1.10 2009-02-27 12:17:50 mneedham Exp $
#ifndef _DeITDetector_H_
#define _DeITDetector_H_

#include "STDet/DeSTDetector.h"
#include "GaudiKernel/VectorMap.h"

static const CLID CLID_DeITDetector = 9201;

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

  /** box vector typedef **/
  typedef std::vector<DeITBox*> Boxes;

  /** Constructor */
  DeITDetector ( const std::string& name = "" ) ;


  /** Destructor */
  ~DeITDetector() override;

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

  StatusCode initialize() override;

  /**
  *  short cut to pick up the wafer corresponding to x,y,z
  * @param  aPoint point in global frame
  * @return sector
  */
  DeSTSector* findSector(const Gaudi::XYZPoint& aPoint) const override;

  /**
  *  find top level DeSTBaseElement by nickname
  * @param nickname
  * @return base element
  */
  DeSTBaseElement* findTopLevelElement(const std::string& nickname) const override;


  /** flat vector of boxes
  * @return vector of boxes
  */
  const Boxes& boxes() const;

  /**  locate the layer based on a channel id
  @return  box */
  DeITBox* findBox(const LHCb::STChannelID aChannel) const;

  /** locate layer based on a point
   *return layer */
  DeITBox* findBox(const Gaudi::XYZPoint& point) const;

  /** get the number of boxes **/
  unsigned int nBox() const;

  /** get the number of readout sectors **/
  unsigned int nBoxPerStation() const;

  /** number of layers per station **/
  unsigned int nLayerPerBox() const;

private:

  /** make flat list of lowest descendents  and also layers*/
  void flatten();

  Boxes m_boxes;

};

inline const DeITDetector::Boxes& DeITDetector::boxes() const{
  return m_boxes;
}

inline unsigned int DeITDetector::nBox() const{
  return boxes().size();
}

inline unsigned DeITDetector::nBoxPerStation() const{
  return nBox()/nStation();
}

inline unsigned DeITDetector::nLayerPerBox() const{
  return nLayer()/nBox();
}

#endif // _DeITDetector_H
