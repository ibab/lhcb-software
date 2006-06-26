#include "PyRPC.h"
#include "RTL/strdef.h"
#include <ctime>
#include <cstdio>
#include <sstream>

PyRPC::ContainerBase::ContainerBase()  {
  data = new std::list<Arg>();
}

PyRPC::ContainerBase::~ContainerBase()  {
  std::list<Arg>* p = (std::list<Arg>*)data;
  if ( p ) delete p;
  data = 0;
}

PyRPC::ContainerBase& PyRPC::ContainerBase::operator=(const ContainerBase& c)  {
  copy(c.data);
  return *this;
}

void PyRPC::ContainerBase::add(const Arg& a)  {
  std::list<Arg>* p = (std::list<Arg>*)data;
  p->push_back(a);
}

void PyRPC::ContainerBase::copy(void* data_ptr)  {
  if ( data_ptr != data )  {
    std::list<Arg>* p1 = (std::list<Arg>*)data;
    std::list<Arg>* p2 = (std::list<Arg>*)data_ptr;
    *p1 = *p2;
  }
}

bool PyRPC::ContainerBase::operator<(const ContainerBase& c) const  {
  std::list<Arg>* p1 = (std::list<Arg>*)data;
  std::list<Arg>* p2 = (std::list<Arg>*)c.data;
  if ( p1->size() < p2->size() )  {
    return true;
  }
  else if ( p1->size() > p2->size() )  {
    return false;
  }
  else {
    std::list<Arg>::const_iterator i1=p1->begin();
    std::list<Arg>::const_iterator i2=p2->begin();
    for(; i1 != p1->end(); ++i1, ++i2)  {
      if ( *i1 < *i2 ) return true;
    }
  }
  return false;
}

bool PyRPC::ContainerBase::operator==(const ContainerBase& c) const  {
  std::list<Arg>* p1 = (std::list<Arg>*)data;
  std::list<Arg>* p2 = (std::list<Arg>*)c.data;
  if ( p1->size() != p2->size() )  {
    return false;
  }
  else  {
    std::list<Arg>::const_iterator i1=p1->begin();
    std::list<Arg>::const_iterator i2=p2->begin();
    for(; i1 != p1->end(); ++i1, ++i2)  {
      if ( !(*i1 == *i2) ) return false;
    }
  }
  return true;
}

static std::ostream& dump(std::ostream& os, const PyRPC::ContainerBase& t, bool brackets,
                          char b_open, char b_close)  {
  std::list<PyRPC::Arg>* p = (std::list<PyRPC::Arg>*)t.data;
  if ( brackets ) os << b_open;
  std::vector<std::string> v;
  for(std::list<PyRPC::Arg>::const_iterator i=p->begin(); i != p->end(); ++i)  {
    std::string s = (*i).str();
    if ( !s.empty() )
       v.push_back(s);
  }
  for(std::vector<std::string>::const_iterator j=v.begin(); j != v.end();)  {
    os << (*j);
    if ( (++j) == v.end() ) break;
    os << ", ";
  }
  if ( brackets ) os << b_close;
  return os;
}

template <> std::string PyRPC::Container<0>::str(bool with_brackets) const  {
  std::stringstream os;
  dump(os,*this,with_brackets,'[',']');
  return os.str();
}

template <> std::string PyRPC::Container<1>::str(bool with_brackets) const  {
  std::stringstream os;
  dump(os,*this,with_brackets,'(',')');
  return os.str();
}

template <> std::string PyRPC::Container<2>::str(bool with_brackets) const  {
  std::stringstream os;
  dump(os,*this,with_brackets,'{','}');
  return os.str();
}

template <> std::string PyRPC::Container<3>::str(bool with_brackets) const  {
  std::stringstream os;
  dump(os,*this,false,'{','}');
  return os.str();
}

PyRPC::Arg::Arg(char n) : type(CHAR)              { data.character = n;       }
PyRPC::Arg::Arg(unsigned char n) : type(CHAR)     { data.character = n;       }
PyRPC::Arg::Arg(int n) : type(INT)                { data.ival = n;            }
PyRPC::Arg::Arg(unsigned int n) : type(INT)       { data.ival = n;            }
PyRPC::Arg::Arg(float n) : type(FLOAT)            { data.fval = n;            }
PyRPC::Arg::Arg(double n) : type(DOUBLE)          { data.dval = n;            }
PyRPC::Arg::Arg(long long int n) : type(INT64)    { data.i64val = n;          }
PyRPC::Arg::Arg(const char* n) : type(STRING)     { data.str = n;             }
PyRPC::Arg::Arg(const std::string& n) : type(STRING2)  { 
  assign(n.c_str());
}

