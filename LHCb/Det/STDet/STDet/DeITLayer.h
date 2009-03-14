// $Id: DeITLayer.h,v 1.8 2009-03-14 09:18:07 mneedham Exp $
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
  DeITLadder* findLadder(const LHCb::STChannelID aChannel);     

  /** locate half module  based on a point  
  @return module */
  DeITLadder* findLadder(const Gaudi::XYZPoint& point) ;  

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

private:

   /** make flat list of lowest descendents  and also layers*/
   void flatten();
 
   Children m_ladders;
   parent_type* m_parent;   

};



#include "STDet/DeITBox.h"
#include "STDet/DeITLadder.h"


inline bool DeITLayer::contains(const LHCb::STChannelID aChannel) const{
  return (elementID().layer() == aChannel.layer() && 
         (m_parent->contains(aChannel))); 
}

inline const DeITLayer::Children& DeITLayer::ladders() const{
  return m_ladders;
}


#endif // _DeITLayer_H








