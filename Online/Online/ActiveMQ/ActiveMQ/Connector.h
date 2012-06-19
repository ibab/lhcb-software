// $Id: Connector.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
//====================================================================
//  ActiveMQ
//--------------------------------------------------------------------
//
//  Package    : ActiveMQ
//
//  Description: DIM enabled ActiveMQ plugin for orbited
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /local/reps/lhcb/Online/ActiveMQ/ActiveMQ/Connector.h,v 1.1 2010/11/01 17:20:21 frankb Exp $
#ifndef ACTIVEMQ_CONNECTOR_H
#define ACTIVEMQ_CONNECTOR_H 1

// C++ include files
#include "ActiveMQ/Data.h"
#include <ctime>

// forward declarations
class Interactor;

/*
 *   ActiveMQ namespace declaration
 */
namespace ActiveMQ {

  /**@class Connector Connector.h ActiveMQ/Connector.h
   *
   * @author M.Frank
   */
  class Connector    {
  public:
    enum { UNKNOWN_T=-1, FLOAT_T=1, DOUBLE_T, SHORT_T, INT_T, LONG_T, STRING_T };

  protected:
    /// Reference to the service manager
    int               m_id, m_id2;
    /// Data type
    int               m_type;
    /// Data point passed to serve clients
    Data              m_data;
    /// Reference to data target
    Interactor*       m_target;
    /// Did service already receive data?
    bool              m_haveData;
    /// Flag if this connector may be removed
    bool              m_mayRemove;
    /// DimInfo overload to process messages
    static void infoHandler(void* tag, void* address, int* size);

  public:
    /// Standard constructor
    Connector(const std::string& tag, bool may_remove, Interactor* target);
    /// Standard destructor
    virtual ~Connector();
    /// Access to removal flag
    bool mayRemove() const    {   return m_mayRemove; }
    /// Access to data items
    const Data& data() const  {   return m_data;  }
    /// Set data item
    void set(int typ, const char* ptr, size_t siz);
    /// Publish data item
    void publish();
  };
}      // End namespace ActiveMQ
#endif /* ACTIVEMQ_CONNECTOR_H */
