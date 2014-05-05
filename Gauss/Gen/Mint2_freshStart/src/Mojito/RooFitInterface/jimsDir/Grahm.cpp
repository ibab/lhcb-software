// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT

#include "Grahm.h"

#include <iostream>
#include <vector>


#include "TMatrixD.h"
#include "TLorentzVector.h"


using namespace std;
double grahm(int howMany, ...){
  bool printArgs=false;

  va_list arg_addr;
  TLorentzVector nextVector;
  
  va_start(arg_addr, howMany);

  vector<TLorentzVector*> upperArgs;
  for(int i=0; i < howMany; i++){
    
    TLorentzVector* nextVector = va_arg(arg_addr, TLorentzVector*);
    upperArgs.push_back(nextVector);
    
  }
  vector<TLorentzVector*> lowerArgs;
  for(int i=howMany; i < 2*howMany; i++){
    
    TLorentzVector* nextVector = va_arg(arg_addr, TLorentzVector*);
    lowerArgs.push_back(nextVector);
    
  }

  if(printArgs){
    cout << " G( ";
    for(vector<TLorentzVector*>::iterator it = upperArgs.begin();
	it != upperArgs.end(); it++){
      cout << "[" 
	   << (*it)->T() << ", " 
	   << (*it)->X() << ", " 
	   << (*it)->Y() << ", " 
	   << (*it)->Z() 
	   << "], ";
    }
    cout << endl;
    for(vector<TLorentzVector*>::iterator it = lowerArgs.begin();
	it != lowerArgs.end(); it++){
      cout << "[" 
	   << (*it)->T() << ", " 
	   << (*it)->X() << ", " 
	   << (*it)->Y() << ", " 
	   << (*it)->Z() 
	   << "], ";
    }
    cout << endl;
  }

  TMatrixD M(howMany, howMany);
  for(int i=0; i< howMany; i++){
    for(int j=0; j<howMany; j++){
      M(i, j) = upperArgs[i]->Dot(*(lowerArgs[j]));
    }
  }

  return M.Determinant();

}

double symGrahm(int howMany, ...){
  va_list arg_addr;
  TLorentzVector nextVector;
  
  va_start(arg_addr, howMany);

  vector<TLorentzVector*> Args;
  for(int i=0; i < howMany; i++){    
    TLorentzVector* nextVector = va_arg(arg_addr, TLorentzVector*);
    Args.push_back(nextVector);    
  }

  TMatrixD M(howMany, howMany);
  for(int i=0; i< howMany; i++){
    for(int j=0; j<howMany; j++){
      M(i, j) = Args[i]->Dot(*(Args[j]));
    }
  }

  return M.Determinant();

}


//
