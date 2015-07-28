//====================================================================
//	XML Stream class definition
//--------------------------------------------------------------------
//
//	Package    : OnlineKernel ( The LHCb Online System)
//
//  Description: Helper to ease the creation of XML data streams
//
//  Author     : M.Frank
//  History    :
// +---------+----------------------------------------------+---------
// |    Date |                 Comment                      | Who     
// +---------+----------------------------------------------+---------
// | 29/06/01| Initial version                              | MF
// +---------+----------------------------------------------+---------
//
//====================================================================
#ifndef ONLINEKERNEL_CPP_XMLSTREAM_H
#define ONLINEKERNEL_CPP_XMLSTREAM_H 1

// STL include files
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>

/*
 *    XML namespace declaration
 */
namespace XML  {
  typedef const std::string CSTR;

  /** Stream XMLStream.h CPP/XMLStream.h
   */
  class Stream   {
  protected:
    std::ostream m_os;
  public:
    /// Standard Constructor
    explicit Stream(std::streambuf* s) : m_os(s) {                        }
    /// Standard Destructor
    virtual ~Stream()                                      {              }
    /// XML header string
    static const char* header()  {
      static const char* h = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
      return h;
    }
    /// Allow access to the underlying STL stream buffer
    virtual std::streambuf* rdbuf()                 {return m_os.rdbuf(); }
    /// Allow access to the underlying STL stream buffer
    virtual std::streambuf* rdbuf(std::streambuf* b){return m_os.rdbuf(b);}
    /// Allow access to the underlying STL stream
    virtual std::ostream* stream()                  {return &m_os;        }
    /// Add syle to the XML stream
    virtual Stream& putStyle(const std::string& url) {
      std::string tmp="<?xml-stylesheet type=\"text/xsl\" href=\""+url+"\"?>\n"; 
      return (*this) << tmp;
    }
    /// Add standard XML header to the stream.
    virtual Stream& putHeader()  { return (*this) << header() << std::endl;}
    Stream& operator<<(const char* arg)         {m_os << arg; return *this;}
    Stream& operator<<(const unsigned char arg) {m_os << arg; return *this;}
    Stream& operator<<(const signed char arg)   {m_os << arg; return *this;}
    Stream& operator<<(char arg)                {m_os << arg; return *this;}
    Stream& operator<<(short arg)               {m_os << arg; return *this;}
    Stream& operator<<(unsigned short arg)      {m_os << arg; return *this;}
    Stream& operator<<(int arg)                 {m_os << arg; return *this;}
    Stream& operator<<(unsigned int arg)        {m_os << arg; return *this;}
    Stream& operator<<(long arg)                {m_os << arg; return *this;}
    Stream& operator<<(unsigned long arg)       {m_os << arg; return *this;}
    Stream& operator<<(float arg)               {m_os << arg; return *this;}
    Stream& operator<<(double arg)              {m_os << arg; return *this;}
    Stream& operator<<(long double arg)         {m_os << arg; return *this;}
    Stream& operator<<(bool arg)                {m_os << arg; return *this;}
    Stream& operator<<(const void* arg)         {m_os << arg; return *this;}
    Stream& operator<<(const std::string& arg)  {m_os << arg; return *this;}
    Stream& operator<<(long long arg)           {
#ifdef WIN32
      int flg = m_os.flags();
      char buf[128];
      (flg & std::ios::hex) ? ::sprintf(buf,"%I64x",arg) : ::sprintf(buf,"%I64d",arg);
      m_os << buf;
#else
      m_os << arg;
#endif
      return *this;
    }
    Stream& operator<<(std::ostream& (*_f)(std::ostream&))     
    { _f(m_os); return *this;                                                  }
    Stream& operator<<(std::ios& (*_f)(std::ios&)) 
    { _f(m_os); return *this;                                                  }
    Stream& operator<<(Stream& (*_f)(Stream&))     
    { _f(*this); return *this;                                                 }
  };

  /// Helper to stream pointers
  template <class T> inline
  Stream& operator << (Stream& s, const T* p)   
  {  if(p)s << *p; return s; }

  /// Helper to stream STL auto pointer objects
  template <class T> inline
  Stream& operator << (Stream& s, const std::auto_ptr<T>& p)   
  {  return s << p.get();    }

  /// Small helper function to replace string items
  inline std::string& replace(std::string& s, CSTR& item, CSTR& rep)  {
    size_t occ, ilen = item.length(), rlen = rep.length();
    while ( (occ=s.find(item)) != std::string::npos )    {
      (rlen==0) ? s.erase(occ, ilen) : s.replace(occ, ilen, rep);
    }
    return s;
  }
  inline std::string clean(const std::string& s)     {
    std::string tmp=s; replace(tmp,">","&gt;"); replace(tmp, "<", "&lt;");
    return tmp;
  }

