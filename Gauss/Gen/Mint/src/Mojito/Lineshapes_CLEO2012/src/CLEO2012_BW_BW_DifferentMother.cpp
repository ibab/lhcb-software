// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/CLEO2012_BW_BW_DifferentMother.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include <iostream>

using namespace std;

const ParticleProperties* CLEO2012_BW_BW_DifferentMother::mumsProperties() const{
  const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  if(0 == PPL){
    cout << "ERROR in CLEO2012_BW_BW_DifferentMother::mumsProperties():"
	 << " Couldn't get ParticlePropertiesList"
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ParticlePropertiesList";
  }
  const ParticleProperties* props = PPL->get(_alternativeMumPDG);
  if(0 == props){
    cout << "ERROR in CLEO2012_BW_BW_DifferentMother::mumsProperties():"
	 << " Got ParticlePropertiesList, but couldn't find "
	 << _alternativeMumPDG
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ParticleProperties";
  }    
  return props;
}

CLEO2012_BW_BW_DifferentMother::CLEO2012_BW_BW_DifferentMother(const AssociatedDecayTree& decay
					     , IDalitzEventAccess* events
					     , int newMumID
					     )
  : CLEO2012_BW_BW(decay, events)
  , _alternativeMumPDG(newMumID)
{}

//
