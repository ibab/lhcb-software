#ifndef _PYRPC_H
#define _PYRPC_H

#include <ctime>
#include <string>
#include <iostream>
#include <algorithm>

// Forward declarations
class TcpNetworkChannel;


/*
 *   Namespace declaration
 */
namespace PyRPC  {
  struct Arg;
  class  Time;
  class  Blob;
  class  Pair;
  class  Param;
  class  Item;
  class  Items;
  class  _ContBase;
  template <int i> struct _Cont;
  class  MethodCall;
  class  MethodResponse;
  class  Server;
  typedef std::pair<int,std::string> Error;

  bool setDebug(bool new_value);
  bool debug();

  class Item {
  public:
    const std::string& m_s;
    std::string m_tag, m_item;
    size_t      m_start, m_end;
    Item(const std::string& s, const std::string& t);
    operator const std::string&() const  { return m_item;         }
    //const char* c_str() const;
    bool empty() const                   { return m_item.empty(); }
    const std::string& get();
    std::string value() const;
  };

  class Items : public Item {
  public:
    Items(const std::string& src, const std::string& tag) : Item(src,tag) {}
    Items& operator++()          { get(); return *this;  }
    Items& operator++(int)       { get(); return *this;  }
  };

  /** @class _ContBase PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class _ContBase {
  public:
    void* data;
    /// Default constructor
    _ContBase();
    /// Copy constructor
    _ContBase(const _ContBase& c)  { copy(c.data); }
    /// Default destructor
    virtual ~_ContBase();

    void add(const Arg& arg);
    _ContBase& copy(const void* data);
    bool operator<(const _ContBase& c) const;
    bool operator==(const _ContBase& c) const;
    _ContBase& operator=(const _ContBase& c);
    /// Print object content to stdout
    void print(const std::string& prefix="") const;
    /// Load the object from the string representation
    void loadValues(const std::string& data, const std::string& tag);
  };

  /** @class _Cont PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  template <int i> struct _Cont : public _ContBase {
    /// Default constructor
    _Cont();
    /// Copy constructor
    _Cont(const _Cont& c);
    template <typename A> _Cont(A a);
    template <typename A, typename B> _Cont(A a, B b);
    template <typename A, typename B, typename C> _Cont(A a, B b, C c);
    template <typename A, typename B, typename C, typename D> _Cont(A a, B b, C c, D d);
    template <typename A, typename B, typename C, typename D, typename E> 
    _Cont(A a, B b, C c, D d, E e);
    template <typename A, typename B, typename C, typename D, typename E, typename F> 
    _Cont(A a, B b, C c, D d, E e, F f);
    template <typename A, typename B, typename C, typename D, typename E, typename F, typename G> 
    _Cont(A a, B b, C c, D d, E e, F f, G g);
    template <typename A, typename B, typename C, typename D, typename E, typename F, typename G, typename H> 
    _Cont(A a, B b, C c, D d, E e, F f, G g, H h);
    /// Default destructor
    virtual ~_Cont();
    /// Retrieve string representation of the object
    std::string str() const;
    /// Load the object from the string representation
    _Cont& load(const  std::string& data);
  };

  typedef _Cont<0> List;
  typedef _Cont<1> Tuple;
  typedef _Cont<2> Dict;
  typedef _Cont<3> Args;
  
  /** @class Arg PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  struct Arg  {
    int type;
    enum {  NONE    = -1,
	    CHAR    =  0, 
            INT     =  1, 
            INT64   =  2,
            FLOAT   =  3,
            DOUBLE  =  4,
            STRING  =  5,
	    TIME    =  6,
	    BINARY  =  7,
	    BOOL    =  8,
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
      char*  str;
      bool   boolean;
      char   character;
      int    ival;
      float  fval;
      double dval;
      long long int i64val;
      Time*  time;
      Blob*  blob;
      List*  list;
      Tuple* tuple;
      Dict*  dict;
      Args*  args;
      Pair*  pair;
      Param* param;
    } data;
    Arg();
    Arg(bool b);
    Arg(char n);
    Arg(unsigned char n);
    Arg(int n);
    Arg(unsigned int n);
    Arg(long n);
    Arg(unsigned long n);
    Arg(long long int n);
    Arg(unsigned long long int n);
    Arg(float n);
    Arg(double n);
    Arg(const char* n);
    Arg(const struct tm& t);
    Arg(const std::string& n);
    Arg(const Arg& c);
    Arg(const Pair& c);
    Arg(const Blob& c);
    Arg(const Time& c);
    Arg(const Param& c);
    Arg(const List& c);
    Arg(const Tuple& c);
    Arg(const Dict& c);
    /// Copy constructor
    Arg(const Args& c);
    /// Default destructor
    ~Arg();
    /// Operator less for maps
    bool operator<(const Arg& c) const;
    /// Equality operator
    bool operator==(const Arg& c) const;
    /// Assignment operator
    Arg& operator=(const Arg& c);
    /// Assign data value
    Arg& assign(const Data& data);
    /// Assign data type and value
    Arg& assign(int, const Data& data);
    /// Clear data content
    void clear();
    /// Get the data content type
    std::string datatype() const;
    /// Retrieve string representation of the object
    std::string str() const;
    /// Access raw item string
    std::string rawstr() const;
    /// Print object content to stdout
    void print(const std::string& prefix="") const;
    /// Load the object from the string representation
    Arg& load(const std::string& data, const std::string& tag="value");
  };

  /** @class Time PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class Time  {
  public:
    /// Data value
    struct tm time;
    /// Default constructor
    Time();
    /// Initializing constructor
    Time(const struct tm& t);
    /// Initializing constructor
    Time(const char* date);
    /// Standard destructor
    ~Time() {}
    /// Assignment operator
    Time& operator=(const Time& c);
    /// Equality operator
    bool operator==(const Time& c) const;
    /// Operator less for maps
    bool operator<(const Time& c)  const;
    /// String representation of the value
    std::string str() const;
    /// Load the object from the string representation
    Time& load(const std::string& data);
  };

  /** @class Blob PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class Blob  {
  public:
    /// Data area
    unsigned char* pointer;
    /// Data length
    size_t length;
    /// Default constructor
    Blob() : pointer(0), length(0) {}
    /// Copy constructor
    Blob(const Blob& b);
    /// Initializing constructor
    Blob(void* data, size_t len, bool adopt);
    /// Destructor
    ~Blob();
    /// Assignment operator
    Blob& operator=(const Blob& c);
    /// Equality operator
    bool operator==(const Blob& c) const;
    /// Operator less for maps
    bool operator<(const Blob& c)  const;
    /// String representation of the value
    std::string str() const;
    /// Print ASCII representation of the value
    void print(const std::string& prefix) const;
    /// Load the object from the string representation
    Blob& load(const std::string& data);
  };

  /** @class Pair PyRPC.h
    *
    *  Class representing a pair of arbitrary types
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class Pair  {
  public:
    /// Key value
    Arg key;
    /// Value item
    Arg val;

    /// Default constructor
    Pair();
    /// Copy constructor
    Pair(const Pair& c);
    /// Initializing constructor
    Pair(const Arg& k, const Arg& v);
    /// Initializing constructor
    template<int i> Pair(const _Cont<i>& k, const Arg& v) : key(Arg(k)), val(v) {}
    /// Initializing constructor
    template<int i> Pair(const Arg& k, const _Cont<i>& v) : key(k), val(Arg(v)) {}
    /// Initializing constructor
    template<int i,int j> Pair(const _Cont<i>& k, const _Cont<j>& v) : key(Arg(k)), val(Arg(v)) {}
    /// Destructor
    ~Pair() {}
    /// Assignment operator
    Pair& operator=(const Pair& c);
    /// Equality operator
    bool operator==(const Pair& c) const;
    /// Operator less for maps
    bool operator<(const Pair& c)  const;
    /// Print object content to stdout
    void print(const std::string& prefix="") const;
    /// Retrieve string representation of the object
    std::string str() const;
    /// Load the object from the string representation
    Pair& load(const std::string& data);
  };

  /** @class Param PyRPC.h
    *
    *  Class representing a named parameter to a python RPC call.
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class Param : public Pair {
  public:
    /// Initializing constructor for Pair object types
    Param(const Arg& k, const Arg& v);
    /// Initializing constructor for Pair object types
    Param(const Pair& c);
    /// Initializing constructor
    template<int i> Param(const _Cont<i>& k, const Arg& v) : Pair(k,v) {}
    /// Initializing constructor
    template<int i> Param(const Arg& k, const _Cont<i>& v) : Pair(k,v) {}
    /// Initializing constructor
    template<int i,int j> Param(const _Cont<i>& k, const _Cont<j>& v) : Pair(k,v) {}
    /// Retrieve string representation of the object
    std::string str() const;
  };

  /** @class MethodCall PyRPC.h
    *
    *  Class representing an XML-RPC method call to a remote object
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class MethodCall  {
  protected:
    /// Server method name
    std::string call;
    /// Input argument dictionary
    Dict        values;

  public:
    /// Initializing constructor
    MethodCall(const std::string& name,const Dict& args);
    /// Retrieve string representation of the header
    std::string header(const std::string& server, int port, size_t len) const;
    /// Retrieve string representation of the body
    std::string body() const;
    /// Retrieve XML string representation of the object
    std::string str(const std::string& server="Unknown", int port=-1) const;
    /// Print object to standard output
    void print(const std::string& prefix="") const;
  };

  /** @class MethodResponse PyRPC.h
    *
    *  Class representing the response of a call to a XML-RPC server method 
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2006
    */
  class MethodResponse  {
  protected:
    /// Result tuple
    Tuple values;
    
