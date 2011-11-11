// $Id:$
#ifndef GAUDIKERNEL_EVENTITEMID_H
#define GAUDIKERNEL_EVENTITEMID_H

/*
 *    Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class EventItemID EventItemID.h GaudiKernel/EventItemID.h
   *  Class to define the identifier of any object located in the 
   *  transient event data store.
   *
   *  @author   Markus Frank
   *  @version  1.0
   */
  class EventItemID {
  protected:    
    /// Item identifier
    int      m_bit;    
  public:
    /// Standard constructor with initializing argument
    EventItemID(const char* tag_name);
    /// Default destructor
    ~EventItemID();
    /// Accesor to the Item identifier
    int bit() const          { return m_bit; }
    /// Accessor to the tag name, return NULL if tag not in registry: Expensive operation!
    const char* name() const;
    /// Accessor to the hash code of the tag name: Expensive operation!
    unsigned int hash() const;
    /// Access to string registry
    static const char* entry_name(unsigned int bit);
    /// Access to string registry
    static unsigned int entry_hash(unsigned int bit);
  };

  // Type definition for the event data service
  typedef EventItemID EventTag_t;

}      // End namespace Gaudi
#endif // GAUDIKERNEL_IEVENTDATASVC_H
