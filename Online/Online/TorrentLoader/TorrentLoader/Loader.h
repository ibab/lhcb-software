// $Id: Loader.h,v 1.6 2008-11-13 12:13:32 frankb Exp $
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
#ifndef TORRENTLOADER_LOADER_H
#define TORRENTLOADER_LOADER_H 1

// C++ include files
#include <string>
#include <vector>

// Framework include files
#include "RTL/rtl.h"
#include "CPP/Event.h"
#include "CPP/Interactor.h"

// Rasterbar torrent includes
#include "libtorrent/torrent_handle.hpp"

// Forward declarations
namespace libtorrent {
  class session;

  class portmap_error_alert;

  class listen_failed_alert;
  class listen_succeeded_alert;

  // Torrent alerts
  class torrent_finished_alert;
  class torrent_deleted_alert;
  class torrent_delete_failed_alert;
  class torrent_checked_alert;
  class torrent_paused_alert;
  class torrent_resumed_alert;

  class state_changed_alert;
  class file_error_alert;
  class file_completed_alert;
  class file_renamed_alert;
  class file_rename_failed_alert;

  class dht_reply_alert;
  class tracker_reply_alert;
  class tracker_announce_alert;
  class tracker_warning_alert;

  class read_piece_alert;
  class piece_finished_alert;
  class hash_failed_alert;

  // Peer alerts
  class peer_error_alert;
  class peer_blocked_alert;
  class peer_connect_alert;
  class peer_disconnected_alert;

  class block_downloading_alert;
  class block_finished_alert;
  class unwanted_block_alert;

  // Monitoring alert
  class stats_alert;
}

/*
 *   BitTorrent namespace declaration
 */
namespace BitTorrent {

  // Forward declarations 
  struct SessionStatus;
  struct TorrentStatus;
  class  NetworkFilter;

