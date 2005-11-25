// $Id: ParticleID.cpp,v 1.2 2005-11-25 16:15:48 cattanem Exp $
// Include files 

#include "Kernel/ParticleID.h"
#include <math.h>

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleID
//
// 19/02/2002 : Gloria Corti
//-----------------------------------------------------------------------------

//=============================================================================
// Is a valid number according to LHCb scheme
//=============================================================================
bool LHCb::ParticleID::isValid( ) const {

  // Reserved GEANT3 particles
  if( extraBits() > 0 ) {
    if( extraBits() >= 45 && extraBits() <= 50 ) {
      return true;
    }
    return false;
  }
  // Meson, Baryon or DiQuark signature
  if( isMeson() || isBaryon() || isDiQuark() ) {
    return true; 
  }
  // Fundamental particle (quark or lepton)
  if( fundamentalID() > 0 ) {
    return true; 
  }
  return false;

}

//=============================================================================
// Mesons
//=============================================================================
bool LHCb::ParticleID::isMeson( ) const {
  if( hasQuarks() ) {
    int aid = abspid();
    if( aid == 130 || aid == 310 ) {
      return true;
    }
    if( digit(nj) > 0 && digit(nq3) > 0 && digit(nq2) > 0 && digit(nq1) == 0 ){
      // check for illegal antiparticles
      if( digit(nq3) == digit(nq2) && pid() < 0 ) {
        return false;
      }
      else {
        return true;
      }
    }
  }
  return false;
}

//=============================================================================
// Baryon
//=============================================================================
bool LHCb::ParticleID::isBaryon( ) const {
  if( hasQuarks() ) {
    if( digit(nj) > 0 && digit(nq3) > 0 && digit(nq2) > 0 && digit(nq1) > 0 ) {
      return true;
    }
  }
  return false;
}

//=============================================================================
// DiQuark
//=============================================================================
bool LHCb::ParticleID::isDiQuark( ) const {
  if( hasQuarks() ) {
    // diquark signature
    if( digit(nj) > 0 && digit(nq3) == 0 && digit(nq2) > 0 && digit(nq1) > 0 ){
      // illegal
      if( digit(nj) == 1 && digit(nq2) == digit(nq1) ) {
          return false; 
      }
      else {
        return true;
      }
    }
    return false;
  }
  return false;
}

//=============================================================================
// Hadron
//=============================================================================
bool LHCb::ParticleID::isHadron( ) const {
  if( extraBits() > 0 ) { 
    return false;
  }
  if( isMeson() || isBaryon() ) { 
    return true;
  }
  return false;
}

//=============================================================================
// Leptons
//=============================================================================
bool LHCb::ParticleID::isLepton( ) const {
  if( extraBits() > 0 ) { 
    return false; 
  }
  if( fundamentalID() >= 11 && fundamentalID() <= 18 ) { 
    return true;
  }
  return false;
}

//=============================================================================
// Nucleus
//=============================================================================
bool LHCb::ParticleID::isNucleus() const {
  if( extraBits() <= 0 ) {
    return false;
  }
  if( extraBits() == 48 || extraBits() == 50 ) {
    return false;
  }
  return true;
}


//=============================================================================
// up
//=============================================================================
bool LHCb::ParticleID::hasUp(  ) const {
  return hasQuark(up);
}

//=============================================================================
// down
//=============================================================================
bool LHCb::ParticleID::hasDown( ) const {
  return hasQuark(down);
}

//=============================================================================
// strange
//=============================================================================
bool LHCb::ParticleID::hasStrange( ) const {
  return hasQuark(strange);
}

//=============================================================================
// charm
//=============================================================================
bool LHCb::ParticleID::hasCharm( ) const {
  return hasQuark(charm);
}

//=============================================================================
// bottom
//=============================================================================
bool LHCb::ParticleID::hasBottom( ) const {
  return hasQuark(bottom);
}

//=============================================================================
// top
//=============================================================================
bool LHCb::ParticleID::hasTop( ) const {
  return hasQuark(top);
}

//=============================================================================
// Composed by quarks and not a nucleus
//=============================================================================
bool LHCb::ParticleID::hasQuarks() const {
  if( extraBits() > 0 ) { 
    return false; 
  }
  if( abspid() <= 100 ) { 
    return false; 
  }
  if( fundamentalID() <= 100 && fundamentalID() > 0 ) { 
    return false; 
  }
  return true;
}

//=============================================================================
// Quark content
//=============================================================================
bool LHCb::ParticleID::hasQuark( const Quark& q ) const {
  if( hasQuarks() ) {
    if( digit(nq3) == q || digit(nq2) == q || digit(nq1) == q ) {
      return true;
    }
  }
  return false;
}

//=============================================================================
// Will return digits for quarks, leptons, higgs, etc. but 0 for mesons,
// baryons,...
//=============================================================================
int LHCb::ParticleID::fundamentalID( ) const {
  if( digit(nq2) == 0 && digit(nq1) == 0) {
    return abspid()%10000;
  } else {
    return 0;
  }
}

//=============================================================================
// Return value of digit in specified location
//=============================================================================
unsigned short LHCb::ParticleID::digit(const Location& loc ) const {
  //  PID digits (base 10) are: n nr nl nq1 nq2 nq3 nj
  //  the location enum provides a convenient index into the PID
  int numerator = (int) pow(10.0,(loc-1));
  return (abspid()/numerator)%10;
}

