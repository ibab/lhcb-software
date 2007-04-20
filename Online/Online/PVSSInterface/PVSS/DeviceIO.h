// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DeviceIO.h,v 1.5 2007-04-20 09:34:16 frankb Exp $
//  ====================================================================
//  DeviceIO.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DeviceIO_H
#define ONLINE_PVSS_DeviceIO_H

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/DataPoint.h"
#include "PVSS/HotLinkCallback.h"
#include <algorithm>
#include <memory>
#include <map>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DataPoint;
  class DevAnswer;
  class DPRef;
  class DPTime;

  /** @class DeviceIO   DeviceIO.h  PVSS/DeviceIO.h
    *
    *  PVSS DeviceIO descriptor.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DeviceIO : public HotLinkCallback  {
  public:
    friend class ControlsManager;
    typedef std::map<DpID,DataPoint*> DataPoints;

    /** @struct GetData DeviceIO.h PVSS/DeviceIO.h
      */
    struct AddData {
      DeviceIO* m_rdr;
      void (DeviceIO::*m_pmf)(DataPoint&);
      AddData(DeviceIO* r, void (DeviceIO::*pmf)(DataPoint&)) : m_rdr(r), m_pmf(pmf) {}
      bool operator()(DataPoint& dp)  {
        (m_rdr->*m_pmf)(dp);
        return true;
      }
      template <class T> bool operator()(T& o)  {
        (m_rdr->*m_pmf)(o.second);
        return true;
      }
    };
    struct AddItem : public AddData {
      std::string      m_itm;
      AddItem(const AddItem& c) : AddData(c), m_itm(c.m_itm) {}
      AddItem(DeviceIO* r, void (DeviceIO::*pmf)(DataPoint&), const std::string& itm) 
      : AddData(r, pmf), m_itm(itm) {}
      bool operator()(DataPoint& o)  {
        if ( !m_itm.empty() ) o = DataPoint(m_rdr->manager(),o.name()+m_itm);
        return this->AddData::operator()(o);
      }
      template <class T> bool operator()(T& o)  {
        if ( !m_itm.empty() ) o.second = DataPoint(m_rdr->manager(),o.second.name()+m_itm);
        return this->AddData::operator()(o);
      }
    };
    AddItem addItem(const std::string& itm){return AddItem(this,&DeviceIO::add, itm);}
    AddItem addOnlineItem(const std::string& itm){return AddItem(this,&DeviceIO::addOnline, itm);}
    AddItem addOriginalItem(const std::string& itm){return AddItem(this,&DeviceIO::addOriginal, itm);}
    AddData addData()         { return AddData(this,&DeviceIO::add);     }
    AddData addOnlineData()   { return AddData(this,&DeviceIO::addOnline);    }
    AddData addOriginalData() { return AddData(this,&DeviceIO::addOriginal);  }

  public:
    class Read;
    class Write;

    /** @class IDevIO  DeviceIO.h  PVSS/DeviceIO.h
      *
      * Helper class interface to facilitate Read and Write actions
      * 
      * @author  M.Frank
      * @version 1.0
      */
    class IDevIO  {
      friend class DeviceIO;
    protected:
      /// Transaction context (dpID-value list)
      void     *m_context;
    public:
      /// Default constructor
      IDevIO() : m_context(0) {}
      /// Default destructor
      virtual ~IDevIO() {}
      /// Access to context
      const void* context() const { return m_context; }
      /// Drop PVSS IO list
      virtual void dropList() = 0;
      /// Execute PVSS IO action
      virtual bool exec(bool keep_list, DeviceIO* par, DevAnswer* a) = 0;
    };
    /** @class Read  DeviceIO.h  PVSS/DeviceIO.h
      *
      * Helper class to facilitate Read actions
      * 
      * @author  M.Frank
      * @version 1.0
      */
    class Read : public IDevIO {
    protected:
      /// Drop PVSS IO list
      virtual void dropList();
      /// Execute PVSS IO action
      virtual bool exec(bool keep_list, DeviceIO* par, DevAnswer* a);
    public:
      /// Default constructor
      Read();
      /// Default destructor
      virtual ~Read();
    };
    /** @class Write  DeviceIO.h  PVSS/DeviceIO.h
      *
      * Helper class to facilitate Write actions
      * 
      * @author  M.Frank
      * @version 1.0
      */
    class Write : public IDevIO {
    protected:
      /// Drop PVSS IO list
      virtual void dropList();
      /// Execute PVSS IO action
      virtual bool exec(bool keep_list, DeviceIO* par, DevAnswer* a);
    public:
      /// Default constructor
      Write();
      /// Default destructor
      virtual ~Write();
    };
  protected:
    /// Reference to the controls manager
    ControlsManager        *m_manager;
    /// Device IO handler
    std::auto_ptr<IDevIO>   m_devIO;
    /// Device datapoints
    DataPoints              m_points;
    /// Initializing constructor for reading
    DeviceIO(ControlsManager* mgr, const Read& );
    /// Initializing constructor for writing
    DeviceIO(ControlsManager* mgr, const Write& );

    /// HotLinkCallback overload: Handle callback for device group
    virtual void handleDataUpdate();
    /// HotLinkCallback overload: Set data value
    virtual void setValue(const DpID& dpid, int typ, const Variable* val);
    /// Internal add datapoint value
    void i_add(const DpID& id, DataPoint& dp);

  public:
    /// Standard destructor
    virtual ~DeviceIO();
    /// Access DeviceIO type manager
    ControlsManager* manager() const    {  return m_manager;      }
    /// Access datapoint container
    const DataPoints& points() const    {  return m_points;       }
    /// Change Device IO to writing mode
    void setWrite();
    /// Change Device IO to writing mode
    void setRead();
    /// Execute dpset list
    bool execute(bool keep_list = false, bool wait = true);
    /// Clear the datapoint container
    void clear();
    /// Length of known datapoints
    size_t length() const;
    /// add datapoint value 
    void add(DataPoint& dp);
    /// add original datapoint
    void addOriginal(DataPoint& dp);
    /// add online datapoint
    void addOnline(DataPoint& dp);
    /// add datapoint values to datapoint map
    void add(std::vector<DataPoint>& dp);
    /// add original datapoint values to datapoint map
    void addOriginal(std::vector<DataPoint>& dp);
    /// add online datapoint values to datapoint map
    void addOnline(std::vector<DataPoint>& dp);
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_DeviceIO_H
