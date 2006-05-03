// $Id: Octmatrix.h,v 1.2 2006-05-03 08:24:32 robbep Exp $

#ifndef LBBCVEGPY_OCTMATRIX_H
#define LBBCVEGPY_OCTMATRIX_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall OCTMATRIX_ADDRESS(void) ;
}
#else
extern "C" {
  void* octmatrix_address__(void) ;
}
#endif

class Octmatrix {
public:
  Octmatrix();
  ~Octmatrix();

  double& coeoct();

  inline void init(); // inlined for speed of access (small function)


private:
  struct OCTMATRIX;
  friend struct OCTMATRIX;
  
  struct OCTMATRIX {
    double coeoct;
  };
  int m_dummy;
  double m_realdummy;
  static OCTMATRIX* s_octmatrix;
};

// Inline implementations for Octmatrix
// initialise pointer
#ifdef WIN32
void Octmatrix::init(void) {
  if ( 0 == s_octmatrix ) s_octmatrix = static_cast<OCTMATRIX*>(OCTMATRIX_ADDRESS());
}
#else
void Octmatrix::init(void) {
  if ( 0 == s_octmatrix ) s_octmatrix = static_cast<OCTMATRIX*>(octmatrix_address__());
}
#endif
#endif // LBBCVEGPY_OCTMATRIX_H
 
