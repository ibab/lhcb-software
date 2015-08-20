// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/ParticleProperties.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"


#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace MINT;

const char* ParticleProperties::pionString = "1.3957018E+02 ,3.5E-04,3.5E-04,2.5284E-14  ,5.0E-18,5.0E-18,1  ,-,0   ,-, ,B,    211,   +, ,R,pi               ,uD              ";

double ParticleProperties::_defaultRadius = 1.5/GeV;
double ParticleProperties::_defaultCharmRadius = 5.0/GeV;

void ParticleProperties::print(ostream& out) const{
  out <<   "Mass  " << mass()  << " +" << mErrPlus() << " -" << mErrMinus()
      << "\nWidth " << width() << " +" << wErrPlus() << " -" << wErrMinus()
      << "\n I=" << I() << ", G=" << G()
      << "\n J=" << J() << ", C=" << C() << ", P=" << P()
      << "\n Q = " << charge()
      << "\n pdg_id " << pdg_id()
      << "\n name " << name()
      << "\n quark content: " << quarks()
      << "\n net-quark-content " << netQuarkContent()
      << "\n is its own antiparticle? " << (isItsOwnAnti() ? "yes" : "no")
      << "\n lifetime " << lifetime_in_ps() << " ps"
      << "\n radius " << radius()*GeV << " /GeV"
     << endl;
}

/*

 0              1       2         3          4       5      6   7 8    91011  12      13 1415 16                 17
*MASS(MeV)    ,Err+   ,Err-   ,WIDTH(MeV)  ,Err+   ,Err-   ,I  ,G,J   ,P,C,A,PDG-MC ,Chrg,R,S,Name             ,Quarks         
0.0E+00       ,0.0E+00,0.0E+00,0.0E+00     ,0.0E+00,0.0E+00,<2 , ,1   ,-,-, ,     22,   0, ,R,gamma            ,                
8.0403E+04    ,2.9E+01,2.9E+01,2.14E+03    ,4.0E+01,4.0E+01,   , ,1   , , ,B,     24,   +, ,R,W                ,                
*/

ParticleProperties::ParticleProperties(std::string pdg_string)
  : _netQuarkContent()
{
  bool debugThis=false;

  _isValid=false;
  if(pdg_string.empty()) return;
  if(pdg_string[0] == '*') return;

  replace(pdg_string.begin(), pdg_string.end(), '\n', ','); // add a ',' to the end
  remove(pdg_string.begin(), pdg_string.end(), ' '); // remove white space
  
  static const unsigned int nFields=18;
  std::string s[nFields];
  unsigned int first= 0;
  unsigned int counter=0;
  unsigned int charCounter=0;
  while(counter < nFields){
    if(pdg_string[charCounter] == ','){
      int n=charCounter-first;
      if(n<0)n=0;
      
      std::string tempString = " ";
      if(n >= 1) tempString = pdg_string.substr(first, n);
      if(debugThis){
	cout << " tempString for charCounter = " << charCounter
	     << ", and counter = " << counter
	     << " is: " << tempString << endl;
      }
      if(tempString.empty()){
	s[counter] = " ";
      }else{
	s[counter] = tempString;
      }

      ++counter;
      first = charCounter+1;
    }
    if(charCounter < pdg_string.size()-1) charCounter++;
  }
  if(counter != nFields) return; // sth wrong

  if(debugThis){
    cout << "the strings : ";
    for(unsigned int i=0; i<nFields; i++) cout << s[i] << "\n";
    cout << endl;
  }

  _mass       = atof(s[0].c_str());
  _mErrPlus   = atof(s[1].c_str());
  _mErrMinus  = atof(s[2].c_str());
  _width      = atof(s[3].c_str());
  _wErrPlus   = atof(s[4].c_str());
  _wErrMinus  = atof(s[5].c_str());
  
  _Isospin    =      s[6];
  char Gchar  =      s[7][0];
  _JtotalSpin =      s[8];
  char Pchar  =      s[9][0];
  char Cchar  =      s[10][0];
  _Aformat    =      s[11][0];
  _pdg_id     = atoi(s[12].c_str());
  _charge     =      s[13];
  _Rexist     = atoi(s[14].c_str());
  _Status     =      s[15][0];
  _name       =      s[16];
  _quarks     =      s[17];

  _netQuarkContent.initFromString(_quarks);

  if(Gchar == '+') _Gparity=  1;
  if(Gchar == '-') _Gparity= -1;
  if(Gchar == ' ') _Gparity=  0;
  if(Gchar == '0') _Gparity=  0;

  if(Cchar == '+') _Cqm=  1;
  if(Cchar == '-') _Cqm= -1;
  if(Cchar == ' ') _Cqm=  0;
  if(Cchar == '0') _Cqm=  0;

  if(Pchar == '+') _Parity=  1;
  if(Pchar == '-') _Parity= -1;
  if(Pchar == ' ') _Parity=  0;
  if(Pchar == '0') _Parity=  0;

  setRadius();

  if(debugThis){
    cout << "ParticleProperties constructor: "
	 << "This is how I parsed the string: ";
    this->print();
  }

  _isValid=true;
}

