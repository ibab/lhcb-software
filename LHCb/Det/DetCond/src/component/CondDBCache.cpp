// $Id: CondDBCache.cpp,v 1.1 2005-06-23 14:14:46 marcocle Exp $
// Include files 


// local
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
  m_highLvl(highLvl),m_lowLvl(lowLvl),m_level(0),m_log(log),m_lastRequestedTime(0)
{
  if ( highLvl == 0 ) {
    m_log << MSG::WARNING << "High level == 0 : forced to 100" << endmsg;
    m_highLvl = 100;
  }  
  if ( highLvl <= lowLvl ) {
    m_log << MSG::WARNING << "High level <= low level : low level forced to 0" << endmsg;
    m_lowLvl = 0;
  }
  m_log << MSG::DEBUG << "Cache initialized with high/low levels = " <<
    m_highLvl << '/' << m_lowLvl << endmsg;
}
//=============================================================================
// Destructor
//=============================================================================
CondDBCache::~CondDBCache() {
  m_log << MSG::DEBUG << "Cache deleted. Level was = " << m_level << endmsg;
}

//=========================================================================
//  Add a new item to the cache
//=========================================================================
bool CondDBCache::insert(const cool::IFolderPtr &folder,const cool::IObjectPtr &obj) {
  // increment object count and check the limit
  if ( m_level >= highLevel() ){
    // needs clean up
    m_log << MSG::DEBUG << "Level above max threshold" << endmsg;
    clean_up();
  }
  m_log << MSG::DEBUG << "Insert  Folder '" << folder->fullPath()
        << "', IOV : " << obj->since() << " - " << obj->until() << endmsg;
  std::map<std::string,CondFolder>::iterator f = m_cache.find(folder->fullPath());
  if (f == m_cache.end()){
    f = m_cache.insert(std::pair<std::string,CondFolder>(folder->fullPath(),CondFolder(folder))).first;
  } else {
    if (f->second.conflict(obj->since(),obj->until()) != f->second.items.end()) {
      m_log << MSG::WARNING << "Conflict found: item not inserted" << endmsg;
      return false;
    }
  }
  // for vectors
  //  f->second.items.push_back(CondItem(&f->second,obj));
  // for lists
  f->second.items.push_front(CondItem(&f->second,obj));
  ++m_level;
  return true;
}

//=========================================================================
//  Add a new folder using the given specification and description. (Bypass the real DB)
//=========================================================================
bool CondDBCache::addFolder(const std::string &path, const std::string &descr,
                            const pool::AttributeListSpecification& spec) {
  std::map<std::string,CondFolder>::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    f = m_cache.insert(std::pair<std::string,CondFolder>(path,CondFolder(descr,spec))).first;
  }
  return true;
}

