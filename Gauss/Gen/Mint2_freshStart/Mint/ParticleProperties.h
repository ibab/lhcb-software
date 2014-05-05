#ifndef PARTICLEPROPERTIES_HH
#define PARTICLEPROPERTIES_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include <string>
#include <iostream>

#include "Mint/MultiQuarkContent.h"

class ParticleProperties{
  static const char* pionString;
 protected:
  double _mass, _mErrPlus, _mErrMinus;
  double _width, _wErrPlus, _wErrMinus;
  std::string _Isospin; // isospin
  int _Gparity; // G-parity
  std::string _JtotalSpin; // total spin
  int _Parity; // Parity
  int _Cqm; // Charge 'parity'
  char _Aformat; // anti-particle format character
  int _pdg_id;
  std::string _charge;
  int _Rexist; //  likelihood of existence, baryons only
  char _Status; // status (estalished or not etc)
  std::string _name;
  std::string _quarks;

  MultiQuarkContent _netQuarkContent;

  double _Radius;
  static double _defaultRadius;
  static double _defaultCharmRadius;

  bool _isValid;
  
  void setRadius();

  void antiQuarks();
  void antiQuarkContent();
  void antiCharge();
 public:
  double mass()const{return _mass;} // mass
  double mErrPlus()const{return _mErrPlus;}
  double mErrMinus()const{return _mErrMinus;}
  
  double width()const{return _width;} // width
  double wErrPlus()const{return _wErrPlus;}
  double wErrMinus()const{return _wErrMinus;}

  std::string I()const{return _Isospin;} // isospin
  int G()const{return _Gparity;}; // G-parity
  std::string J()const{return _JtotalSpin;} // total spin
  int P()const{return _Parity;} // Parity
  int C()const{return _Cqm;} // Charge 'parity'
  int pdg_id()const{return _pdg_id;}
  std::string charge()const{return _charge;}
  int R()const{return _Rexist;} //  likelihood of existence, baryons only
  char S()const{return _Status;} // status (estalished or not etc)
  std::string name() const;
  std::string quarks() const{return _quarks;}
  const MultiQuarkContent& netQuarkContent() const{return _netQuarkContent;}

  double lifetime() const;
  double ctau() const;
  double ctau_in_microns() const;
  double lifetime_in_ns() const;
  double lifetime_in_ps() const;

  double radius() const;

  std::string ScalarVectorPseudoAxialVectorTensor() const;
  std::string SVPAT() const{
    return ScalarVectorPseudoAxialVectorTensor();
  }
  bool isVector() const{return ScalarVectorPseudoAxialVectorTensor() == "V";}
  bool isScalar() const{return ScalarVectorPseudoAxialVectorTensor() == "S";}
  bool isPseudoScalar() const{return ScalarVectorPseudoAxialVectorTensor() == "P";}
  bool isAxialVector() const{return ScalarVectorPseudoAxialVectorTensor() == "A";}
  bool isTensor() const{return ScalarVectorPseudoAxialVectorTensor() == "T";}
  bool isPseudoTensor() const{return ScalarVectorPseudoAxialVectorTensor() == "PT";}

  //  double mass_in_GeV() const{return mass()/1000.0;}

  bool isValid() const{return _isValid;}

  bool hasDistinctAnti() const;
  bool isItsOwnAnti() const{return ! hasDistinctAnti();}

  ParticleProperties(std::string pdg_string="");

  void print(std::ostream& out = std::cout) const;

  bool operator==(const ParticleProperties& rhs) const{
    if(_pdg_id==0 && rhs._pdg_id==0){
      return name() == rhs.name();
    }
    return _pdg_id == rhs._pdg_id;
  }
  bool operator<(const ParticleProperties& rhs) const{
    if(_pdg_id==0 || rhs._pdg_id==0){
      return name() < rhs.name();
    }
    return _pdg_id < rhs._pdg_id;
  }
  bool operator>(const ParticleProperties& rhs) const{
    return ! (*this == rhs || *this < rhs);
  }
  bool operator<=(const ParticleProperties& rhs) const{
    return (*this < rhs || *this == rhs);
  }
  bool operator>=(const ParticleProperties& rhs) const{
    return (*this > rhs || *this == rhs);
  }

  bool antiThis();
  ParticleProperties anti() const;

  bool isNonResonant() const;

};

std::ostream& operator<<(std::ostream& out, const ParticleProperties& pp);

#endif
//
