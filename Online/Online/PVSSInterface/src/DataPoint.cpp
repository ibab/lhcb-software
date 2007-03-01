// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/src/DataPoint.cpp,v 1.6 2007-03-01 22:00:01 frankb Exp $
//  ====================================================================
//  DataPoint.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: DataPoint.cpp,v 1.6 2007-03-01 22:00:01 frankb Exp $
#ifdef _WIN32
  // Disable warning C4250: 'const float' : forcing value to bool 'true' or 'false' (performance warning)
  #pragma warning ( disable : 4800 )
#endif

// Framework include files
#include "PVSS/DevType.h"
#include "PVSS/DataPoint.h"
#include "PVSS/Internals.h"
#include "PVSS/DevTypeElement.h"
#include "PVSSManager/DatapointIO.h"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <vector>

// PVSS namespace usage
using namespace PVSS;

namespace {
  DpIdentifier s_nullDP(0);
  struct _Init {
    _Init()  {  pvss_setup_null_dp(&s_nullDP,sizeof(s_nullDP));    }
  };
  _Init s_init;
}

template <> int DataValue<bool>::type_id()                         { return DevTypeElement::BIT;       }
template <> int DataValue<char>::type_id()                         { return DevTypeElement::CHAR;      }
template <> int DataValue<unsigned char>::type_id()                { return DevTypeElement::CHAR;      }
template <> int DataValue<short>::type_id()                        { return DevTypeElement::NOELEMENT; }
template <> int DataValue<unsigned short>::type_id()               { return DevTypeElement::NOELEMENT; }
template <> int DataValue<int>::type_id()                          { return DevTypeElement::INT;       }
template <> int DataValue<unsigned int>::type_id()                 { return DevTypeElement::INT;       }
template <> int DataValue<long>::type_id()                         { return DevTypeElement::NOELEMENT; }
template <> int DataValue<unsigned long>::type_id()                { return DevTypeElement::NOELEMENT; }
template <> int DataValue<float>::type_id()                        { return DevTypeElement::FLOAT;     }
template <> int DataValue<double>::type_id()                       { return DevTypeElement::NOELEMENT; }
//template <> int DataValue<time_t>::type_id()                       { return DevTypeElement::TIME;    }
template <> int DataValue<std::string>::type_id()                  { return DevTypeElement::TEXT;      }
template <> int DataValue<DpIdentifier>::type_id()                 { return DevTypeElement::DPID;      }
template <> int DataValue<DPTime>::type_id()                       { return DevTypeElement::TIME;      }
template <> int DataValue<DPRef>::type_id()                        { return DevTypeElement::NOELEMENT; }
template <> int DataValue<std::vector<bool> >::type_id()           { return DevTypeElement::DYNBIT;    }
template <> int DataValue<std::vector<char> >::type_id()           { return DevTypeElement::DYNCHAR;   }
template <> int DataValue<std::vector<unsigned char> >::type_id()  { return DevTypeElement::DYNCHAR;   }
template <> int DataValue<std::vector<short> >::type_id()          { return DevTypeElement::NOELEMENT; }
template <> int DataValue<std::vector<unsigned short> >::type_id() { return DevTypeElement::NOELEMENT; }
template <> int DataValue<std::vector<int> >::type_id()            { return DevTypeElement::DYNINT;    }
template <> int DataValue<std::vector<unsigned int> >::type_id()   { return DevTypeElement::DYNINT;    }
template <> int DataValue<std::vector<long> >::type_id()           { return DevTypeElement::NOELEMENT; }
template <> int DataValue<std::vector<unsigned long> >::type_id()  { return DevTypeElement::NOELEMENT; }
template <> int DataValue<std::vector<float> >::type_id()          { return DevTypeElement::DYNFLOAT;  }
template <> int DataValue<std::vector<double> >::type_id()         { return DevTypeElement::NOELEMENT; }
//template <> int DataValue<std::vector<time_t> >::type_id()       { return DevTypeElement::DYNTIME;   }
template <> int DataValue<std::vector<std::string> >::type_id()    { return DevTypeElement::DYNTEXT;   }
template <> int DataValue<std::vector<DpIdentifier> >::type_id()   { return DevTypeElement::DYNDPID;   }
template <> int DataValue<std::vector<DPTime> >::type_id()         { return DevTypeElement::DYNTIME;   }
template <> int DataValue<std::vector<DPRef> >::type_id()          { return DevTypeElement::NOELEMENT; }

