// author: Philippe d'Argent (p.dargent@cern.ch) & Jonas Rademacker
#include "Mint/ResonancePropertiesFitRef.h"
#include "Mint/ResonanceProperties.h"

using namespace std;
using namespace MINT;

ResonancePropertiesFitRef::ResonancePropertiesFitRef(const ResonanceProperties& repo
						     , IFitParRegister* daddy)
  : FitParDependent(daddy)
  , _fitMass(repo._fitMass, this)
  , _fitWidth(repo._fitWidth, this)
  , _fitRadius(repo._fitRadius, this)
  , _fitAlpha(repo._fitAlpha, this)
  , _fitFermiLambda(repo._fitFermiLambda, this)
  , _fitFermiS0(repo._fitFermiS0, this)
{
}

//