  /** Text. Useful to write strings with special chars to XML */
  //template <class T> inline T text(const T& t)   
  //{ return t;                                                           }
  inline std::string text(const std::string& s)   
  { return "<![CDATA[" + s + "]]>";                                     }
  inline std::string text(const char* s)   
  { return "<![CDATA[" + std::string(s) + "]]>";                        }

  /**          */
  struct tag   {
    std::string name;
    int         number;
    explicit tag(CSTR& t, int i=-1) : name(t), number(i)  {}
  };

  /** Start an XML element                                                */
  struct start : public tag { explicit start(CSTR& t,int i=-1):tag(t,i){} };
  /** End an XML tag                                                      */
  struct end : public tag  { explicit end(CSTR& t,int i=-1):tag(t,i)  {}  };
  /** Serialize individual items                                          */
  template <class T> struct _data{ T value; explicit _data(T t):value(t){}};
  /** Serialize (nearly) any XML item                                     */
  template<class T> struct _item : public tag, public _data<T*>  
  { _item(CSTR& t, T* o, int i=-1)  : tag(t,i), _data<T*>(o)  {}          };
  template<> struct _item<short> : public tag, public _data<short>  
  { _item(CSTR& t, short* o, int i=-1): tag(t,i), _data<short>(*o) {}     };
  template<> struct _item<long> : public tag, public _data<long>  
  { _item(CSTR& t, long* o, int i=-1): tag(t,i), _data<long>(*o) {}       };
  template<> struct _item<unsigned short> : public tag, public _data<unsigned short>  
  { _item(CSTR& t, unsigned short* o, int i=-1): tag(t,i), _data<unsigned short>(*o) {} };
  template<> struct _item<unsigned int> : public tag, public _data<unsigned int>  
  { _item(CSTR& t, unsigned int* o, int i=-1): tag(t,i), _data<unsigned int>(*o) {}     };
  template<> struct _item<unsigned long> : public tag, public _data<unsigned long>  
  { _item(CSTR& t, unsigned long* o, int i=-1): tag(t,i), _data<unsigned long>(*o) {}   };
  template<> struct _item<float> : public tag, public _data<float>  
  { _item(CSTR& t, float* o, int i=-1): tag(t,i), _data<float>(*o) {}     };
  template<> struct _item<double> : public tag, public _data<double>  
  { _item(CSTR& t, double* o, int i=-1): tag(t,i), _data<double>(*o) {}   };
  template<> struct _item<char*> : public tag, public _data<std::string>  
  { _item(CSTR& t, const char* o, int i=-1): tag(t,i), _data<std::string>(o) {} };
  template<> struct _item<std::string> : public tag, public _data<std::string>  
  { _item(CSTR& t, CSTR& o, int i=-1): tag(t,i), _data<std::string>(o) {} };
  template<> struct _item<std::string*> : public tag, public _data<std::string>
  { _item(CSTR& t, CSTR* o, int i=-1): tag(t,i), _data<std::string>(*o) {} };

  template <class T> inline _item<T> item(CSTR& t, const T& o, int i=-1)
  { return _item<T>(t,const_cast<T*>(&o),i);                              }
  template <class T> inline _item<T> item(CSTR& t, const T* o, int i=-1)
  { return _item<T>(t,const_cast<T*>(o),i);                               }
  inline _item<std::string> item(CSTR& t, std::string o, int i=-1)
  { return _item<std::string>(t, o, i);                                   }
  inline _item<std::string> item(CSTR& t, const char* o, int i=-1)
  { return _item<std::string>(t, std::string(o), i);                      }
  inline _item<std::string> item(const char* t, CSTR& o, int i=-1)
  { return _item<std::string>(t, o, i);                                   }
  inline _item<std::string> item(const char* t, const char* o, int i=-1)
  { return _item<std::string>(t, std::string(o), i);                      }

  /** Serialize single row in a collection                                */
  template <class T> struct _row : public _item<T>  
  { _row(CSTR& t, const T& o, int i): _item<T>(t,o,i) {}                  };
  template <class T> inline _row<std::string> row(CSTR& t, const T& o, int i)  
  { return _row<std::string>(t,o,i);                                      }
  inline _row<std::string> row(CSTR& t, CSTR& o, int i)  
  { return _row<std::string>(t,o,i);                                      }
  inline _row<std::string> row(CSTR& t, const char* o, int i)  
  { return row<std::string>(t,std::string(o),i);                          }

  /** Default itration processing predicate.                              */
  template <class T> struct _predicate   {
    _predicate() {} 
    template <class P> P& operator()(P& t) const    {   return t;         }
  };

