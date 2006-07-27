
#ifndef LESTER_DEREFERENCING_OSTREAM_ITERATOR_H
#define LESTER_DEREFERENCING_OSTREAM_ITERATOR_H

#include "Utils/DereferencingOstreamIterator.fwd"
#include <ostream>


/// DereferencingOstreamIterator < class T , N > is a replacement for std::ostream_iterator. DereferencingOstreamIterator is useful when the data type that the iterator points to needs to be DEREFERENCED before being sent to the ostream.
/** 

If non-negative, the template parameter N indicates number of times to dereference the type T.  If negative, the template parameter N indicates a desire to dereference AS MANY TIMES AS POSSIBLE! (i.e. till a non pointer type is reached!)  N defaults to 0 in the .fwd file, i.e. the default is to reproduce the behaviour of std::ostream_iterator .


Use for example as follows:


 #include \<string\>\n
 #include \<vector\>\n
 #include \<iostream\>\n
 #include \<algorithm\>\n
 #include "Utils/DereferencingOstreamIterator.h"\n

int main() {\n
  typedef std::vector<const std::string *> PVec;\n
   PVec pVec;\n
   const std::string s1 = "Hello";\n
   const std::string s2 = "World";\n
   copy(pVec.begin(), pVec.end(), DereferencingOstreamIterator<const std::string *,1>(std::cout , " "));\n
   std::cout<<std::endl;\n
   return 0;\n
};\n

The above code should generate send "Hello World\n" to std::cout.
The generic implementation will be OK provided ALL the pointers it encounters are normal ones (not smart or refcounted pointers that look like ordinary objects to the compiler).  The specialisations for N=0 to 10 get around this problem but only by explicit instantiation for all these values ... ugly!
*/
template<class T, int N >
class DereferencingOstreamIterator {
private:
  bool m_outputted;
  std::ostream & m_os;
private:
  const char * const m_delimiter;
  template<class Q>
  void test(std::ostream & os, Q * t) {
    test(os, *t);
  };
  template<class Q>
  void test(std::ostream & os, Q t) {
    os << t;
  };
  template<class Q>
  void test(std::ostream & os, Q * t, const unsigned int derefsToDo) {
    if (derefsToDo>=1) {
      test(os, *t, derefsToDo-1);
    } else {
      // print out what we have EVEN IF IT IS STILL A POINTER TYPE!
      os << t;
    };
  };
  template<class Q>
  void test(std::ostream & os, Q t, const unsigned int derefsToDo) {
    if (derefsToDo==0) {
      os << t;
    } else {
      os << "DAMN: "<<derefsToDo<<" TOO EARLY " << t;
    };
  };
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    m_outputted(false),
    m_os(os),
    m_delimiter(delimiter) {
  };
  DereferencingOstreamIterator & operator=(const T & t) {
    if (m_outputted) {
      m_os << m_delimiter;
    } else {
      m_outputted=true;
    };
    //dereferenceMTimesAndSendToOstream<N>(m_os, t);
    //MultiDereferencer<*T, N>::dereferenceMTimesAndSendToOstream(m_os, t);
    //m_os << *t;
    if (N>=0) {
      test(m_os,t,static_cast<unsigned int>(N));
    } else {
      test(m_os,t);
    };
    return *this;
  };
  DereferencingOstreamIterator & operator*() {
    return *this;
  };
  DereferencingOstreamIterator & operator++() { return *this; };
};


template<class T>
class DereferencingOstreamIterator<T,0> {
private:
  bool m_outputted;
protected:
  std::ostream & m_os;
  const char * const m_delimiter;
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    m_outputted(false),
    m_os(os),
    m_delimiter(delimiter) {
  };
  DereferencingOstreamIterator & operator=(const T & t) {
    if (m_outputted) {
      m_os << m_delimiter;
    } else {
      m_outputted=true;
    };
    output(t);
    return *this;
  };
  DereferencingOstreamIterator & operator*() {
    return *this;
  };
  DereferencingOstreamIterator & operator++() { return *this; };
private:
  virtual void output(const T & t) {
    m_os << t;
  };
};

template<class T>
class DereferencingOstreamIterator<T,1> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << *t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,2> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << **t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,3> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << ***t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,4> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << ****t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,5> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << *****t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,6> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << ******t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,7> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << *******t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,8> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << ********t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,9> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << *********t;
  };  
};

template<class T>
class DereferencingOstreamIterator<T,10> : public DereferencingOstreamIterator<T,0> {
public:
  DereferencingOstreamIterator(std::ostream & os, 
			       const char * const delimiter=" ") :
    DereferencingOstreamIterator<T,0>(os, delimiter) {
  };
private:
  virtual void output(const T & t) {
    m_os << **********t;
  };  
};





/*
template<class T>
DereferencingOstreamIterator<T,1> & DereferencingOstreamIterator<T,1>::operator=(const T & t); {
  if (m_outputted) {
    m_os << m_delimiter;
  } else {
    m_outputted=true;
  };
  //dereferenceMTimesAndSendToOstream<N>(m_os, t);
  //MultiDereferencer<*T, N>::dereferenceMTimesAndSendToOstream(m_os, t);
  m_os << *t;
  
  return *this;
};
*/

#endif