  public:
    /// Initializing constructor: read response and fill result tuple
    MethodResponse(const std::string& values);
    /// Copy constructor
    MethodResponse(const MethodResponse& r) : values(r.values) {}
    
    /// Add new response entry from string
    void add(const std::string& val);
    /// Check if error occurred
    bool hasError() const;
    /// Retrieve error information
    std::pair<int,std::string> error() const;
    /// Retrieve string representation of the object
    std::string str() const;
    /// Print object to standard output
    void print(const std::string& prefix="") const;
    /// Data access to response
    const Tuple& result() const { return values; }
  };

  /** @class Server PyRPC.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    02.05.2008
    */
  class Server {
  public:
    typedef MethodResponse Response;
    typedef MethodCall Call;
  protected:
    /// Server name
    std::string        m_server;
    /// Server port
    int                m_port;
    /// Network connection to server
    TcpNetworkChannel* m_channel;

  public:
    /// Initializing constructor: sets up the connection
    Server(const std::string& server, unsigned short port);
    /// Default constructor without explicit connection
    Server();
    /// Default destructor
    virtual ~Server();
    /// Check if server is connected
    bool connected() const { return m_channel != 0; }
    /// Connect to server
    bool connect(const std::string& server, unsigned short port);
    /// Disconnect server
    bool disconnect();
    /// Execute RPC call
    Response operator() (const Call& call);
    /// Error return
    Response error(int err_num) const;
  };

}

