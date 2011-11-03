// Framework include files
#include "Framework/ItemRegistry.h"
#include "Framework/Helpers.h"
#include "RTL/rtl.h"

// C++ include files
#include <climits>

using namespace std;

namespace Framework {

  /// Register a single item if type size_t
  template <> int ItemRegistry::registerItem<size_t>(const size_t& hash)  {
    Items_t::const_iterator i = _items.find(hash);
    if ( i != _items.end() )  {
      return (*i).second;
    }
    char text[64];
    size_t len = _items.size();
    _items[hash] = len;
    _mask.extend(len);
    _mask.set_bit(len);
    ::sprintf(text,"Hash-item-%016lX",long(hash));
    m_stringMap[len] = text;
    return len;
  }

  /// Register a single item if type char
  template <> int ItemRegistry::registerItem<char>(const char& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type unsigned char
  template <> int ItemRegistry::registerItem<unsigned char>(const unsigned char& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type short
  template <> int ItemRegistry::registerItem<short>(const short& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type unsigned short
  template <> int ItemRegistry::registerItem<unsigned short>(const unsigned short& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type int
  template <> int ItemRegistry::registerItem<int>(const int& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type unsigned int
  template <> int ItemRegistry::registerItem<unsigned int>(const unsigned int& item)  {
    return registerItem<size_t>(size_t(item));
  }

  /// Register a single item if type long
  template <> int ItemRegistry::registerItem<long>(const long& item)  {
    return registerItem<size_t>(size_t(item));
  }

#if ULONG_MAX==UINT_MAX
  /// Register a single item if type unsigned long
  template <> int ItemRegistry::registerItem<unsigned long>(const unsigned long& item)  {
    return registerItem<size_t>(size_t(item));
  }
#endif

  /// Register a single item if type string
  template <> int ItemRegistry::registerItem<string>(const string& item)  {
    int result = registerItem<size_t>(hash32(item));
    m_stringMap[result] = item;
    return result;
  }

  string ItemRegistry::lookup(int bit)  const {
    StringMap::const_iterator i=m_stringMap.find(bit);
    if ( i == m_stringMap.end() )  return "Unknown";
    return (*i).second;
  }

  /// Print missing bits
  void ItemRegistry::printMissing(const AVXMemory& required, const AVXMemory& availible) const {
    AVXMemory::Registers r = required;
    for(size_t i=0; i<r.second; ++i) {
      if ( required.test_bit(i) && !availible.test_bit(i) ) {
	string miss = lookup(i);
	::lib_rtl_output(LIB_RTL_ALWAYS,"Missing IO Bit[%d] : %s",int(i),miss.c_str());
      }
    }
  }

  /// Register a single item if type char
  template <> ItemRegistry::Mask_t ItemRegistry::registerItems(const vector<string>& items)  {
    size_t len = _items.size()+items.size();
    Mask_t result(len);
    _mask.extend(len);
    for(vector<string>::const_iterator i=items.begin(); i!=items.end();++i)  {
      int bit = registerItem(*i);
      int wrd = bit/32;
      ::lib_rtl_output(LIB_RTL_DEBUG,"Register IO Bit[%d] %d : %d 0x%08X as %d",bit,wrd,bit%32,hash32(*i),(*i).c_str());
      result.set_bit(bit);
    }
    return result;
  }
}
