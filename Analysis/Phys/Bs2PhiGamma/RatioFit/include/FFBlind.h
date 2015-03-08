#ifndef FFBLIND_H
#define FFBLIND_H

#include "TMath.h"
#include "TString.h"

/**
    @brief Copy of the RooFit blinding algorithm (RooBlindTools)
*/
class FFBlind
{
    enum Method {
        Gaussian = 1,
        Uniform = 2
    };
public:
    FFBlind(const char *StringAlphabet, Double_t centralValue, Double_t scale); ///< Gaussian
    FFBlind(const char *StringAlphabet, Double_t scale); ///< Uniform
    double hide(const double input) const;
    double unHide(const double input) const;

private:
    Double_t MakeGaussianOffset(const char *StringAlphabet) const;
    Double_t MakeOffset(const char *StringAlphabet) const;
    Double_t MakeSignFlip(const char *StringAlphabet) const;
    Double_t Randomizer(const char *StringAlphabet) const;

    Method _method;
    TString _stSeed;
    Double_t _PrecisionCentralValue;
    Double_t _PrecisionOffset;
    Double_t _PrecisionOffsetScale;
    Double_t _PrecisionSignFlip;
    Double_t _PrecisionUniform;
};

#endif // FFBLIND_H
