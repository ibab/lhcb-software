#include "PyRPC.h"
#include "RTL/rtl.h"
#include <list>
#include <cmath>
#include <cerrno>
#include <sstream>
#include <cstring>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include "RTL/time.h"
#include "NET/defs.h"
#define DATA_LEN   6
#define PARAM_LEN  7
#define VALUE_LEN  7
#define MEMBER_LEN 8
#define STRUCT_LEN 8

using namespace std;
using namespace PyRPC;

namespace PyRPC {
static string dump(const _ContBase& t, const string& pre="", const string& post="")  {
  stringstream os;
  list<Arg>* p = (list<Arg>*)t.data;
  for(list<Arg>::const_iterator i=p->begin(); i != p->end(); ++i)  {
    string s = (*i).str();
    if ( !s.empty() ) os << pre << s << post;
  }
  return os.str();
}
template <> _Cont<0>& _Cont<0>::load(const string& vals) 
{  loadValues(vals,"value");  return *this;                   }

template <> string _Cont<0>::str() const  
{  return dump(*this);                                        }

template <> _Cont<1>& _Cont<1>::load(const string& vals)
{  loadValues(vals,"value");  return *this;                   }

template <> string _Cont<1>::str() const
{  return dump(*this);                                        }

template <> _Cont<2>& _Cont<2>::load(const string& vals) 
{  loadValues(vals,"member");  return *this;                  }

template <> string _Cont<2>::str() const
{  return dump(*this);                                        }

template <> _Cont<3>& _Cont<3>::load(const string& vals) 
{  loadValues(vals,"value");   return *this;                  }

template <> string _Cont<3>::str() const  
{  return dump(*this,"<param>\n","</param>\n");               }
}

#define TIME_FMT "%Y%m%dT%H:%M:%S" 

static bool s_debug = false;

bool PyRPC::setDebug(bool value) {
  bool tmp = s_debug;
  s_debug = value;
  return tmp;
}

bool PyRPC::debug() {
  return s_debug;
}

Item::Item(const string& s, const string& tag) : m_s(s)  {
  m_tag   = tag+">";
  m_item  = "";
  m_start = 0;
  m_end   = 0;
  get();
}

const string& Item::get() {
  if ( m_start != string::npos && m_end != string::npos ) {
    size_t id = m_start;
    int    lvl = 0;
    char   c;
    while( (id=m_s.find(m_tag,id)) != string::npos )  {
      c = m_s[id-1];
      switch(c) {
      case '<':
        if ( ++lvl == 1 ) m_start = id-1;
        break;
      case '/':
        if ( --lvl > 0 ) break;
        m_end   = id-2;
        m_item  = m_s.substr(m_start,m_end-m_start+m_tag.length()+2);
        m_start = m_end+m_tag.length()+1;
        return m_item;
        break;
      default:
        break;
      }
      id += m_tag.length();
    }
    m_start = m_end = string::npos;
    m_item = "";
  }
  return m_item;
}

string Item::value() const {
  if ( !empty() ) {
    size_t i=0, l=m_tag.length()+1;
    string s = m_item.substr(l,m_item.length()-2*l-1);
    while(::isspace(s[i]))++i;
    return &s[i];
  }
  return "";
}

_ContBase::_ContBase()  {  data = new list<Arg>();  }

_ContBase::~_ContBase()  {
  list<Arg>* p = (list<Arg>*)data;
  if ( p ) delete p;
  data = 0;
}

_ContBase& _ContBase::operator=(const _ContBase& c)  
{ return copy(&c.data);                   }

void _ContBase::add(const Arg& a)  
{ ((list<Arg>*)data)->push_back(a);       }

_ContBase& _ContBase::copy(const void* data_ptr)  {
  if ( data_ptr != data )  {
    list<Arg>* p1       = (list<Arg>*)data;
    const list<Arg>* p2 = (const list<Arg>*)data_ptr;
    *p1 = *p2;
  }
  return *this;
}

bool _ContBase::operator<(const _ContBase& c) const  {
  list<Arg>* p1 = (list<Arg>*)data;
  list<Arg>* p2 = (list<Arg>*)c.data;
  if ( p1->size() < p2->size() )
    return true;
  else if ( p1->size() > p2->size() )
    return false;
  list<Arg>::const_iterator i1=p1->begin();
  list<Arg>::const_iterator i2=p2->begin();
  for(; i1 != p1->end(); ++i1, ++i2)
    if ( *i1 < *i2 ) return true;
  return false;
}

