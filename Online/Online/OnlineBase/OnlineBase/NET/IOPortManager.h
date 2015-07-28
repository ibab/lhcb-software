#ifndef ONLINEKERNEL_NET_IOPORTMANAGER_H
#define ONLINEKERNEL_NET_IOPORTMANAGER_H

#include "NET/NetworkChannel.h"

/** @class IOPortManager IOPortManager.h NET/IOPortManager.h
 *
 * Simple class to manage groups of select calls.
 * - Groups are identified by a main port number to be added with type 0.
 * - Subsequent channels may be added using type 1.
 * - Each group is managed by one thread.
 *
 * @author  M.Frank
 * @version 1.0
 */
class IOPortManager  {
protected:
  /// Data member: port number identifier for this connection group.
  __NetworkPort__ m_port;  
public:
  /// Standard constructor
  explicit IOPortManager(NetworkChannel::Port p): m_port(p)  {}
  /// Set port number for this connection group
  void setPort(NetworkChannel::Port p)  {  m_port = p;    }
  /// Access port number for this connection group
  __NetworkPort__ port() const          {  return m_port; }
  /// Stop group select thread
  void stop(void* entry);
  /// Join group select thread
  void join(void* entry);
  /// Add a new network channel to the IO Select map.
  /** @param typ      [in]   port type: if 0, main access port
   *                        if 1, deferred port, which belongs to this group
   * @param chan     [in]   Channel number. if 0 only the entry is generated and
   *                        the group thread is started.
   * @param callback [in]   Callback being clled on data ready
   * @param param    [in]   Callback context parameter.
   *
   * @return  Status code indicating success (1) or failure (0/-1)
   */
  int add(int typ, NetworkChannel::Channel chan, int (*callback)(void*), void* param);
  /// Add a new network channel to the IO Select map.
  /** @param typ      [in]   port type: if 0, main access port
   *                        if 1, deferred port, which belongs to this group
   * @param chan     [in]   Channel number. 0 is accepted as stdin!
   *                        The entry is generated and
   *                        the group thread is started.
   * @param callback [in]   Callback being clled on data ready
   * @param param    [in]   Callback context parameter.
   *
   * @return  Status code indicating success (1) or failure (0/-1)
   */
  int addEx(int typ, NetworkChannel::Channel chan, int (*callback)(void*), void* param);
  /// Remove an existing network channel to the IO Select map.
  /** @param chan      [in]  Channel number. if 0 only the entry is generated and
   *                        the group thread is started.
   * @param need_lock [in]  Flag to indicate if the manager object needs locking during removal. 
   *
   * @return  Status code indicating success (1) or failure (0/-1)
   */
  int remove(NetworkChannel::Channel c, bool need_lock=true);
  /// Lock select group. return locking context. Blocks select thread
  void* lock();
  /// Unlock select group. Unblocks select thread.
  void unlock(void* entry);
  /// Access to the number of data bytes ready on a given channel
  static int getAvailBytes(int fd);
  /// Get single data byte from connection channel (file/terminal I/O)
  static int getChar(int fd, char* c);
};
#endif // ONLINEKERNEL_NET_IOPORTMANAGER_H
