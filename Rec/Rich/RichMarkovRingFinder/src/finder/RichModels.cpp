
#include "RichModels.h"
#include <string>

namespace Lester {
  
  //const std::string thetaFileDeJour = "/usera/lester/proj/c++/rich/DATA/rich2_thetaDist_20030812a.txt";
  //const std::string thetaFileDeJour = "/data/Rich2_MarkovRingFinder_thetaDist.txt" ;
  const std::string thetaDirDeJour = getenv("PARAMFILESROOT");
  const std::string thetaFileR2DeJour = "/data/Rich2_MarkovRingFinder_thetaDist.txt";

  //const std::string thetaFullFileR2DeJour = thetaDirDeJour + thetaFileR2DeJour;
  //const Rich2AThetaSampler rich2AThetaSampler(thetaFullFileR2DeJour);
  //const Rich2AThetaDistribtion rich2AThetaDistribution(thetaFullFileR2DeJour);
  //const Rich2AProbabilityThetaAbove rich2AProbabilityThetaAbove(thetaFullFileR2DeJour);  

  const Rich2AThetaSampler rich2AThetaSampler(thetaFileR2DeJour,thetaDirDeJour);
  const Rich2AThetaDistribtion rich2AThetaDistribution(thetaFileR2DeJour,thetaDirDeJour);
  const Rich2AProbabilityThetaAbove rich2AProbabilityThetaAbove(thetaFileR2DeJour,thetaDirDeJour);

};
