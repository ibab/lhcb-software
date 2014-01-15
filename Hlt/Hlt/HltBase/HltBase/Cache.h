// $Id:$ 
// ============================================================================
#ifndef HltEvent_CACHE_H_
#define HltEvent_CACHE_H_
// ============================================================================
// Incldue files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <map>
#include <vector>
#include <ostream>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/VectorMap.h"
// ============================================================================
/** @file   Cache.h
 *  @author alexander.mazurov@gmail.com
 *  @date   Jun 15, 2010
 *  @brief  Cache implementation for stage objects
 */
// ============================================================================
namespace Hlt
{
  // ==========================================================================
  using Gaudi::StringKey;
  using GaudiUtils::VectorMap;
  // ==========================================================================
  /** @class Cache 
   *  @brief  Cache implementation for stage objects
   *
   *  Cache for stage objects is a string -> value mapping.
   *
   *  @author alexander.mazurov@gmail.com
   *  @date   Jun 15, 2010
   *  @attention Currently implementation is bases on VectorMap class and uses
   *            StringKey as key. After some tests those classes may be will be
   *            replaced by more time and memory effective competitor.
   * @see Hlt::Stage
   */
  // ==========================================================================
  class Cache
  {
    // ========================================================================
  public:
    // ========================================================================
    /// the actual data types for cache 
    enum Values {
      Int         ,  
      Bool        , 
      Double      , 
      String      
    } ;  
    // ==========================================================================
  public:
    // ========================================================================
    typedef Gaudi::StringKey                              KeyType    ;
    typedef GaudiUtils::VectorMap<StringKey, bool>        BooleanMap ;
    typedef GaudiUtils::VectorMap<StringKey, int>         IntegerMap ;
    typedef GaudiUtils::VectorMap<StringKey, double>      DoubleMap  ;
    typedef GaudiUtils::VectorMap<StringKey, std::string> StringMap  ;
    // ========================================================================
    /**
     * @brief Check if cache contains value with the specified key
     * @param[in] key
     */
    bool has(const StringKey& key) const;
    /**
     * @brief Erase all values with the specified key
     * @param[in] key
     */
    void erase(const StringKey& key);
    /**
     * @brief Return a list of existent keys
     */
    const std::vector<const StringKey*> keys() const;
    /// Check if cache contains value with the specified key and type
    template<typename T> bool has_(const StringKey& key) const;
    /** Add value with the specified type and key
     *  @return true if value was added and false if we try insert an existing key
     */
    template<typename T> bool insert(const StringKey& key, const T& value);
    /** Update value with the specified type and key. If the value with the same
     * key already exists it will be replaced.
     */
    template<typename T> void update(const StringKey& key, const T& value);
    /**
     * Get the value associated with the specified type and key.
     * @returns The value associated with the specified type and key. If the such
     * value does not exists def is returned.
     */
    template<typename T> const T& info(const StringKey& key, const T& def) const;
    /// Cache to string 
    std::string toString() const;
    /// Fill the ASCII output stream
    std::ostream& fillStream(std::ostream& s) const;
    // ========================================================================
  private:
    // ========================================================================
    BooleanMap m_boolean_store ;
    IntegerMap m_integer_store ;
    DoubleMap  m_double_store  ;
    StringMap  m_string_store  ;
    // ========================================================================
    template<typename T> void _insert(const StringKey& key, const T& value);
    template<typename T> void _update(const StringKey& key, const T& value);
    // ========================================================================
  }; // Cache
  // ==========================================================================
  template<typename T> 
  inline bool Cache::insert( const StringKey& key, const T& value) 
  {
    if (has(key)) return false;
    _insert(key,value);
    return true;
  }
  template<typename T> 
  inline void Cache::update ( const StringKey& key,const T& value) 
  {
    erase(key);
    _update(key,value);
  }
  // ==========================================================================
  // Boolean
  // ==========================================================================
  template<> 
  inline bool Cache::has_<bool>(const StringKey& key) const 
  {
    return  m_boolean_store.find(key) != m_boolean_store.end();
  }
  template<> 
  inline void Cache::_insert(const StringKey& key, const bool& value) 
  {
    m_boolean_store.insert(key,value);
  }
  template<> 
  inline void Cache::_update(const StringKey& key,
                             const bool& value) 
  {
    m_boolean_store.update(key,value);
  }
  template<> 
  inline const bool& Cache::info<bool>(const StringKey& key,
                                       const bool& def) const 
  {
    auto i = m_boolean_store.find(key);
    return i != m_boolean_store.end() ? i->second : def ;
  }
  // ==========================================================================
  // Integer
  // ==========================================================================
  template<> 
  inline bool Cache::has_<int>(const StringKey& key) const 
  {
    return  m_integer_store.find(key) != m_integer_store.end();
  }
  template<> 
  inline void Cache::_insert(const StringKey& key,
                             const int& value) 
  {
    m_integer_store.insert(key, value);
  }
  template<> 
  inline void Cache::_update(const StringKey& key,
                             const int& value) 
  {
    m_integer_store.update(key, value);
  }
  template<> 
  inline const int& Cache::info<int>(const StringKey& key,
                                     const int& def) const 
  {
    auto i = m_integer_store.find(key);
    return i != m_integer_store.end() ? i->second : def ;
  }
  // ==========================================================================
  // Double
  // ==========================================================================
  template<> inline bool Cache::has_<double>(const StringKey& key) const 
  {
    return  m_double_store.find(key) != m_double_store.end();
  }
  template<> inline void Cache::_insert(const StringKey& key,
                                        const double& value) 
  {
    m_double_store.insert(key, value);
  }
  template<> 
  inline void Cache::_update(const StringKey& key,
                             const double& value) 
  {
    m_double_store.update(key, value);
  }
  template<> 
  inline const double& Cache::info<double>(const StringKey& key,
                                           const double& def)  const 
  {
    auto i = m_double_store.find(key);
    return i != m_double_store.end() ? i->second : def ;
  }
  // ==========================================================================
  // String
  // ==========================================================================
  template<> 
  inline bool Cache::has_<std::string>(const StringKey& key) const 
  {
    return  m_string_store.find(key) != m_string_store.end();
  }
  template<> 
  inline void Cache::_insert(const StringKey& key,
                             const std::string& value) 
  {
    m_string_store.insert(key, value);
  }
  template<> 
  inline void Cache::_update(const StringKey& key,
                             const std::string& value) 
  {
    m_string_store.update(key, value);
  }
  template<>
  inline const std::string& Cache::info<std::string>(const StringKey& key,
                                                     const std::string& def) const 
  {
    auto i = m_string_store.find(key);
    return i != m_string_store.end() ? i->second : def ;
  }
  // =========================================================================
  inline bool Cache::has(const StringKey& key) const 
  {
    return has_<bool>(key)   || has_<int>(key) 
        || has_<double>(key) || has_<std::string>(key);
  }
  // ==========================================================================
  inline void Cache::erase(const StringKey& key) 
  {
    m_boolean_store.erase(key);
    m_integer_store.erase(key);
    m_double_store.erase(key);
    m_string_store.erase(key);
  }
  // ==========================================================================
  inline std::ostream& operator << (std::ostream& str, const Cache& obj) 
  {
    return obj.fillStream(str);
  }
  // ==========================================================================
  inline std::string Cache::toString() const 
  {
    std::ostringstream s ;
    fillStream(s) ;
    return s.str();
  }
  // ==========================================================================
  template <unsigned int> 
  struct CacheValues ;
  // ==========================================================================
  template <>
  struct CacheValues<Hlt::Cache::Int>    { typedef int         Type ; } ;   
  // ==========================================================================
  template <>
  struct CacheValues<Hlt::Cache::Bool>   { typedef bool        Type ; } ;
  // ==========================================================================
  template <>
  struct CacheValues<Hlt::Cache::Double> { typedef double      Type ; } ;
  // ==========================================================================
  template <>
  struct CacheValues<Hlt::Cache::String> { typedef std::string Type ; } ;
  // ==========================================================================
} //                                                       end of namespace Hlt
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif /* HltEvent_CACHE_H_ */
// ============================================================================
