#ifndef taggingutils_H 
#define taggingutils_H 1

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <Riostream.h>
#include "TString.h"
#include "TLorentzVector.h"
#include <map>

//======================================
void PrintAdvance(int n, float nmax, TString& filename);

//======================================
bool wait() ;

//======================================
ostream& debug() ;
ostream& verbose();
ostream& info()    ;
ostream& warning() ;
ostream& err()     ;
ostream& fatal()   ;

typedef std::map<const TString, const double > propertyMap;
typedef std::map<const TString, const TString > propertyStringMap;

//functions////////////////////////////////////////////////////////////////
bool isinTextFile(int run, int evt, TString& s);
bool isinTextFile(int run, int evt);

int getwordnr(TString line, TString tok=" ,;:");
TString getword(int i, TString line, TString tok=" ,;:");
TString getlastword(TString word);
TString readString(TString varname,TString optsfilename="tag.opts");
double  read(TString varname) ;
double  read(const char* varname  ) ;

TLorentzVector build4V(double p, double pt, double phi, double ID);

double dPhi(double f1, double f2);
double dQ(const TLorentzVector& BSpart, const TLorentzVector& ipart);
bool isNonResonantD(float id) ;
bool isD(int id) ;

//=====================================================================
std::vector<TString> getFiles(string dir, const char* extension=".root") ;

void declareProperty( const TString& variablename, double& value) ;

void declareProperty( const TString& variablename, int& value );

void declareProperty( const TString& variablename, TString& value);

void declareProperty( const TString& variablename, bool& value);

#endif
