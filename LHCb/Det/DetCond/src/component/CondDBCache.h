// $Id: CondDBCache.h,v 1.5 2006-04-25 17:20:20 marcocle Exp $
#ifndef COMPONENT_CONDDBCACHE_H 
#define COMPONENT_CONDDBCACHE_H 1

// Include files
#include <string>
#include <vector>
#include <list>

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/HashMap.h"

#include "CoolKernel/types.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IFolder.h"

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
  bool insert(const cool::IFolderPtr &folder,const cool::IObjectPtr &obj, const cool::ChannelId &channel = 0);

  bool addFolder(const std::string &path, const std::string &descr, const cool::ExtendedAttributeListSpecification& spec);
  bool addFolderSet(const std::string &path, const std::string &descr);
  bool addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                 const coral::AttributeList& al, const cool::ChannelId &channel, IOVType *iov_before = NULL);
  /// (version kept for backward compatibility)
  inline bool addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                        const coral::AttributeList& al, IOVType *iov_before = NULL)
  {
    return addObject(path,since,until,al,0,iov_before);
  }
  

  /// Search an entry in the cache and returns the data string or an empty string if no object is found.
  bool get(const std::string &path, const cool::ValidityKey &when,
           const cool::ChannelId &channel,
           cool::ValidityKey &since, cool::ValidityKey &until,
           std::string &descr, boost::shared_ptr<coral::AttributeList> &payload) ;

  /// Search an entry in the cache and returns the data string or an empty string if no object is found.
  /// (version kept for backward compatibility)
  inline bool get(const std::string &path, const cool::ValidityKey &when,
                  cool::ValidityKey &since, cool::ValidityKey &until,
                  std::string &descr, boost::shared_ptr<coral::AttributeList> &payload) {
    return get(path,when,0,since,until,descr,payload);
  }

  void getSubNodes(const std::string &path, std::vector<std::string> &node_names);

  /// Remove all entries from the cache;
  inline void clear() {m_cache.clear();}
  
  /// Get the number of items cached.
  inline size_t size() const;
  
  inline void setHighLevel(size_t lvl) { m_highLvl = lvl; }
  inline void setLowLevel(size_t lvl) { m_lowLvl = lvl; }
  inline size_t highLevel() const { return m_highLvl; }
  inline size_t lowLevel() const { return m_lowLvl; }
  
  inline size_t level() const { return m_level; }
  
  void clean_up();

  /// Check if the given path is present in the cash.
  inline bool hasPath(const std::string &path) const { return m_cache.count(path); }
  
  /// Check if the given path,time pair is present in the cash.
  bool hasTime(const std::string &path, const cool::ValidityKey &when, const cool::ChannelId &channel = 0) const;

  void dump();
  
protected:

private:

  struct CondFolder;
  struct CondItem;

  typedef std::string FolderIdType;
  //typedef std::vector<CondItem> ItemListType;
  typedef std::list<CondItem> ItemListType;
  //  typedef std::map<FolderIdType,CondFolder> FolderListType;
  typedef GaudiUtils::HashMap<FolderIdType,CondFolder> StorageType;  

  /// Internal class used to record IOV+data pairs
  struct CondItem {
    /// Constructor.
    CondItem(CondFolder *myFolder, const cool::IObjectPtr &obj):
      folder(myFolder),iov(obj->since(),obj->until()),
      data(new coral::AttributeList(obj->payload())),score(1.0) {}
    CondItem(CondFolder *myFolder, const cool::ValidityKey &since, const cool::ValidityKey &until,
             const coral::AttributeList &al):
      folder(myFolder),iov(since,until), data(new coral::AttributeList(al)),score(1.0) {}
    CondFolder *folder;
    IOVType iov;
    boost::shared_ptr<coral::AttributeList> data;
    float score;
    /// Check if the CondItem is valid at the given time.
    inline bool valid(const cool::ValidityKey &when) const {
      return iov.first <= when && when < iov.second;
    }
  };

  /// Internal class used to keep the items common to a given path.
  struct CondFolder {

    typedef GaudiUtils::HashMap<cool::ChannelId,ItemListType> StorageType;
    
    CondFolder(const cool::IFolderPtr &fld):
      description(fld->description()),
      spec(new cool::ExtendedAttributeListSpecification(fld->extendedPayloadSpecification())),
      sticky(false) {}
    CondFolder(const std::string &descr, const cool::ExtendedAttributeListSpecification& new_spec):
      description(descr),spec(new cool::ExtendedAttributeListSpecification(new_spec)),sticky(true) {}
    // for a folderset (FolderSets are identified by missing spec)
    CondFolder(const std::string &descr):
      description(descr),sticky(true) {}
    std::string description;
    boost::shared_ptr<cool::ExtendedAttributeListSpecification> spec;
    StorageType items;
    bool sticky;
    /// Search for the first item in the storage valid at the given time.
    inline ItemListType::iterator find(const cool::ValidityKey &when, const cool::ChannelId &channel = 0) {
      ItemListType &lst = items[channel];
      ItemListType::iterator i;
      for ( i = lst.begin(); i != lst.end() && !i->valid(when) ; ++i ){}
      return i;
    }
    /// Const version of the search method.
    inline ItemListType::const_iterator find(const cool::ValidityKey &when, const cool::ChannelId &channel = 0) const {
      const ItemListType &lst = (*const_cast<StorageType *>(&items))[channel];
      ItemListType::const_iterator i;
      for ( i = lst.begin(); i != lst.end() && !i->valid(when) ; ++i ){}
      return i;
    }
    inline ItemListType::iterator conflict(const cool::ValidityKey &since, const cool::ValidityKey &until,
                                           const cool::ChannelId &channel = 0) {
      ItemListType &lst = items[channel];
      ItemListType::iterator i;
      for ( i = lst.begin(); i != lst.end() ; ++i ){
        if ( ( i->iov.first >= since ? i->iov.first : since ) < ( i->iov.second <= until ? i->iov.second : until ) ) return i;
      }
      return i;
    }
    inline ItemListType::const_iterator conflict(const cool::ValidityKey &since, const cool::ValidityKey &until,
                                                 const cool::ChannelId &channel = 0) const {
      const ItemListType &lst = (*const_cast<StorageType *>(&items))[channel];
      ItemListType::const_iterator i;
      for ( i = lst.begin(); i != lst.end() ; ++i ){
        if ( ( i->iov.first >= since ? i->iov.first : since ) < ( i->iov.second <= until ? i->iov.second : until ) ) return i;
      }
      return i;
    }
    inline void erase (const cool::ValidityKey &when, const cool::ChannelId &channel = 0) {
      items[channel].erase(find(when,channel));
    }
    inline bool empty() const {
      for (StorageType::const_iterator ch = items.begin(); ch != items.end(); ++ch ) {
        if (! ch->second.empty()) return false;
      }    
      return true;
    }
      
  };

  /// Actual storage
  StorageType m_cache;

  size_t m_highLvl;
  size_t m_lowLvl;
  size_t m_level;

  MsgStream m_log;

  cool::ValidityKey m_lastRequestedTime;
};

inline size_t CondDBCache::size() const {
  size_t count = 0;
  StorageType::const_iterator folder;
  for (folder = m_cache.begin(); folder != m_cache.end(); ++folder) {
    for (CondFolder::StorageType::const_iterator ch = folder->second.items.begin(); ch != folder->second.items.end(); ++ch)
      count += ch->second.size();
  }
  return count;
}

#endif // COMPONENT_CONDDBCACHE_H