PyRPC::Arg::Arg(const Pair& p) : type(PAIR)    { 
  assign(&p);
}

PyRPC::Arg::Arg(const Param& p) : type(PARAM)    { 
  assign(&p);
}

PyRPC::Arg::Arg(const List& l) : type(LIST)    { 
  assign(&l);
}

PyRPC::Arg::Arg(const Tuple& t) : type(TUPLE)  { 
  assign(&t);
}

PyRPC::Arg::Arg(const Dict& d) : type(DICT)  { 
  assign(&d);
}

PyRPC::Arg::Arg(const Args& d) : type(ARGS)  { 
  assign(&d);
}

PyRPC::Arg::Arg(const Arg& c) : type(c.type)   { 
  assign(c.data);
}

PyRPC::Arg::~Arg()  {
  clear();
}

PyRPC::Arg& PyRPC::Arg::operator=(const Arg& c)  {
  if ( this != &c ) {
    clear();
    type = c.type;
    assign(c.data);
  }
  return *this;
}

bool PyRPC::Arg::operator<(const Arg& c) const  {
  if ( type < c.type ) return true;
  else if ( type > c.type ) return false;
  switch(type)  {
    case CHAR:       return data.character < c.data.character;
    case INT64:      return data.i64val    < c.data.i64val;
    case INT:        return data.ival      < c.data.ival;
    case FLOAT:      return data.fval      < c.data.fval;
    case DOUBLE:     return data.dval      < c.data.dval;
    case PAIR:       return *data.pair     < *c.data.pair;
    case PARAM:      return *data.param    < *c.data.param;
    case TUPLE:      return *data.tuple    < *c.data.tuple;
    case LIST:       return *data.list     < *c.data.list;
    case DICT:       return *data.dict     < *c.data.dict;
    case ARGS:       return *data.args     < *c.data.args;
    case STRING2:
    case STRING:     return strcmp(data.str,c.data.str) < 0;
  }
  return false;
}

bool PyRPC::Arg::operator==(const Arg& c) const  {
  if ( type != c.type ) return false;
  switch(type)  {
    case CHAR:       return data.character == c.data.character;
    case INT64:      return data.i64val == c.data.i64val;
    case INT:        return data.ival   == c.data.ival;
    case FLOAT:      return data.fval   == c.data.fval;
    case DOUBLE:     return data.dval   == c.data.dval;
    case PAIR:       return *data.pair  == *c.data.pair;
    case PARAM:      return *data.param == *c.data.param;
    case TUPLE:      return *data.tuple == *c.data.tuple;
    case LIST:       return *data.list  == *c.data.list;
    case DICT:       return *data.dict  == *c.data.dict;
    case ARGS:       return *data.args  == *c.data.args;
    case STRING2:
    case STRING:     return strcmp(data.str,c.data.str) == 0;
  }
  return false;
}
template<class T> static inline void _get(T*& pointer, T* source)  {
  pointer = new T();
  pointer->copy(source->data);
}

void PyRPC::Arg::assign(const Data& d)  {
  switch(type)  {
    case STRING2: data.str = new char[strlen(d.str)+1];
                  strcpy((char*)data.str,d.str);    break;
    case PAIR:   data.pair  = new Pair(d.pair->key,d.pair->value);    break;
    case PARAM:  data.param = new Param(d.param->key,d.param->value); break;
    case TUPLE:  _get(data.tuple, d.tuple);              break;
    case LIST:   _get(data.list,  d.list);               break;
    case DICT:   _get(data.dict,  d.dict);               break;
    case ARGS:   _get(data.args,  d.args);               break;
    default:     data.i64val = d.i64val;                 break;
  }
}
void PyRPC::Arg::clear()  {
  switch(type)  {
    case STRING2:if ( data.str )   delete data.str;      break;
    case PAIR:   if ( data.pair )  delete data.pair;     break;
    case PARAM:  if ( data.param ) delete data.param;    break;
    case TUPLE:  if ( data.tuple ) delete data.tuple;    break;
    case LIST:   if ( data.list )  delete data.list;     break;
    case DICT:   if ( data.dict )  delete data.dict;     break;
    case ARGS:   if ( data.args )  delete data.args;     break;
  }
}

