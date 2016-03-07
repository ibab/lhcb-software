//$Id: ParamValidDataObject.h,v 1.11 2008-07-11 16:34:50 marcocle Exp $
#ifndef DETDESC_PARAMVALIDDATAOBJECT_H
#define DETDESC_PARAMVALIDDATAOBJECT_H 1

// Base classes
#include "DetDesc/ValidDataObject.h"

#include "DetDesc/ParamList.h"
#include "DetDesc/ParamException.h"

/** @class ParamValidDataObject ParamValidDataObject.h DetDesc/ParamValidDataObject.h
 *
 *  A DataObject implementing the IParamSetEditor and IValidity
 *  interfaces. This is the basic type of condition data and detector
 *  elements in the transient data store
 *
 *  @author Sebastien Ponce 
 *  @author Andrea Valassi 
 *  @author Marco Clemencic
*/
class ParamValidDataObject : public ValidDataObject {

 public: 
  
  /// Default constructor
  ParamValidDataObject();

  /// Copy constructor 
  ParamValidDataObject (const ParamValidDataObject& obj);
  
  /// Destructor 
  virtual ~ParamValidDataObject();
  
 public:

  /// Reset to the initial state.
  void reset();

 public:

  // Re-implemented from ValidDataObject
  using ValidDataObject::update; // due to partial override
  /// Do the deep copy
  virtual void update ( ValidDataObject& obj );
  
  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream( std::ostream& s ) const;

 public:

  /// Check if the parameter with the given name exists.
  bool exists(const std::string &name) const;
  
  /// TypeId of the parameter.
  virtual const std::type_info &type(const std::string &name) const;

  /// Check the type of a parameter (commodity function).
  template <class T>
  inline bool is(const std::string &name) const {
    return type(name) == typeid(T);
  }
  
  /// Check if the parameter is a vector.
  bool isVector(const std::string &name) const;

  /// Get the comment of a parameter.
  virtual std::string comment (const std::string &name) const;
  
  /// Set the comment of a parameter.
  virtual void setComment (const std::string &name, const char *comm);
  
  /// Give a read-only accessor to a parameter.
  template <class T>
  inline const T &param(const std::string &name) const {
    ParamList::const_iterator i = m_paramList.find(name);
    if ( i == m_paramList.end() ) throw ParamException(name);
    try {
      return i->second-> template get<T>();
    } catch (std::bad_cast) {
      throw ParamException(name,typeid(T),i->second->type());
    }
  }
  
  /// Give a read/write accessor to a parameter.
  template <class T>
  inline T &param(const std::string &name) {
    ParamList::iterator i = m_paramList.find(name);
    if ( i == m_paramList.end() ) throw ParamException(name);
    try {
      return i->second-> template get<T>();
    } catch (std::bad_cast) {
      throw ParamException(name,typeid(T),i->second->type());
    }
  }
  
  /// Give a read-only accessor to a parameter vector.
  template <class T>
  inline const std::vector<T> &paramVect(const std::string &name) const {
    return param<std::vector<T> >(name);
  }
  
  /// Give a read/write accessor to a parameter vector.
  template <class T>
  inline std::vector<T> &paramVect(const std::string &name) {
    return param<std::vector<T> >(name);
  }
  
  /// Get the value of a parameter, as a string.
  virtual const std::string &paramAsString (const std::string &name) const;
  
  /// Get the value of a parameter, as a string (non const version).
  virtual std::string &paramAsString (const std::string &name);
  
  /// Get the value of a parameter, as an int.
  virtual int paramAsInt (const std::string &name) const;
  
  /// Get the value of a parameter, as a double.
  virtual double paramAsDouble (const std::string &name) const;
  
  /// Get the value of a parameter, as a string.
  virtual const std::vector<std::string> &paramAsStringVect (const std::string &name) const;
  
  /// Get the value of a parameter, as a string.
  virtual std::vector<std::string> &paramAsStringVect (const std::string &name);
  
  /// Get the value of a parameter, as an int.
  virtual const std::vector<int> &paramAsIntVect (const std::string &name) const;
  
  /// Get the value of a parameter, as an int.
  virtual std::vector<int> &paramAsIntVect (const std::string &name);
  
  /// Get the value of a parameter, as a double.
  virtual const std::vector<double> &paramAsDoubleVect (const std::string &name) const;
  
  /// Get the value of a parameter, as a double.
  virtual std::vector<double> &paramAsDoubleVect (const std::string &name);

  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> paramNames() const;

  /// Print the parameters on a string.
  virtual std::string printParams() const ;
  
  /// Convert a parameter to a string (for xml representation).
  virtual std::string paramToString(const std::string &name, int precision) const ;
 
public:

  template <class T>
  inline void addParam(const std::string &name, const T &value, const std::string &comment=std::string()){
  	m_paramList.add(name,value);
  	if (!comment.empty()) {
  	  m_comments[name] = comment;
  	}
  }

  inline void addBasicParam(const std::string &name, const BasicParam *p, const std::string &comment=std::string()){
  	m_paramList.addBasicParam(name,p);
  	if (!comment.empty()) {
  	  m_comments[name] = comment;
  	}
  }

private:
  
  typedef std::map<std::string,std::string> CommentMap;
  
  ParamList m_paramList;
  CommentMap m_comments;
  
};

#endif // DETDESC_PARAMVALIDDATAOBJECT_H
