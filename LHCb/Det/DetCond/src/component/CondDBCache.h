// $Id: CondDBCache.h,v 1.9 2008-06-26 14:22:45 marcocle Exp $
#ifndef COMPONENT_CONDDBCACHE_H 
#define COMPONENT_CONDDBCACHE_H 1

// Include files
#include <string>
#include <vector>
#include <list>

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/HashMap.h"

#include "CoolKernel/types.h"
#include "CoolKernel/pointers.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IRecord.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/IRecordSpecification.h"
#include "CoolKernel/RecordSpecification.h"

#include "DetCond/ICondDBReader.h"

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

  bool addFolder(const std::string &path, const std::string &descr, const cool::IRecordSpecification& spec);
  bool addFolder(const std::string &path, const std::string &descr, const cool::IRecordSpecification& spec, 
                 const std::map<cool::ChannelId,std::string>& ch_names);
  bool addFolderSet(const std::string &path, const std::string &descr);
  bool addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                 const cool::IRecord& rec, const cool::ChannelId &channel, IOVType *iov_before = NULL);
  /// (version kept for backward compatibility)
  inline bool addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                        const cool::IRecord& rec, IOVType *iov_before = NULL)
  {
    return addObject(path,since,until,rec,0,iov_before);
  }
  

  /// Search an entry in the cache and returns the data string or an empty string if no object is found.
  bool get(const std::string &path, const cool::ValidityKey &when,
           const cool::ChannelId &channel,
           cool::ValidityKey &since, cool::ValidityKey &until,
           std::string &descr, ICondDBReader::DataPtr &payload);

  /// Search an entry in the cache and returns the data string or an empty string if no object is found.
  /// (version kept for backward compatibility)
  inline bool get(const std::string &path, const cool::ValidityKey &when,
                  cool::ValidityKey &since, cool::ValidityKey &until,
                  std::string &descr, ICondDBReader::DataPtr &payload) {
    return get(path,when,0,since,until,descr,payload);
  }

  /// Find the value of the channel id for the given channel name in a folder
  /// (if present in the cache).
  /// Returns true if the channel name in the folder was found
  bool getChannelId(const std::string &path,const std::string &name,
                    cool::ChannelId &channel) const;
  
  void getSubNodes(const std::string &path, std::vector<std::string> &node_names);

  void getSubNodes(const std::string &path, std::vector<std::string> &folders, std::vector<std::string> &foldersets);

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

  /// Check if the given path is present in the cache.
  inline bool hasPath(const std::string &path) const { return m_cache.count(path) != 0; }
  
  /// Check if the path is a folderset.
  inline bool isFolderSet(const std::string &path) const {
    return hasPath(path) && (m_cache.find(path)->second.spec.get() == 0);
  }

  /// Check if the path is a folderset.
  inline bool isFolder(const std::string &path) const {
    return hasPath(path) && (m_cache.find(path)->second.spec.get() != 0);
  }
  
  /// Check if the given path,time pair is present in the cache.
  bool hasTime(const std::string &path, const cool::ValidityKey &when, const cool::ChannelId &channel = 0) const;

  void dump();

  /// Set the flag to enable the check that the inserted IOVs are not compatible with the latest
  /// requested time (needed to avoid that the cache is modified for the current event).
  /// @return previous value
  bool setIOVCheck(bool enable) 
  {
    bool old = m_checkLastReqTime;
    m_checkLastReqTime = enable;
    return old;
  }

  /// Tell if the check on inserted IOVs is enabled.
  bool IOVCheck() { return m_checkLastReqTime; }

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
      data(new cool::Record(obj->payload())),score(1.0) {}
    CondItem(CondFolder *myFolder, const cool::ValidityKey &since, const cool::ValidityKey &until,
             const cool::IRecord &rec):
      folder(myFolder),iov(since,until), data(new cool::Record(rec)),score(1.0) {}
    CondFolder *folder;
    IOVType iov;
    ICondDBReader::DataPtr data;
    float score;
    /// Check if the CondItem is valid at the given time.
    inline bool valid(const cool::ValidityKey &when) const {
      return iov.first <= when && when < iov.second;
    }
  };

  /// Internal class used to keep the items common to a given path.
  struct CondFolder {

    typedef GaudiUtils::Map<cool::ChannelId,ItemListType> StorageType;
    typedef GaudiUtils::HashMap<std::string,cool::ChannelId> ChannelNamesMapType;
    
    CondFolder(const cool::IFolderPtr &fld):
      description(fld->description()),
      spec(new cool::RecordSpecification(fld->payloadSpecification())),
      sticky(false) {}
    CondFolder(const std::string &descr, const cool::IRecordSpecification& new_spec):
      description(descr),spec(new cool::RecordSpecification(new_spec)),sticky(true) {}
    // for a folderset (FolderSets are identified by missing spec)
    CondFolder(const std::string &descr):
      description(descr),sticky(true) {}
    std::string description;
    boost::shared_ptr<cool::IRecordSpecification> spec;
    StorageType items;
    ChannelNamesMapType channelNames;
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
  bool m_checkLastReqTime;
  
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