bool _ContBase::operator==(const _ContBase& c) const  {
  list<Arg>* p1 = (list<Arg>*)data;
  list<Arg>* p2 = (list<Arg>*)c.data;
  if ( p1->size() != p2->size() )
    return false;
  list<Arg>::const_iterator i1=p1->begin();
  list<Arg>::const_iterator i2=p2->begin();
  for(; i1 != p1->end(); ++i1, ++i2)
    if ( !(*i1 == *i2) ) return false;
  return true;
}

void _ContBase::loadValues(const string& vals, const string& tag)  {
  for(Items i(vals,tag); !i.empty(); ++i)
    add(Arg().load(i,tag));
}

/// Print object content to stdout
void _ContBase::print(const string& prefix) const {
  list<Arg>* l = (list<Arg>*)data;
  for(list<Arg>::const_iterator i=l->begin(); i != l->end(); ++i)
    (*i).print(prefix+" ->");
}

Arg::Arg() : type(NONE)                    { data.i64val = 0;          }
Arg::Arg(bool b) : type(BOOL)              { data.boolean = b;         }
Arg::Arg(char n) : type(CHAR)              { data.character = n;       }
Arg::Arg(unsigned char n) : type(CHAR)     { data.character = n;       }
Arg::Arg(int n) : type(INT)                { data.ival = n;            }
Arg::Arg(unsigned int n) : type(INT)       { data.ival = n;            }
Arg::Arg(long n) : type(INT64)             { data.i64val = n;          }
Arg::Arg(unsigned long n) : type(INT64)    { data.i64val = n;          }
Arg::Arg(long long int n) : type(INT64)    { data.i64val = n;          }
Arg::Arg(unsigned long long int n) : type(INT64) { data.i64val = n;    }
Arg::Arg(float n) : type(FLOAT)            { data.fval = n;            }
Arg::Arg(double n) : type(DOUBLE)          { data.dval = n;            }
Arg::Arg(const char* n) : type(STRING)     { assign(n);                }
Arg::Arg(const string& n) : type(STRING)   { assign(n.c_str());        }
Arg::Arg(const struct tm& t) : type(TIME)  { Time ti(t); assign(&ti);  }
Arg::Arg(const Time& t)  : type(TIME)      { assign(&t);      }
Arg::Arg(const Pair& p)  : type(PAIR)      { assign(&p);      }
Arg::Arg(const Param& p) : type(PARAM)     { assign(&p);      }
Arg::Arg(const List& l)  : type(LIST)      { assign(&l);      }
Arg::Arg(const Tuple& t) : type(TUPLE)     { assign(&t);      }
Arg::Arg(const Dict& d)  : type(DICT)      { assign(&d);      }
Arg::Arg(const Blob& b)  : type(BINARY)    { assign(&b);      }
Arg::Arg(const Args& d)  : type(ARGS)      { assign(&d);      }
Arg::Arg(const Arg& c)   : type(c.type)    { assign(c.data);  }
Arg::~Arg()  {  clear();  }

Arg& Arg::operator=(const Arg& c)  {
  if ( this != &c ) {
    clear();
    assign(c.type,c.data);
  }
  return *this;
}

bool Arg::operator<(const Arg& c) const  {
  if ( type < c.type ) return true;
  else if ( type > c.type ) return false;
  switch(type)  {
  case BOOL:       return data.boolean   <  c.data.boolean;
  case CHAR:       return data.character <  c.data.character;
  case INT64:      return data.i64val    <  c.data.i64val;
  case INT:        return data.ival      <  c.data.ival;
  case FLOAT:      return data.fval      <  c.data.fval;
  case DOUBLE:     return data.dval      <  c.data.dval;
  case PAIR:       return *data.pair     < *c.data.pair;
  case PARAM:      return *data.param    < *c.data.param;
  case TUPLE:      return *data.tuple    < *c.data.tuple;
  case LIST:       return *data.list     < *c.data.list;
  case DICT:       return *data.dict     < *c.data.dict;
  case ARGS:       return *data.args     < *c.data.args;
  case BINARY:     return *data.blob     < *c.data.blob; 
  case TIME:       return *data.time     < *c.data.time;
  case STRING:     return strcmp(data.str,c.data.str) < 0;
  }
  return false;
}