  struct toText : public _predicate<std::string>    {
    toText() {}
    std::string operator()(std::string t)  const  
    {   return text(t);                                                   }
    std::string operator()(std::string* t)  const  
    {   return text(*t);                                                  }
    template <class A, class B> inline
    std::pair< A, B> operator()(std::pair< A, B> t)   const 
    { return std::pair< A, B>(text(t.first), text(t.second));             }
  };

  /** Serialize iteration using iterator ranges                           */
  template<class T, class P> struct _iteration : public tag  {
    T begin, end;
    const P& pred;
    _iteration(CSTR& t, T b, T e, const P& p)
      : tag(t), begin(b), end(e), pred(p) {}
  };
  template <class T> inline 
  _iteration<T, _predicate<T> > iteration(CSTR& t, T b, T e)
  { return _iteration<T, _predicate<T> >(t,b,e,_predicate<T>());          }
  template <class T, class P> inline 
  _iteration<T, P> iteration(CSTR& t, T b, T e, const P& p)
  { return _iteration<T, P>(t,b,e,p);                                     }

  /** Serialize collections                                               */
  template<class T, class P> 
  struct _collection : public _iteration<typename T::const_iterator, P>  {
    _collection(CSTR& t, const T& d) 
      : _iteration<typename T::const_iterator, P >(t,d.begin(),d.end()) {}
  };
  template <class T> inline 
  _collection<T, _predicate<T> > collection(CSTR& t, const T& d)  
  { return _collection<T, _predicate<T> >(t, d); }
  template <class T, class P> inline 
  _collection<T, P> collection(CSTR& t, const T& d, const P& p)
  { return _collection<T, P>(t,d,p);  }

  /// Serialize XML data item
  template <class T> inline 
  Stream& operator<<(Stream& s, const _data<T>& o)
  {  return s << o.value;                                                  }
  /// Serialize XML start tag
  inline Stream& operator<<(Stream& s, const XML::start& e)  { 
    if ( e.number < 0 ) return s << "<" << e.name << ">";
    return s << "<" << e.name << " num=\"" << e.number << "\">";
  }
  /// Serialize XML end tag 
  inline Stream& operator<<(Stream& s, const XML::end& e)  
  { return s << "</" << e.name << ">" << std::endl;  }
  /// Serialize single item to XML
  template <class T> inline 
  Stream& operator<<(Stream& s, const _item<T>& o)
  {  return s << start(o.name, o.number) << o.value << end(o.name);        }
  /// Serialize single collection row to XML
  template <class T> inline
  Stream& operator << (Stream& s, const _row<T>& c)  {
    s << start("ROW", c.number) << _item<T>(c.name, c.value);
    return s << end("ROW", c.number);
  }
  /// Serialize a std pair object
  template <class U, class V> inline
  Stream& operator<<(Stream& o, const std::pair<U, V>& p)
  {  return o << XML::item("P1",p.first) << XML::item("P2",p.second);     }
  /// Swerialize a collection of obejcts to XML
  template <class T, class P> inline 
  Stream& operator << (Stream& s, const _iteration<T, P>& c)  {
    T i;
    int tot = 0, cnt = 1;
    for ( i = c.begin; i != c.end; ++i ) tot++;
    s << start(c.name) << std::endl << item("COUNT",tot);
    for ( i = c.begin; i != c.end; ++i, ++cnt ) 
    { s << start("ROW", cnt) << c.pred(*i) << end("ROW",cnt);  }
    s << end(c.name);
    return s;
  }
  /// Write header tag to XML stream.                
  inline Stream& header(Stream& s)     
  { return s.putHeader();                                                 }
  /// Submit a style sgeet to the XML stream
  struct style : public std::string  
  { explicit style(CSTR& url)  { this->assign(url.begin(),url.end());  }  };
  inline Stream& operator << (Stream& s, const style& o)   
  { return s.putStyle(o);                                                 }

  /** Submit guard, which closes itself on destruction                    */
  struct Guard : public tag {
    Stream& m_s;
    /// Submit guard, which closes itself on destruction: Constructor
    Guard(Stream& s, CSTR& t, int i=-1) : tag(t,i), m_s(s)    
    {  m_s << start(name, number) << std::endl;                            }
    /// Submit guard, which closes itself on destruction: Destructor
    ~Guard()        {  m_s << end(name, number) << std::endl;              }
  };
  /// Add comment to XML output stream
  struct comment : public std::string  {
    explicit comment(CSTR& c)    {
      std::string tmp="\n<!-- " + c + " -->\n";
      this->assign(tmp.begin(),tmp.end());
    }
  };
}
#endif // ONLINEKERNEL_CPP_XMLSTREAM_H
