#include "Mint/NamedParameter.h"
#include <iostream>
#include <cmath>

using namespace MINT;
using namespace std;

int main(){
  NamedParameter<double> E("E"), p("p");
  cout << E << ", " << p << ", m = " << sqrt(E*E - p*p) << endl;
  return 0;
}