bool Arg::operator==(const Arg& c) const  {
  if ( type != c.type ) return false;
  switch(type)  {
  case BOOL:       return data.boolean   == c.data.boolean;
  case CHAR:       return data.character == c.data.character;
  case INT64:      return data.i64val == c.data.i64val;
  case INT:        return data.ival   == c.data.ival;
  case FLOAT:      return fabs(data.fval-c.data.fval)<std::numeric_limits<float>::epsilon();
  case DOUBLE:     return fabs(data.dval-c.data.dval)<std::numeric_limits<double>::epsilon();
  case PAIR:       return *data.pair  == *c.data.pair;
  case PARAM:      return *data.param == *c.data.param;
  case TUPLE:      return *data.tuple == *c.data.tuple;
  case LIST:       return *data.list  == *c.data.list;
  case DICT:       return *data.dict  == *c.data.dict;
  case ARGS:       return *data.args  == *c.data.args;
  case BINARY:     return *data.blob  == *c.data.blob; 
  case TIME:       return *data.time  == *c.data.time;
  case STRING:     return strcmp(data.str,c.data.str) == 0;
  }
  return false;
}

template<class T> static inline void _get(T*& pointer, T* source)  {
  pointer = new T();
  pointer->copy(source->data);
}

Arg& Arg::assign(int typ, const Data& d)  {
  type = typ;
  return assign(d);
}

Arg& Arg::assign(const Data& d)  {
  switch(type)  {
  case CHAR:
  case BOOL:
  case DOUBLE:
  case FLOAT:
  case INT:
  case INT64:   data.i64val  = d.i64val;                             break;
  case STRING:  ::strcpy(data.str=new char[strlen(d.str)+1],d.str);  break;
  case TIME:    data.time  = new Time(*d.time);                      break;
  case PAIR:    data.pair  = new Pair(d.pair->key,d.pair->val);      break;
  case PARAM:   data.param = new Param(d.param->key,d.param->val);   break;
  case BINARY:  data.blob  = new Blob(*d.blob);                      break;
  case TUPLE:   _get(data.tuple, d.tuple);                           break;
  case LIST:    _get(data.list,  d.list);                            break;
  case DICT:    _get(data.dict,  d.dict);                            break;
  case ARGS:    _get(data.args,  d.args);                            break;
  default:      data.i64val = d.i64val;                              break;
  }
  return *this;
}

Arg& Arg::load(const string& v, const string& tag) {
  Item it(v,tag);
  string item = it.value();
  if ( !item.empty() )  {
    if ( debug() ) cout << endl << "ITEM:" << item << endl << "End_ITEM" << endl;
    switch(item[1]) {
    case 'i':  {                                   // <int>
      string v = Item(item,"int").value();
      type = INT64;
      data.i64val = ::atol(v.c_str());
      return *this;
    }
    case 'd':                                      // <double>, <dateTime.iso8601>
      switch(item[2]) {
    case 'o':  {                                   // double
      string v = Item(item,"double").value();
      type = DOUBLE;
      data.dval = ::atof(v.c_str());
      return *this;
    }
    case 'a':
      return assign(TIME,&Time().load(Item(item,"dateTime.iso8601").value()));
    default:
      break;
      }
      break;
    case 'b':
      switch(item[2]) {
    case 'a':
      return assign(BINARY,&Blob().load(Item(item,"base64")));
    case 'o':
      type = BOOL;
      data.boolean = Item(item,"boolean").value()=="1";
      return *this;
      }
      break;
    case 'n':                                        // <member> Load dict member
      return assign(PAIR,&Pair().load(Item("<member>"+item+"</member>","member")));
    case 'm':                                        // <member> Load dict member
      return assign(PAIR,&Pair().load(Item(item,"member")));
    case 'a':                                        // <array> Load tuple/list
      return assign(TUPLE,&Tuple().load(Item(item,"data")));
    case 's':                                        // <string>
      if ( strncmp(&item[1],"string",6)==0)  {
	string v = Item(item,"string").value();
        return assign(STRING,v.c_str());
      }
      else if ( strncmp(&item[1],"struct",6)==0) {   // Load dictionary
        return assign(DICT,&Dict().load(Item(item,"struct")));
      }
    default:
      break;
    }
  }
  throw runtime_error("PyRPC: Invalid data conversion!");
}

