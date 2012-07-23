#ifndef LBALPGEN_AHIO_H
#define LBALPGEN_AHIO_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall AHIO_ADDRESS(void);
}
#else
extern "C" {
  void* ahio_address__(void);
}
#endif

/** @class Ahio Ahio.h LbAlpGen/Ahio.h
 *
 *  Class to access AHIO AlpGen Common block from C++ code.
 * 
 *  @author Patrick Robbe
 *  @date   2012-07-17
 */

class Ahio {
public:
  Ahio();
  ~Ahio();
  int& nunit();
  int& nunitout() ;
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths

private:
  struct AHIO;
  friend struct AHIO;
  struct AHIO {
    int nunit;
    int nunitout;
    int nunitinit ;
    char filename[ 100 ] ;
  };

  static AHIO* s_ahio;
};

// Inline implementations for AHIO
// initialise pointer
#ifdef WIN32
void Ahio::init(void) {
  if ( 0 == s_ahio ) s_ahio = static_cast<AHIO*>(AHIO_ADDRESS());
}
#else
void Ahio::init(void) {
  if ( 0 == s_ahio ) s_ahio = static_cast<AHIO*>(ahio_address__());
}
#endif
#endif // LBALPGEN_AHIO_H
