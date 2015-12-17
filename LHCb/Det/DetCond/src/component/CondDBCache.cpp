// Include files
#include "CondDBCache.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBCache
//
// 2005-06-13 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBCache::CondDBCache(const MsgStream& log, size_t highLvl, size_t lowLvl):
  m_highLvl(highLvl), m_lowLvl(lowLvl),
  m_level(0),
  m_log(log),
  m_lastRequestedTime(0), m_checkLastReqTime(true),
  m_silentConflicts(false)
{
  if ( highLvl == 0 ) {
    m_log << MSG::WARNING << "High level == 0 : forced to 100" << endmsg;
    m_highLvl = 100;
  }
  if ( highLvl <= lowLvl ) {
    m_log << MSG::WARNING << "High level <= low level : low level forced to 0" << endmsg;
    m_lowLvl = 0;
  }
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Cache initialized with high/low levels = " <<
      m_highLvl << '/' << m_lowLvl << endmsg;
}
//=============================================================================
// Destructor
//=============================================================================
CondDBCache::~CondDBCache() {
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Cache deleted. Level was = " << m_level << endmsg;
}

//=========================================================================
//  Add a new item to the cache
//=========================================================================
bool CondDBCache::insert(const cool::IFolderPtr &folder,const cool::IObject &obj, const cool::ChannelId &channel) {
  // increment object count and check the limit
  if ( m_level >= highLevel() ){
    // needs clean up
    if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
      m_log << MSG::DEBUG << "Level above max threshold" << endmsg;
    clean_up();
  }
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Insert  Folder '" << folder->fullPath()
          << "', IOV : " << obj.since() << " - " << obj.until()
          << ", channel : " << channel << endmsg;

  FolderIdType id(folder->fullPath());
  StorageType::iterator f = m_cache.find(id);
  if (f == m_cache.end()){
    f = m_cache.insert(StorageType::value_type(id, CondFolder(folder))).first;
    // Fill the map of channel names.
    std::map<cool::ChannelId,std::string>::const_iterator p;
    std::map<cool::ChannelId,std::string> ch_names = folder->listChannelsWithNames();
    for (p = ch_names.begin(); p != ch_names.end(); ++p) {
      f->second.channelNames[p->second] = p->first;
    }
  } else {
    if (f->second.items[channel].end() != f->second.conflict(obj.since(), obj.until(), channel)) {
      const MSG::Level lvl = (m_silentConflicts ? MSG::DEBUG : MSG::WARNING);
      m_log << lvl << "Conflict found: item not inserted" << endmsg;
      ItemListType::iterator i = f->second.conflict(obj.since(), obj.until(), channel);
      m_log << lvl << " IOV : " << i->iov.first << " - " << i->iov.second << endmsg;
      return false;
    }
  }
  // for vectors
  //  f->second.items.push_back(CondItem(&f->second,obj));
  // for lists
  ItemListType &items = f->second.items[channel];
  ItemListType::iterator pos = items.begin();
  while (pos != items.end() &&  pos->iov.first < obj.since()) {
    ++pos;
  }
  items.insert(pos, CondItem(&f->second, obj));

  ++m_level;
  return true;
}

//=========================================================================
//  Add a new folder using the given specification and description. (Bypass the real DB)
//=========================================================================
bool CondDBCache::addFolder(const std::string &path, const std::string &descr,
                            const cool::IRecordSpecification& spec) {
  StorageType::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    f = m_cache.insert(StorageType::value_type(path,CondFolder(descr,spec))).first;
  }
  return true;
}
bool CondDBCache::addFolder(const std::string &path, const std::string &descr,
                            const cool::IRecordSpecification& spec,
                            const std::map<cool::ChannelId,std::string>& ch_names) {
  StorageType::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    f = m_cache.insert(StorageType::value_type(path,CondFolder(descr,spec))).first;
    // Fill the map of channel names.
    std::map<cool::ChannelId,std::string>::const_iterator p;
    for (p = ch_names.begin(); p != ch_names.end(); ++p) {
      f->second.channelNames[p->second] = p->first;
    }
  }
  return true;
}

//=========================================================================
//  Add a new folder using the given specification and description. (Bypass the real DB)
//=========================================================================
bool CondDBCache::addFolderSet(const std::string &path, const std::string &descr) {
  StorageType::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    f = m_cache.insert(StorageType::value_type(path,CondFolder(descr))).first;
  }
  return true;
}

