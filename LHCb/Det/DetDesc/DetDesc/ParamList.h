// $Id: ParamList.h,v 1.2 2005-05-27 11:57:42 marcocle Exp $
#ifndef DETDESC_PARAMLIST_H 
#define DETDESC_PARAMLIST_H 1

// Include files

#include <DetDesc/Param.h>

#include <map>


/** @class ParamList ParamList.h DetDesc/ParamList.h
 *  
 *  Simple implementation of a polimorfic list of parameters idetified by
 *  a std::string.
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-02-22
 */
class ParamList: public std::map<std::string,BasicParam *> {
private:
  typedef std::map<std::string,BasicParam *> map_type;

public: 
  /// Standard constructor
  ParamList();

  /// Copy constructor
  ParamList(const ParamList &pl);

  /// Destructor
  virtual ~ParamList();

  /// Add a new parameter to the list (or replace if already there)
  template <class T>
  void add(const std::string &key, const T &val) {
    iterator i = find(key);
    if ( i != end() ) { // key already used
      i->second->set(val);
    } else {
      //(*this)[key] = new Param<T>(val);
      insert(std::pair<std::string,Param<T>*>(key,new Param<T>(val)));
    }
  }

  /// return a vector containing all the stored keys
  virtual std::vector<std::string> getKeys() const;

  /// Copy a list into this one (deleting this one)
  ParamList& operator= (const ParamList &pl);
  
  /// Merge two lists (overwriting objects with the same name)
  ParamList& operator+= (const ParamList &pl);
  
  /// Remove all elements from the list, deleting the objects
  void clear();

protected:

private:
  void deleteItems();

};
#endif // DETDESC_PARAMLIST_H