void Arg::clear()  {
  switch(type)  {
  case TIME:   if ( data.time )  delete data.time;     break;
  case STRING: if ( data.str )   delete data.str;      break;
  case PAIR:   if ( data.pair )  delete data.pair;     break;
  case BINARY: if ( data.blob )  delete data.blob;     break;
  case PARAM:  if ( data.param ) delete data.param;    break;
  case TUPLE:  if ( data.tuple ) delete data.tuple;    break;
  case LIST:   if ( data.list )  delete data.list;     break;
  case DICT:   if ( data.dict )  delete data.dict;     break;
  case ARGS:   if ( data.args )  delete data.args;     break;
  }
}

string Arg::str() const  {
  stringstream os;
  switch(type)  {
  case BOOL:       os << "<value><boolean>"<< (data.boolean?1:0) << "</boolean></value>\n"; break;
  case CHAR:       os << "<value><string>" << data.character   << "</string></value>\n";    break;
  case INT:        os << "<value><int>"    << data.ival        << "</int></value>\n";       break;
  case INT64:      os << "<value><int>"    << data.i64val      << "</int></value>\n";       break;
  case FLOAT:      os << "<value><double>" << data.fval        << "</double></value>\n";    break;
  case DOUBLE:     os << "<value><double>" << data.dval        << "</double></value>\n";    break;
  case STRING:     os << "<value><string>" << data.str         << "</string></value>\n";    break;
  case PAIR:       os << data.pair->str();                                                  break;
  case PARAM:      os << data.param->str();                                                 break;
  case TIME:       os << "<value><dateTime.iso8601>"<< data.time->str() << "</dateTime.iso8601></value>\n"; break;
  case BINARY:     os << "<value><base64>\n"      << data.blob->str()   << "</base64></value>\n";         break;
  case TUPLE:      os << "<value><array><data>\n" << data.tuple->str()  << "</data></array></value>\n";   break;
  case LIST:       os << "<value><array><data>\n" << data.list->str()   << "</data></array></value>\n";   break;
  case DICT:       os << "<value><struct>\n"      << data.dict->str()   << "</struct></value>\n";         break;
  case ARGS:       os << "<params>\n"             << data.args->str()   << "</params>\n";                 break;
  }
  return os.str();
}

string Arg::rawstr() const  {
  stringstream os;
  switch(type)  {
  case BOOL:       os << (data.boolean ?1:0);  break;
  case CHAR:       os << data.character;       break;
  case INT:        os << data.ival;            break;
  case INT64:      os << data.i64val;          break;
  case FLOAT:      os << data.fval;            break;
  case DOUBLE:     os << data.dval;            break;
  case BINARY:     os << data.blob->str();     break;
  case PAIR:       os << data.pair->str();     break;
  case PARAM:      os << data.param->str();    break;
  case TUPLE:      os << data.tuple->str();    break;
  case LIST:       os << data.list->str();     break;
  case DICT:       os << data.dict->str();     break;
  case ARGS:       os << data.args->str();     break;
  case TIME:       os << data.time->str();     break;
  case STRING:     os << data.str;             break;
  }
  return os.str();
}

/// Get the data content type
string Arg::datatype() const {
  switch(type)  {
#define __T(x)  case x: return #x
    __T(BOOL);
    __T(CHAR);
    __T(INT);
    __T(INT64);
    __T(FLOAT);
    __T(DOUBLE);
    __T(PAIR);
    __T(PARAM);
    __T(TUPLE);
    __T(LIST);
    __T(DICT);
    __T(ARGS);
    __T(TIME);
    __T(STRING);
    __T(BINARY);
  default: return "UNKNOWN";
  }
#undef __T
}