//=============================================================================
// Everything beyond the 7th digit (e.g. outside the PDG numbering scheme)
//=============================================================================
int LHCb::ParticleID::extraBits( ) const {
  return abspid()/10000000;
}

//=============================================================================
// Three times the charge in e+ units
//=============================================================================
int LHCb::ParticleID::threeCharge( ) const {

  int charge = 0;
  
  // illegal or uninitialized pid
  if( pid() == 0 ) {
    return charge;
  }

  // Special LHCb particles 
  if( extraBits() > 0 ) {
    charge = 0;
    // H2 or H3
    if( extraBits() == 45 || extraBits() == 46 ) {
      charge = 3;
    }
    // Alpha or He3
    if( extraBits() == 47 || extraBits() == 49 ) {
      charge = 6;
    }
    return charge;
  }

  // Ks, Kl 
  if( digit(nj) == 0 ) {
    return charge;
  }

  int ch100[100] = { -1,  2, -1, 2, -1, 2, -1, 2, 0, 0,
                     -3,  0, -3, 0, -3, 0, -3, 0, 0, 0,
                      0,  0,  0, 3,  0, 0,  0, 0, 0, 0,
                      0,  0,  0, 3,  0, 0,  3, 0, 0, 0,
                      0, -1,  0, 0,  0, 0,  0, 0, 0, 0,
                      0,  6,  3, 6,  0, 0,  0, 0, 0, 0,
                      0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
                      0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
                      0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
                      0,  0,  0, 0,  0, 0,  0, 0, 0, 0 };
  int sid = fundamentalID();
  unsigned short q1, q2, q3;
  q1 = digit(nq1);
  q2 = digit(nq2);
  q3 = digit(nq3);

  // For fundamental particles use table
  if( sid > 0 && sid <= 100 ) {
    charge = ch100[sid-1];
  }
  // Mesons
  else if( q1 == 0 ) {
    if( q2 == 3 || q2 == 5 ) {
      charge = ch100[q3-1] - ch100[q2-1];
    }
    else {
      charge = ch100[q2-1] - ch100[q3-1];
    }
  }
  // Di-quark
  else if( q3 == 0 ) {
    charge = ch100[q2-1] + ch100[q1-1];
  }
  // Baryons
  else {
    charge = ch100[q3-1] + ch100[q2-1] + ch100[q1-1];
  }
  // Set charge of antiparticle
  if( charge != 0 ) {
    if( pid() < 0 ) {
      charge = -charge; 
    }
  }
  return charge;
}

//=============================================================================
// Returns 2J+1, where J is the total spin
//=============================================================================
int LHCb::ParticleID::jSpin( ) const {

  if( fundamentalID() > 0 && fundamentalID() <= 100 ) { 
    return 0; 
  }
  if( extraBits() > 0 ) { 
    return 0; 
  }
  return abspid()%10;

}
//=============================================================================
// Returns 2S+1, where S is the spin
//=============================================================================
int LHCb::ParticleID::sSpin( ) const {

  if( !isMeson() ) {
    return 0;
  }
  if( !isMeson() ) {
    return 0;
  }
  int nl = (abspid()/10000)%10;
  int tent = (abspid()/1000000)%10;
  int js = abspid()%10;
  // tentative assignment
  if( tent == 9 ) {
    return 0;
  }
  if( nl == 0 && js >= 3 ) { 
    return 1;
  }
  else if( nl == 0  && js == 1 ) {
    return 0;
  } 
  else if( nl == 1  && js >= 3 ) {
    return 0;
  }
  else if( nl == 2  && js >= 3 ) {
    return 1;
  }
  else if( nl == 1  && js == 1 ) {
    return 1;
  }
  else if( nl == 3  && js >= 3 ) {
    return 1;
  }
  // default set to zero
  return 0;
}

//=============================================================================
// Returns 2L+1, where L is the orbital angular momentum"
//=============================================================================
int LHCb::ParticleID::lSpin( ) const {
  
  if( !isMeson() ) {
    return 0;
  }
  int nl = (abspid()/10000)%10;
  int tent = (abspid()/1000000)%10;
  int js = abspid()%10;
  // tentative assignment
  if( tent == 9 ) {
    return 0;
  }
  if( nl == 0 && js == 3 ) { 
    return 0;
  }
  else if( nl == 0 && js == 5 ) {
    return 1;
  }
  else if( nl == 0 && js == 7 ) {
    return 2;
  }
  else if( nl == 0 && js == 9 ) {
    return 3;
  } 
  else if( nl == 0  && js == 1 ) {
    return 0;
  }
  else if( nl == 1  && js == 3 ) {
    return 1;
  }
  else if( nl == 1  && js == 5 ) {
    return 2;
  }
  else if( nl == 1  && js == 7 ) {
    return 3;
  }
  else if( nl == 1  && js == 9 ) {
    return 4;
  }
  else if( nl == 2  && js == 3 ) {
    return 1;
  }
  else if( nl == 2  && js == 5 ) {
    return 2;
  }
  else if( nl == 2  && js == 7 ) {
    return 3;
  }
  else if( nl == 2  && js == 9 ) {
    return 4;
  }
  else if( nl == 1  && js == 1 ) {
    return 1;
  }
  else if( nl == 3  && js == 3 ) {
    return 2;
  }
  else if( nl == 3  && js == 5 ) {
    return 3;
  }
  else if( nl == 3  && js == 7 ) {
    return 4;
  }
  else if( nl == 3  && js == 9 ) {
    return 5;
  }
  // default set to zero
  return 0;
  
}
