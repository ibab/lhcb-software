#ifndef _DEUTLAYER_H_
#define _DEUTLAYER_H_

#include <string>
#include <vector>

#include "STDet/DeSTLayer.h"

class DeUTModule;
class DeUTStation;

/** @class DeUTLayer DeUTLayer.h STDet/DeUTLayer.h
 *
 *  UT Layer detector element
 *
 *  @author Jianchun Wang, Matt Needham
 *  @date   2012-07-10
 *
 */

static const CLID CLID_DeUTLayer = 9303;

class DeUTLayer : public DeSTLayer  {

public:

  /** parent type */
  typedef STDetTraits<DeUTLayer>::parent parent_type;

  /** child type */
  typedef STDetTraits<DeUTLayer>::child child_type;

  /** children */
  typedef std::vector<child_type*> Children;

  /** Constructor */
  DeUTLayer ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeUTLayer();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
   static const CLID& classID(){return CLID_DeUTLayer;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /**  locate module based on a channel id
  @return  module */
  DeUTModule* findModule(const LHCb::STChannelID aChannel);

  /** locate module  based on a point
  @return module */
  DeUTModule* findModule(const Gaudi::XYZPoint& point) ;

  /** check whether contains
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

  /** vector of children */
  const Children& modules() const;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

 private:

  /** make flat list of lowest descendents  and also layers*/
  void flatten();

  Children m_modules;
  parent_type* m_parent;

};

#include "STDet/DeUTModule.h"

inline bool DeUTLayer::contains(const LHCb::STChannelID aChannel) const{
  return (elementID().station() == aChannel.station() &&
         (elementID().layer() == aChannel.layer())) ;
}

inline const DeUTLayer::Children& DeUTLayer::modules() const{
  return m_modules;
}

#endif // _DEUTLAYER_H
