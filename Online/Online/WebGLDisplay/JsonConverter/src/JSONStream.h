#ifndef JSONSTREAM_H
#define JSONSTREAM_H 1

// STL include files
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sstream>
#include <utility>

/**
 *    JSON namespace declaration
 */
namespace JSON  {

  typedef unsigned long long ulonglong;
  const std::string SEP = ",";
  enum class Container {LIST, MAP};

  std::map<Container, std::string> startM = { { Container::LIST, "["}, 
                                              { Container::MAP, "{"} };
  std::map<Container, std::string> endM = { { Container::LIST, "]"}, 
                                            { Container::MAP, "}"} };


/**
 * Stream JSONStream.h
 */
  class Stream   {

  private:
    std::stringstream m_s;
    std::ostream m_os;
    Container m_container;
    bool m_hasdata;

    void checkSep() {
        if (m_hasdata) {
            m_os << ", ";
        } else {
            m_hasdata = true;
        }
    }

  public:
    
    /// Constructors
    Stream(Container c) : m_os(m_s.rdbuf()), m_container(c), m_hasdata(false){
      m_os << startM[m_container];
    }

    Stream(const Stream& s) : m_os(m_s.rdbuf()), m_container(s.m_container), m_hasdata(false){
      m_os << s.str();
    }


    /// Standard Destructor
    virtual ~Stream() { }
    /// Allow access to the underlying STL stream buffer
    virtual std::streambuf* rdbuf()                 {return m_os.rdbuf(); }
    /// Allow access to the underlying STL stream buffer
    virtual std::streambuf* rdbuf(std::streambuf* b){return m_os.rdbuf(b);}
    /// Allow access to the underlying STL stream
    virtual std::ostream* stream()                  {return &m_os;  }



    /// Templating for basic types
    template <class T>
    Stream& operator<<(T arg)  {checkSep(); return append(arg);}

    Stream& operator<<(Stream& arg)  {
      checkSep(); return append(arg);
    }

 
    Stream& operator<<(std::pair<std::string, Stream>& myPair)  {
      checkSep();
      append(myPair.first);
      m_os << ":";
      append(myPair.second);
      return *this;
    }



    /// Real implementation of the append method
    virtual Stream& append(std::string arg)  {m_os << "\"" << arg << "\"";
                                               return *this;}
    virtual Stream& append(int arg)  { m_os << arg; return *this;}
    virtual Stream& append(double arg)  { m_os << arg; return *this;}
    virtual Stream& append(unsigned int arg)  {  m_os << arg;return *this;}
    virtual Stream& append(ulonglong arg)  { m_os << arg;return *this;}
    virtual Stream& append(Stream& arg)  {
        m_os << arg.str();
        return *this;
    }

    template <class K, class V>
    Stream& append(std::pair<K, V>& myPair)  {
        append(myPair.first);
        m_os << ":";
        append(myPair.second);
        return *this;
    }

    template <class T>
    Stream &append(const std::vector<T>& vec)  {
        m_os << "[";
        std::string  sep = "";
        for (T i : vec ) {
            m_os << sep;
            sep = ", ";
            append(i);
        }
        m_os << "]\n";
        return *this;
    }

    template <class K, class V>
    Stream&append(const std::map<K, V>& map)  {
        m_os << "{";
        std::string  sep = "";
        for (const auto &myPair : map ) {
            m_os << sep;
            sep = ", ";
            append(myPair.first);
            m_os << ":";
            append(myPair.second);
        }
        m_os << "}\n";
        return *this;
    }

    virtual const std::string str() const {
      return m_s.str() + endM[m_container];
    }
  };
}
#endif // JSONSTREAM_H
