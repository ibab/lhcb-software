#ifndef _DEUTSTATION_H_
#define _DEUTSTATION_H_

#include <string>
#include <vector>

#include "STDet/DeSTStation.h"

class DeUTLayer;
class DeUTDetector;

/** @class DeUTStation DeUTStation.h STDet/DeUTStation.h
 *
 *  UT Station detector element
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

static const CLID CLID_DeUTStation = 9302;

class DeUTStation : public DeSTStation  {

public:

  /** parent type */
  typedef STDetTraits<DeUTStation>::parent parent_type;

  /** child type */
  typedef STDetTraits<DeUTStation>::child child_type;

  /** children */
  typedef std::vector<child_type*> Children;

  /** Constructor */
  DeUTStation ( std::string name = {} ) ;

  /** Destructor */
  virtual ~DeUTStation() = default;

   /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
   static const CLID& classID(){return CLID_DeUTStation;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /**  locate the layer based on a channel id
  @return  layer */
  DeUTLayer* findLayer(const LHCb::STChannelID aChannel);

  /** locate layer based on a point
  @return layer */
  DeUTLayer* findLayer(const Gaudi::XYZPoint& point) ;

  /** vector of children */
  const Children& layers() const;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

private:


  parent_type* m_parent = nullptr;
  Children m_layers;

};


inline const DeUTStation::Children& DeUTStation::layers() const{
  return m_layers;
}

#endif // _DEUTSTATION_H
