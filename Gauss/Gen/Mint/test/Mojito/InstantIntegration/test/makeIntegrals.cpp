
#include "Mint/FitAmpSum.h"
#include "Mint/FitAmpPairList.h"
#include "Mint/NamedParameter.h"

#include "Mint/MakePreIntegratedAmplitudes.h"
#include "Mint/DiskResidentEventList.h"

#include <iostream>
#include <string>

using namespace std;
using namespace MINT;

int main(){
  NamedParameter<double> IntegPrecision("IntegPrecision", 1.e-3);
  NamedParameter<int> EventPattern("Event Pattern", 421, 211, 310, -211);
  NamedParameter<string> IntegratorFileName("IntegratorFileName"
					    , (string) "");
  NamedParameter<string> GenerateYourOwn("GenerateEventsIfNeeded"
					 , (string) "DoGenerate"
					 );
  DalitzEventPattern pat(EventPattern.getVector());

  DiskResidentEventList list((string) IntegratorFileName);
  DalitzHistoSet histos(list.histoSet());
  histos.save("IntegrationHistos.root");
  histos.draw("IntegrationHistos_");

  MakePreIntegratedAmplitudes makem(pat, "NoBgSpinZero"
				    , (double) IntegPrecision
				    , (string)IntegratorFileName
				    , (string) GenerateYourOwn
				    );
  makem.doit();
  return 0;
}
int saveAndRetrieve(){
  
  NamedParameter<int> EventPattern("Event Pattern", 421, 321, -312, -211, 211);
  DalitzEventPattern pdg(EventPattern.getVector());

  FitAmpSum ampSum(pdg);

  FitAmpPairList fapList;
  for(unsigned int i=0; i < ampSum.size(); i++){
    if(ampSum.getAmpPtr(i)->canBeIgnored()) continue;
    for(unsigned int j=i; j < ampSum.size(); j++){
      if(ampSum.getAmpPtr(j)->canBeIgnored()) continue;
      fapList.addAmps( (ampSum.getAmpPtr(i)),  (ampSum.getAmpPtr(j)));
      cout << "name 1 : " << ampSum.getAmpPtr(i)->name()
	   << "\nname2 : " << ampSum.getAmpPtr(j)->name()
	   << "\n both  : " << fapList.back().name()
	   << "\n dirName : " << fapList.back().dirName()
	   << endl;
      fapList.back().save("testDir");
    }
  }

  fapList.save("fullListSaved");

  fapList.retrieve("fullListSaved");
  fapList.save("fullListSavedAgain");

  return 0;
}
