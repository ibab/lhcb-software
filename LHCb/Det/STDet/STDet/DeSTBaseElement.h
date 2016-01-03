#ifndef _DeSTBaseElement_H_
#define _DeSTBaseElement_H_

#include <string>
#include <vector>
#include <algorithm>

#include "STDet/STDetTraits.h"
#include "Kernel/STChannelID.h"
#include "DetDesc/DetectorElement.h"
#include "GaudiKernel/GaudiException.h"

#include "GaudiKernel/IUpdateManagerSvc.h"

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

  /** transform global to local point
  * @param  point global point
  * @return       local point
  */
  Gaudi::XYZPoint toLocal(const Gaudi::XYZPoint& point) const;

  /** transform global to local point
  * @param  point  local point
  * @return        global point
  */
  Gaudi::XYZPoint toGlobal(const Gaudi::XYZPoint& point) const;

  /** centre in global frame
  * @return global centre
  */
  Gaudi::XYZPoint globalCentre() const;


  /** convert x,y,z in local frame to global frame
  * @param x local x
  * @param y local y
  * @param z local z
  * @return point
  */
  Gaudi::XYZPoint globalPoint(const double x,
                              const double y,
                              const double z) const;


  /** detector element id  - for experts only !*/
  LHCb::STChannelID elementID() const;

  /** check whether contains
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const = 0;

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

  /**
  * call back for update service
  * @param caller
  * @param object
  * @param mf
  * @param forceUpdate force update
  * @return StatusCode Success or Failure
  */
  template<typename CallerClass, typename ObjectClass>
  StatusCode registerCondition(CallerClass* caller, ObjectClass* object,
                               typename ObjectMemberFunction<CallerClass>::MemberFunctionType mf,
                               bool forceUpdate = true );


  /**
  * call back for update service
  * @param caller
  * @param conditionName
  * @param mf
  * @param forceUpdate force update
  * @return StatusCode Success or Failure
  */
  template<typename CallerClass>
  StatusCode registerCondition(CallerClass* caller, const std::string& conditionName,
                               typename ObjectMemberFunction<CallerClass>::MemberFunctionType mf,
                               bool forceUpdate = true );

private:

  StatusCode cachePoint();

  bool duplicate(const std::string& testString,
                 const std::vector<std::string>& names) const;

  LHCb::STChannelID m_elementID;
  Gaudi::XYZPoint m_globalCentre;

};

#include "DetDesc/IGeometryInfo.h"

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
  const unsigned int nElem = childIDetectorElements().size();
  std::vector<cType*> childVector; childVector.reserve(nElem);
  std::vector<std::string> names; names.reserve(nElem);

  IDetectorElement::IDEContainer::const_iterator iChild = childBegin();
  for (; this->childEnd() != iChild; ++iChild) {
    cType* aChild = dynamic_cast<cType*>(*iChild);
    if (aChild !=0){
      if (duplicate(aChild->name(),names) == false) {
        names.push_back(aChild->name());
        childVector.push_back(aChild);
      }
      else {
        MsgStream msg(msgSvc(), name() );
        msg << MSG::WARNING
            << "tried to make duplicate detector element !" << aChild->name()  << endmsg;
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

template<typename CallerClass,typename ObjectClass>
inline StatusCode DeSTBaseElement::registerCondition(CallerClass* caller, ObjectClass* object,
                            typename ObjectMemberFunction<CallerClass>::MemberFunctionType mf, bool forceUpdate ){

 // initialize method
 MsgStream msg(msgSvc(), name() );
 StatusCode sc = StatusCode::SUCCESS;

 try {
   //   if (forceUpdate) updMgrSvc()->invalidate(this);
   if( msg.level() <= MSG::DEBUG )
     msg << MSG::DEBUG << "Registering conditions" << endmsg;
   updMgrSvc()->registerCondition(caller,object,mf);
   if( msg.level() <= MSG::DEBUG )
     msg << MSG::DEBUG << "Start first update" << endmsg;
   if (forceUpdate){
       sc = updMgrSvc()->update(caller);
       if (sc.isFailure()){
       msg << MSG::WARNING << "failed to update detector element " << endmsg;
     }
   }
 }
 catch (DetectorElementException &e)
{
   msg << MSG::ERROR << " Failed to update condition:" << caller->name() << endmsg;
   msg << MSG::ERROR << e << endmsg;
   return StatusCode::FAILURE;
 }
 return sc;
}

template<typename CallerClass>
inline StatusCode DeSTBaseElement::registerCondition(CallerClass* caller, const std::string& conditionName ,
                            typename ObjectMemberFunction<CallerClass>::MemberFunctionType mf, bool forceUpdate ){

 // initialize method
 MsgStream msg(msgSvc(), name() );
 StatusCode sc = StatusCode::SUCCESS;

 try {
   //if (forceUpdate) updMgrSvc()->invalidate(this);
   if( msg.level() <= MSG::DEBUG )
     msg << MSG::DEBUG << "Registering " << conditionName << " condition" << endmsg;
   updMgrSvc()->registerCondition(caller,condition(conditionName).path(),mf);
   if( msg.level() <= MSG::DEBUG )
     msg << MSG::DEBUG << "Start first update" << endmsg;
   if (forceUpdate){
      sc = updMgrSvc()->update(caller);
      if (sc.isFailure()){
       msg << MSG::WARNING << "failed to update detector element " <<  condition(conditionName).path() << endmsg;
      }
   }
 }
 catch (DetectorElementException &e) {
   msg << MSG::ERROR << " Failed to update condition:" << conditionName  << " " << caller->name() << endmsg;
   msg << MSG::ERROR << "Message is " << e << endmsg;
   return StatusCode::FAILURE;
 }
 return sc;
}


#endif
