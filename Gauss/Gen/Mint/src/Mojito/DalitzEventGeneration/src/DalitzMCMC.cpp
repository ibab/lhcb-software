// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 19 Dec 2013 19:17:59 GMT

#include <iostream>

#include "Mint/DalitzMCMC.h"

DalitzMCMC::DalitzMCMC( const DalitzEventPattern& pat,
			const unsigned int& seed )
  : _pat(pat)
  , _nbody(_pat.numDaughters())
  , _myOwnPSet()
  , _pdf(_pat, &_myOwnPSet)
{
  //Setup TGenPhaseSpace
  _P.SetXYZM( 0.0, 0.0, 0.0, _pat[0].mass() );

  double masses[_nbody];
  for( unsigned int i=0; i<_nbody; i++ )
    masses[i] = _pat[i+1].mass();

  gRandom->SetSeed(seed);
  _event.SetDecay(_P, _nbody, masses);
}

void DalitzMCMC::FillEventList( DalitzEventList& evtList,
				const unsigned int& NEvents )
{
  //Markov Chain
  double ratio;

  //Start event
  DalitzEvent candEvent_start = GetFlatEvent();

  //Next event
  DalitzEvent candEvent_next;

  for( unsigned int i=0; i<NEvents; i++ ){
    candEvent_next = GetFlatEvent();

    _pdf.setEvent(&candEvent_start);
    const double pdf_start = _pdf.Prob();

    _pdf.setEvent(&candEvent_next);
    const double pdf_next = _pdf.Prob();

    ratio = pdf_next/pdf_start;
    if( ratio >= 1.0 ){
      candEvent_start = candEvent_next;
      evtList.Add(candEvent_next);
    }else{
      const double height = gRandom->Rndm();
      if( height < ratio ){
	candEvent_start = candEvent_next;
	evtList.Add(candEvent_next);
      }else{
	evtList.Add(candEvent_start);
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
