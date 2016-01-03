#ifndef _DeITLayer_H_
#define _DeITLayer_H_

#include <string>
#include "STDet/DeSTLayer.h"


/** @class DeITLayer DeITLayer.h "STDet/DeITLayer.h"
 *
 *  IT Station detector element
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

static const CLID CLID_DeITLayer = 9204; // needs fixing !

class DeITSector;
class DeSTSector;
class DeITLadder;

class DeITLayer : public DeSTLayer  {

public:


   /** parent type */
   typedef STDetTraits<DeITLayer>::parent parent_type;

   /** child type */
   typedef STDetTraits<DeITLayer>::child child_type;

   /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
   static const CLID& classID(){return CLID_DeITLayer;}

   /** children */
   typedef std::vector<child_type*> Children;

   /** Constructor */
   DeITLayer ( const std::string& name = "" ) ;

   /** Destructor */
   virtual ~DeITLayer();

   /**
   * another reference to class identifier
   * @return the class identifier for this class
   */
   const CLID& clID () const;

   /** initialization method
   * @return Status of initialisation
   */
   virtual StatusCode initialize();

  /**  locate half module based on a channel id
  @return  module */
  DeITLadder* findLadder(const LHCb::STChannelID aChannel) const;

  /** locate half module  based on a point
  @return module */
  DeITLadder* findLadder(const Gaudi::XYZPoint& point) const;

  /** check whether contains
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

  /** vector of children */
  const DeITLayer::Children& ladders() const;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

  /** get the first sector in the layer */
  const DeITSector* firstSector() const;

  /** get the last sector in the layer */
  const DeITSector* lastSector() const;

private:

   /** make flat list of lowest descendents  and also layers*/
   void flatten();

   Children m_ladders;
   parent_type* m_parent;
   DeITSector* m_firstSector;
   DeITSector* m_lastSector;

};




#include "STDet/DeITLadder.h"


inline const DeITLayer::Children& DeITLayer::ladders() const{
  return m_ladders;
}

inline const DeITSector* DeITLayer::firstSector() const {
  return m_firstSector;
}

inline const DeITSector* DeITLayer::lastSector() const{
  return m_lastSector;
}

#endif // _DeITLayer_H