//=========================================================================
//  Add a new object to a given folder
//=========================================================================
bool CondDBCache::addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                            const cool::IRecord& rec, const cool::ChannelId &channel, IOVType *iov_before) {
  // new objects cannot be already valid. check it!
  if ( IOVCheck() && (m_lastRequestedTime != 0)
       && ( since <= m_lastRequestedTime && m_lastRequestedTime < until ) ) {
    m_log << (m_silentConflicts ? MSG::DEBUG : MSG::WARNING)
          << "New item IOV is compatible with last requested time:"
          << " not allowed to avoid inconsistencies" << endmsg;
    return false;
  }
  // increment object count and check the limit
  if ( m_level >= highLevel() ){
    // needs clean up
    if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
      m_log << MSG::DEBUG << "Level above max threshold" << endmsg;
    clean_up();
  }
  StorageType::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    m_log << MSG::WARNING << "Could not find the folder: object not added" << endmsg;
    return false;
  }
  if (!f->second.spec) { // no specification means FolderSet
    m_log << MSG::WARNING << '"' << path << '"' << " is a FolderSet: object not added" << endmsg;
    return false;
  }

  // when bypassing the DB, the conflicts must be solved
  /*
  if (f->second.conflict(since,until) != f->second.items.end()) {
      m_log << MSG::WARNING << "Conflict found: item not inserted" << endmsg;
      return false;
  }
  */
  // **** COOL single version style --> [x;+inf] + [y(>x);z] = [x;y], [y;z]
  // scan for conflicting items (from the end)
  ItemListType::iterator i = f->second.conflict(since,until,channel);
  if ( i != f->second.items[channel].end() ) { // conflict found
    if ( i->iov.second == cool::ValidityKeyMax && i->iov.first < since ) {
      // solvable conflict
      if (iov_before) *iov_before = i->iov;
      if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
        m_log << MSG::DEBUG << "Solvable conflict found: old until = " << i->iov.second << endmsg;
      i->iov.second = since;
      if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
        m_log << MSG::DEBUG << "                         new until = " << i->iov.second << endmsg;
    } else {
      // conflict not solvable
      m_log << MSG::WARNING << "Unsolvable conflict found: item not inserted" << endmsg;
      return false;
    }
  }
  // for vectors
  //  f->second.items.push_back(CondItem(&f->second,since,until,rec));
  // for lists
  f->second.items[channel].push_front(CondItem(&f->second,since,until,rec));
  ++m_level;
  return true;
}

//=========================================================================
//  Get data from given path and valid at given time
//=========================================================================
bool CondDBCache::get(const std::string &path, const cool::ValidityKey &when,
                      const cool::ChannelId &channel,
                      cool::ValidityKey &since, cool::ValidityKey &until,
                      std::string &descr, ICondDBReader::DataPtr &payload ) {
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Request Folder '" << path
          << "'  @ " << when << " channel " << channel;
  m_lastRequestedTime = when;
  StorageType::iterator folder = m_cache.find(path);
  if (folder != m_cache.end()) {
    if ( ! folder->second.spec ) {
      // It's a FolderSet! no objects inside
      since = cool::ValidityKeyMin;
      until = cool::ValidityKeyMax;
      descr = folder->second.description;
      payload.reset();
      if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
        m_log << " FOUND (FolderSet)" << endmsg;
      return true;
    }
    ItemListType::iterator i = folder->second.find(when,channel);
    if ( i != folder->second.items[channel].end() ) {
      since   = i->iov.first;
      until   = i->iov.second;
      descr   = folder->second.description;
      payload = i->data;
      //i->score += 1.0;
      if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
        m_log << " FOUND" << endmsg;
      return true;
    }
  }
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << " MISSING" << endmsg;
  return false;
}
//=========================================================================
//
//=========================================================================
bool CondDBCache::getChannelId(const std::string &path,const std::string &name,
                               cool::ChannelId &channel) const {
  StorageType::const_iterator f = m_cache.find(path);
  if ( m_cache.end() != f ) {
    CondFolder::ChannelNamesMapType::const_iterator id = f->second.channelNames.find(name);
    if ( f->second.channelNames.end() != id ) {
      channel = id->second;
      return true;
    }
  }
  channel = 0;
  return false;
}
//=========================================================================
//
//=========================================================================
void CondDBCache::getSubNodes (const std::string &path, std::vector<std::string> &folders, std::vector<std::string> &foldersets) {

  folders.clear();
  foldersets.clear();

  StorageType::iterator f;
  for ( f = m_cache.begin(); f != m_cache.end(); ++f ) {
    const std::string &p = f->first;
    if ( p.find(path) == 0  // the string must start with path
         && ( p.size() > path.size() ) // it must contain something more than the path
         && ( p.find('/',path.size()+1) == p.npos ) ) { // and I should have only one extra name
      if ( f->second.spec.get() ) {
        // this is a folder
        folders.push_back(p.substr(path.size()));
      } else {
        // this is a folderset
        foldersets.push_back(p.substr(path.size()));
      }
    }
  }
}
//=========================================================================
//
//=========================================================================
void CondDBCache::getSubNodes (const std::string &path, std::vector<std::string> &node_names) {
  // @todo: could be implemented as getSubNodes(path,node_names,node_names);

  node_names.clear();

  StorageType::iterator f;
  for ( f = m_cache.begin(); f != m_cache.end(); ++f ) {
    const std::string &p = f->first;
    if ( p.find(path) == 0  // the string must start with path
         && ( p.size() > path.size() ) // it must contain something more than the path
         && ( p.find('/',path.size()+1) == p.npos ) ) { // and I should have only one extra name
      node_names.push_back(p.substr(path.size()));
    }
  }
}
//=========================================================================
//  Remove unused entries from the cache
//=========================================================================

