#ifndef FTRAWBANKPARAMS_H 
#define FTRAWBANKPARAMS_H 1

namespace FTRawBank {
  enum shifts {
    sipmShift     = 4,  // for the header: SiPM Number + number of clusters
    fractionShift = 0,
    cellShift     = 2,
    sipmIdShift   = 9,
    sizeShift     = 11,
    chargeShift   = 13
  };
  
  enum maxima {
    nbClusMaximum   = 15,  // 4 bits ??
    fractionMaximum = 3,   // 2 bits allocted
    cellMaximum     = 127, // 0 to 127, coded on 7 bits
    sipmIdMaximum   = 3,   // 2 bits allocated
    sizeMaximum     = 3,   // 2 bits allocated
    chargeMaximum   = 7    // 3 bits allocated
  };

  enum BankProperties {
    NbBanks = 48,
    NbSiPMPerTELL40 = 128
  };

}

#endif // FTRAWBANKPARAMS_H
