// F. Zhang 01-04-11
#ifndef LBGENXICC_COUNTER_H
#define LBGENXICC_COUNTER_H 1

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
  // F. Zhang 01-04-11
  // int& ibcstate();
  int& ixiccstate();
  int& nev();
  inline void init(); // inlined for speed of access (small function)
private:
  struct COUNTER;
  friend struct COUNTER;
  
  struct COUNTER {
    //int ibcstate;
    int ixiccstate; // F. Zhang 01-04-11
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
#endif // LBGENXICC_COUNTER_H
 
