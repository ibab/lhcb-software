#ifndef MINT_EVTTRANDOM_HH 
#define MINT_EVTTRANDOM_HH 1

// Include files
#ifndef ROOT_TRandom
#include "TRandom.h"
#endif

/** @class EvtTRandom EvtTRandom.h Mint/EvtTRandom.h
 *  
 *
 *  @author Philip Hunt (LHCB)
 *  @date   2013-09-20
 */
namespace MINT {
  class IEvtRandom
  {
  public:
    virtual void SetSeed(UInt_t seed)=0;
    virtual UInt_t GetSeed() const=0;
    virtual double Rndm(Int_t i)=0;
    virtual ~IEvtRandom(){};
  };
    
  class EvtTRandom : public TRandom {
  public:
    EvtTRandom(MINT::IEvtRandom* rnd=NULL); 
    virtual ~EvtTRandom( ); ///< Destructor
    virtual UInt_t GetSeed() const;
    virtual Double_t Rndm(Int_t i=0);
    virtual void RndmArray(Int_t n, Float_t* array);
    virtual void RndmArray(Int_t n, Double_t* array);
    virtual void SetSeed(UInt_t seed=0);   
    enum {
      kBigNumber=1234567890
    };
  private:
    MINT::IEvtRandom* m_rnd;
  };
}

#endif // MINT_EVTTRANDOM_HH
