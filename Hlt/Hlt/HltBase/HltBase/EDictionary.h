#ifndef dictionary_h
#define dictionary_h 1

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>
#include "boost/any.hpp"

namespace Estd {

  //! Key of the dictionary is a std::string
  typedef std::string Key;
  typedef std::vector<Key> key_vector;
  typedef std::vector<Key>::iterator key_iterator;
  typedef std::vector<Key>::const_iterator key_const_iterator;

  //! exception class: not valid key
  class invalid_key : public std::logic_error {
  public:
    invalid_key(const Estd::Key& key):logic_error(key) {
      std::cerr << "--ERROR: Key '" << key << "' not in dictionary" << std::endl;
    }
  };

  class dictionary  {
  public:

    //! default constructor
    dictionary() { }

    //! default destructor
    virtual ~dictionary() 
      {clear();}

    template <class T>
    bool add(const Key& key, const T& t){
      if (has_key(key)) return false;
      _map.insert( std::make_pair(key, boost::any(t)) );
      _keys.push_back(key);
      return true;
    }
    
    bool has_key(const Key& key) const 
    {return (_map.find(key) != _map.end());}

    const std::vector<Key>& keys() const {return _keys;}
    
    size_t size() const {return _keys.size();}
    bool empty() const {return _keys.empty();}
    
    //! return/access the element by key
    //! if it is not there raise an exception
    template <class T>
    T& retrieve(const Key& key) {
      if (!has_key(key)) throw invalid_key(key);
      return boost::any_cast<T&>(_map[key]);
    }

    template <class T>
    const T& retrieve(const Key& key) const {
      if (!has_key(key)) throw invalid_key(key);
      return boost::any_cast<T&>(_map[key]);
    } 

    //! clear the dictionary
    virtual void clear() {
      _map.clear();
      _keys.clear();
    }
    
  private:
    //! map of the item roots
    std::vector<Key> _keys;
    std::map<Key,boost::any> _map;
    
  };
};
# endif