/// Print object content to stdout
void Arg::print(const string& prefix) const {
  ostream& os=cout;
  string typ = datatype();
  os << prefix << setw(12) << left << typ << "[ ";
  switch(type)  {
  case BOOL:       os << (data.boolean?"true":"false") << " ] " << endl; return;
  case CHAR:       os << data.character << " ] " << endl;       return;
  case INT:        os << data.ival << " ] " << endl;            return;
  case INT64:      os << data.i64val << " ] " << endl;          return;
  case FLOAT:      os << data.fval << " ] " << endl;            return;
  case DOUBLE:     os << data.dval << " ] " << endl;            return;
  case STRING:     os << data.str << " ] " << endl;             return;
  case TIME:       os << data.time->str() << " ] " << endl;     return;
  case PAIR:       os << endl; data.pair->print(prefix+"  ");    break;
  case BINARY:     os << endl; data.blob->print(prefix+"  ");    break;
  case PARAM:      os << endl; data.param->print(prefix+"  ");   break;
  case TUPLE:      os << endl; data.tuple->print(prefix+"  ");   break;
  case LIST:       os << endl; data.list->print(prefix+"  ");    break;
  case DICT:       os << endl; data.dict->print(prefix+"  ");    break;
  case ARGS:       os << endl; data.args->print(prefix+"  ");    break;
  }
  os << setw(prefix.length()) << "" 
    // << setw(12) << left << "" 
    << "] End_" << typ << endl;
}

Time::Time()  {
  ::memset(&time,0,sizeof(time));
}

/// Initializing constructor
Time::Time(const struct tm& t) : time(t) {}

/// Initializing constructor
Time::Time(const char* date) {
  if ( ::strptime(date,TIME_FMT,&time) ) return;
  string err = "XMLRPC: Failed to convert data item of type dateTime.iso8601: ";
  throw runtime_error(err + date);
}

bool Time::operator<(const Time& c)  const  {
  return ::memcmp(&time,&c.time,sizeof(time)) < 0;
}

/// Equality operator
bool Time::operator==(const Time& c)  const  {
  return 0==::memcmp(&time,&c.time,sizeof(time));
}

Time& Time::operator=(const Time& c)  {
  if ( this != &c ) time = c.time;
  return *this;
}

string Time::str() const  {
  char text[128];
  ::strftime(text,sizeof(text),TIME_FMT,&time);
  return text;
}

/// Load object from string representation
Time& Time::load(const string& date) {
  if ( ::strptime(date.c_str(),TIME_FMT,&time) ) return *this;
  string err = "XMLRPC: Failed to convert data item of type dateTime.iso8601: ";
  throw runtime_error(err + date);
}

/// Copy constructor
Blob::Blob(const Blob& b) {
  pointer = new unsigned char[length=b.length];
  ::memcpy(pointer,b.pointer,length);
}

/// Initializing constructor
Blob::Blob(void* data, size_t len, bool adopt) {
  length = len;
  adopt ? pointer=(unsigned char*)data : ::memcpy(pointer=new unsigned char[length],data,length);
}

/// Destructor
Blob::~Blob() {
  if ( pointer ) delete [] pointer;
  pointer = 0;
}

bool Blob::operator<(const Blob& c)  const  {
  if ( length < c.length ) return true;
  return ::memcmp(pointer,c.pointer,length) < 0;
}

/// Equality operator
bool Blob::operator==(const Blob& c)  const  {
  return length == c.length && 0==::memcmp(pointer,c.pointer,length);
}

Blob& Blob::operator=(const Blob& c)  {
  if ( this != &c ) {
    if ( pointer ) delete [] pointer;
    pointer = new unsigned char[length=c.length];
    ::memcpy(pointer,c.pointer,length);
  }
  return *this;
}

string Blob::str() const  {
  return "";
}

/// Print object content to stdout
void Blob::print(const string& prefix) const {
  cout << prefix << "Length:" << length << " Pointer: " << pointer << endl;
}

/// Load object from string representation
Blob& Blob::load(const string& ) {
  return *this;
}

ostream& operator<<(ostream& os, const Arg& a)   
{  return os << "<param>\n" << a.str() << "</param>\n";   }

Pair::Pair() : key(), val() {}
Pair::Pair(const Arg& k, const Arg& v) : key(k), val(v) {}
Pair::Pair(const Pair& c) : key(c.key), val(c.val)    {}
bool Pair::operator==(const Pair& c) const                { return key == c.key && val == c.val;  }

bool Pair::operator<(const Pair& c)  const  {
  if ( key < c.key ) return true;
  if ( c.key < key ) return false;
  return val < c.val;
}

Pair& Pair::operator=(const Pair& c)  {
  if ( this != &c ) key=c.key, val=c.val;
  return *this;
}