template <int i> PyRPC::_Cont<i>::_Cont(const _Cont& c) : _ContBase()   {
  copy(c.data);
}

template <int i> PyRPC::_Cont<i>::_Cont() : _ContBase() {}

template <int i> PyRPC::_Cont<i>::~_Cont()  {}

template <int i> template <typename A> PyRPC::_Cont<i>::_Cont(A a) : _ContBase()  {
  add(Arg(a));
}

template <int i> template <typename A, typename B> 
PyRPC::_Cont<i>::_Cont(A a, B b) : _ContBase() {
  add(Arg(a));
  add(Arg(b));
}

template <int i> template <typename A, typename B, typename C> 
PyRPC::_Cont<i>::_Cont(A a, B b, C c) : _ContBase()  {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
}

template <int i> template <typename A, typename B, typename C, typename D> 
PyRPC::_Cont<i>::_Cont(A a, B b, C c, D d)  : _ContBase() {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E> 
PyRPC::_Cont<i>::_Cont(A a, B b, C c, D d, E e) : _ContBase() {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E, 
                             typename F> 
PyRPC::_Cont<i>::_Cont(A a, B b, C c, D d, E e, F f) : _ContBase() {
  add(Arg(a));
  add(Arg(b));
  add(Arg(c));
  add(Arg(d));
  add(Arg(e));
  add(Arg(f));
}

template <int i> template <typename A, typename B, typename C, typename D, typename E, 
                             typename F, typename G> 
PyRPC::_Cont<i>::_Cont(A a, B b, C c, D d, E e, F f, G g) : _ContBase() {
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
PyRPC::_Cont<i>::_Cont(A a, B b, C c, D d, E e, F f, G g, H h) : _ContBase() {
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
template <int i> std::ostream& operator<<(std::ostream& os, const PyRPC::_Cont<i>& t)  {
  return os << t.str().c_str();
}
#endif   // _PYRPC_H
