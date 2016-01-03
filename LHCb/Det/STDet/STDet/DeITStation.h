// $Id: DeITStation.h,v 1.4 2008-07-14 07:38:36 mneedham Exp $
#ifndef _DeITStation_H_
#define _DeITStation_H_

#include <string>
#include <vector>

#include "STDet/DeSTStation.h"

class DeITBox;
class DeITDetector;

/** @class DeITStation DeITStation.h "STDet/DeITStation.h"
 *
 *  IT Station detector element
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

static const CLID CLID_DeITStation = 9202; // needs fixing !

class DeITStation : public DeSTStation  {

public:

   /** parent type */
   typedef STDetTraits<DeITStation>::parent parent_type;

   /** child type */
   typedef STDetTraits<DeITStation>::child child_type;

   /** children */
   typedef std::vector<child_type*> Children;

   /** Constructor */
   DeITStation ( const std::string& name = "" ) ;

   /** Destructor */
   virtual ~DeITStation();

   /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
   static const CLID& classID(){return CLID_DeITStation;}

   /**
   * another reference to class identifier
   * @return the class identifier for this class
   */
   const CLID& clID () const;

   /** initialization method
   * @return Status of initialisation
   */
   virtual StatusCode initialize();


  /**  locate the box based on a channel id
  @return  layer */
  DeITBox* findBox(const LHCb::STChannelID aChannel);

  /** locate box  based on a point
  @return layer */
  DeITBox* findBox(const Gaudi::XYZPoint& point) ;

  /** vector of children */
  const DeITStation::Children& boxes() const;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

private:


   Children m_boxes;
   parent_type* m_parent;

};

#include "STDet/DeITBox.h"

inline const DeITStation::Children& DeITStation::boxes() const{
  return m_boxes;
}

#endif // _DeITStation_H


