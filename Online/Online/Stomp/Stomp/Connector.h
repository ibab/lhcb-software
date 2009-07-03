// $Id: Connector.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
//====================================================================
//  Stomp
//--------------------------------------------------------------------
//
//  Package    : Stomp
//
//  Description: DIM enabled Stomp plugin for orbited
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Stomp/Stomp/Connector.h,v 1.1 2009-07-03 18:10:07 frankb Exp $
#ifndef STOMP_CONNECTOR_H
#define STOMP_CONNECTOR_H 1

// C++ include files
#include "Stomp/Data.h"

// forward declarations
class Interactor;

/*
 *   Stomp namespace declaration
 */
namespace Stomp {

  /**@class Connector Connector.h Stomp/Connector.h
   *
   * @author M.Frank
   */
  class Connector    {
  public:
    enum { UNKNOWN_T=-1, FLOAT_T=1, DOUBLE_T, SHORT_T, INT_T, LONG_T, STRING_T };

  protected:
    /// Reference to the service manager
    int               m_id;
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
}      // End namespace Stomp
#endif /* STOMP_CONNECTOR_H */
