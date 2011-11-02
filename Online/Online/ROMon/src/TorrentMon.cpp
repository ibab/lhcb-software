// $Id: Moniotor.cpp,v 1.6 2008-11-13 12:13:32 frankb Exp $
//====================================================================
//  Online
//--------------------------------------------------------------------
//
//  Package    : TorrentLoader
//
//  Structure of basic items published by the readout monitoring.
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header:$

// Framework include files
#include "ROMon/TorrentMon.h"

// C++ includes 
#include <cstdlib>
#include <cstring>
#include <climits>

#if defined(WIN32) && !defined(PATH_MAX)
#define PATH_MAX _PATH_MAX
#endif

using namespace BitTorrent;

/// Reset object structure
TorrentStatus* TorrentStatus::reset() {
  ::memset(this,0,sizeof(TorrentStatus));  
  return this;  
}

/// Set torrent name
void TorrentStatus::setName(const std::string& nam)   {
  const char* n = nam.c_str();
  ::strncpy(name,n,PATH_MAX);
  name[PATH_MAX-1]=0;
  nameLen = ::strlen(name);
}

/// Set torrent error message
void TorrentStatus::setMessage(const std::string& msg)   {
  char* start = name+nameLen+1;
  const char* m = msg.c_str();
  ::strncpy(start,m,128);
  start[127] = 0;
  msgLen = ::strlen(start);
}

/// Set torrent error message
void TorrentStatus::setFile(const std::string& file)   {
  char* start = name+nameLen+1+msgLen+1;
  const char* f = file.c_str();
  ::strncpy(start,f,PATH_MAX);
  start[PATH_MAX-1] = 0;
  fileLen = ::strlen(start);
}

/// Copy data to new buffer. returns pointer to newly created object in buffer
TorrentStatus* TorrentStatus::copyTo(void* new_buffer)  const {
  ::memcpy(new_buffer,this,sizeof(TorrentStatus)+nameLen+msgLen+2*sizeof(char));  
  return (TorrentStatus*)new_buffer;
}

/// Allocate object structure with num_torrent TorrentStatus slots
SessionStatus* SessionStatus::allocate(size_t num_torrent) {
  size_t len = sizeof(SessionStatus)+num_torrent*(sizeof(Torrents::value_type)+2*PATH_MAX+128);
  SessionStatus* ptr = (SessionStatus*)::malloc(len);
  ::memset(ptr,0,len);
  return ptr;
}

/// Allocate object structure with num_torrent TorrentStatus slots
SessionStatus* SessionStatus::reallocate(size_t num_torrent) {
  size_t len = sizeof(SessionStatus)+num_torrent*(sizeof(Torrents::value_type)+2*PATH_MAX+128);
  SessionStatus* ptr = (SessionStatus*)::realloc(this,len);
  return ptr;
}

/// Duplicate object structure. Careful: this allocates memory
SessionStatus* SessionStatus::clone()   const  {
  size_t len = length();
  SessionStatus* ptr = (SessionStatus*)::malloc(len);
  ::memcpy(ptr,this,len);
  return ptr;
}

/// Free status block
void SessionStatus::free() {
  ::free(this);
}

SessionStatus* SessionStatus::reset() {
  ::memset(this,0,sizeof(SessionStatus));  
  return this;
}

/// Copy data to new buffer. returns pointer to newly created object in buffer
SessionStatus* SessionStatus::copyTo(void* new_buffer)  const {
  ::memcpy(new_buffer,this,length());  
  return (SessionStatus*)new_buffer;  
}

/// Reset object structure
SubfarmTorrentStatus* SubfarmTorrentStatus::reset() {
  ::memset(this,0,sizeof(SubfarmTorrentStatus));
  type = TYPE;
  return this;  
}

/// Retrieve timestamp of earliest updated node
TimeStamp SubfarmTorrentStatus::firstUpdate() const {
  return _firstUpdate(sessions);
}

/// Retrieve timestamp of most recent updated node
TimeStamp SubfarmTorrentStatus::lastUpdate() const {
  return _lastUpdate(sessions);
}