void ParticleProperties::setRadius(){
  // set radius (not part of mass_width.csv):
  if (abs(pdg_id()) == 421 
      || abs(pdg_id()) == 411 
      || abs(pdg_id()) == 431){
    _Radius = _defaultCharmRadius;
  }else{
    _Radius = _defaultRadius;
  }

  string pdg_id_string = anythingToString(abs(pdg_id()));
  string parName = pdg_id_string + "_Radius_invGeV";
  NamedParameter<double> specificR( parName
				   , (double) -9999.0
				   , (const char*) 0
				   , NamedParameterBase::QUIET
				    );
  if(specificR > -9998){
    _Radius = specificR/GeV;
  }else{
    NamedParameter<double> genericR("Default_Radius_invGeV"
				    , (double) -9999.0
				    , (const char*) 0
				    , NamedParameterBase::QUIET);
   if(genericR > -9998){
      _Radius = genericR/GeV;
    }
  }

  return;
}

void ParticleProperties::antiQuarks(){
  if(_quarks.empty()) return;
  
  replace(_quarks.begin(), _quarks.end(), 'U', 'h');
  replace(_quarks.begin(), _quarks.end(), 'D', 'l');
  replace(_quarks.begin(), _quarks.end(), 'C', 'i');
  replace(_quarks.begin(), _quarks.end(), 'S', 'm');
  replace(_quarks.begin(), _quarks.end(), 'T', 'j');
  replace(_quarks.begin(), _quarks.end(), 'B', 'n');

  replace(_quarks.begin(), _quarks.end(), 'u', 'U');
  replace(_quarks.begin(), _quarks.end(), 'd', 'D');
  replace(_quarks.begin(), _quarks.end(), 'c', 'C');
  replace(_quarks.begin(), _quarks.end(), 's', 'S');
  replace(_quarks.begin(), _quarks.end(), 't', 'T');
  replace(_quarks.begin(), _quarks.end(), 'b', 'B');

  replace(_quarks.begin(), _quarks.end(), 'h', 'u');
  replace(_quarks.begin(), _quarks.end(), 'l', 'd');
  replace(_quarks.begin(), _quarks.end(), 'i', 'c');
  replace(_quarks.begin(), _quarks.end(), 'm', 's');
  replace(_quarks.begin(), _quarks.end(), 'j', 't');
  replace(_quarks.begin(), _quarks.end(), 'n', 'b');

  // since this also changes sqrt to sqrT, fix that:
  unsigned int pos = _quarks.find("SqrT");
  if(pos < _quarks.size()){
    _quarks.replace(pos, 4, "sqrt");
  }
}

void ParticleProperties::antiQuarkContent(){
  _netQuarkContent.antiThis();
}

void ParticleProperties::antiCharge(){
  replace(_charge.begin(), _charge.end(), '+', 'f');
  replace(_charge.begin(), _charge.end(), '-', '+');
  replace(_charge.begin(), _charge.end(), 'f', '-');
}
bool ParticleProperties::hasDistinctAnti() const{
  return ! (_Aformat == ' ');
}
bool ParticleProperties::antiThis(){
  if(! hasDistinctAnti()) return false;
  antiCharge();
  antiQuarks();
  antiQuarkContent();
  _pdg_id *= -1;
  return true;
}

ParticleProperties ParticleProperties::anti() const{
  ParticleProperties PP(*this);
  PP.antiThis();
  return PP;
}

std::string ParticleProperties::name() const{
  std::string fullName = _name;
  if(_pdg_id < 0 && _Aformat == 'F') fullName += "bar";
  fullName += _charge;
  return fullName;
}

double ParticleProperties::lifetime() const{
  if(width()<0) return -9999.0;
  if(width() == 0) return 9.9999e109;
  return hbar_Planck/width();
}
double  ParticleProperties::lifetime_in_ns() const{
  return lifetime()/ns;
}
double  ParticleProperties::lifetime_in_ps() const{
  return lifetime()/picosecond;
}
double ParticleProperties::ctau() const{
  return lifetime()*c_light;
}
double ParticleProperties::ctau_in_microns() const{
  return ctau()/micrometer;
}

double ParticleProperties::radius() const{
  return _Radius;
  // not set in mass_width.csv, but in normal options file
  // has default: 5/GeV for D0, 1.5/GeV otherwise
  // Set in your usual options file like this (units: inverse GeV)
  // <pdg_id>_Radius_invGeV  <number in inverse GeV>
  // for example:
  // 421_Radius_invGeV 1.24
  //

}

std::string ParticleProperties::ScalarVectorPseudoAxialVectorTensor() const{

  if(J()=="0" && P() == -1) return "P";
  else if(J()=="1" && P() == -1) return "V";
  else if(J()=="0" && P() ==  1) return "S";
  else if(J()=="1" && P() ==  1) return "A";
  else if(J()=="2" && P() ==  1) return "T";
  else if(J()=="2" && P() == -1) return "PT";
  else return "?";
}

bool ParticleProperties::isNonResonant() const{
return (   9981 == abs(pdg_id()) 
	|| 9991 == abs(pdg_id())
	|| 9983 == abs(pdg_id())
	|| 9993 == abs(pdg_id())
	|| 9985 == abs(pdg_id())
	|| 9975 == abs(pdg_id())
	);
}

std::ostream& operator<<(std::ostream& out, const ParticleProperties& pp){
  pp.print(out);
  return out;
}


//
