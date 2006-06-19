#ifndef _PYRPC_H
#define _PYRPC_H

#include <map>
#include <list>
#include <vector>
#include <string>
#include <iostream>

/*
 *   Namespace declaration
 */
namespace PyRPC  {
  struct Arg;
  struct Pair;
  struct Param;

  /** @class ContainerBase PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct ContainerBase {
    void* data;
    ContainerBase();
    ~ContainerBase();
    void add(const Arg& arg);
    void copy(void* data);
    bool operator<(const ContainerBase& c) const;
    bool operator==(const ContainerBase& c) const;
    ContainerBase& operator=(const ContainerBase& c);
  };

  /** @class Container PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  template <int i> struct Container : public ContainerBase {
    ~Container();
    Container();
    Container(const Container& c);
    template <typename A> Container(A a);
    template <typename A, typename B> Container(A a, B b);
    template <typename A, typename B, typename C> Container(A a, B b, C c);
    template <typename A, typename B, typename C, typename D> Container(A a, B b, C c, D d);
    template <typename A, typename B, typename C, typename D, typename E> 
    Container(A a, B b, C c, D d, E e);
    template <typename A, typename B, typename C, typename D, typename E, typename F> 
    Container(A a, B b, C c, D d, E e, F f);
    template <typename A, typename B, typename C, typename D, typename E, typename F, typename G> 
    Container(A a, B b, C c, D d, E e, F f, G g);
    template <typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H> 
    Container(A a, B b, C c, D d, E e, F f, G g, H h);
    std::string str(bool with_brackets=true) const;
  };

  typedef Container<0> List;
  typedef Container<1> Tuple;
  typedef Container<2> Dict;
  typedef Container<3> Args;
  
  /** @class Arg PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct Arg  {
    int type;
    enum {  CHAR = 0, 
            INT = 1, 
            FLOAT = 2,
            DOUBLE = 3,
            STRING = 4,
            STRING2 = 5,
            PARAM,
            PAIR,
            TUPLE,
            LIST,
            DICT,
            ARGS
    };
    union Data {
      Data(const void* p=0) { ptr = p; }
      const void *ptr;
      const char *str;
      char   character;
      int    ival;
      float  fval;
      double dval;
      List*  list;
      Tuple* tuple;
      Dict*  dict;
      Args*  args;
      Pair*  pair;
      Param* param;
    } data;
    Arg(char n);
    Arg(int n);
    Arg(float n);
    Arg(double n);
    Arg(const char* n);
    Arg(const std::string& n);
    Arg(const Arg& c);
    Arg(const Pair& c);
    Arg(const Param& c);
    Arg(const List& c);
    Arg(const Tuple& c);
    Arg(const Dict& c);
    Arg(const Args& c);
    ~Arg();
    bool operator<(const Arg& c) const;
    bool operator==(const Arg& c) const;
    Arg& operator=(const Arg& c);
    std::string str(bool quotes=true) const;
    void assign(const Data& data);
    void clear();
  };

  /** @class Time PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct Time : public Arg  {
    Time();
    Time(time_t t);
    Time(const char* date);
  };

  /** @class Pair PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct Pair  {
    Arg key;
    Arg value;
    Pair(const Pair& c);
    Pair(const Arg& k, const Arg& v);
    template<int i> Pair(const Container<i>& k, const Arg& v)
      : key(Arg(k)), value(v) {}
    template<int i> Pair(const Arg& k, const Container<i>& v)
      : key(k), value(Arg(v)) {}
    template<int i,int j> Pair(const Container<i>& k, const Container<j>& v)
      : key(Arg(k)), value(Arg(v)) {}
    bool operator==(const Pair& c) const;
    Pair& operator=(const Pair& c);
    bool operator<(const Pair& c)  const;
    std::string str(bool quote_key=true,bool quote_val=true) const;
  };

  /** @class Param PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct Param : public Pair {
    Param(const Arg& k, const Arg& v);
    Param(const Pair& c);
    template<int i> Param(const Container<i>& k, const Arg& v) : Pair(k,v) {}
    template<int i> Param(const Arg& k, const Container<i>& v) : Pair(k,v) {}
    template<int i,int j> Param(const Container<i>& k, const Container<j>& v)
      : Pair(k,v) {}
    std::string str() const;
  };

  /** @class MethodCall PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct MethodCall  {
    std::string call;
    Dict        args;
    MethodCall(const std::string& name,const Dict& args);
    std::string str() const;
  };

  /** @class ResultReader PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class ResultReader  {
    std::string m_data;
    std::string m_sep;
    char*       m_token;
    int         m_status;
    char        m_line[512];
  public:
    ResultReader(const std::string& s, const std::string& seps="\t\n',[]{}()");
    void reset(const std::string& s);
    const char* next();
    int status()  const  {  return m_status;      }
    bool ok() const      {  return m_status == 1; }
  };

  /** @class Reader
    *
    */
  template <typename T> class Reader  {
  public:
    typedef typename T Result;
  protected:
    ResultReader       m_rdr;
  public:
    Reader(const std::string& s) : m_rdr(s)  {              }
    int status()  const               {  return m_rdr.status();   }
    bool ok()     const               {  return m_rdr.ok();       }
    void reset(const std::string& s)  {  m_rdr.reset(s);          }
    Result next();
  };

}

template <int i> PyRPC::Container<i>::Container(const Container& c) {
  copy(c.data);
}

template <int i> PyRPC::Container<i>::Container() {
}

template <int i> PyRPC::Container<i>::~Container()  {
}

template <int i> template <typename A> PyRPC::Container<i>::Container(A a) {
  add(Arg(a));
}

template <int i> template <typename A, typename B> 
PyRPC::Container<i>::Container(A a, B b)  {
  add(Arg(a));
  add(Arg(b));
}

template <int i> template <typename A, typename B, typename C> 
PyRPC::Container<i>::Container(A a, B b, C c)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
}

template <int i> template <typename A, typename B, typename C, typename D> 
PyRPC::Container<i>::Container(A a, B b, C c, D d)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E> 
PyRPC::Container<i>::Container(A a, B b, C c, D d, E e)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E, 
                             typename F> 
PyRPC::Container<i>::Container(A a, B b, C c, D d, E e, F f)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
  add(Arg(f));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E, 
                             typename F, typename G> 
PyRPC::Container<i>::Container(A a, B b, C c, D d, E e, F f, G g)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
  add(Arg(f));
  add(Arg(g));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E, 
                             typename F, typename G, typename H> 
PyRPC::Container<i>::Container(A a, B b, C c, D d, E e, F f, G g, H h)  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
  add(Arg(f));
  add(Arg(g));
  add(Arg(f));
  add(Arg(h));
}

std::ostream& operator<<(std::ostream& os, const PyRPC::Arg& a);
std::ostream& operator<<(std::ostream& os, const PyRPC::Pair& p);
std::ostream& operator<<(std::ostream& os, const PyRPC::MethodCall& c);
template <int i> std::ostream& operator<<(std::ostream& os, const PyRPC::Container<i>& t)  {
  return os << t.str().c_str();
}
#endif   // _PYRPC_H