std::string PyRPC::Arg::str(bool quotes) const  {
  std::stringstream os;
  const char* q = (quotes) ? "\"" : "";
  switch(type)  {
    case CHAR:       os << q << data.character << q;     break;
    case INT:        os << data.ival;                    break;
    case INT64:      os << data.i64val;                  break;
    case FLOAT:      os << data.fval;                    break;
    case DOUBLE:     os << data.dval;                    break;
    case PAIR:       os << data.pair->str();             break;
    case PARAM:      os << data.param->str();            break;
    case TUPLE:      os << data.tuple->str();            break;
    case LIST:       os << data.list->str();             break;
    case DICT:       os << data.dict->str();             break;
    case ARGS:       os << data.args->str();             break;
    case STRING2:
    case STRING:     os << q << data.str << q;           break;
  }
  return os.str();
}

PyRPC::Time::Time() : Arg("") {
  char timestr[256];
  time_t t;
  ::time(&t);
  ::strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",::gmtime(&t));
  type = STRING2;
  assign(timestr);
}

PyRPC::Time::Time(time_t t) : Arg("")  {
  char timestr[256];
  ::strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",::gmtime(&t));
  type = STRING2;
  assign(timestr);
}

PyRPC::Time::Time(const char* date) : Arg("")  {
  type = STRING2;
  assign(date);
}

std::ostream& operator<<(std::ostream& os, const PyRPC::Arg& a)   {
  return os << a.str();
}

PyRPC::Pair::Pair(const Arg& k, const Arg& v) : key(k), value(v) {
}

PyRPC::Pair::Pair(const Pair& c) : key(c.key), value(c.value) {
}

bool PyRPC::Pair::operator==(const Pair& c) const  {
  if ( key == c.key )  {
    if ( value == c.value )  {
      return true;
    }
  }
  return false;
}

bool PyRPC::Pair::operator<(const Pair& c)  const  {
  if ( key < c.key ) return true;
  if ( c.key < key ) return false;
  return value < c.value;
}

PyRPC::Pair& PyRPC::Pair::operator=(const Pair& c)  {
  if ( this != &c )  {
    key   = c.key;
    value = c.value;
  }
  return *this;
}

std::string PyRPC::Pair::str(bool quote_key,bool quote_val) const  {
  std::stringstream os;
  os << key.str(quote_key) << ": " << value.str(quote_val);
  return os.str();
}

std::ostream& operator<<(std::ostream& os, const PyRPC::Pair& p)   {
  return os << p.str();
}

PyRPC::Param::Param(const Arg& k, const Arg& v) : Pair(k,v) {
}

PyRPC::Param::Param(const Pair& c) : Pair(c) {
}

std::string PyRPC::Param::str() const  {
  std::stringstream os;
  os << key.str(false) << "=" << value.str();
  return os.str();
}

std::ostream& operator<<(std::ostream& os, const PyRPC::Param& p)   {
  return os << p.str();
}

PyRPC::MethodCall::MethodCall(const std::string& name,const Dict& a)
: call(name), args(a)
{
}

std::string PyRPC::MethodCall::str() const  {
  std::stringstream os;
  os << call << "(" << args.str(false) << ")";
  return os.str();
}

std::ostream& operator<<(std::ostream& os, const PyRPC::MethodCall& c)  {
  return os << c.str();
}

PyRPC::ResultReader::ResultReader(const std::string& s, const std::string& seps) 
: m_data(s), m_sep(seps), m_status(0)   {
  m_token = ::strtok((char*)m_data.c_str(),m_sep.c_str());
  if ( m_token ) m_status = ::atol(m_token);
}

void PyRPC::ResultReader::reset(const std::string& s)  {
  m_data = s;
  m_token = ::strtok((char*)m_data.c_str(),m_sep.c_str());
  if ( m_token ) m_status = ::atol(m_token);
}
const char* PyRPC::ResultReader::next()   {
  if ( m_status == 1 )  {
    char *tok = m_token;
    for( size_t cnt=0; tok != 0; ) {
      tok = ::strtok(0, m_sep.c_str());
      if ( tok )  {
        size_t len = sizeof(m_line);
        ::str_trim(tok,m_line,&len);
        if ( ::strlen(m_line)>0 )  {
          return m_line;
        }
      }
    }
  }
  return 0;
}

