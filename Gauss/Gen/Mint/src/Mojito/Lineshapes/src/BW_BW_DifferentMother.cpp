// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/BW_BW_DifferentMother.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <iostream>

using namespace std;

const ParticleProperties* BW_BW_DifferentMother::mumsProperties() const{
  const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  if(0 == PPL){
    cout << "ERROR in BW_BW_DifferentMother::mumsProperties():"
	 << " Couldn't get ParticlePropertiesList"
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ParticlePropertiesList";
  }
  const ParticleProperties* props = PPL->get(_alternativeMumPDG);
  if(0 == props){
    cout << "ERROR in BW_BW_DifferentMother::mumsProperties():"
	 << " Got ParticlePropertiesList, but couldn't find "
	 << _alternativeMumPDG
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ParticleProperties";
  }    
  return props;
}

BW_BW_DifferentMother::BW_BW_DifferentMother(const AssociatedDecayTree& decay
					     , int newMumID
					     )
  : BW_BW(decay)
  , _alternativeMumPDG(newMumID)
{}

//
