#include "FFBlind.h"

/**
    @class FFBlind
    @brief Blinding class emulating RooBlindTools (RooFit)

    Example:
    @code
        FFBlind blindEngine("The Blinding String", 2); //for Uniform
        //FFBlind blindEngine("The Blinding String", -0.1, 4); //for Gaussian
        double param_unblind = 2.3;
        double param = blindEngine.Hide(param_unblind);
        cout << "Blinded parameter: " << param << endl;
    @endcode
*/

/**
    @brief Constructor for Gaussian blinding (RooUnblindPrecision equivalent in RooFit)
*/
FFBlind::FFBlind(const char *StringAlphabet, Double_t centralValue, Double_t scale)
{
    _method = Gaussian;
    _stSeed = StringAlphabet;
    _PrecisionCentralValue = centralValue;
    _PrecisionOffsetScale = scale;

    _PrecisionSignFlip = MakeSignFlip("klmnopqrstuvwxyzabcdefghij");
    _PrecisionOffset = _PrecisionOffsetScale * MakeGaussianOffset("opqrstuvwxyzabcdefghijklmn");
}

/**
    @brief Constructor for uniform blinding (RooUnblindUniform equivalent in RooFit)
*/
FFBlind::FFBlind(const char *StringAlphabet, Double_t scale)
{
    _method = Uniform;
    _stSeed = StringAlphabet;
    _PrecisionOffsetScale = scale;

    _PrecisionSignFlip = MakeSignFlip("klmnopqrstuvwxyzabcdefghij");
    _PrecisionUniform = _PrecisionOffsetScale * MakeOffset("jihgfedcbazyxwvutsrqponmlk");
}

Double_t FFBlind::MakeGaussianOffset(const char *StringAlphabet) const
{
  Double_t theRan1 = Randomizer(StringAlphabet);
  Double_t theRan2 = Randomizer("cdefghijklmnopqrstuvwxyzab");

  if (theRan1==0.0 || theRan1==1.0){
    theRan1 = 0.5;
  }
  if (theRan2==0.0 || theRan2==1.0){
    theRan2 = 0.5;
  }

  Double_t theOffset = sin(2.0*3.14159*theRan1)*sqrt(-2.0*log(theRan2));

  return theOffset;   //theOffset is Gaussian with mean 0, sigma 1
}

Double_t FFBlind::MakeOffset(const char *StringAlphabet) const
{
  Double_t theRan = Randomizer(StringAlphabet);

  Double_t theOffset = (2.0)*theRan - (1.0);

  return theOffset;   //theOffset lies between -1.0 and 1.0
}

Double_t FFBlind::MakeSignFlip(const char *StringAlphabet) const
{
  Double_t theRan = Randomizer(StringAlphabet);

  Double_t theSignFlip = 1.0;
  if (theRan>0.5){
    theSignFlip = 1.0;
  } else {
    theSignFlip = -1.0;
  }

  return theSignFlip;  //theSignFlip is = +1 or -1
}

Double_t FFBlind::Randomizer(const char *StringAlphabet) const
{
  char lowerseed[1024];
  strlcpy(lowerseed,_stSeed,1024);

  Int_t lengthSeed = strlen(lowerseed);

  for (Int_t j=0; j<lengthSeed; j++){
    lowerseed[j] = tolower(_stSeed[j]);
  }
  Int_t sumSeed = 0;
  for (Int_t i=0; i<lengthSeed; i++){
    for (Int_t iAlphabet=0; iAlphabet<26; iAlphabet++){
      if ( lowerseed[i] == StringAlphabet[iAlphabet] ){
        sumSeed = sumSeed + iAlphabet ;
      }
    }
  }

  Int_t ia = 8121;
  Int_t ic = 28411;
  Int_t im = 134456;
  UInt_t jRan = (sumSeed*ia + ic) % im;

  jRan = (jRan*ia + ic) % im;
  jRan = (jRan*ia + ic) % im;
  jRan = (jRan*ia + ic) % im;

  Double_t theRan = (float) jRan / (float) im;

  return theRan;    //theRan is between 0.0 - 1.0
}

/**
 *  @brief Returns the blinded value
 *  @param input The unblinded value
 */
double FFBlind::hide(const double input) const
{
    switch (_method)
    {
    case Gaussian:
        if(_PrecisionSignFlip > 0)
        {
            return input + _PrecisionOffset;
        }
        else
        {
            return 2.0 * _PrecisionCentralValue - input + _PrecisionOffset;
        }
    case Uniform:
        return input + _PrecisionUniform;
    }
    return 0;
}

/**
 *  @brief Returns the unblinded value
 *  @param input The blinded value
 */
double FFBlind::unHide(const double input) const
{
    switch (_method)
    {
    case Gaussian:
        if(_PrecisionSignFlip > 0)
        {
            return input - _PrecisionOffset;
        }
        else
        {
            return 2.0 * _PrecisionCentralValue - input + _PrecisionOffset;
        }
    case Uniform:
        return input - _PrecisionUniform;
    }
    return 0;
}
