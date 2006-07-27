
#include "Utils/UpperGaussianDist.h"
#include <iostream>
#include "CLHEP/Random/RandFlat.h"

int main() {
  const UpperGaussianDist ugd(3,2,-1);
  unsigned long n=0;
  double sigW;
  while(true) {
    const double x = RandFlat::shoot(-10.,20.);
    const double w = ugd.probabilityOf(x);
    ++n;
    sigW+=w;
    // The first number should tend to one.
    std::cout << (30.*sigW/n) << " " << ugd.inverseTruncatedArea(3,2,-1) << std::endl;
  };
};
