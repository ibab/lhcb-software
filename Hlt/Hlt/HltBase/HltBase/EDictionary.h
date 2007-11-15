#ifndef dictionary_h
#define dictionary_h 1

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <stdexcept>

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
      std::cout << "--ERROR: not valid key in dictionary '" 
                << key << "' "<< std::endl;
    }
  };

  class dictionary  {
  protected:
    
    //! internal class for the root of a item
    class Item_root {
    public:
      Key _key;
    public:
      Item_root(const Key& key):_key(key) {}
      virtual ~Item_root() {}
      const Key& key() {return _key;}
    };
    
    //! internal template class for an iterm
    template <class T>
    class Item : public Item_root {
    public:
      T _t; 
    public:
      Item(const Key& key, const T& t ):Item_root(key) {_t = t;} 
      virtual ~Item() {}
      const T& item() const {return _t;};
      T& item() {return _t;}
    };
    
  public:

    //! default constructor
    dictionary() { }

    //! default destructor
    virtual ~dictionary() 
      {clear();}

    template <class T>
    bool add(const Key& key, const T& t){
      if (has_key(key)) return false;
      _map[key] = new Item<T>(key,t);
      _keys.push_back(key);
      return true;
    }
    
    // template <class T>
    // bool add(const Key& key, const T& t)
    // {return register(key,t);}
    
    bool has_key(const Key& key) const 
    {return (_map.find(key) != _map.end());}

    const std::vector<Key>& keys() const {return _keys;}
    
    size_t size() const {return _keys.size();}
    
    //! return/access the element by key
    //! if it is not there raise an exception
    template <class T>
    T& retrieve(const Key& key) {
      if (!has_key(key)) throw invalid_key(key);
      return dynamic_cast<Item<T>&>(*_map[key]).item();
    }

    template <class T>
    const T& retrieve(const Key& key) const {
      if (!has_key(key)) throw invalid_key(key);
      return dynamic_cast<Item<T>&>(*_map.find(key)->second()).item();
    } 

    //! clear the dictionary
    virtual void clear() {
      _map.clear();
      _keys.clear();
    }
    
  protected:
    
    //! map of the item roots
    std::vector<Key> _keys;
    std::map<Key,Item_root*> _map;
    
  };
};
# endif
