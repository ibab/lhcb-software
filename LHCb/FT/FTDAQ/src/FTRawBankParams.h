// $Id: $
#ifndef FTRAWBANKPARAMS_H 
#define FTRAWBANKPARAMS_H 1

namespace FTRawBank {
  enum shifts {
    sipmShift     = 4,
    fractionShift = 0,
    cellShift     = 3,
    sizeShift     = 10,
    chargeShift   = 13 
  };
  
  enum maxima {
    nbClusMaximum   = 15,
    fractionMaximum = 7,
    cellMaximum     = 127,
    sizeMaximum     = 7,
    chargeMaximum   = 7 
  };
};


#endif // FTRAWBANKPARAMS_H