void CondDBCache::clean_up(){
  typedef std::vector<std::pair<float,std::pair<CondDBCache::CondFolder*,std::pair<cool::ValidityKey,cool::ChannelId> > > >
    _vec_t;
  _vec_t all_items;
  float score = 0;
  size_t old_level = level();

  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) ) {
    m_log << MSG::DEBUG << "Start cleaning up (level = " << level() << ")" << endmsg;
    m_log << MSG::DEBUG << "            Last requested time = " << m_lastRequestedTime << endmsg;
  }
  // collect all items info in order
  StorageType::iterator folder;
  for ( folder = m_cache.begin() ; folder != m_cache.end() ; ++folder ) {

    if ( ! folder->second.spec ) continue; // It's a FolderSet! no objects inside: skip it

    CondFolder::StorageType::iterator ch;
    ItemListType::iterator i;
    if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
      m_log << MSG::DEBUG << "Folder " << folder->first << endmsg;
    for ( ch = folder->second.items.begin(); ch != folder->second.items.end() ; ++ch ){
      if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
        m_log << MSG::DEBUG << " channel : " << ch->first << endmsg;
      for ( i = ch->second.begin(); i != ch->second.end() ; ++i ){
        if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
          m_log << MSG::DEBUG << "  IOV : " << i->iov.first << " - " << i->iov.second << endmsg;
        if ( ! (i->iov.first <= m_lastRequestedTime && i->iov.second > m_lastRequestedTime) ) {
          if ( m_lastRequestedTime < i->iov.first ) {
            score = (float)m_lastRequestedTime - i->iov.first;
          } else {
            score = (float)i->iov.second - m_lastRequestedTime;
          }
          if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
            m_log << MSG::DEBUG << "     score = " << score << endmsg;
          all_items.push_back(
             std::make_pair(score,
                std::make_pair(&folder->second,
                   std::make_pair(i->iov.first,ch->first))));
          //        i->score = 0;
        }
      }
    }
  }
  std::sort(all_items.begin(),all_items.end());

  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Remove items" << endmsg;
  // remove items
  _vec_t::iterator it = all_items.begin();
  while ( m_level > m_lowLvl && it != all_items.end()) {
   if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
     m_log << MSG::DEBUG << "Remove item since " << it->second.second.first <<
       " channel " << it->second.second.second <<
       // " from '" << it->second.first->path << "'" <<
       " (score =" << it->first << ")" << endmsg;
    // folder                    when
    it->second.first->erase(it->second.second.first,it->second.second.second);
    --m_level;
    ++it;
  }

  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Remove empty folders:" << endmsg;
  // remove empty folders
  std::vector<FolderIdType> to_remove;
  for ( StorageType::iterator i = m_cache.begin(); i != m_cache.end(); ++i ) {
    if (!i->second.sticky && i->second.empty()) { // delete folders which are empty but not sticky
      to_remove.push_back(i->first);
    }
  }
  for ( std::vector<FolderIdType>::iterator i = to_remove.begin(); i != to_remove.end(); ++i ) {
    if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
      m_log << MSG::DEBUG << "   " << *i << endmsg;
    m_cache.erase(m_cache.find(*i));
  }
  if( UNLIKELY( m_log.level() <= MSG::DEBUG ) )
    m_log << MSG::DEBUG << "Clean up finished (level = " << level() << ")" << endmsg;
  if ( UNLIKELY(old_level == level()) ) {
    m_log << MSG::WARNING << "No item removed: I increase high threshold" << endmsg;
    setHighLevel(highLevel()+(highLevel()-lowLevel())/10+1);
    m_log << MSG::WARNING << "New threshold = " << highLevel() << endmsg;
  }
}

