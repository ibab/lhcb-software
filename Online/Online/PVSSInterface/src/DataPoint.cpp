// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DataPoint.cpp,v 1.31 2007-10-29 13:08:23 frankm Exp $
//  ====================================================================
//  DataPoint.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DataPoint.cpp,v 1.31 2007-10-29 13:08:23 frankm Exp $
#ifdef _WIN32
  // Disable warning C4250: 'const float' : forcing value to bool 'true' or 'false' (performance warning)
  #pragma warning ( disable : 4800 )
#endif


// Framework include files
#include "PVSS/DevType.h"
#include "PVSS/DataPoint.h"
#include "PVSS/Internals.h"
#include "PVSS/PVSSIO.h"
#include "PVSS/ControlsManager.h"
#include "PVSS/DevTypeManager.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <vector>

namespace {
  PVSS::DpID s_nullDP(0);
  struct _Init {
    _Init()  {  pvss_setup_null_dp(&s_nullDP,sizeof(s_nullDP));    }
  };
  _Init s_init;
}
namespace PVSS {
  template <> int DataValue<bool>::type_id()                         { return DevTypeElement::BIT;       }
  template <> int DataValue<char>::type_id()                         { return DevTypeElement::CHAR;      }
  template <> int DataValue<unsigned char>::type_id()                { return DevTypeElement::CHAR;      }
  template <> int DataValue<short>::type_id()                        { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<unsigned short>::type_id()               { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<int>::type_id()                          { return DevTypeElement::INT;       }
  template <> int DataValue<unsigned int>::type_id()                 { return DevTypeElement::UINT;      }
  template <> int DataValue<long>::type_id()                         { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<unsigned long>::type_id()                { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<float>::type_id()                        { return DevTypeElement::FLOAT;     }
  template <> int DataValue<double>::type_id()                       { return DevTypeElement::NOELEMENT; }
  //template <> int DataValue<time_t>::type_id()                     { return DevTypeElement::TIME;    }
  template <> int DataValue<std::string>::type_id()                  { return DevTypeElement::TEXT;      }
  template <> int DataValue<DpID>::type_id()                         { return DevTypeElement::DPID;      }
  template <> int DataValue<DPTime>::type_id()                       { return DevTypeElement::TIME;      }
  template <> int DataValue<DPRef>::type_id()                        { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<std::vector<bool> >::type_id()           { return DevTypeElement::DYNBIT;    }
  template <> int DataValue<std::vector<char> >::type_id()           { return DevTypeElement::DYNCHAR;   }
  template <> int DataValue<std::vector<unsigned char> >::type_id()  { return DevTypeElement::DYNCHAR;   }
  template <> int DataValue<std::vector<short> >::type_id()          { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<std::vector<unsigned short> >::type_id() { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<std::vector<int> >::type_id()            { return DevTypeElement::DYNINT;    }
  template <> int DataValue<std::vector<unsigned int> >::type_id()   { return DevTypeElement::DYNUINT;   }
  template <> int DataValue<std::vector<long> >::type_id()           { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<std::vector<unsigned long> >::type_id()  { return DevTypeElement::NOELEMENT; }
  template <> int DataValue<std::vector<float> >::type_id()          { return DevTypeElement::DYNFLOAT;  }
  template <> int DataValue<std::vector<double> >::type_id()         { return DevTypeElement::NOELEMENT; }
  //template <> int DataValue<std::vector<time_t> >::type_id()       { return DevTypeElement::DYNTIME;   }
  template <> int DataValue<std::vector<std::string> >::type_id()    { return DevTypeElement::DYNTEXT;   }
  template <> int DataValue<std::vector<DpID> >::type_id()           { return DevTypeElement::DYNDPID;   }
  template <> int DataValue<std::vector<DPTime> >::type_id()         { return DevTypeElement::DYNTIME;   }
  template <> int DataValue<std::vector<DPRef> >::type_id()          { return DevTypeElement::NOELEMENT; }
}

namespace PVSS {
  template <class T, class Q> Q convertValue(Value* v) {    return (Q)((DataValue<T>*)v)->data(); }
  template <typename T> T default_value()                        { return T();                    }
  template <> std::string  default_value<std::string>()          { return std::string("");        }
  template <> DpID default_value<DpID>()         { return DpID(s_nullDP); }

  inline bool chk(const Value* v1,const Value* v2)  { return v1->type() == v2->type(); }
  inline void _checkBasicTypeCompatibility(int v1, int v2)  {
    if ( v1 != v2 )  {
      char e[132];
      sprintf(e,"The type %d is incompatible with %d.",v1,v2);
      throw std::runtime_error(e);
    }
  }
  inline void checkBasicTypeCompatibility(const Value* v1, int v2)  {
    _checkBasicTypeCompatibility(v1 ? v1->type() : DevTypeElement::NOELEMENT,v2);
  }
  template <typename T> inline void checkBasicTypeCompatibility(const Value* v1, const DataValue<T>* )  {
    checkBasicTypeCompatibility(v1,DataValue<T>::type_id());
  }
  template <typename T> void checkTypeCompatibility(const Value* v1, const DataValue<T>* v2)    { 
    checkBasicTypeCompatibility(v1,v2);
  }
  template <> void checkTypeCompatibility(const Value* v1, const DataValue<std::vector<char> >* v2)  {
    if ( !(v1 && v2 && (chk(v1,v2) || v1->type()==DataValue<std::vector<unsigned char> >::type_id()) ) )
      checkBasicTypeCompatibility(v1,v2);    
  }
  template <> void checkTypeCompatibility(const Value* v1, const DataValue<std::vector<unsigned char> >* v2)  {
    if ( !(v1 && v2 && (chk(v1,v2) || v1->type()==DataValue<std::vector<char> >::type_id()) ) )
      checkBasicTypeCompatibility(v1,v2);    
  }
  template <> void checkTypeCompatibility(const Value* v1, const DataValue<std::vector<int> >* v2)  {
    if ( !(v1 && v2 && (chk(v1,v2) || v1->type()==DataValue<std::vector<unsigned int> >::type_id()) ) )
      checkBasicTypeCompatibility(v1,v2);    
  }
  template <> void checkTypeCompatibility(const Value* v1, const DataValue<std::vector<unsigned int> >* v2)  {
    if ( !(v1 && v2 && (chk(v1,v2) || v1->type()==DataValue<std::vector<int> >::type_id()) ) )
      checkBasicTypeCompatibility(v1,v2);    
  }
  /// Allocate data buffer
  Value* createValue(int typ)   {
    switch(typ)  {
    case DevTypeElement::DYNDPID:
      return new DataValue<std::vector<DpID> >(typ);
    case DevTypeElement::DYNCHAR:
      return new DataValue<std::vector<char> >(typ);
    case DevTypeElement::DYNINT:
      return new DataValue<std::vector<int> >(typ);
    case DevTypeElement::DYNUINT:
      return new DataValue<std::vector<unsigned int> >(typ);
    case DevTypeElement::DYNFLOAT:
      return new DataValue<std::vector<float> >(typ);
    case DevTypeElement::DYNTEXT:
      return new DataValue<std::vector<std::string> >(typ);
    case DevTypeElement::DYNTIME:
      return new DataValue<std::vector<time_t> >(typ);
    case DevTypeElement::DYNBIT:
      return new DataValue<std::vector<bool> >(typ);
    case DevTypeElement::DPID:
      return new DataValue<DpID>(typ,s_nullDP);
    case DevTypeElement::CHAR:
      return new DataValue<char>(typ);
    case DevTypeElement::INT:
      return new DataValue<int>(typ);
    case DevTypeElement::UINT:
      return new DataValue<unsigned int>(typ);
    case DevTypeElement::FLOAT:
      return new DataValue<float>(typ);
    case DevTypeElement::TEXT:
      return new DataValue<std::string>(typ);
    case DevTypeElement::TIME:
      return new DataValue<time_t>(typ);
    case DevTypeElement::BIT:
      return new DataValue<bool>(typ);
    case DevTypeElement::BIT32:
    default:
      DataPoint::invalidValue(typeid(void));
    }
    return 0;
  }
}

// PVSS namespace usage
using namespace PVSS;

// Some hacks due to comipler hickup!
#ifdef _WIN32
template <typename T> struct GetRef   {   
  bool b1(const DataPoint& d) { return d.reference<T>()==d.data<T>(); } 
  void set(DataPoint& d)      { d.set(d.reference<T>()=d.data<T>());  }
};

#define REF_SPECIALIZATIONS(x)   namespace PVSS { template GetRef< x >; }
#define DATA_SPECIALIZATIONS(x)  namespace PVSS { template GetRef< x >;}

#else
#define __TEMPLATE template
#define REF_SPECIALIZATIONS(x) namespace PVSS {          \
  __TEMPLATE void DataPoint::set< x >(const x&);         \
  __TEMPLATE x& DataPoint::reference< x >();             \
  __TEMPLATE const x& DataPoint::reference< x >() const; }

#define DATA_SPECIALIZATIONS(x) namespace PVSS {         \
  __TEMPLATE x DataPoint::data< x >();                   \
  __TEMPLATE const x DataPoint::data< x >() const; }
#endif


#define EXPLICIT_DATA_SPECIALIZATIONS(x) namespace PVSS {\
  template <> x DataPoint::data< x >()             { return this->reference< x >();} \
  template <> const x DataPoint::data< x >() const { return this->reference< x >();} }

#define BASIC_SPECIALIZATIONS(x)   namespace PVSS {             \
  template <> int Value::type_id< x > (const x&);               \
  template <> DataValue< x > createDataValue< x >(const x& o);} \
  REF_SPECIALIZATIONS(x)

  //template <> int DataValue< x >::type_id();

#define SPECIALIZATIONS(x)          BASIC_SPECIALIZATIONS(x) DATA_SPECIALIZATIONS(x)

#define VECTOR_SPECIALIZATIONS(x)   BASIC_SPECIALIZATIONS(std::vector< x >) \
                                    EXPLICIT_DATA_SPECIALIZATIONS(std::vector< x >)
#define EXPLICIT_SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(x) \
                                    EXPLICIT_DATA_SPECIALIZATIONS(x)

// Default constructor
DataPoint::DataPoint(ControlsManager* m) 
: m_id(s_nullDP), m_valid(0),  m_name(), m_mgr(m), m_val(0)
{
}

// Initializing constructor
DataPoint::DataPoint(ControlsManager* m, const std::string& nam) 
: m_id(s_nullDP), m_valid(0), m_name(), m_mgr(m), m_val(0)
{
  m_name = m_mgr->dpFullName(nam);
}

/// Initializing constructor
DataPoint::DataPoint(ControlsManager* m, const DpID& dpid)  
: m_id(dpid), m_valid(1), m_name(), m_mgr(m), m_val(0) 
{
  char* nam = 0;
  if ( !pvss_lookup_name(m_id,nam) )  {
    m_valid = 0;
    m_id = s_nullDP;
    return;
  }
  m_name = m_mgr->dpFullName(nam);
}

// Copy constructor
DataPoint::DataPoint(const DataPoint& c) 
: m_id(c.m_id), m_valid(0), m_name(c.m_name), m_mgr(c.m_mgr), m_val(0)
{
  if ( c.m_val ) m_val = c.m_val->clone();

  m_flag[0] = c.m_flag[0];
  m_flag[1] = c.m_flag[1];
  m_flag[2] = c.m_flag[2];
}

// Standard destructor
DataPoint::~DataPoint()   {
  // std::cout << "Destructed datapoint:" << m_name << std::endl;
  if ( m_val ) delete m_val;
}

/// load datapoint identifier
const DpID& DataPoint::load() const  {
  DataPoint* p = const_cast<DataPoint*>(this);
  if ( !pvss_lookup_dpid(m_name.c_str(),p->m_id) )    {
    throw std::invalid_argument("DataPoint> The datapoint:"+m_name+" does not exist!");
  }
  p->m_valid = true;
  return m_id;
}

// Assignment
DataPoint& DataPoint::operator=(const DataPoint& c)  {
  if ( this != &c ) {
    m_mgr  = c.manager();
    m_name = c.name();
    m_id   = c.m_id;
    m_val  = 0;
  }
  return *this;
}

// Equivalence check
bool DataPoint::operator==(const DataPoint& c) const    {
  return (this == &c) ? true : m_name==c.m_name && m_id == c.m_id;
}

// Operator <
bool DataPoint::operator<(const DataPoint& c) const    {
  if (this != &c) {
    if      ( m_name < c.m_name ) return true;
    else if ( m_name > c.m_name ) return false;
    else if ( m_id   < c.m_id   ) return true;
  }
  return false;
}

void DataPoint::invalidValue(const std::type_info& tid)  {
  throw std::runtime_error(std::string("Invalid value data:")+tid.name());
}
void DataPoint::invalidConversion(const std::type_info& tid)  {
  throw std::runtime_error(std::string("Invalid type conversion to ")+tid.name());
}

/// Construct name of online value
std::string DataPoint::online(const std::string& dp)  {
  return dp.find(".")==std::string::npos ? dp+".:_online.._value" : dp+":_online.._value";
}

/// Construct name of online value
std::string DataPoint::original(const std::string& dp)  {
  return dp.find(".")==std::string::npos ? dp+".:_original.._value" : dp+":_original.._value";
}

/// Extract name of datapoint from online/original name
std::string DataPoint::dpname(const std::string& dp)    {
  return ControlsManager::dpName(dp);
}

/// Extract name of datapoint from online/original name
std::string DataPoint::dpname(const DpID& dpid)    {
  char* nam = (char*)"";
  if ( !pvss_lookup_name(*(DpID*)&dpid,nam) )  {
    return "";
  }
  return nam;
}

/// Extract name of datapoint from online/original name
std::string DataPoint::dpname()  const {
  return dpname(id());
}

/// Extract system name of datapoint from online/original name
std::string DataPoint::sysname(const std::string& dp)   {
  return ControlsManager::dpSystemName(dp);
}

/// Extract system name of datapoint from online/original name
std::string DataPoint::sysname(const DpID& dp)   {
  char* nam = 0;
  if ( !pvss_lookup_name(dp,nam) )
    return "";
  return sysname(nam);
}

/// Extract system name of datapoint from online/original name
std::string DataPoint::sysname()  const {
  return sysname(name());
}

/// Extract element name of datapoint from online/original name
std::string DataPoint::elementName(const std::string& dp)   {
  return ControlsManager::dpElementName(dp);
}

/// Extract element name of datapoint from online/original name
std::string DataPoint::elementName(const DpID& dp)   {
  char* nam = 0;
  if ( !pvss_lookup_name(dp,nam) )
    return "";
  return elementName(nam);
}

/// Extract element name of datapoint from online/original name
std::string DataPoint::elementName()  const {
  return elementName(name());
}

std::string DataPoint::typeName() const {
  if ( !m_valid ) load();
  const DevType* typ = manager()->typeMgr()->type(m_id.type());
  const DevType::Elements& elts = typ->elements();
  for(DevType::Elements::const_iterator i=elts.begin(); i!= elts.end(); ++i) {
    const DevTypeElement* e = *i;
    if ( e->id() == m_id.element() )  {
      return e->name();
    }
  }
  return "Unknown";
}

/// Check datapoint existencs
bool DataPoint::exists(const std::string& name)   {
  DpID id(0);
  return pvss_lookup_dpid(name.c_str(),id);
}

template <class T> T DataPoint::data()  {
  if ( m_val )  {
    switch(m_val->type())  {
      case DevTypeElement::FLOAT: return convertValue<float,T>(m_val);
      case DevTypeElement::CHAR:  return convertValue<char,T>(m_val);
      case DevTypeElement::INT:   return convertValue<int,T>(m_val);
      case DevTypeElement::UINT:  return convertValue<unsigned int,T>(m_val);
      case DevTypeElement::TIME:  return convertValue<time_t,T>(m_val);
      case DevTypeElement::BIT:   return convertValue<bool,T>(m_val);
      default:                    break;
    }
    invalidConversion(typeid(T));
  }
  invalidValue(typeid(T));
  return default_value<T>();
}

template <class T> const T PVSS::DataPoint::data() const {
  if ( m_val )  {
    switch(m_val->type())  {
      case DevTypeElement::FLOAT: return convertValue<float,T>(m_val);
      case DevTypeElement::CHAR:  return convertValue<char,T>(m_val);
      case DevTypeElement::INT:   return convertValue<int,T>(m_val);
      case DevTypeElement::UINT:  return convertValue<unsigned int,T>(m_val);
      case DevTypeElement::TIME:  return convertValue<time_t,T>(m_val);
      case DevTypeElement::BIT:   return convertValue<bool,T>(m_val);
      default:                    break;
    }
    invalidConversion(typeid(T));
  }
  invalidValue(typeid(T));
  return default_value<T>();
}
namespace PVSS {
template <> const std::string DataPoint::data<std::string>() const  {
  if ( m_val )  {
    std::stringstream os;
    switch(m_val->type())  {
      case DevTypeElement::FLOAT:
        os << this->data<float>();
        return os.str();
      case DevTypeElement::CHAR:
        os << this->data<char>();
        return os.str();
      case DevTypeElement::INT:
        os << this->data<int>();
        return os.str();
      case DevTypeElement::UINT:
        os << this->data<unsigned int>();
        return os.str();
      case DevTypeElement::TIME:
        os << this->data<time_t>();
        return os.str();
      case DevTypeElement::BIT:
        os << this->data<bool>();
        return os.str();
      case DevTypeElement::TEXT:
        return ((DataValue<std::string>*)m_val)->data();
      default: 
        break;
    }
    invalidConversion(typeid(std::string));
  }
  invalidValue(typeid(std::string));
  return "";
}

template <> std::string DataPoint::data<std::string>()  {
  if ( m_val )  {
    std::stringstream os;
    switch(m_val->type())  {
      case DevTypeElement::FLOAT:
        os << this->data<float>();
        return os.str();
      case DevTypeElement::CHAR:
        os << this->data<char>();
        return os.str();
      case DevTypeElement::INT:
        os << this->data<int>();
        return os.str();
      case DevTypeElement::UINT:
        os << this->data<unsigned int>();
        return os.str();
      case DevTypeElement::TIME:
        os << this->data<time_t>();
        return os.str();
      case DevTypeElement::BIT:
        os << this->data<bool>();
        return os.str();
      case DevTypeElement::TEXT:
        return ((DataValue<std::string>*)m_val)->data();
      default: 
        break;
    }
    invalidConversion(typeid(std::string));
  }
  invalidValue(typeid(std::string));
  return "";
}
}

template <class T> T& DataPoint::reference()  {
  checkTypeCompatibility(m_val,(DataValue<T>*)m_val);
  return ((DataValue<T>*)m_val)->data();
}

template <class T> const T& DataPoint::reference()  const  {
  checkTypeCompatibility(m_val,(DataValue<T>*)m_val);
  return ((DataValue<T>*)m_val)->data();
}


BASIC_SPECIALIZATIONS(bool)
BASIC_SPECIALIZATIONS(char)
SPECIALIZATIONS(unsigned char)
SPECIALIZATIONS(short)
SPECIALIZATIONS(unsigned short)
BASIC_SPECIALIZATIONS(int)
BASIC_SPECIALIZATIONS(unsigned int)
BASIC_SPECIALIZATIONS(long)
SPECIALIZATIONS(unsigned long)
BASIC_SPECIALIZATIONS(float)
SPECIALIZATIONS(double)

BASIC_SPECIALIZATIONS(std::string)
EXPLICIT_SPECIALIZATIONS(DpID)
EXPLICIT_SPECIALIZATIONS(DPRef)
EXPLICIT_SPECIALIZATIONS(DPTime)

VECTOR_SPECIALIZATIONS(bool)
VECTOR_SPECIALIZATIONS(char)
VECTOR_SPECIALIZATIONS(unsigned char)
VECTOR_SPECIALIZATIONS(short)
VECTOR_SPECIALIZATIONS(unsigned short)
VECTOR_SPECIALIZATIONS(int)
VECTOR_SPECIALIZATIONS(unsigned int)
VECTOR_SPECIALIZATIONS(long)
VECTOR_SPECIALIZATIONS(unsigned long)
VECTOR_SPECIALIZATIONS(float)
VECTOR_SPECIALIZATIONS(double)
VECTOR_SPECIALIZATIONS(std::string)
VECTOR_SPECIALIZATIONS(DpID)
VECTOR_SPECIALIZATIONS(DPRef)
VECTOR_SPECIALIZATIONS(DPTime)

/// Set value data
void DataPoint::setValue(int typ, const Variable* variable)  {
  try {
    if ( variable ) {
      if ( !m_val ) m_val = createValue(typ);
      genReadIO(variable,typ,m_val->ptr());
      if ( pvss_debug() > 0 )  {
        const char* val_typ = pvss_type_name(m_val->type());
        const char* true_typ = pvss_type_name(typ);
        std::cout << "DataPoint::setValue> " << name() << " Typ:" << val_typ << " - " << true_typ << std::endl;
      }
      return;
    }
    std::cout << "DataPoint::setValue> " << name() << " No Variable pointer. Action ignored." << std::endl;    
  }
  catch(const std::exception& e)  {
    throw e;
  }
  catch(const char* msg)  {
    throw std::runtime_error(msg ? msg : "Unknown error");
  }
  catch(...)  {
    throw std::runtime_error("Unknown error in DataPoint::setValue");
  }
}

/// Set value data (for publishing data to PVSS
template <typename T> void DataPoint::set(const T& val)  {
  int dbg = pvss_debug();
  try  {
    if ( !m_val )  {
      if ( !m_valid ) load();
      const DevType* typ = manager()->typeMgr()->type(m_id.type());
      const DevType::Elements& elts = typ->elements();
      for(DevType::Elements::const_iterator i=elts.begin(); i!= elts.end(); ++i) {
        const DevTypeElement* e = *i;
        if ( e->id() == m_id.element() )  {
          m_val = createValue(e->content());
          break;
        }
      }
      if ( !m_val )  {
        invalidValue(typeid(T));
      }
    }
    const char* val_typ = pvss_type_name(m_val->type());
    const char* true_typ = pvss_type_name(DataValue<T>::type_id());
    if ( dbg > 0 )  {
      std::cout << "DataPoint::set> " << dpname() << " Typ:" << val_typ << " - " << true_typ << std::endl;
    }
    if ( m_val->type() != DataValue<T>::type_id() )  {
      if ( dbg > 0 )  {
        std::cout << "                " << dpname() << " Typ:" << typeid(T).name() << std::endl;
      }
      invalidConversion(typeid(T));
    }
    reference<T>() = val;
    return;
  }
  catch(const char* msg)  {
    if ( dbg > 0 )  {
      std::cout << dpname() << "> Exception in DataPoint::set(" << typeid(T).name() << "): "
		<< (const char*)(msg ? msg : "(Unknwon message)")  << " " << typeName()
		<< std::endl;
    }
    if ( m_val ) delete m_val;
    m_val = 0;
    throw std::runtime_error(msg);
  }
  catch(const std::exception& e)  {
    if ( dbg > 0 )  {
      std::cout << dpname() << "> Exception in DataPoint::set(" << typeid(T).name() << "): " 
                << e.what() << " " << typeName() << std::endl;
    }
    if ( m_val ) delete m_val;
    m_val = 0;
    throw std::runtime_error(e.what());
  }
  catch(...)  {
    if ( dbg > 0 )  {
      std::cout << dpname() << "> Exception in DataPoint::set(" << typeid(T).name() << "): "
                << "[Unknown exception] " << typeName() << std::endl;
    }
    if ( m_val ) delete m_val;
    m_val = 0;
    invalidConversion(typeid(T));
  }
}

namespace {
  struct ListHandler {
    DPListActor* m_l;
    ListHandler(DPListActor* l) : m_l(l) {}
    void operator()(const DpID& dpid)  {
      m_l->operator()(dpid);
    }
  };
}

DPListActor::DPListActor(ControlsManager* m) : m_manager(m)  {
}

DPListActor::~DPListActor() {
}

bool DPListActor::lookup(const std::string& nam, const DevType* typ)  {
  int id = (typ) ? typ->id() : 0;
  std::string dp_name = m_manager->dpFullName(nam);
  if ( pvss_lookup_dpidset(dp_name.c_str(),m_dpids,m_count,id) )  {
    std::for_each(&m_dpids[0],&m_dpids[m_count],ListHandler(this));
    return true;
  }
  throw std::runtime_error("Cannot load datapoint identifiers for type:"+dp_name);
}

bool DPListActor::lookupOriginal(const std::string& nam, const DevType* typ)  {
  return this->lookup(DataPoint::original(nam),typ);
}