string Pair::str() const  {
  stringstream os;
  os << "<member>\n"
    << "<name>" << key.rawstr() << "</name>\n" << val.str() 
    << "</member>\n";
  return os.str();
}

/// Print object content to stdout
void Pair::print(const string& prefix) const {
  key.print(  prefix+" KEY:  ");
  val.print(prefix+" VALUE:");
}

Pair& Pair::load(const string& v) {
  string item;
  size_t val_start = v.find("<name>"), val_end = v.rfind("</name>");
  if ( val_start != string::npos && val_end != string::npos )  {
    item = "<value><string>"+v.substr(val_start+6,val_end-val_start-6)+"</string></value>";
    key.load(item);
    val_start = v.find("<value>");
    val_end = v.rfind("</value>");
    if ( val_start != string::npos && val_end != string::npos )  {
      item = v.substr(val_start,val_end-val_start+VALUE_LEN+1);
      val.load(item);
      //cout << "KEY:" <<key.str() << endl << "VAL:" <<val.str() << endl;
      return *this;
    }
  }
  throw runtime_error("Unknwon pair entry for RPC call");
}

Param::Param(const Arg& k, const Arg& v) : Pair(k,v) {}
Param::Param(const Pair& c) : Pair(c) {}

string Param::str() const  {
  stringstream os;
  os << "<name>" << key.rawstr() << "</name>\n" << val.str();
  return os.str();
}

MethodCall::MethodCall(const string& name,const Dict& a) : call(name), values(a) { }

/// Retrieve string representation of the body
string MethodCall::body() const {
  stringstream os, hdr;
  string tag = "methodCall"; // "methodResponse";
  tag = "methodResponse";
  os << "<?xml version='1.0'?>\n"
    << "<" << tag << ">" << "<methodName>" << call << "</methodName>\n" 
    << values.str() << "</" << tag << ">\n";
  return os.str();
}

/// Retrieve string representation of the object
string MethodCall::header(const string& host, int port, size_t len) const {
  stringstream hdr;
  hdr << "POST /RPC2 HTTP/1.0\r\n"
    << "Host: " << host << ":" << port << "\r\n"
    << "User-Agent: LHCb-XMLRPC 1.0\r\n"
    << "Content-Type: text/xml\r\n"
    << "Content-Length: " << (int)len << "\r\n\r\n";
  return hdr.str();
}

/// Retrieve string representation of the object
string MethodCall::str(const string& host, int port) const  {
  string b(body());
  return header(host, port, b.length())+b;
}

/// Print object to standard output
void MethodCall::print(const string& prefix) const {
  cout << prefix << "Method call:" << endl;
  values.print(prefix+"  ");
}

/// Initializing constructor: read response and fill result tuple
MethodResponse::MethodResponse(const string& vals) {
  size_t start_response = vals.find("<methodResponse>");
  if ( start_response != string::npos ) {
    Item params(vals,"params");
    string pars = params.value();
    if ( !pars.empty() ) {
      for(Items it(pars,"param"); !it.empty(); ++it)
        add(it);
      return;
    }
    // ... we have an error
    add(Item(vals,"fault"));
  }
}

/// Add new response entry from string
void MethodResponse::add(const string& val) {
  if ( !val.empty() ) {
    Arg arg;
    arg.load(val);
    values.add(arg);
  }
}

/// Check if error occurred
bool MethodResponse::hasError() const {
  list<Arg>* l = (list<Arg>*)values.data;
  if ( l->size() == 1 ) {
    l = (list<Arg>*)(*l->begin()).data.dict->data;
    for(list<Arg>::const_iterator i=l->begin(); i != l->end(); ++i) {
      if ( (*i).type == Arg::PAIR )  {
        Pair* p = (*i).data.pair;
        if ( p->key.type==Arg::STRING && ::strncmp(p->key.data.str,"faultCode",9)==0) 
          return true;
      }
    }
  }
  return false;
}

