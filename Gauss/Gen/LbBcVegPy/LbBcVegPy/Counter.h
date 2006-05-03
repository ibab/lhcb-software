// $Id: Counter.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBBCVEGPY_COUNTER_H
#define LBBCVEGPY_COUNTER_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall COUNTER_ADDRESS(void) ;
}
#else
extern "C" {
  void* counter_address__(void) ;
}
#endif

class Counter {
public:
  Counter();
  ~Counter();
  int& ibcstate();
  int& nev();
  inline void init(); // inlined for speed of access (small function)
private:
  struct COUNTER;
  friend struct COUNTER;
  
  struct COUNTER {
    int ibcstate;
    int nev;
  };
  int m_dummy;
  double m_realdummy;
  static COUNTER* s_counter;
};

// Inline implementations for Counter
// initialise pointer
#ifdef WIN32
void Counter::init(void) {
  if ( 0 == s_counter ) s_counter = static_cast<COUNTER*>(COUNTER_ADDRESS());
}
#else
void Counter::init(void) {
  if ( 0 == s_counter ) s_counter = static_cast<COUNTER*>(counter_address__());
}
#endif
#endif // LBBCVEGPY_COUNTER_H
 
