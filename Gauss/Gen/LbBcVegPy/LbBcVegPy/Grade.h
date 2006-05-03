// $Id: Grade.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBBCVEGPY_GRADE_H
#define LBBCVEGPY_GRADE_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall GRADE_ADDRESS(void);
}
#else
extern "C" {
  void* grade_address__(void);
}
#endif

// Lengths of array in Grade common
static const int s_lenXi = 10;
static const int s_widthXi = 50;

class Grade {
public:
  Grade();
  ~Grade();
  //  const double& xi(int i, int j) const;
  double& xi(int i, int j) ;
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenXi() const {return s_lenXi;}
  int widthXi() const {return s_widthXi;}

private:
  struct GRADE;
  friend struct GRADE;
  struct GRADE {
    double xi[s_lenXi][s_widthXi];
  };
  int m_dummy;
  double m_realdummy;
  static GRADE* s_grade;
};

// Inline implementations for Grade
// initialise pointer
#ifdef WIN32
void Grade::init(void) {
  if ( 0 == s_grade ) s_grade = static_cast<GRADE*>(GRADE_ADDRESS());
}
#else
void Grade::init(void) {
  if ( 0 == s_grade ) s_grade = static_cast<GRADE*>(grade_address__());
}
#endif
#endif // LBBCVEGPY_GRADE_H