/// Retrieve error information
Error MethodResponse::error() const {
  Error result;
  list<Arg>* l = (list<Arg>*)values.data;
  if ( l->size() == 1 ) {
    l = (list<Arg>*)(*l->begin()).data.dict->data;
    for(list<Arg>::const_iterator i=l->begin(); i != l->end(); ++i) {
      if ( (*i).type == Arg::PAIR )  {
        Pair* p = (*i).data.pair;
        if ( p->key.type==Arg::STRING && ::strncmp(p->key.data.str,"faultCode",9)==0) 
          result.first = (int)p->val.data.i64val;
        if ( p->val.type==Arg::STRING && ::strncmp(p->key.data.str,"faultString",11)==0) 
          result.second = p->val.data.str;
      }
    }
  }
  return result;
}

/// Retrieve string representation of the object
string MethodResponse::str() const 
{  return dump(values,"<param>\n","</param>\n");               }

/// Print object to standard output
void MethodResponse::print(const string& prefix) const {
  cout << prefix << "Method response:" << endl;
  values.print(prefix+"  ");
}

ostream& operator<<(ostream& os, const Pair& p)        {  return os << p.str();  }
ostream& operator<<(ostream& os, const Param& p)       {  return os << p.str();  }
ostream& operator<<(ostream& os, const MethodCall& c)  {  return os << c.str();  }

#include "NET/TcpNetworkChannel.h"
#define Connect_TMO 0

/// Initializing constructor: sets up the connection
Server::Server(const string& server, unsigned short port) : m_channel(0) {  connect(server,port); }

/// Default constructor without explicit connection
Server::Server() : m_server(), m_port(-1), m_channel(0) { }

/// Default destructor
Server::~Server() {  disconnect();   }

/// Connect to server
bool Server::connect(const string& server, unsigned short port) {
  NetworkChannel::Address sin;
  struct hostent *h = ::gethostbyname(server.c_str());
  disconnect();
  m_server = server;
  m_port = port;
  if ( h ) {
    ::memcpy (&sin.sin_addr,  h->h_addr, h->h_length);
    ::memset(sin.sin_zero,0,sizeof(sin.sin_zero));
    sin.sin_port = NetworkChannel::Port(htons(port));
    sin.sin_family = AF_INET;
    m_channel = new TcpNetworkChannel();
    if (m_channel->connect(sin,Connect_TMO) != -1) {
      return true;
    }
    cout << RTL::processName() << " Connect failed to " << server
         << "  [" << ::strerror(m_channel->error()) << "]" << endl;
    delete m_channel;
    m_channel = 0;
    return false;
  }
  cout << RTL::processName() << " Host not found.!" << endl;
  return false;
}

/// Disconnect server
bool Server::disconnect() {
  if ( m_channel ) delete m_channel;
  m_channel = 0;
  return true;
}

Server::Response Server::error(int err_num) const {
  cout << RTL::processName() << " " << ::strerror(err_num) << endl;
  return Response("");
}

/// Execute RPC call
Server::Response Server::operator() (const Server::Call& call) {
  if ( m_channel ) {
    string s;
    string b = call.body();
    string h = call.header(m_server,m_port,b.length());
    int nb, nbyte = m_channel->send(h.c_str(),h.length(),0,0);
    if ( nbyte == (int)h.length() ) {
      nbyte = m_channel->send(b.c_str(),b.length());
      if ( nbyte == (int)b.length() ) {
        char buf[4096];
        nb = 0;
        while (nbyte>0)  {
          nbyte = m_channel->recv(buf,sizeof(buf),0,0);
          if ( nbyte==0 ) break;
          if ( nbyte==-1 ) {
            switch(m_channel->error()) 
            {
            case ESOCK_AGAIN:
            case ESOCK_INTR:
              continue;
            case ESOCK_TIMEDOUT:
            case ESOCK_NETRESET:
            case ESOCK_NETDOWN:
            case ESOCK_CONNREFUSED:
            case ESOCK_CONNRESET:
            case ESOCK_NOTSOCK:
            case ESOCK_FAULT:
            case ESOCK_INVAL:
            case ESOCK_BADF:
            case ESOCK_OPNOTSUPP:
            case ESOCK_NOBUFS:
            case EIO:
            case ENOMEM:
            default:
              // Real error: stop processing.....
              return error(m_channel->error());
            }
          }
          nb += nbyte;
          buf[nbyte] = 0;
          s += buf;
        }
        if ( debug() ) cout << s << endl;
        disconnect();
        return Response(s);
      }
    }
    // error sending to server
    disconnect();
    return error(m_channel->error());
  }
  return error(ESOCK_NOTCONN);
}
