// $Id: ROMon.h,v 1.2 2008-02-08 17:28:51 frankm Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Structure of basic items published by the readout
//               monitoring.
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMon.h,v 1.2 2008-02-08 17:28:51 frankm Exp $
#ifndef GAUDIONLINE_ROMON_H
#define GAUDIONLINE_ROMON_H 1

// ++ include files
#include <ctime>
// Framework include files
#include "MBM/bmdef.h"
#include "MBM/bmstruct.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class FixItems ROMon.h GaudiOnline/ROMon.h
   *
   * Class to store fixed length items in a flat memory area.
   *
   * @author M.Frank
   */
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
    const_iterator next(const_iterator prev) const
      {  return ++prev;                                }
    /// Add new element to container and return pointer to next object
    pointer_type add(pointer_type /* prev */)      
      {  return begin()+(++m_size);                    }
  };

  /**@class VarItems ROMon.h GaudiOnline/ROMon.h
   *
   * Class to store variable length items in a flat memory area.
   *
   * @author M.Frank
   */
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
    const_iterator next(const_iterator prev) const
      {  return (const_iterator)(((char*)prev)+prev->length());}

    /// Iterator implementation: Next element of iteration
    iterator next(iterator prev) 
      {  return (iterator)(((char*)prev)+prev->length());}

    /// Add new element to container and return pointer to next object
    pointer_type add(pointer_type prev)            {
      m_size++;
      m_length += prev->length();
      return end()->reset();
    }
  };

  /**@class MBMClient ROMon.h GaudiOnline/ROMon.h
   *
   * Class which represents an MBM client in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
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

  /**@class MBMBuffer ROMon.h GaudiOnline/ROMon.h
   *
   * Class which represents an MBM buffer in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
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

  /**@class FSMTask ROMon.h GaudiOnline/ROMon.h
   *
   * Class which represents a FSM task in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
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
    /// Default constructor
    FSMTask();
    /// Copt constructor
    FSMTask(const FSMTask& copy);
    /// Copy operator
    FSMTask& operator=(const FSMTask& copy);
    /// Length of the object in bytes
    int length()  const {  return sizeof(FSMTask); }
    /// Reset object structure
    FSMTask* reset();
  };

  /**@class Node ROMon.h GaudiOnline/ROMon.h
   *
   * Class which represents a node with MBM Buffers and 
   * FSM tasks in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class Node {
  public:
    typedef VarItems<MBMBuffer> Buffers;
    typedef FixItems<FSMTask>   Tasks;
    enum { TYPE = 1 };
    /// First word: Data type descriptor (MUST always be 1)
    int  type;
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

  /**@class Nodeset ROMon.h GaudiOnline/ROMon.h
   *
   * Class which represents a set of nodes in the ROMon monitoring structure.
   *
   * @author M.Frank
   */
  class Nodeset {
  public:
    typedef VarItems<Node> Nodes;
    enum { TYPE = 2 };
    /// First word: Data type descriptor (MUST always be 2)
    int  type;
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

  union ROMonData {
    char*      str;
    void*      ptr;
    MBMBuffer* buffer;
    FSMTask*   task;
    Node*      node;
    Nodeset*   nodeset;
    ROMonData(void* buffer) { ptr=buffer; }
    int type() { return this->ptr ? *(int*)this->ptr : -1; }
  };
}
#endif /* GAUDIONLINE_ROMON_H */