  /** @class Publisher TorrentLoader.h   TorrentLoader/TorrentLoader.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct Publisher: public Interactor {
    /// Parent object
    Interactor*    m_parent;
    /// Pointer to monitoring information
    SessionStatus* m_status;
    /// Monitor service identifier
    int            m_svcID;

  public:
    /// Standard constructor
    Publisher(Interactor* parent, const std::string& nam);

    /// Default destructor
    virtual ~Publisher();

    /// Monitor single torrent data
    void monitorTorrent(const libtorrent::stats_alert& data);

    /// Interactor overload: Handle external interrupts
    virtual void handle(const Event& event);

    /// Dim callback to place information datablocks
    static void feedMonitor(void* tag, void** address, int* size, int* first);
  };

  /** @class Command TorrentLoader.h   TorrentLoader/TorrentLoader.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct Command : public Interactor {
    /// Reply with status
    std::string  m_reply;
    /// Parent name for DIM notifications
    std::string  m_parent;
    /// DIM service ID
    int          m_svcID;
    /// DIM command ID
    int          m_cmdID;
    /// reference to the actual command target
    Interactor*  m_handler;

  public:
    /// Standard constructor
    Command( Interactor* handler, const std::string& parent, const std::string& nam);

    /// Default destructor
    virtual ~Command();

    /// Interactor overload: Handle external interrupts
    virtual void handle(const Event& event);

    /// DIM Command handler callback
    static void handleCommand(void* tag, void* address, int* size);

    /// DIM service handler callback
    static void feedStatus(void* tag, void** address, int* size, int* first);

  };

  /** @class Session TorrentLoader.h   TorrentLoader/TorrentLoader.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct Session : public Interactor {
    typedef std::map<std::string, libtorrent::torrent_handle> Torrents;
    typedef std::vector<std::string> Trackers;
    typedef lib_rtl_thread_t thread_t;
    typedef lib_rtl_lock_t   lock_t;
    typedef libtorrent::session* session_t;

    /// Handle to parent interactor for notifications
    Interactor*      m_parent;
    /// Torrent session
    session_t        m_session;
    /// Handle to event poll thread
    thread_t         m_thread;
    /// handle to lock mutex (needed to keep monitoring consistent)
    lock_t           m_lock;
    /// Flag to indicate the tier (0=SEEDER, 1=WORKER_NODE)
    int              m_tier;
    /// Flag to enable monitoring via DIM
    int              m_monitor;
    /// Printout of alerts: configuration
    unsigned long    m_alertConfig;
    /// Name of the download directory
    std::string      m_saveDir;
    /// Container of known torrents
    Torrents         m_torrents;
    /// Container of known trackers
    Trackers         m_trackers;
    /// Reference to monitoring information
    mutable SessionStatus* m_status;

  public:
    enum AlertCodes {
      code_portmap_error_alert     = 1<<0,

      code_listen_failed_alert     = 1<<1,
      code_listen_succeeded_alert  = 1<<2,

      // Torrent alerts
      code_torrent_finished_alert  = 1<<3,
      code_torrent_deleted_alert   = 1<<4,
      code_torrent_delete_failed_alert = 1<<5,
      code_torrent_checked_alert   = 1<<6,
      code_torrent_paused_alert    = 1<<7,
      code_torrent_resumed_alert   = 1<<8,

      code_state_changed_alert     = 1<<9,
      code_file_error_alert        = 1<<10,
      code_file_completed_alert    = 1<<11,
      code_file_renamed_alert      = 1<<12,
      code_file_rename_failed_alert= 1<<13,

      code_dht_reply_alert         = 1<<14,
      code_tracker_reply_alert     = 1<<15,
      code_tracker_announce_alert  = 1<<16,
      code_tracker_warning_alert   = 1<<17,

      code_read_piece_alert        = 1<<18,
      code_piece_finished_alert    = 1<<19,

      // Peer alerts: 0x100000 - 0xF00000
      code_peer_error_alert        = 1<<20,  // 0x100000
      code_peer_blocked_alert      = 1<<21,
      code_peer_connect_alert      = 1<<22,
      code_peer_disconnected_alert = 1<<23,

      code_block_downloading_alert = 1<<24,
      code_block_finished_alert    = 1<<25,
      code_unwanted_block_alert    = 1<<26,

      // Monitoring alert
      code_stats_alert             = 1<<27,

      // Fatal alert: seems to be unrecoverable: 
      code_hash_failed_alert       = 1<<28
    };

  public:
    /// Maximum number of allowed alerts for hash_failed_alert
    mutable unsigned long    m_NUM_hash_failed_alert;
    void operator()(libtorrent::hash_failed_alert const& a) const;

  public:
    /// Initializing constructor
    Session(Interactor* parent,
	    int         tier,
	    bool        enable_monitoring,
	    const std::string& save_to,
	    const std::string& interface, 
	    unsigned short first_port=1, 
	    unsigned short last_port=0xFFFF,
	    const Trackers& trackers=Trackers());
    /// Default destructor
    virtual ~Session();
    /// Start execution
    void start();
    /// Set the alert mask for configuring the printout
    void setAlertMask(unsigned long mask) { m_alertConfig = mask; }
    /// Set IP filter
    void setIPFilter(const NetworkFilter& filter);
    /// Update the session part of the monitoring information block
    void setSessionInfo() const;
    /// Update the session monitoring block. Does reallocate the memory block if necessary
    void setTorrentInfo() const;
    /// Update monitoring information
    void updateMonitoring() const;
    /// Event poll thread routine
    static int eventPoll(void* arg);
    /// Event poll callback
    int handleAlerts();
    /// Interactor overload: Handle external interrupts
    virtual void handle(const Event& event);
    /// Add a new torrent
    void addTorrent(const std::string& nam);
    /// Remove an existing torrent
    void removeTorrent(const std::string& nam);
    /// Remove all torrents and optionally clear caches
    void removeAllTorrents(bool flush=false);
    /// Set the upload limit for this session
    void setUploadLimit(int mb_per_second);
    /// Set the download limit for this session
    void setDownloadLimit(int mb_per_second);
    /// Reannounce trackers in reverse order
    void reannounce(libtorrent::torrent_handle handle)  const;

    void operator()(libtorrent::portmap_error_alert const& a) const;
    void operator()(libtorrent::listen_succeeded_alert const& a) const;
    void operator()(libtorrent::listen_failed_alert const& a) const;

    void operator()(libtorrent::dht_reply_alert const& a) const;

    void operator()(libtorrent::tracker_announce_alert const& a) const;
    void operator()(libtorrent::tracker_reply_alert const& a) const;
    void operator()(libtorrent::tracker_warning_alert const& a) const;

    void operator()(libtorrent::torrent_finished_alert const& a) const;
    void operator()(libtorrent::torrent_deleted_alert const& a) const;
    void operator()(libtorrent::torrent_delete_failed_alert const& a) const;
    void operator()(libtorrent::torrent_checked_alert const& a) const;
    void operator()(libtorrent::torrent_paused_alert const& a) const;
    void operator()(libtorrent::torrent_resumed_alert const& a) const;

    void operator()(libtorrent::state_changed_alert const& a) const;
    void operator()(libtorrent::file_error_alert const& a) const;
    void operator()(libtorrent::file_completed_alert const& a) const;
    void operator()(libtorrent::file_renamed_alert const& a) const;
    void operator()(libtorrent::file_rename_failed_alert const& a) const;

    void operator()(libtorrent::read_piece_alert const& a) const;
    void operator()(libtorrent::piece_finished_alert const& a) const;

    void operator()(libtorrent::peer_connect_alert const& a) const;
    void operator()(libtorrent::peer_disconnected_alert const& a) const;
    void operator()(libtorrent::peer_error_alert const& a) const;
    void operator()(libtorrent::peer_blocked_alert const& a) const;

    void operator()(libtorrent::block_downloading_alert const& a) const;
    void operator()(libtorrent::block_finished_alert const& a) const;
    void operator()(libtorrent::unwanted_block_alert const& a) const;

    void operator()(libtorrent::stats_alert const& a) const;
  };

  /** @class Session TorrentLoader.h   TorrentLoader/TorrentLoader.h
   *
   *  @author  M.Frank
   *  @version 1.0
   */
  struct Main : public Interactor {
    Interactor* m_session;
    Interactor* m_publish;
    std::vector<Interactor*> m_command;

  public:
    /// Standard constructor
    Main(int argc, char** argv);

    /// Default destructor
    virtual ~Main();

    /// Interactor overload: Handle external interrupts
    virtual void handle(const Event& event);

    /// Run the application
    int run();
  };
}    /* End namespace BitTorrent  */

#endif    //    TORRENTLOADER_LOADER_H
