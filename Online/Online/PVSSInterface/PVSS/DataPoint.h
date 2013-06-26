// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DataPoint.h,v 1.10 2009-02-10 19:32:35 frankb Exp $
//  ====================================================================
//  DataPoint.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DATAPOINT_H
#define ONLINE_PVSS_DATAPOINT_H

// Framework include files
#include "PVSS/Kernel.h"
#include "PVSS/DpID.h"

// C++ include files
#include <set>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <typeinfo>

// Forward declarations
class Variable;

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class ControlsManager;
  class DevType;
  class Value;
  /** @class Value   DataPoint.h  PVSS/DataPoint.h
    *
    *  Interface to data received from PVSS
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class Value {
  protected:
    Value(int t) : m_type(t) {}
    int m_type;
  public:
    /// Standard destructor
    virtual ~Value() {}
    /// PVSS data type of the value
    int type() const  {   return m_type;  }
    /// Access to data pointer
    virtual void* ptr() = 0;
    /// Pointer to data item (CONST)
    virtual const void* ptr() const = 0;
    /// C++ Type id
    virtual const std::type_info& id() const = 0;
    /// Clone object
    virtual Value* clone() const = 0;
    /// PVSS data type
    template <class T> static int type_id(const T&);
  };

  /** @class DataValue   DataPoint.h  PVSS/DataPoint.h
    *
    *  Holder of data received from PVSS
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  template <typename T> class DataValue : public Value {
  protected:
    /// Buffer for data value
    T  m_data;
  public:
#ifdef __GCCXML
    DataValue() : Value(0), m_data(T())       {                   }
#endif
    /// Initializing constructor
    DataValue(int typ) : Value(typ)           {                   }
    /// Initializing constructor
    DataValue(int typ, const T& v)   : Value(typ), m_data(v)    { }
    /// Copy constructor
    DataValue(const DataValue& c) 
    : Value(c.type()), m_data(c.data())       {                   }
    /// Standard destructor
    virtual ~DataValue()                      {                   }
    /// Pointer to data item (CONST)
    virtual const void* ptr() const           { return &m_data;   }
    /// Pointer to data item
    virtual void* ptr()                       { return &m_data;   }
    /// Clone object
    virtual Value* clone() const { return new DataValue<T>(*this);}
    /// Access to data
    T& data()                                 { return m_data;    }
    /// Access to data (CONST)
    const T& data() const                     { return m_data;    }
    /// Type conversion to data
    operator T&()                             { return m_data;    }
    /// Type conversion to data (CONST)
    operator const T&()                       { return m_data;    }
    /// Explicit type conversion
    template <typename Q> Q value() const     { return Q(m_data); }
    /// C++ Type id
    virtual const std::type_info& id() const  { return typeid(T); }
    /// C++ Type id
    static  const std::type_info& type_info() { return typeid(T); }
    /// PVSS Type id
    static int type_id();
  };


  template <typename T> 
  int Value::type_id(const T&) { return DataValue<T>::type_id();  }
  template <typename T>
  DataValue< T > createDataValue(const T& o) { return DataValue< T >(Value::type_id(o));}

  /** @class DPRef   DataPoint.h  PVSS/DataPoint.h
    *
    *  Wrapper for references.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DPRef  {
    std::string m_name;
  public:
    /// Default constructor
    DPRef() : m_name() {}
    /// Initializing constructor
    DPRef(const std::string& nam) : m_name(nam) {}
    /// Copy constructor
    DPRef(const DPRef& c) : m_name(c.m_name) {}
    /// Destructor
    ~DPRef() {}
    /// Name of referenced datapoint
    const std::string& name() const {  return m_name; }
    /// Equivalence check
    bool operator==(const DPRef& r) const { return m_name == r.m_name; }
  };

  /** @class DPTime   DataPoint.h  PVSS/DataPoint.h
    *
    *  Wrapper for time variables.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DPTime  {
    /// Internal time
    time_t m_time;
  public:
    /// Default constructor
    DPTime() : m_time(0) {}
    /// Copt constructor
    DPTime(const DPTime& c) : m_time(c.m_time) {}
    /// Initializing constructor
    DPTime(time_t t) : m_time(t) {}
    /// Standard destructor
    ~DPTime() {}
    /// Access to internal time variable
    time_t& time()             {  return m_time; }
    /// Access to internal time variable (CONST)
    const time_t& time() const {  return m_time; }
    /// Equivalence check
    bool operator==(const DPTime& r) const { return m_time == r.m_time; }
  };

  /** @class DataPoint   DataPoint.h  PVSS/DataPoint.h
    *
    *  This class represents a PVSS datapoint and stores
    *  on reading the retrieved values.
    *  For simple types (primitives) basic type conversion is
    *  performed.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class DataPoint   {
  protected:
    /// Datapoint identifier
    DpID             m_id;
    /// Flag to check if datapoint identifier is valid
    char             m_valid;
    /// Flag for debugging (and padding)
    char             m_flag[3];
    /// Datapoint name
    std::string      m_name;
    /// Reference to controls manager
    ControlsManager* m_mgr;
    /// Pointer to data value
    Value*           m_val;
    /// load datapoint identifier
    const DpID& load() const;
#ifdef __GCCXML
  public:
    /// Default constructor
    DataPoint() : m_valid(false), m_mgr(0), m_val(0) {
      invalidValue(typeid(DataPoint));
    }
#endif

  public:
    /// Standard constructor
    DataPoint(ControlsManager* m);
    /// Initializing constructor
    DataPoint(ControlsManager* m, const std::string& nam);
    /// Initializing constructor
    DataPoint(ControlsManager* m, const DpID& dpid);
    /// Copy constructor
    DataPoint(const DataPoint& nam);
    /// Standard destructor
    virtual ~DataPoint();
    /// Access to the Datapoint's name
    const std::string& name()  const {    return m_name;        }
    /// Access to controls manager environment
    ControlsManager* manager() const {    return m_mgr;         }
    /// Assignment
    DataPoint& operator=(const DataPoint& c);
    /// Equivalence check
    bool operator==(const DataPoint& c) const;
    /// Operator less
    bool operator<(const DataPoint& c) const;
    /// Datapoint identifier
    const DpID& id() const 
    { return m_valid ? m_id : load();                           }
    /// Set value data
    void setValue(int typ, const Variable* data);
    /// Access to datapoint's value
    Value* value() const             {  return m_val;           }
    /// Set debug flag
    void setFlag(int which, signed char val)
    { if ( which >= 0 && which < 3 ) m_flag[which]=val;         }
    /// Access debug flag
    int flag(int which)  const
    { return (which >= 0 && which < 3) ? m_flag[which] : -1; }
    /// Template access to data - overloaded for concrete types
    template <class T> T data();
    /// Template access to data (CONST) - overloaded for concrete types
    template <class T> const T data() const;
    /// Set value data (for publishing data to PVSS)
    template <class T> void set(const T& val);
    /// Template access to reference of data - overloaded for concrete types
    template <class T> T& reference();
    /// Template access to reference of data - overloaded for concrete types
    template <class T> const T& reference() const;
    /// Extract element name of datapoint from online/original name
    std::string elementName() const;
    /// Extract element name of datapoint from online/original name
    std::string typeName() const;
    /// Extract system name of datapoint from online/original name
    std::string sysname() const;
    /// Extract name of datapoint from online/original name
    std::string dpname()  const;
    /// Construct online value
    static std::string online(const std::string& dp);
    /// Construct name of online value
    static std::string original(const std::string& dp);
    /// Extract name of datapoint from online/original name
    static std::string dpname(const std::string& dp);
    /// Extract name of datapoint from online/original name
    static std::string dpname(const DpID& dpid);
    /// Extract system name of datapoint from online/original name
    static std::string sysname(const std::string& dp);
    /// Extract system name of datapoint from online/original name
    static std::string sysname(const DpID& dp);
    /// Extract element name of datapoint from online/original name
    static std::string elementName(const std::string& dp);
    /// Extract element name of datapoint from online/original name
    static std::string elementName(const DpID& dpid);
    /// Check datapoint existencs
    static bool exists(const std::string& name);
    /// Exception function
    static void invalidValue(const std::type_info& tid);
    /// Exception function
    static void invalidConversion(const std::type_info& tid);
  };

  template <typename T> struct DPContainerInserter  {
    ControlsManager* manager;
    T*               container;
    /// Initializing constructor
    DPContainerInserter(ControlsManager* m, T& c) : manager(m), container(&c) {}
  };

  /**@class DPPushback DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  template <typename T> struct DPPushback : public DPContainerInserter<T> {
    /// Initializing constructor
    DPPushback(ControlsManager* m, T& c) : DPContainerInserter<T>(m,c) {}
    /// Callback function for datapoint insertion into container
    virtual void operator()(const DpID& dpid)  
    {   this->container->push_back(DataPoint(this->manager,dpid));  }
  };

  /**@class DPInsert DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  template <typename T> struct DPInsert : public DPContainerInserter<T> {
    /// Initializing constructor
    DPInsert(ControlsManager* m, T& c) : DPContainerInserter<T>(m,c) {}
    /// Callback function for datapoint insertion into container
    virtual void operator()(const DpID& dpid)
    {   this->container->insert(DataPoint(this->manager,dpid));    }
  };

  /// Helper function to create data point insertion object for STL sets
  template <typename T> 
  DPInsert<std::set<T> > dpInserter(ControlsManager* m, std::set<T>& s) 
  {  return DPInsert<std::set<T> >(m,s);         }
  /// Helper function to create data point insertion object for STL vectors
  template <typename T> 
  DPPushback<std::vector<T> > dpInserter(ControlsManager* m, std::vector<T>& s) 
  {  return DPPushback<std::vector<T> >(m,s);    }
  /// Helper function to create data point insertion object for STL lists
  template <typename T> 
  DPPushback<std::list<T> > dpInserter(ControlsManager* m, std::list<T>& s) 
  {  return DPPushback<std::list<T> >(m,s);    }

  /**@class DPListActor DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  struct DPListActor  {
  protected:
    /// Datapoint identifiers contained in selected list
    DpID*            m_dpids;
    /// Number of Datapoint identifiers contained in selected list
    int              m_count;
    /// Reference to controls manager
    ControlsManager* m_manager;
  public:
    /// Initializing constructor
    DPListActor(ControlsManager* m);
    /// Standard destructor
    virtual ~DPListActor();
    /// Lookup datapoint items
    bool lookup(const std::string& typ_nam, const DevType* typ);
    /// Lookup datapoint items
    bool lookupOriginal(const std::string& typ_nam, const DevType* typ);
    /// Callback function to operate on datapoints
    virtual void operator()(const DpID& dpid) = 0;
  };

  template <typename T> struct DPContainerActor  {
    /// Container buffer
    T container;
    /// Initializing constructor
    DPContainerActor(T c) : container(c) {  container.clear(); }
    /// Container modifier
    template <typename Q> void apply(const Q& operand)  
    {  std::for_each(container.begin(),container.end(),operand);        }
  };

  /**@class DataPointMap DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  template<typename T> struct DataPointMapActor : public DPListActor, public DPContainerActor<T>  {
    /// Initializing constructor
    DataPointMapActor(ControlsManager* m, T c=T()) : DPListActor(m), DPContainerActor<T>(c)  {}
    /// Callback function to operate on datapoints
    void operator()(const DpID& dpid) {
      std::string dpn = DataPoint::dpname(dpid);
      this->container.insert(std::make_pair(dpn,DataPoint(this->m_manager,dpn)));
    }
  };

  /**@class DataPointContainer DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  template<typename T> struct DataPointContainerActor : public DPListActor, public DPContainerActor<T> {
    /// Initializing constructor
    DataPointContainerActor(ControlsManager* m, T c=T()) : DPListActor(m), DPContainerActor<T>(c)  {}
    /// Callback function to operate on datapoints
    void operator()(const DpID& dpid)  
    {  this->container.push_back(DataPoint(this->m_manager,DataPoint::dpname(dpid))); }
  };

  /**@class DataPointSet DataPoint.h PVSS/DataPoint.h 
    *
    * @author  M.Frank
    * @version 1.0
    */
  template<typename T> struct DataPointSetActor : public DPListActor, public DPContainerActor<T> {
    /// Initializing constructor
    DataPointSetActor(ControlsManager* m, T c=T()) : DPListActor(m), DPContainerActor<T>(c)  {}
    /// Callback function to operate on datapoints
    void operator()(const DpID& dpid)  
    {  this->container.insert(DataPoint(this->m_manager,DataPoint::dpname(dpid)));  }
  };

}      // End namespace PVSS
#endif // ONLINE_PVSS_DATAPOINT_H