namespace PVSS {
  static Value* invalidValue() 
  { throw std::runtime_error("Invalid type for data conversion.");                        }
  template <typename T> static DataValue<T>& holder(Value* v)   { 
    if(v->type()==DataValue<T>::type_id()) return *(DataValue<T>*)v;
    invalidValue();
    return *(DataValue<T>*)v;
  }
  template <class T, class Q> Q convertValue(Value* v) {
    if ( v->type() != DataValue<T>::type_id())invalidValue();
#ifdef _WIN32
    return ((DataValue<T>*)v)->value<Q>();
#else
    return (Q)((DataValue<T>*)v)->data();
#endif
  }
  template <typename T> T default_value()                        { return T();                    }
  template <> std::string  default_value<std::string>()          { return std::string("");        }
  template <> DpIdentifier default_value<DpIdentifier>()         { return DpIdentifier(s_nullDP); }
  static void copy_string  (std::string& t,const char* s)        { t = s;                         }
  static void insert_bool  (std::vector<bool>& t,bool s)         { t.push_back(s);                }
  static void insert_char  (std::vector<char>& t,char s)         { t.push_back(s);                }
  static void insert_short (std::vector<short>& t,short s)       { t.push_back(s);                }
  static void insert_int   (std::vector<int>& t,int s)           { t.push_back(s);                }
  static void insert_float (std::vector<float>& t,float s)       { t.push_back(s);                }
  static void insert_time_t(std::vector<time_t>& t,time_t s)     { t.push_back(s);                }
  static void insert_dpid  (std::vector<DpIdentifier>& t,const DpIdentifier& s) { t.push_back(s); }
  static void insert_string(std::vector<std::string>& t,const char* s)          { t.push_back(s); }

