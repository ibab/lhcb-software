
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "Mint/Random3.h"
#include "Mint/Mint/Utils.h"

using namespace std;

std::string doNumber(std::string line, double f, string opt="relative"){

  int n = line.find_first_of(" \t");
  n = line.find_first_of("012", n);
  n = line.find_first_not_of("012", n);
  n = line.find_first_of(".0123456789+-", n);
  int n2 = line.find_first_not_of(".0123456789+-", n);
  
  string nstring = line.substr(n, n2-n);
  
  double val = atof(nstring.c_str());
  double sigma;
  if("relative" == opt) sigma=val*f;
  else sigma = f;

  val += gRandom->Gaus(0, sigma);
  
  line.replace(n, n2-n, MINT::anythingToString(val));

  return line;
}

int main(int argc, char *argv[]){
  int seed = time(0);
  if(argc >= 2){
    seed = atoi(argv[1]);
  }
  cout << "** randomised file with seed = " << seed << endl;
  string line;
  TRandom3 rnd;
  gRandom->SetSeed(seed);

  while(getline(cin, line)){
    //cout << line << endl;
    if(string::npos != line.find("_Amp")){
      cout << doNumber(line, 0.2, "relative") << endl;
    }else if(string::npos != line.find("_Phase")){
      cout << doNumber(line, 0.25, "absolute") << endl;
    }else if(string::npos != line.find("_Frac")){
      ;
    }else{
      cout << line << endl;
    }
  }
}
