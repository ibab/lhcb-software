
#ifndef LBPYTHIA_CONFINE_H
#define LBPYTHIA_CONFINE_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall CONFINE_ADDRESS(void) ;
}
#else
extern "C" {
  void* confine_address__(void) ;
}
#endif

class Confine {
public:
  Confine();
  ~Confine();

  double& ptcut();
  double& etacut();
  double& psetacut(); //F. Zhang 01-04-11 
  double& pscutmin();
  double& pscutmax();

  inline void init(); // inlined for speed of access (small function)

private:
  struct CONFINE;
  friend struct CONFINE;
  
  struct CONFINE {
    double ptcut;
    double etacut; 
    double psetacut; //F. Zhang 01-04-11
    double pscutmin; // G.Graziani 14-02-12
    double pscutmax;
    
  };
  int m_dummy;
  double m_realdummy;
  static CONFINE* s_confine;
};

// Inline implementations for Confine
// initialise pointer
#ifdef WIN32
void Confine::init(void) {
  if ( 0 == s_confine ) s_confine = static_cast<CONFINE*>(CONFINE_ADDRESS());
}
#else
void Confine::init(void) {
  if ( 0 == s_confine ) s_confine = static_cast<CONFINE*>(confine_address__());
}
#endif
#endif // LBPYTHIA_CONFINE_H
 
