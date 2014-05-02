// author: Nunya Business (ya.mum@bristol.ac.uk)
//         The bunny (claire.prouve@bristol.ac.uk)
// status:  Mon 19 Dec 2013 19:17:59 GMT

#include <iostream>
#include "TRandom3.h"
#include "Mint/DalitzMCMC.h"

DalitzMCMC::DalitzMCMC( const DalitzEventPattern& pat, const unsigned int& seed, const bool makeCPconj)
  : _pat(pat)
  , _nbody(_pat.numDaughters())
  , _myOwnPSet()
  , _pdf(_pat, &_myOwnPSet)
{
  gRandom->SetSeed(seed);
  initialise(makeCPconj);
}

DalitzMCMC::DalitzMCMC(const DalitzEventPattern& pat , FitAmpSum& other, const unsigned int& seed, const bool makeCPconj)
  : _pat(pat)
  , _nbody(_pat.numDaughters())
  , _myOwnPSet(*other.getMinuitParameterSet())
  , _pdf(other)
{
  gRandom->SetSeed(seed);
  initialise(makeCPconj);
}


bool DalitzMCMC::initialise(bool makeCPconj){
  if(makeCPconj) MakeCPConjugate();

  _P.SetXYZM( 0.0, 0.0, 0.0, _pat[0].mass() );

  double masses[_nbody];
  for( unsigned int i=0; i<_nbody; i++ )
    masses[i] = _pat[i+1].mass();

  _event.SetDecay(_P, _nbody, masses);

  std::cout << "The DalitzMCMC has undergone some sweet sweet initialisation!" << std::endl;
  return true;
}



void DalitzMCMC::MakeCPConjugate(){
  _pdf.CPConjugateSameFitParameters();
}


// The rejectionFactor is the number of events from which ONE is chosen and put into the eventlist, i.e. rejectionFactor = 1000 means that a random number between 0 and 1000 is uniformly generared and if this number is < 1, the event is saved in the eventList
void DalitzMCMC::FillEventList( DalitzEventList& evtList, const unsigned int& NEvents, double rejectionFactor )
{
  //Markov Chain
  double ratio;

  //Start event
  DalitzEvent candEvent_start = GetFlatEvent();

  //Next event
  DalitzEvent candEvent_next;

  //sortvar is used in combination with the rejectionFactor to reject events already during the generation process
  TRandom3* sort = new TRandom3();
  double sortvar;
  bool printi = true;
  std::cout << "Generated events:" <<std::endl;
  for( unsigned int i=0; i<NEvents; i++ ){
    if(i%5000 == 0 && printi){
      std::cout << "    " << i << std::endl;
      printi = false;
    }

    sortvar = sort->Uniform(rejectionFactor);
    candEvent_next = GetFlatEvent();
    const double pdf_start = _pdf.Prob(candEvent_start);

    const double pdf_next = _pdf.Prob(candEvent_next);
    ratio = pdf_next/pdf_start;

    if(ratio != ratio){
      std::cout << "The Signal PDF was found to be zero (at least at on point in Dalitz-Space). If you are sure that your PDF should be (mainly) non-zero contact: J.Dalseno@bristol.ac.uk, Claire.Prouve@bristol.ac.uk or Jonas.Rademacker@bristol.ac.uk" << std::endl << std::endl;
	exit(1);
    }
    if( ratio >= 1.0 ){
      candEvent_start = candEvent_next;
      if(sortvar<1){
	printi = true;
	evtList.Add(candEvent_next);
      }else{
	i--;
      }
    }else{
      const double height = gRandom->Rndm();
      if( height < ratio ){
	candEvent_start = candEvent_next;
	if(sortvar<1){
	  evtList.Add(candEvent_next);
	  printi = true;
	}else{
	  i--;
	}
      }else{
	if(sortvar <1){
	  printi = true;
	  evtList.Add(candEvent_start);
	}else{
	  i--;
	}
      }
    }
  }
}



DalitzEvent DalitzMCMC::GetFlatEvent()
{
  std::vector<TLorentzVector> p4_mumAndDgtr(_nbody+1);

  //Fill mother momentum
  p4_mumAndDgtr[0] = _P;

  //Fill daughter momenta
  while(true){
    const double weight = _event.Generate();
    const double height = gRandom->Uniform(1.0/weight);

    if( height < 1.0 ){
      for( unsigned int i=0; i<_nbody; i++ )
	p4_mumAndDgtr[i+1] = *(_event.GetDecay(i));

      break;
    }
  }

  const DalitzEvent flatEvent(_pat, p4_mumAndDgtr);
  return flatEvent;
}

//
