// $Id: DeSTBaseElement.h,v 1.6 2006-04-19 07:41:37 mneedham Exp $
#ifndef _DeSTBaseElement_H_
#define _DeSTBaseElement_H_

#include <string>
#include <vector>
#include <algorithm>

#include "STDet/STDetTraits.h"
#include "Kernel/STChannelID.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/GaudiException.h"

class DeSTBaseElement : public DetectorElement  {

public:

  /** Constructor */
  DeSTBaseElement ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTBaseElement(); 
 
  /** initialization method 
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** check whether point is inside element
  * @param point 
  * @return true if inside
  */
  bool isInside(const Gaudi::XYZPoint& point) const;

  /** transform global to local point
  * @param global point 
  * @return local point
  */
  Gaudi::XYZPoint toLocal(const Gaudi::XYZPoint& point) const;

  /** transform global to local point
  * @param local point 
  * @return global point
  */
  Gaudi::XYZPoint toGlobal(const Gaudi::XYZPoint& point) const;

  /** centre in global frame
  * @return global centre
  */
  Gaudi::XYZPoint globalCentre() const;


  /** convert x,y,z in local frame to global frame
  * @param local x
  * @param local y
  * @param local z
  * @return point 
  */
  Gaudi::XYZPoint globalPoint(const double x, 
                              const double y, 
                              const double z) const;


  /** detector element id  - for experts only !*/
  LHCb::STChannelID elementID() const;

  /** set detector element id  - for experts only !*/
  void setElementID(const LHCb::STChannelID& chanID);

  /** get the parent of the detector element 
  @return parenttype */
  template <typename TYPE>
  typename STDetTraits<TYPE>::parent* getParent() const;

  /** retrieve the children
  * @return children  
  */
  template <typename TYPE>
  std::vector<typename STDetTraits<TYPE>::child*> getChildren();

private:


  bool duplicate(const std::string& testString, 
                 const std::vector<std::string>& names) const;

  LHCb::STChannelID m_elementID;
  Gaudi::XYZPoint m_globalCentre; 

};

#include "DetDesc/IGeometryInfo.h"

inline bool DeSTBaseElement::isInside(const Gaudi::XYZPoint& point) const{
  return this->geometry()->isInside(point);
}

inline Gaudi::XYZPoint DeSTBaseElement::toLocal(const Gaudi::XYZPoint& point) const{
  return this->geometry()->toLocal(point);
}

inline Gaudi::XYZPoint DeSTBaseElement::toGlobal(const Gaudi::XYZPoint& point) const{
  return this->geometry()->toGlobal(point);
}

inline Gaudi::XYZPoint DeSTBaseElement::globalCentre() const{
  return m_globalCentre;
}

template <typename TYPE>
inline typename STDetTraits<TYPE>::parent* DeSTBaseElement::getParent() const{

  typedef typename STDetTraits<TYPE>::parent parentType;
  parentType* parent = dynamic_cast<parentType*>(this->parentIDetectorElement());
  if (parent == 0) {
    throw GaudiException ("Orphaned detector element", "DeSTBaseElement", 
                           StatusCode::FAILURE);
  }

  return parent;
}

inline LHCb::STChannelID DeSTBaseElement::elementID() const{
  return m_elementID;
}

inline void DeSTBaseElement::setElementID(const LHCb::STChannelID& chanID){
  m_elementID = chanID;
}

template<typename TYPE>
inline std::vector<typename STDetTraits<TYPE>::child*> DeSTBaseElement::getChildren(){

  typedef typename STDetTraits<TYPE>::child cType;
  std::vector<cType*> childVector;
  std::vector<std::string> names;

  IDetectorElement::IDEContainer::const_iterator iChild;
  for (iChild = this->childBegin(); this->childEnd() != iChild; ++iChild) {
    cType* aChild = dynamic_cast<cType*>(*iChild);
    if (aChild !=0){
      if (duplicate(aChild->name(),names) == false) { 
        names.push_back(aChild->name());
        childVector.push_back(aChild);
      }
      else {
        MsgStream msg(msgSvc(), name() );
        msg << MSG::WARNING 
            << "tried to make duplicate detector element !" << aChild->name()  << endreq;
      } // if
    } // if 
  } // iStation

  if (childVector.empty()) {
    throw GaudiException ("Sterile detector element", "DeSTBaseElement", 
                           StatusCode::FAILURE);
  }

  return childVector;
}

inline bool DeSTBaseElement::duplicate(const std::string& testString, const std::vector<std::string> & names) const{
  std::vector<std::string >::const_iterator iter = std::find(names.begin(), names.end(), testString);
  return (iter == names.end() ? false : true); 
}


#endif
