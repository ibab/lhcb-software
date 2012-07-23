#ifndef LBALPGEN_AIOUNITS_H
#define LBALPGEN_AIOUNITS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall IOUNITS_ADDRESS(void);
}
#else
extern "C" {
  void* iounits_address__(void);
}
#endif

/** @class Aiounits Aiounits.h LbAlpGen/Aiounits.h
 *
 *  Class to access IOUNITS AlpGen Common block from C++ code.
 * 
 *  @author Patrick Robbe
 *  @date   2012-07-22
 */

class Aiounits {
public:
  Aiounits();
  ~Aiounits();
  int& niosta();
  int& niopar() ;
  int& niowgt() ;
  int& niounw() ;
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths

private:
  struct AIOUNITS;
  friend struct AIOUNITS;
  struct AIOUNITS {
    int niosta;
    int niopar;
    int niowgt;
    int niounw;
  };

  static AIOUNITS* s_aiounits;
};

// Inline implementations for AIOUNITS
// initialise pointer
#ifdef WIN32
void Aiounits::init(void) {
  if ( 0 == s_aiounits ) s_aiounits = static_cast<AIOUNITS*>(IOUNITS_ADDRESS());
}
#else
void Aiounits::init(void) {
  if ( 0 == s_aiounits ) s_aiounits = static_cast<AIOUNITS*>(iounits_address__());
}
#endif
#endif // LBALPGEN_AHIO_H