//=========================================================================
//  Add a new object to a given folder
//=========================================================================
bool CondDBCache::addObject(const std::string &path, const cool::ValidityKey &since, const cool::ValidityKey &until,
                            const pool::AttributeList& al, IOVType *iov_before) {
  // new objects cannot be already valid. check it!
  if ( m_lastRequestedTime != 0 && ( since <= m_lastRequestedTime && m_lastRequestedTime < until ) ) {
    m_log << MSG::WARNING << "New item IOV is compatible with last requested time: I cannot add it" << endmsg;
    return false;
  }
  // increment object count and check the limit
  if ( m_level >= highLevel() ){
    // needs clean up
    m_log << MSG::DEBUG << "Level above max threshold" << endmsg;
    clean_up();
  }
  std::map<std::string,CondFolder>::iterator f = m_cache.find(path);
  if (f == m_cache.end()){
    m_log << MSG::WARNING << "Could not find the folder: object not added" << endmsg;
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
  ItemListType::iterator i = f->second.conflict(since,until);
  if ( i != f->second.items.end() ) { // conflict found
    if ( i->iov.second == cool::ValidityKeyMax && i->iov.first < since ) {
      // solvable conflict
      if (iov_before) *iov_before = i->iov;
      m_log << MSG::DEBUG << "Solvable conflict found: old until = " << i->iov.second << endmsg;
      i->iov.second = since;
      m_log << MSG::DEBUG << "                         new until = " << i->iov.second << endmsg;
    } else {
      // conflict not solvable
      m_log << MSG::WARNING << "Unsolvable conflict found: item not inserted" << endmsg;
      return false;
    }
  }
  // for vectors
  //  f->second.items.push_back(CondItem(&f->second,since,until,al));
  // for lists
  f->second.items.push_front(CondItem(&f->second,since,until,al));
  ++m_level;
  return true;
}

//=========================================================================
//  Get data from given path and valid at given time
//=========================================================================
bool CondDBCache::get(const std::string &path, const cool::ValidityKey &when,
                      cool::ValidityKey &since, cool::ValidityKey &until,
                      std::string &descr, boost::shared_ptr<pool::AttributeList> &payload ) {
  m_log << MSG::DEBUG << "Request Folder '" << path 
        << "'  @ " << when ;
  m_lastRequestedTime = when;
  std::map<std::string,CondFolder>::iterator folder = m_cache.find(path);
  if (folder != m_cache.end()) {
    ItemListType::iterator i = folder->second.find(when);
    if ( i != folder->second.items.end() ) {
      since   = i->iov.first;
      until   = i->iov.second;
      descr   = folder->second.description;
      payload = i->data;
      //i->score += 1.0;
      m_log << " FOUND" << endmsg;
      return true;
    }
  }
  m_log << " MISSING" << endmsg;
  return false;
}

//=========================================================================
//  Remove unused entries from the cache
//=========================================================================

void CondDBCache::clean_up(){
  typedef std::vector<std::pair<float,std::pair<CondDBCache::CondFolder*,cool::ValidityKey> > > _vec_t;
  _vec_t all_items;
  float score = 0;
  size_t old_level = level();

  m_log << MSG::DEBUG << "Start cleaning up (level = " << level() << ")" << endmsg;
  m_log << MSG::DEBUG << "            Last requested time = " << m_lastRequestedTime << endmsg;
  // collect all items info in order
  StorageType::iterator folder;
  for ( folder = m_cache.begin() ; folder != m_cache.end() ; ++folder ) {
    ItemListType::iterator i;
    m_log << MSG::DEBUG << "Folder " << folder->first << endmsg;
    for ( i = folder->second.items.begin(); i != folder->second.items.end() ; ++i ){
      m_log << MSG::DEBUG << "  IOV : " << i->iov.first << " - " << i->iov.second << endmsg;
      if ( ! (i->iov.first <= m_lastRequestedTime && i->iov.second > m_lastRequestedTime) ) {
        if ( m_lastRequestedTime < i->iov.first ) {
          score = (float)m_lastRequestedTime - i->iov.first;
        } else {
          score = (float)i->iov.second - m_lastRequestedTime;
        }
        m_log << MSG::DEBUG << "     score = " << score << endmsg;
        all_items.push_back(
           std::make_pair(score,
              std::make_pair<CondDBCache::CondFolder*,cool::ValidityKey>(&folder->second,i->iov.first)));
        //        i->score = 0;
      }
    }
  }
  std::sort(all_items.begin(),all_items.end());

  m_log << MSG::DEBUG << "Remove items" << endmsg;
  // remove items
  _vec_t::iterator it = all_items.begin();
  while ( m_level > m_lowLvl && it != all_items.end()) {
    m_log << MSG::DEBUG << "Remove item since " << it->second.second <<
      // " from '" << it->second.first->path << "'" <<
      " (score =" << it->first << ")" << endmsg;
    // folder                    when
    it->second.first->erase(it->second.second);
    --m_level;
    ++it;
  }

  m_log << MSG::DEBUG << "Remove empty folders:" << endmsg;
  // remove empty folders
  std::vector<FolderIdType> to_remove;
  for ( StorageType::iterator i = m_cache.begin(); i != m_cache.end(); ++i ) {
    if (!i->second.sticky && i->second.empty()) { // delete folders which are empty but not sticky
      to_remove.push_back(i->first);
    }
  }
  for ( std::vector<FolderIdType>::iterator i = to_remove.begin(); i != to_remove.end(); ++i ) {
    m_log << MSG::DEBUG << "   " << *i << endmsg;
    m_cache.erase(m_cache.find(*i));
  }
  m_log << MSG::DEBUG << "Clean up finished (level = " << level() << ")" << endmsg;
  if ( old_level == level() ) {
    m_log << MSG::WARNING << "No item removed: I increase high threshold" << endmsg;
    setHighLevel(highLevel()+(highLevel()-lowLevel())/10+1);
    m_log << MSG::WARNING << "New threshold = " << highLevel() << endmsg;
  }
}

//=========================================================================
//  Check if an entry for the give path+time is in the cache
//=========================================================================
bool CondDBCache::hasTime(const std::string &path, const cool::ValidityKey &when) const {
  std::map<std::string,CondFolder>::const_iterator folder = m_cache.find(path);
  if (folder != m_cache.end()) {
    ItemListType::const_iterator i = folder->second.find(when);
    return i != folder->second.items.end();
  }
  return false;
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
    m_log << MSG::DEBUG << "Folder '" << i->first << "' " << ((i->second.sticky)?"(sticky)":"") << endmsg;
    std::ostringstream type_spec;
    i->second.spec->print(type_spec);
    m_log << MSG::DEBUG << "     Type: " << type_spec.str() << endmsg;
    size_t cnt = 0;
    for(ItemListType::const_iterator j = i->second.items.begin(); j != i->second.items.end(); ++j) {
      std::ostringstream data;
      j->data->print(data);
      m_log << MSG::DEBUG << "  Object " << cnt++ << endmsg;
      m_log << MSG::DEBUG << "    Score: " << j->score << endmsg;
      m_log << MSG::DEBUG << "    Validity: " << j->iov.first << " - " << j->iov.second << endmsg;
      m_log << MSG::DEBUG << "    Data: " << data.str()  << endmsg;
    }
  }
  m_log << MSG::DEBUG << "Cache content dump --------------------- END" << endmsg;  
}
//=============================================================================

