// $Id: IUpdateManagerSvc.h,v 1.1 2005-05-03 12:40:08 marcocle Exp $
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

static const InterfaceID IID_IUpdateManagerSvc ( "IUpdateManagerSvc", 1, 0 );

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

template <class CallerClass>
class ObjectMemberFunction: public BaseObjectMemberFunction {
public:
	typedef	StatusCode (CallerClass::*MemberFunctionType)();
	virtual StatusCode operator() () const {
	  return (m_memberFunction != NULL)?
      (m_instance->*m_memberFunction)() :
      StatusCode::SUCCESS;
	}
	
  virtual BaseObjectMemberFunction *makeCopy() const {
    return new ObjectMemberFunction<CallerClass>(m_instance,m_memberFunction);
  }
  
  virtual const std::type_info &type() const { return typeid(CallerClass); }
    
	virtual bool match(BaseObjectMemberFunction *bmf) const {
		if ( bmf == (BaseObjectMemberFunction *)this ) return true;
		if (type() == bmf->type()) {
			ObjectMemberFunction * mf = dynamic_cast<ObjectMemberFunction *>(bmf);
			return m_instance == mf->m_instance &&
				m_memberFunction == mf->m_memberFunction;
		}
		return false;
	}

	
  virtual DataObject* castToDataObject() const {
    return dynamic_cast<DataObject*>(m_instance);
  }
	virtual ValidDataObject* castToValidDataObject() const {
    return dynamic_cast<ValidDataObject*>(m_instance);
  }
	virtual void* castToVoid() const {
		return dynamic_cast<void*>(m_instance);
	}
    
protected:
	
	ObjectMemberFunction(CallerClass *instance, const MemberFunctionType &mf):
		m_instance(instance),m_memberFunction(mf){}
	
	CallerClass *m_instance;
	MemberFunctionType m_memberFunction;
	
	friend class IUpdateManagerSvc;
};

/** @class IUpdateManagerSvc IUpdateManagerSvc.h DetCond/IUpdateManagerSvc.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-03-30
 */
class IUpdateManagerSvc : virtual public IInterface {
public: 

  /// \return the interface ID
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

  // TODO: remove this method, which is there just for testing
  //virtual StatusCode runAll() const = 0;
  
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
