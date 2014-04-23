// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include <iostream>
#include <string>

#include "Mint/NamedParameter.h"

/*
  compile: make
  run: 
  $ testNamed < testNamed.txt

*/

using namespace std;
using namespace MINT;

int testNamed(){

  NamedParameterBase::setDefaultInputFile("testNamed.txt");

  // try a few named data types:
  NamedParameter<double> dbl("dbl");
  NamedParameter<int>    integ("integ");
  NamedParameter<string> strg("strg");
  NamedParameter<char>   chr("that's a complicated name for a char!");

  // arrays are declared in the same way.
  // it figures out when reading the file
  // that it's an array (from the number
  // of vallues given).
  NamedParameter<double> dblArray("dblArray");
  NamedParameter<int>    integArray("integ-Array");
  NamedParameter<string> strgArray("string Array");
  NamedParameter<char>   chrArray("charArray");

  NamedParameter<string> strgFile("string from file"
				  , "OtherParameterFile.txt");

  NamedParameter<char>   unInitialised("This is what happens if "\
				       "a parameter does not get "\
				       "initialised.");

  NamedParameter<string> initFromStringStream("initialisedFromStringStream");
  stringstream strm("init value from stream");
  strm >> initFromStringStream ;
  
  NamedParameter<int> initConInt("init in constr to -3", -3);

  // note: If you don't have the (std::string)
  // in the 2nd argument below, it will
  // assume that the 2nd argument is a filename.
  NamedParameter<string> initConStr("also initialised in constr"
				 ,(std::string) "init in cr string" );


  cout << "here are the Named Parameters" << endl;

  cout << initFromStringStream
       << "\n" << initConInt 
       << "\n" << initConStr
       << endl;
  cout << dbl << '\n' << integ << '\n' 
       << strg << '\n' << chr << endl;

  cout << dblArray << '\n' << integArray 
       << '\n' << strgArray << '\n' << chrArray << endl;

  cout << strgFile << endl;
  return 0;
}

int main(){

  return testNamed();
}