  /// Allocate data buffer
  static PVSS::Value* createValue(int typ)   {
    switch(typ)  {
    case DevTypeElement::DYNDPID:
      return new DataValue<std::vector<DpIdentifier> >(typ);
    case DevTypeElement::DYNCHAR:
      return new DataValue<std::vector<char> >(typ);
    case DevTypeElement::DYNINT:
      //return new DataValue<std::vector<short> >(typ);
    case DevTypeElement::DYNUINT:
      return new DataValue<std::vector<int> >(typ);
    case DevTypeElement::DYNFLOAT:
      return new DataValue<std::vector<float> >(typ);
    case DevTypeElement::DYNTEXT:
      return new DataValue<std::vector<std::string> >(typ);
    case DevTypeElement::DYNTIME:
      return new DataValue<std::vector<time_t> >(typ);
    case DevTypeElement::DYNBIT:
      return new DataValue<std::vector<bool> >(typ);
    case DevTypeElement::DPID:
      return new DataValue<DpIdentifier>(typ,s_nullDP);
    case DevTypeElement::CHAR:
      return new DataValue<char>(typ);
    case DevTypeElement::INT:
      //return new DataValue<short>(typ);
    case DevTypeElement::UINT:
      return new DataValue<int>(typ);
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

// Default constructor
DataPoint::DataPoint(ControlsManager* m) 
: m_id(s_nullDP), m_valid(0), m_mgr(m), m_val(0)
{
}

// Initializing constructor
DataPoint::DataPoint(ControlsManager* m, const std::string& nam) 
: m_id(s_nullDP), m_valid(0), m_name(nam), m_mgr(m), m_val(0)
{
}

/// Initializing constructor
DataPoint::DataPoint(ControlsManager* m, const DpIdentifier& dpid)  
: m_id(dpid), m_valid(1), m_name(), m_mgr(m), m_val(0) 
{
  char* nam = 0;
  if ( !pvss_lookup_name(m_id,nam) )  {
    m_valid = 0;
    m_id = s_nullDP;
    return;
  }
  m_name = nam;
}

// Copy constructor
DataPoint::DataPoint(const DataPoint& c) 
: m_id(c.m_id), m_valid(0), m_name(c.m_name), m_mgr(c.m_mgr), m_val(0)
{
}

// Standard destructor
DataPoint::~DataPoint()   {
  if ( m_val ) delete m_val;
}

/// load datapoint identifier
const DpIdentifier& DataPoint::load() const  {
  DataPoint* p = const_cast<DataPoint*>(this);
  if ( !pvss_lookup_dpid(m_name.c_str(),p->m_id) )    {
    throw std::invalid_argument("The datapoint:"+m_name+" does not exist!");
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
std::string DataPoint::dpname(const DpIdentifier& dpid)    {
  char* nam = "";
  if ( !pvss_lookup_name(*(DpIdentifier*)&dpid,nam) )  {
    return "";
  }
  return nam;
}

/// Extract name of datapoint from online/original name
std::string DataPoint::dpname(const std::string& dp)    {
  std::string::size_type id1 = dp.find(":");
  std::string::size_type id2 = dp.rfind(":");
  if ( id1 == std::string::npos && id2 == std::string::npos )
    return dp;
  else if ( id2 > id1 )
    return dp.substr(0,id2);
  else if ( id1 == id2 )  {
    std::string s = dp.substr(id1,3);
    if( s == ":_o")
      return dp.substr(0,id1);
  }
  return dp;
}

/// Extract system name of datapoint from online/original name
std::string DataPoint::sysname(const std::string& dp)   {
  std::string::size_type id1 = dp.find(":");
  std::string::size_type id2 = dp.rfind(":",id1+1);
  if ( id1 == std::string::npos && id2 == std::string::npos )
    return "";
  else if ( id2 == std::string::npos )
    return dp.substr(0,id1);
  return dp.substr(0,id1);
}

template <typename T> T DataPoint::data()  {
  if ( m_val )  {
    switch(m_val->type())  {
      case DevTypeElement::FLOAT: return convertValue<float,T>(m_val);
      case DevTypeElement::CHAR:  return convertValue<char,T>(m_val);
      case DevTypeElement::INT:   return convertValue<int,T>(m_val);
      case DevTypeElement::UINT:  return convertValue<int,T>(m_val);
      case DevTypeElement::TIME:  return convertValue<time_t,T>(m_val);
      case DevTypeElement::BIT:   return convertValue<bool,T>(m_val);
      default:                    break;
    }
    invalidConversion(typeid(T));
  }
  invalidValue(typeid(T));
  return default_value<T>();
}

template <typename T> const T DataPoint::data()  const {
  if ( m_val )  {
    switch(m_val->type())  {
      case DevTypeElement::FLOAT: return convertValue<float,T>(m_val);
      case DevTypeElement::CHAR:  return convertValue<char,T>(m_val);
      case DevTypeElement::INT:   return convertValue<int,T>(m_val);
      case DevTypeElement::UINT:  return convertValue<int,T>(m_val);
      case DevTypeElement::TIME:  return convertValue<time_t,T>(m_val);
      case DevTypeElement::BIT:   return convertValue<bool,T>(m_val);
      default:                    break;
    }
    invalidConversion(typeid(T));
  }
  invalidValue(typeid(T));
  return default_value<T>();
}

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
        os << this->data<short>();
        return os.str();
      case DevTypeElement::UINT:
        os << this->data<int>();
        return os.str();
      case DevTypeElement::TIME:
        os << this->data<time_t>();
        return os.str();
      case DevTypeElement::BIT:
        os << this->data<bool>();
        return os.str();
      case DevTypeElement::TEXT:
        return holder<std::string>(m_val).data();
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
        os << this->data<short>();
        return os.str();
      case DevTypeElement::UINT:
        os << this->data<int>();
        return os.str();
      case DevTypeElement::TIME:
        os << this->data<time_t>();
        return os.str();
      case DevTypeElement::BIT:
        os << this->data<bool>();
        return os.str();
      case DevTypeElement::TEXT:
        return holder<std::string>(m_val).data();
      default: 
        break;
    }
    invalidConversion(typeid(std::string));
  }
  invalidValue(typeid(std::string));
  return "";
}

template <class T> T& DataPoint::reference()  {
  if ( m_val->type()!=DataValue<T>::type_id() ) invalidValue(DataValue<T>::type_info());
  return ((DataValue<T>*)m_val)->data();
}

template <class T> const T& DataPoint::reference()  const  {
  if ( m_val->type()!=DataValue<T>::type_id() ) invalidValue(DataValue<T>::type_info());
  return ((DataValue<T>*)m_val)->data();
}

#define BASIC_SPECIALIZATIONS1(x)   namespace PVSS {            \
  template <> int Value::type_id< x > (const x&);               \
  template <> int DataValue< x >::type_id();                    \
  template <> void DataPoint::set< x >(const x&);               \
  template <> DataValue< x > createDataValue< x >(const x& o);  \
}

// Some hacks due to comipler hickup!
#ifdef _WIN32
template <typename T> struct GetRef   {   
  T& ref1(DataPoint& dp)             { return dp.reference<T>(); } 
  const T& ref2(const DataPoint& dp) { return dp.reference<T>(); } 
};
template <typename T> struct GetData {
  T get1(const DataPoint& dp) { return dp.data<T>();  } 
  T get2(DataPoint& dp)       { return dp.data<T>(); } 
};
#define BASIC_SPECIALIZATIONS(x)   BASIC_SPECIALIZATIONS1(x) namespace PVSS { template GetRef< x >; }
#define SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(x) namespace PVSS { template GetData< x >;}

#else
#define BASIC_SPECIALIZATIONS(x)   BASIC_SPECIALIZATIONS1(x) namespace PVSS { \
  template <> x& DataPoint::reference< x >();             \
  template <> const x& DataPoint::reference< x >() const; }

