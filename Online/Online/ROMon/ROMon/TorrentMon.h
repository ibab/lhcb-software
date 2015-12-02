// $Id: Monitor.h,v 1.6 2008-11-13 12:13:32 frankb Exp $
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
#ifndef ROMON_TORRENTMON_H
#define ROMON_TORRENTMON_H 1

#include "CPP/Collections.h"
#include "RTL/Pack.h"

/*
 *   BitTorrent namespace declaration
 */
namespace BitTorrent {

  /**
   * Definition of the TorrentLoader Monitoring information
   */
  using Online::FixItems;
  using Online::VarItems;
  using Online::TimeStamp;
  using Online::_firstUpdate;
  using Online::_lastUpdate;

  /** @class TorrentStatus  Monitor.h   TorrentLoader/Monitor.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  PACK_DATA(class) TorrentStatus {
  public:
    enum state_t  {
      queued_for_checking,
      checking_files,
      downloading_metadata,
      downloading,
      finished,
      seeding,
      allocating,
      checking_resume_data
    };
    /// Overall torrent state
    int     state;             /// State (see enum above)
    float   progress;          /// Progress of downloading
    
    /// Information concerning data download
    int     num_seeds;         /// Number of seeds serving the torrent
    int     num_pieces_total;  /// Total number of pieces
    int     num_pieces_done;   /// Pieces downloaded
    size_t  total_done;        /// Total Byes of the torrent present
    size_t  total_wanted;      /// Total bytes of the torrent wanting to download

    /// Information concerning uploads to other clients
    int     num_peers;         /// Number of peers the torrent is connected to
    int     num_uploads;       /// Number of uploads performed by this client
    size_t  total_upload;      /// Total Bytes uploaded
    size_t  total_download;    /// Total Bytes downloaded
    int     nameLen;
    int     msgLen;
    int     fileLen;
    char    name[1];           /// Torrent name

  public:    // Public data accessors

    /// Length of the object in bytes
    int length()  const {  return sizeof(TorrentStatus) + nameLen + msgLen + fileLen + 2; }

    /// Pointer to the error buffer (reading only)
    const char* message() const {  return name + nameLen + 1; }

    /// Pointer to the error buffer (reading only)
    const char* file() const {  return name + nameLen + 1 + msgLen + 1; }

    /// Set torrent name
    void setName(const std::string& nam);

    /// Set torrent error message
    void setMessage(const std::string& msg);

    /// Set torrent error message
    void setFile(const std::string& file);

    /// Reset object structure
    TorrentStatus* reset();

    /// Copy data to new buffer. returns pointer to newly created object in buffer
    TorrentStatus* copyTo(void* new_buffer)  const;
  };
  
  /** @class SessionStatus  Monitor.h   TorrentLoader/Monitor.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  PACK_DATA(class) SessionStatus {
  public:
    typedef VarItems<TorrentStatus> Torrents;    
    /// Total number of peer connections this session has.
    int      num_peers;
    /// The number of blocks that were requested from the bittorrent engine (from peers), that were served from disk or cache.
    int      blocks_read;
    /// The number of blocks that were served from cache.
    int      blocks_read_hit;
    /// Total number of write operations performed since this session was started.
    int      blocks_written;
    /// Upload rate in bytes/second
    int      upload_rate;
    /// Download rate in bytes/second
    int      download_rate;
    /// Total number of bytes uploaded
    size_t   total_upload;
    /// Total number of bytes downloaded
    size_t   total_download;
    /// Time stamp of the monitor snapshot [seconds since epoche]
    int  time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Node name
    char name[32];
    /// Fixed size container with torrent information
    Torrents torrents;

  public:
    /// Length of the object in bytes
    int length()  const {  return sizeof(SessionStatus)+torrents.data_length(); }

    /// Allocate object structure with num_torrent TorrentStatus slots
    static SessionStatus* allocate(size_t num_torrent);

    /// Allocate object structure with num_torrent TorrentStatus slots
    SessionStatus* reallocate(size_t num_torrent);

    /// Duplicate object structure. Careful: this allocates memory
    SessionStatus* clone()  const;

    /// Free status block
    void free();

    /// Reset object structure
    SessionStatus* reset();

    /// Copy data to new buffer. returns pointer to newly created object in buffer
    SessionStatus* copyTo(void* new_buffer)  const;
  };

  /** @class SubfarmTorrentStatus  Monitor.h   TorrentLoader/Monitor.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  PACK_DATA(class) SubfarmTorrentStatus {
  public:
    typedef VarItems<SessionStatus> Sessions;    
    enum { TYPE = 1 };
    /// First word: Data type descriptor (MUST always be 1)
    int  type;
    /// Object name
    char name[32];
    /// Variable size array of session items
    Sessions sessions;

  public:
    /// Reset object structure
    SubfarmTorrentStatus* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(SubfarmTorrentStatus)+sessions.data_length(); }
    /// Retrieve timestamp of earliest updated node
    TimeStamp firstUpdate() const;
    /// Retrieve timestamp of most recent updated node
    TimeStamp lastUpdate() const;
  };

#if 0
  
  NOT USED !

  /** @class FarmTorrentStatus  Monitor.h   TorrentLoader/Monitor.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  PACK_DATA(class) FarmTorrentStatus {
  public:
    typedef VarItems<SubfarmTorrentStatus> Subfarms;    
    enum { TYPE = 2 };
    /// First word: Data type descriptor (MUST always be 2)
    int  type;
    /// Object name
    char name[32];
    /// Variable size array of subfarm items
    Subfarms subfarms;

  public:
    /// Reset object structure
    FarmTorrentStatus* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(FarmTorrentStatus)+subfarms.data_length(); }
    /// Retrieve timestamp of earliest updated node
    TimeStamp firstUpdate() const;
    /// Retrieve timestamp of most recent updated node
    TimeStamp lastUpdate() const;
  };
#endif


}    /* End namespace BitTorrent  */
#include "RTL/Unpack.h"

#endif    //    ROMON_TORRENTMON_H
