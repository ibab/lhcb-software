// $Id: CondDBCache.h,v 1.1 2005-06-23 14:14:46 marcocle Exp $
#ifndef COMPONENT_CONDDBCACHE_H 
#define COMPONENT_CONDDBCACHE_H 1

// Include files
#include <string>
#include <map>
#include <vector>
#include <list>

#include "GaudiKernel/MsgStream.h"

#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IFolder.h"

#include "AttributeList/AttributeValueAccessor.h"

/** @class CondDBCache CondDBCache.h component/CondDBCache.h
 *  
 *  Class used to manage in memory conditions.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-13
 */
class CondDBCache {

public:

  typedef std::pair<cool::ValidityKey,cool::ValidityKey> IOVType;

  //--------------------------------------------------------------------------------
  /// Standard constructor
  CondDBCache(const MsgStream& log, size_t highLevel = 100, size_t lowLevel = 10);

  virtual ~CondDBCache( ); ///< Destructor

  /// Add a new data object to the cache.
  /// \warning {no check performed}
  bool insert(const cool::IFolderPtr &folder,const cool::IObjectPtr &obj);

  bool addFolder(const std::string &path, const std::string &descr, const pool::AttributeListSpecification& spec);
  bool addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                 const pool::AttributeList& al, IOVType *iov_before = NULL);

  /// Search an entry in the cache and returns the data string or an empty string if no object is found.
  bool get(const std::string &path, const cool::ValidityKey &when,
           cool::ValidityKey &since, cool::ValidityKey &until,
           std::string &descr, boost::shared_ptr<pool::AttributeList> &payload) ;

  /// Remove all entries from the cache;
  inline void clear() {m_cache.clear();}
  
  /// Get the number of items cached.
  inline size_t size() const {
    size_t count = 0;
    std::map<std::string,CondFolder>::const_iterator folder;
    for (folder = m_cache.begin(); folder != m_cache.end(); ++folder)
      count += folder->second.items.size();
    return count;
  }
  
  inline void setHighLevel(size_t lvl) { m_highLvl = lvl; }
  inline void setLowLevel(size_t lvl) { m_lowLvl = lvl; }
  inline size_t highLevel() const { return m_highLvl; }
  inline size_t lowLevel() const { return m_lowLvl; }
  
  inline size_t level() const { return m_level; }
  
  void clean_up();

  /// Check if the given path is present in the cash.
  inline bool hasPath(const std::string &path) const { return m_cache.count(path); }
  
  /// Check if the given path,time pair is present in the cash.
  bool hasTime(const std::string &path, const cool::ValidityKey &when) const;

  void dump();
  
protected:

private:

  struct CondFolder;
  struct CondItem;

  typedef std::string FolderIdType;
  //typedef std::vector<CondItem> ItemListType;
  typedef std::list<CondItem> ItemListType;
  //  typedef std::map<FolderIdType,CondFolder> FolderListType;
  typedef std::map<FolderIdType,CondFolder> StorageType;
  

  /// Internal class used to record IOV+data pairs
  struct CondItem {
    /// Constructor.
    CondItem(CondFolder *myFolder, const cool::IObjectPtr &obj):
      folder(myFolder),iov(obj->since(),obj->until()), data(new pool::AttributeList(folder->spec)),score(1.0) {
      for(pool::AttributeListSpecification::const_iterator a = folder->spec->begin();
          a != folder->spec->end(); ++a) {
        pool::AttributeValueAccessor(obj->payload()[a->name()])
          .copyData(pool::AttributeValueAccessor((*data)[a->name()]).getMemoryAddress());
      }
    }
    CondItem(CondFolder *myFolder, const cool::ValidityKey &since, const cool::ValidityKey &until,
             const pool::AttributeList &al):
      folder(myFolder),iov(since,until), data(new pool::AttributeList(folder->spec)),score(1.0) {
      for(pool::AttributeListSpecification::const_iterator a = folder->spec->begin();
          a != folder->spec->end(); ++a) {
        pool::AttributeValueAccessor(al[a->name()])
          .copyData(pool::AttributeValueAccessor((*data)[a->name()]).getMemoryAddress());
      }
    }
    CondFolder *folder;
    IOVType iov;
    boost::shared_ptr<pool::AttributeList> data;
    float score;
    /// Check if the CondItem is valid at the given time.
    inline bool valid(const cool::ValidityKey &when) const {
      return iov.first <= when && when < iov.second;
    }
  };

  /// Internal class used to keep the items common to a given path.
  struct CondFolder {

    CondFolder(const cool::IFolderPtr &fld):
      description(fld->description()),spec(new pool::AttributeListSpecification),sticky(false) {
      for (pool::AttributeListSpecification::const_iterator a = fld->payloadSpecification().begin();
           a != fld->payloadSpecification().end(); ++a) {
        spec->push_back(a->name(),a->type());
      }
      //spec->append_and_merge(fld->payloadSpecification());
    }
    CondFolder(const std::string &descr, const pool::AttributeListSpecification& new_spec):
      description(descr),spec(new pool::AttributeListSpecification),sticky(true) {
      for (pool::AttributeListSpecification::const_iterator a = new_spec.begin();
           a != new_spec.end(); ++a) {
        spec->push_back(a->name(),a->type());
      }
    }
    std::string description;
    boost::shared_ptr<pool::AttributeListSpecification> spec;
    ItemListType items;
    bool sticky;
    /// Search for the first item in the storage valid at the given time.
    inline ItemListType::iterator find(const cool::ValidityKey &when) {
      ItemListType::iterator i;
      for ( i = items.begin(); i != items.end() && !i->valid(when) ; ++i ){}
      return i;
    }
    /// Const version of the search method.
    inline ItemListType::const_iterator find(const cool::ValidityKey &when) const {
      ItemListType::const_iterator i;
      for ( i = items.begin(); i != items.end() && !i->valid(when) ; ++i ){}
      return i;
    }
    inline ItemListType::iterator conflict(const cool::ValidityKey &since, const cool::ValidityKey &until) {
      ItemListType::iterator i;
      for ( i = items.begin(); i != items.end() ; --i ){
        if ( ( i->iov.first >= since ? i->iov.first : since ) <  ( i->iov.second <= until ? i->iov.second : until ) ) return i;
      }
      return i;
    }
    inline ItemListType::const_iterator conflict(const cool::ValidityKey &since, const cool::ValidityKey &until) const {
      ItemListType::const_iterator i;
      for ( i = items.begin(); i != items.end() ; --i ){
        /*
        if ( ( i->iov.first >= since && i->iov.first < until )
             || ( i->iov.second > since && i->iov.second < until )
             || ( i->iov.first < since && i->iov.second >= until ) ) {
          std::cout << "old " << i->iov.first << " - " << i->iov.second << std::endl;
          std::cout << "new " << since << " - " << until << std::endl;
          return i;
          }
        */
        if ( ( i->iov.first >= since ? i->iov.first : since ) <  ( i->iov.second <= until ? i->iov.second : until ) ) return i;
      }
      return i;
    }
    inline void erase (const cool::ValidityKey &when) { items.erase(find(when)); }
    inline bool empty() const { return items.empty(); }
      
  };

  /// Actual storage
  StorageType m_cache;

  size_t m_highLvl;
  size_t m_lowLvl;
  size_t m_level;

  MsgStream m_log;

  cool::ValidityKey m_lastRequestedTime;
};
#endif // COMPONENT_CONDDBCACHE_H
