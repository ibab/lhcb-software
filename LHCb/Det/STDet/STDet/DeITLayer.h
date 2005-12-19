// $Id: DeITLayer.h,v 1.1 2005-12-19 15:18:24 mneedham Exp $
#ifndef _DeITLayer_H_
#define _DeITLayer_H_

#include <string>

#include "STDet/DeSTLayer.h"

/** @class DeSTStation DeITLayer.h "STDet/DeITLayer.h"
 *
 *  IT Station detector element
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

static const CLID& CLID_DeITLayer = 9204; // needs fixing !

class DeITBox;

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
  DeITSector* findSector(const LHCb::STChannelID aChannel);     

  /** locate half module  based on a point  
  @return module */
  DeITSector* findSector(const Gaudi::XYZPoint& point) ;  

  /** check whether contains 
  *  @param channel
  *  @return bool
  */ 
  bool contains(const LHCb::STChannelID aChannel) const; 

  /** vector of children */
  const DeITLayer::Children& sectors() const;

private:
  
   Children m_sectors;
   parent_type* m_parent;   

};


#include "STDet/DeITBox.h"
#include "STDet/DeITSector.h"


inline bool DeITLayer::contains(const LHCb::STChannelID aChannel) const{
  return (elementID().layer() == aChannel.layer() && 
         (m_parent->contains(aChannel))); 
}

inline const DeITLayer::Children& DeITLayer::sectors() const{
  return m_sectors;
}


#endif // _DeITLayer_H








