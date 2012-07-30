// $Id: $
#ifndef FTRAWBANKPARAMS_H 
#define FTRAWBANKPARAMS_H 1

namespace FTRawBank {
  enum shifts {
    sipmShift     = 4,  // for the header: SiPM Number + number of clusters
    fractionShift = 0,
    cellShift     = 2,
    sizeShift     = 9,
    chargeShift   = 12 
  };
  
  enum maxima {
    nbClusMaximum   = 15,
    fractionMaximum = 3,
    cellMaximum     = 127,
    sizeMaximum     = 7,
    chargeMaximum   = 15 
  };
};


#endif // FTRAWBANKPARAMS_H
