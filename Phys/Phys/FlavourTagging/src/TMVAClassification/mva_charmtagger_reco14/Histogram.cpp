// Include files 
#include <iostream>
#include <cmath>

// local
#include "Histogram.h"

//-----------------------------------------------------------------------------
// Implementation file for class : public Histogram
//
// 2014-10-08 : Jack Timpson Wimberley
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Histogram::Histogram(int N, double a, double b) {
  numBins = N;
  min = a;
  max = b;
  binLefts = new double[numBins+1];
  vals = new double[numBins];
  for (int k = 0; k <= numBins; k++) {
    binLefts[k] = min + k*(max-min)/numBins;
    if (k != N)
      vals[k] = 0.0;
  }
}

//=============================================================================
// Destructor
//=============================================================================
Histogram::~Histogram() {
  delete binLefts;
  delete vals;
}

//=============================================================================

void Histogram::SetBinContent(int bin, double value) {
  if (bin >= 1 and bin <= numBins)
    vals[bin-1] = value;
  else
    std::cerr << "Invalid bin" << std::endl;
}

int Histogram::FindBin(double x) {
  if (x < min or x > max) {
    return -1;
  } else {
    // should be min + k*(max-min)/numBins <= x <= min + (k+1)*(max-min)/numBins
    // k <= (x-min)/(max-min) * numBins <= k+1
    double expected = (x-min)/(max-min) * numBins;
    int lower = static_cast<int>(floor(expected));
    if (binLefts[lower] <= x and binLefts[lower+1] >= x) {
      return lower + 1;
    } else {
      std::cerr << "ERROR finding bin" << std::endl;
      return -1;
    }
  }
}

double Histogram::GetBinContent(int bin) {
  if (bin >= 1 and bin <= numBins) {
    return vals[bin-1];
  } else {
    std::cerr << "Invalid bin" << std::endl;
    return 0.0;
  }
}

double Histogram::GetPurityAtBDT(double x) {
  return GetBinContent(FindBin(x));
}

int Histogram::NumBins() {
  return numBins;
}
