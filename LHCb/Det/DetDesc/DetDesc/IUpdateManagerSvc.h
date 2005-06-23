// $Id: IUpdateManagerSvc.h,v 1.4 2005-06-23 14:21:30 marcocle Exp $
#ifndef DETCOND_IUPDATEMANAGERSVC_H 
#define DETCOND_IUPDATEMANAGERSVC_H 1

// Include files
// from STL
#include <string>
#include <typeinfo>

// from Gaudi
#include "GaudiKernel/IInterface.h"

// forward declarations
class DataObject;
class ValidDataObject;
class IUpdateManagerSvc;
class IDataProviderSvc;
class IDetDataSvc;
class ITime;
class TimePoint;

static const InterfaceID IID_IUpdateManagerSvc ( "IUpdateManagerSvc", 1, 0 );

/** @class BaseObjectMemberFunction
 *
 * Base class of ObjectMemberFunction. It is used to allow to use containers of
 * different types of object member functions. \see ObjectMemberFunction for details.
 *
 * @author Marco Clemencic
 */
class BaseObjectMemberFunction {
public:
  virtual StatusCode operator() () const = 0;
	
  virtual BaseObjectMemberFunction *makeCopy() const = 0;

  virtual const std::type_info &type() const = 0;
	
  virtual bool match(BaseObjectMemberFunction *) const = 0;
	
  virtual DataObject* castToDataObject() const = 0;
  virtual ValidDataObject* castToValidDataObject() const = 0;
  virtual void* castToVoid() const = 0;
};

/** @class ObjectMemberFunction
 *
 * This class is used by IUpdateManagerSvc to keep pairs made of a member function and a pointer
 * to the object for which that member function has to be called.
 *
 * @author Marco Clemencic
 */
template <class CallerClass>
class ObjectMemberFunction: public BaseObjectMemberFunction {
public:
  /// MemberFunctionType is the type for a pointer to a member function of class CallerClass.
  typedef	StatusCode (CallerClass::*MemberFunctionType)();

  /// Calls the member function of the object and returns the StatusCode.
  /// If the pointer to the member function is NULL, do nothing and return success.
  virtual StatusCode operator() () const {
    return (m_memberFunction != NULL)?
      (m_instance->*m_memberFunction)() :
      StatusCode::SUCCESS;
  }

  /// Clone method to be able to copy an ObjectMemberFunction from the BaseObjectMemberFunction
  /// interface.
  virtual BaseObjectMemberFunction *makeCopy() const {
    return new ObjectMemberFunction<CallerClass>(m_instance,m_memberFunction);
  }
  
  /// Returns the type_info of the CallerClass
  virtual const std::type_info &type() const { return typeid(CallerClass); }

  /// Comparison between two BaseObjectMemberFunction instances.
  virtual bool match(BaseObjectMemberFunction *bmf) const {
    if ( bmf == (BaseObjectMemberFunction *)this ) return true;
    if (type() == bmf->type()) {
      ObjectMemberFunction * mf = dynamic_cast<ObjectMemberFunction *>(bmf);
      return m_instance == mf->m_instance &&
        m_memberFunction == mf->m_memberFunction;
    }
    return false;
  }

  /// Cast the object to DataObject.
  virtual DataObject* castToDataObject() const {
    return dynamic_cast<DataObject*>(m_instance);
  }

  /// Cast the object to ValidDataObject.
  virtual ValidDataObject* castToValidDataObject() const {
    return dynamic_cast<ValidDataObject*>(m_instance);
  }

  /// Cast the object to void with dynamic_cast.
  virtual void* castToVoid() const {
    return dynamic_cast<void*>(m_instance);
  }

protected:

  /// Standard constructor. Protected so that can be called only by itself or IUpdateManagerSvc.
  ObjectMemberFunction(CallerClass *instance, const MemberFunctionType &mf):
    m_instance(instance),m_memberFunction(mf){}

  /// Pointer to the object.
  CallerClass *m_instance;

  /// Pointer to the member function.
  MemberFunctionType m_memberFunction;
	
  friend class IUpdateManagerSvc;
};

/** @class IUpdateManagerSvc IUpdateManagerSvc.h DetCond/IUpdateManagerSvc.h
 *
 *  Interface class to the Update Manager service. Users should only use this interface.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-03-30
 */
