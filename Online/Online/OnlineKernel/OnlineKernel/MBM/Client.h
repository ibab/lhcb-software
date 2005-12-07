#ifndef _MBM_CLIENT_H
#define _MBM_CLIENT_H

#include "MBM/bmdef.h"
#include <string>

/*
 *  MBM namespace declaration
 */
namespace MBM {

  class EventDesc {
  public:
    int* data;
    int  len;
    int  type;
    int  mask[4];
    EventDesc() : data(0), len(0), type(0) {
      mask[0] = mask[1] = mask[2] = mask[4] = 0;
    }
  };

  class Client {
  protected:
    BMID         m_bmid;
    std::string  m_buffName;
    std::string  m_name;
    int          m_partID;
    /// Facility code for asynchronous operation
    unsigned int m_facility;
    /// Flag to run in blocking mode
    bool         m_blocking;

  public:
    /// Initializing constructor
    Client(const std::string& buffer_name, const std::string& client_name, int partition_id);

    /// Initializing constructor
    Client(BMID bmid, const std::string& client_name, int partition_id);

    /// Standard destructor
    virtual ~Client();

    /// Access to process id
    static int pid();

    /// Buffer identifier
    BMID  id()  const {
      return m_bmid;
    }

    /// Buffer name
    const std::string& bufferName()  const {
      return m_buffName;
    }

    /// Client name
    const std::string& name() const {
      return m_name;
    }

    /// Partition ID
    int partitionID() const {
      return m_partID;
    }

    /// Include MBM client into buffer
    virtual int include();

    /// Exclude MBM client into buffer
    virtual int exclude();

    /// Switch to non-blocking asynchronous execution mode using WT
    virtual void setNonBlocking(int facility, bool subscribe);

    /// Switch to blocking synchronous execution mode
    void setBlocking();

    /// Run the application with WT
    static int run();

  };
}
#endif  // _MBM_CLIENT_H
