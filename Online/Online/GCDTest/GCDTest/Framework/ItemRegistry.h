#ifndef FRAMEWORK_ITEMREGISTRY_H
#define FRAMEWORK_ITEMREGISTRY_H

// C++ include files
#include <map>
#include <string>
#include <vector>

// Framework include files
#include "Framework/AVX.h"

/*
*    Framework namespace declaration
*/
namespace Framework {

  /** @class ItemRegistry ItemRegistry.h Framework/ItemRegistry.h
  *  Any item to be registered requires that it can be hashed into a 4 byte integer
  *   
  *
  * @author  M.Frank
  * @version 1.0
  * @date    20/06/2011
  */
  class ItemRegistry   {
  public:
    typedef AVXHeap                   Mask_t;
    typedef std::map<int,size_t>      Items_t;
    typedef std::map<int,std::string> StringMap;
  protected:
    /// AVX item mask
    Mask_t  _mask;
    /// map of hashed items
    Items_t _items;
    /// Map of string values
    StringMap m_stringMap;
  public:
    /// Default constructor
    ItemRegistry()  {}
    /// Default destructor
    virtual ~ItemRegistry() {}
    /// Lookup value by bit
    std::string lookup(int bit) const;
    /// Print missing bits
    void printMissing(const AVXMemory& required, const AVXMemory& availible) const;
    /// Register a sinlge item
    template <typename T> int    registerItem(const T& item);
    /// Register a group of items
    template <typename T> Mask_t registerItems(const std::vector<T>& items);
  };
}
#endif // FRAMEWORK_ITEMREGISTRY_H