class IUpdateManagerSvc : virtual public IInterface {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IUpdateManagerSvc; }

  /// Give access to the data provider.
  virtual IDataProviderSvc *dataProvider() const = 0;

  /// Give access to the detector data service interface (usualy of the data provider itself).
  virtual IDetDataSvc *detDataSvc() const = 0;

  /// Register an object (algorithm instance) to the service.
  /// The object should provide a method to be called in case of an update of
  /// the needed condition. An object can register multiple conditions using the
  /// same method: it will be called if at least one of the specified conditions
  /// is updated, but only when all of them are up to date.
  /// \return StatusCode::SUCCESS if the registration went right.
  template <class CallerClass>
  inline StatusCode registerCondition(CallerClass *instance, const std::string &condition,
                                      const typename ObjectMemberFunction<CallerClass>::MemberFunctionType &mf){
  	return registerCondition(condition, new ObjectMemberFunction<CallerClass>(instance,mf));
  }

  /// See above. Needed to avoid conflicts with the next one.
  template <class CallerClass>
  inline StatusCode registerCondition(CallerClass *instance, const char *condition,
                                      const typename ObjectMemberFunction<CallerClass>::MemberFunctionType &mf){
  	return registerCondition(std::string(condition), new ObjectMemberFunction<CallerClass>(instance,mf));
  }

  /// Like the first version of registerCondition, but instead declaring the dependency on a condition of the service
  /// privider, it uses an already registered object. It means that a generic object can depend on another generic
  /// object that depends on a ValidDataObject. The dependency network is kept consistent by the UpdateManagerSvc.
  template <class CallerClass, class ObjectClass>
  inline StatusCode registerCondition(CallerClass *instance, ObjectClass *obj,
                                      const typename ObjectMemberFunction<CallerClass>::MemberFunctionType &mf){
  	return registerCondition(dynamic_cast<void*>(obj), new ObjectMemberFunction<CallerClass>(instance,mf));
  }

  /// Generic objects can be unregistered from the UpdateManagerSvc. The dependency network is always consistent, but
  /// internal IOVs are not modified. \warning{Removing a node which other nodes depends on can create problems if the
  /// methods of this other nodes are called.}
  template <class CallerClass>
  inline StatusCode unregister(CallerClass *instance) {
    return unregister(dynamic_cast<void*>(instance));
  }

  /// Invalidate the given object in the dependency network. It means that all the objects depending on that one will
  /// be updated before the next event.
  template <class CallerClass>
  inline void invalidate(CallerClass *instance) {invalidate(dynamic_cast<void*>(instance));}

  /// Retrieve the interval of validity (in the UpdateManagerSvc) of the given item.
  /// @return false if the item was not found.
  virtual bool getValidity(const std::string path, TimePoint& since, TimePoint &until, bool path_to_db = false) = 0;

  /// Change the interval of validity of the given item to the specified values, updating parents if needed.
  /// The change can only restrict the current IOV, If you want to expand the validity you should act on the transient data store
  /// and the change will be reflected at the next update.
  virtual void setValidity(const std::string path, const TimePoint& since, const TimePoint &until, bool path_to_db = false) = 0;

  /// Start an update loop using the event time given by the detector data service.
  virtual StatusCode newEvent() = 0;
  /// Start an update loop using the give event time.
  /// NOTE: the given event time is only used to check if updates are needed, the real update is done
  /// using the detector data service event time. (may change in future)
  virtual StatusCode newEvent(const ITime &) = 0;

  /// Update the given instance. This method should be called by the object after registration to ensure
  /// that the needed operations is done immediately and not before the next event.
  template <class CallerClass>
  inline StatusCode update(CallerClass *instance){
    return update(dynamic_cast<void*>(instance));
  }
  
  /// Debug method: it dumps the dependency network through the message service (not very readable, for experts only).
  virtual void dump() = 0;
  
protected:

  virtual StatusCode registerCondition(const std::string &condition, BaseObjectMemberFunction *mf) = 0;
  virtual StatusCode registerCondition(void *obj, BaseObjectMemberFunction *mf) = 0;
  virtual StatusCode update(void *instance) = 0;
  virtual StatusCode unregister(void *instance) = 0;
  virtual void       invalidate(void *instance) = 0;

private:

};
#endif // DETCOND_IUPDATEMANAGERSVC_H
