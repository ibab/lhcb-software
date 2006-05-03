// $Id: Genefull.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBBCVEGPY_GENEFULL_H
#define LBBCVEGPY_GENEFULL_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall GENEFULL_ADDRESS(void) ;
}
#else
extern "C" {
  void* genefull_address__(void) ;
}
#endif

class Genefull {
public:
  Genefull();
  ~Genefull();

  bool& generate();

  inline void init(); // inlined for speed of access (small function)

private:
  struct GENEFULL;
  friend struct GENEFULL;
  
  struct GENEFULL {
    bool generate;
  };
  int m_dummy;
  double m_realdummy;
  bool m_booldummy; 
  static GENEFULL* s_genefull;
};

// Inline implementations for Genefull
// initialise pointer
#ifdef WIN32
void Genefull::init(void) {
  if ( 0 == s_genefull ) s_genefull = static_cast<GENEFULL*>(GENEFULL_ADDRESS());
}
#else
void Genefull::init(void) {
  if ( 0 == s_genefull ) s_genefull = static_cast<GENEFULL*>(genefull_address__());
}
#endif
#endif // LBBCVEGPY_GENEFULL_H
 