#define SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(x) namespace PVSS { \
  template <> x DataPoint::data< x >();                   \
  template <> const x DataPoint::data< x >() const; }


#endif


#define VECTOR_SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(std::vector< x >)

BASIC_SPECIALIZATIONS(bool)
BASIC_SPECIALIZATIONS(char)
SPECIALIZATIONS(unsigned char)
BASIC_SPECIALIZATIONS(short)
SPECIALIZATIONS(unsigned short)
BASIC_SPECIALIZATIONS(int)
SPECIALIZATIONS(unsigned int)
BASIC_SPECIALIZATIONS(long)
SPECIALIZATIONS(unsigned long)
BASIC_SPECIALIZATIONS(float)
SPECIALIZATIONS(double)
BASIC_SPECIALIZATIONS(std::string)
BASIC_SPECIALIZATIONS(DpIdentifier)
BASIC_SPECIALIZATIONS(DPRef)
BASIC_SPECIALIZATIONS(DPTime)

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
VECTOR_SPECIALIZATIONS(DpIdentifier)

/// Set value data
void DataPoint::setValue(int typ, const Variable* variable)  {
  try {
    if ( !m_val ) m_val = createValue(typ);
    switch(m_val->type())  {
    case DevTypeElement::CHAR:
      DatapointIO::value(variable,reference<char>());
      break;
    case DevTypeElement::INT:
      //DatapointIO::value(variable,reference<short>());
      //break;
    case DevTypeElement::UINT:
      DatapointIO::value(variable,reference<int>());
      break;
    case DevTypeElement::TIME:
      DatapointIO::value(variable,reference<time_t>());
      break;
    case DevTypeElement::BIT:
      DatapointIO::value(variable,reference<bool>());
      break;
    case DevTypeElement::FLOAT:
      DatapointIO::value(variable,reference<float>());
      break;
    case DevTypeElement::DPID:
      DatapointIO::value(variable,reference<DpIdentifier>());
      break;
    case DevTypeElement::TEXT:
      DatapointIO::value(variable,copy_string,reference<std::string>());
      break;
    case DevTypeElement::DYNCHAR:
      DatapointIO::value(variable,insert_char,reference<std::vector<char> >());
      break;
    case DevTypeElement::DYNINT:
      //do_vector_io(variable,reference<std::vector<short> >());
      //break;
    case DevTypeElement::DYNUINT:
      DatapointIO::value(variable,insert_int,reference<std::vector<int> >());
      break;
    case DevTypeElement::DYNTEXT:
      DatapointIO::value(variable,insert_string,reference<std::vector<std::string> >());
      break;
    case DevTypeElement::DYNTIME:
      DatapointIO::value(variable,insert_time_t,reference<std::vector<time_t> >());
      break;
    case DevTypeElement::DYNBIT:
      DatapointIO::value(variable,insert_bool,reference<std::vector<bool> >());
      break;
    case DevTypeElement::DYNFLOAT:
      DatapointIO::value(variable,insert_float,reference<std::vector<float> >());
      break;
    case DevTypeElement::DYNDPID:
      DatapointIO::value(variable,insert_dpid,reference<std::vector<DpIdentifier> >());
      break;
    case DevTypeElement::BIT32:
    default:
      invalidValue(typeid(void));
      break;
    }
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
  if ( !m_val ) m_val = createValue(DataValue<T>::type_id());
  if ( m_val->type() != DataValue<T>::type_id() )  {
    invalidValue(typeid(T));
  }
  reference<T>() = val;
}

namespace {
  struct ListHandler {
    DPListActor* m_l;
    ListHandler(DPListActor* l) : m_l(l) {}
    void operator()(const DpIdentifier& dpid)  {
      m_l->operator()(dpid);
    }
  };
}

DPListActor::DPListActor(ControlsManager* m) : m_manager(m)  {
}

DPListActor::~DPListActor() {
}

bool DPListActor::lookup(const std::string& typ_nam, const DevType* typ)  {
  if ( pvss_lookup_dpidset(typ_nam.c_str(),m_dpids,m_count,typ->id()) )  {
    std::for_each(&m_dpids[0],&m_dpids[m_count],ListHandler(this));
    return true;
  }
  throw std::runtime_error("Cannot load datapoint identifiers for type:"+typ_nam);
}
