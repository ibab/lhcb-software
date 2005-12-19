#ifndef _DeTTStation_H_
#define _DeTTStation_H_

#include <string>
#include <vector>

#include "STDet/DeSTStation.h"

class DeTTLayer;
class DeTTDetector;

/** @class DeSTStation DeTTStation.h "STDet/DeTTStation.h"
 *
 *  TT Station detector element
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

static const CLID& CLID_DeTTStation = 9102;

class DeTTStation : public DeSTStation  {

public:

  /** parent type */
  typedef STDetTraits<DeTTStation>::parent parent_type;

  /** child type */
  typedef STDetTraits<DeTTStation>::child child_type;

  /** children */  
  typedef std::vector<child_type*> Children; 

  /** Constructor */
  DeTTStation ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeTTStation(); 

   /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
   static const CLID& classID(){return CLID_DeTTStation;}

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
  DeTTLayer* findLayer(const LHCb::STChannelID aChannel);     

  /** locate layer based on a point  
  @return layer */
  DeTTLayer* findLayer(const Gaudi::XYZPoint& point) ;   

  /** vector of children */
  const Children& layers() const; 

private:


  parent_type* m_parent;
  Children m_layers;

};


inline const DeTTStation::Children& DeTTStation::layers() const{
  return m_layers;
}

#endif // _DeTTStation_H