//=========================================================================
//  Check if an entry for the give path+time is in the cache
//=========================================================================
bool CondDBCache::hasTime(const std::string &path, const cool::ValidityKey &when, const cool::ChannelId &channel) const {
  StorageType::const_iterator folder = m_cache.find(path);
  if (folder != m_cache.end()) {

    if ( !folder->second.spec ) return true; // It's a FolderSet! They ignore time

    ItemListType::const_iterator i = folder->second.find(when,channel);
    const ItemListType &lst = (*const_cast<CondFolder::StorageType *>(&folder->second.items))[channel];
    return i != lst.end();
  }
  return false;
}

ICondDBReader::IOVList CondDBCache::getIOVs(const std::string & path, const ICondDBReader::IOV & iov, cool::ChannelId channel)
{
  ICondDBReader::IOVList result;
  StorageType::const_iterator folder = m_cache.find(path);
  if (folder != m_cache.end()) {
    if (folder->second.spec) {
      // find the first recorded interval which overlaps with requested IOV
      ItemListType::const_iterator i = folder->second.conflict(iov.since.ns(), iov.until.ns(), channel);
      // marker for the end of IOVs in the cache for the channel
      const ItemListType::const_iterator end = folder->second.end(channel);
      // we add all the IOVs in the cache starting from the one found until
      // we are in the list and the IOV is in the requested range.
      for(; i != end && (i->iov.first < static_cast<cool::ValidityKey>(iov.until.ns())); ++i) {
        result.push_back(ICondDBReader::IOV(i->iov.first, i->iov.second));
      }
    } else {
      // it's a FolderSet: IOV is infinite
      result.push_back(ICondDBReader::IOV(Gaudi::Time::epoch(), Gaudi::Time::max()));
    }
  }
  return result;
}

//=========================================================================
//  Dump the content of the cache to the message service.
//=========================================================================
void CondDBCache::dump() {
  if (m_log.level() > MSG::DEBUG) return; // do not dump outside for non debug
  m_log << MSG::DEBUG << "Cache content dump --------------------- BEGIN" << endmsg;
  m_log << MSG::DEBUG << " Thresholds (high/low) -> " <<  m_highLvl << '/' << m_lowLvl << endmsg;
  m_log << MSG::DEBUG << " Level = " << level() << endmsg;
  for(StorageType::const_iterator i = m_cache.begin(); i != m_cache.end(); ++i ) {
    if ( !i->second.spec ) { // It's a FolderSet! They ignore time
      m_log << MSG::DEBUG << "FolderSet '" << i->first << "' " << ((i->second.sticky)?"(sticky)":"") << endmsg;
      continue;
    } else {
      m_log << MSG::DEBUG << "Folder '" << i->first << "' " << ((i->second.sticky)?"(sticky)":"") << endmsg;
    }

    for(CondFolder::StorageType::const_iterator ch = i->second.items.begin(); ch != i->second.items.end(); ++ch) {
      m_log << MSG::DEBUG << "  Channel " << ch->first << endmsg;
      size_t cnt = 0;
      for(ItemListType::const_iterator j = ch->second.begin(); j != ch->second.end(); ++j) {
        m_log << MSG::DEBUG << "  Object " << cnt++ << endmsg;
        m_log << MSG::DEBUG << "    Score: " << j->score << endmsg;
        m_log << MSG::DEBUG << "    Validity: " << j->iov.first << " - " << j->iov.second << endmsg;
        m_log << MSG::DEBUG << "    Data: " << *(j->data)<< endmsg;
      }
    }
  }
  m_log << MSG::DEBUG << "Cache content dump --------------------- END" << endmsg;
}
//=============================================================================

