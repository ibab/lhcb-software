#ifndef GAUDIONLINE_ROMON_H
#define GAUDIONLINE_ROMON_H 1

#include <ctime>
#include "MBM/bmdef.h"
#include "MBM/bmstruct.h"

namespace ROMon {

  template <typename T> class FixItems {
  public:
    typedef T  item_type;
    typedef T* pointer_type;
    typedef T* iterator;
    typedef const T* const_iterator;

    /// Number of fixed size elements in the container
    int  m_size;

  public:    // Public data accessors
    /// Reset object and return iterator to first element
    iterator reset()
      {  m_size = 0;  return begin();                  }
    /// Return number of elements in the object
    int size()  const
      {  return m_size;                                }
    /// Length of the object in bytes
    int length() const
      {  return sizeof(int)+size()*sizeof(T);          }
    /// Length of the object's payload in bytes
    int data_length() const
      {  return size()*sizeof(T);                      }
    /// Acces to opaque data buffer 
    char* data() const                       
      {  return ((char*)&m_size)+sizeof(m_size);       }
    /// Iterator implementation: begin of object iteration
    pointer_type begin()  const
      {  return (pointer_type)data();                  }
    /// Iterator implementation: end of object iteration
    const pointer_type end() const 
      {  return begin()+size();                        }
    /// Iterator implementation: Next element of iteration
    const pointer_type next(pointer_type prev) const
      {  return ++prev;                                }
    /// Add new element to container and return pointer to next object
    pointer_type add(pointer_type /* prev */)      
      {  return begin()+(++m_size);                    }
  };

  template <typename T> class VarItems {
  public:
    typedef T  item_type;
    typedef T* pointer_type;
    typedef T* iterator;
    typedef const T* const_iterator;

    /// Number of variable size elements in the container
    int  m_size;
    /// Length of the data payload
    int  m_length;

  public:    // Public data accessors
    /// Reset object and return iterator to first element
    iterator reset()
      {  m_size = m_length = 0;  return begin();                 }
    /// Return number of elements in the object
    int size()  const
      {  return m_size;                                          }
    /// Length of the object in bytes
    int length() const
      {  return 2*sizeof(int)+m_length;                          }
    /// Length of the object's payload in bytes
    int data_length() const
      {  return m_length;                                        }
    /// Acces to opaque data buffer 
    char* data() const
      {  return ((char*)&m_length)+sizeof(int);                  }
    /// Iterator implementation: begin of object iteration
    pointer_type begin()  const
      {  return (pointer_type)data();                            }
    /// Iterator implementation: end of object iteration
    const pointer_type end()   const
      {  return (pointer_type)(data()+data_length());            }
    /// Iterator implementation: Next element of iteration
    const pointer_type next(pointer_type prev) const
      {  return (pointer_type)(((char*)prev)+prev->length());    }
    /// Add new element to container and return pointer to next object
    pointer_type add(pointer_type prev)            {
      m_size++;
      m_length += prev->length();
      return end()->reset();
    }
  };

  class MBMClient {
  public:
    /// Object name
    char name[BM_USER_NAME_LEN];
    int  partitionID;
    int  processID;
    int  events;
    char reqs[4];
    char type;
    char state;
    /// 3 Byte payload padding to ensure 32 bit alignment
    char pad[2];
  public:    // Public data accessors
    /// Length of the object in bytes
    int length()  const {  return sizeof(MBMClient); }
    /// Reset object structure
    MBMClient* reset();
  };

  class MBMBuffer {
  public:
    typedef FixItems<MBMClient> Clients;
    /// Object name
    char    name[BM_USER_NAME_LEN];
    /// Copy of the BM control structure
    CONTROL ctrl;
    /// Variable array of fixed size clients
    Clients clients;
  public:    // Public data accessors
    /// Length of the object in bytes
    int length()  const {  return sizeof(MBMBuffer)+clients.data_length(); }
    /// Reset object structure
    MBMBuffer* reset();
  };

  class FSMTask {
  public:
    /// Object name
    char name[BM_USER_NAME_LEN];
    /// Process identifier on node
    int  processID;
    /// Current FSM state
    char state;
    /// Target state of the transition
    char targetState;
    /// FSM meta state
    char metaState;
    /// 1 Byte payload padding to ensure 32 bit alignment
    char __pad;
    /// Time stamp of last command issued
    time_t lastCmd;
    /// Time stamp of comamnd completion
    time_t doneCmd;
  public:    // Public data accessors
    /// Length of the object in bytes
    int length()  const {  return sizeof(FSMTask); }
    /// Reset object structure
    FSMTask* reset();
  };

  class Node {
  public:
    typedef VarItems<MBMBuffer> Buffers;
    typedef FixItems<FSMTask>   Tasks;
    /// Total size of the node information
    int  totalSize;
    /// Size of the MBM buffer information
    int  bmSize;
    /// Size of the FSM task list
    int  taskSize;
    /// Time stamp of the monitor snapshot [seconds since epoche]
    time_t time;
    /// Time stamp of the monitor snapshot [milli seconds]
    unsigned int millitm;
    /// Object name
    char name[32];

  public:    // Public data accessors
    /// Reset object structure
    Node* reset();
    /// Access to the variable length array of variable length buffers. On filling buffers MUST be first filled!
    Buffers* buffers() const;
    /// Access to the variable length array of Tasks
    Tasks*   tasks() const;
    /// Fix-up object sizes
    void fixup();
    /// Length of the object in bytes
    int length()  const {  return totalSize; }
  };

  class Nodeset {
  public:
    typedef VarItems<Node> Nodes;
    /// Object name
    char name[32];
    /// Variable size array of node items
    Nodes nodes;
    /// Reset object structure
    Nodeset* reset();
    /// Length of the object in bytes
    int length()  const {  return sizeof(Nodeset)+nodes.data_length(); }    
  };

  typedef VarItems<Nodeset> Processorfarm;

}
#endif /* GAUDIONLINE_ROMON_H */

