
#include "Event/MCFun.h"

double LHCb::MC::zInteraction(const LHCb::MCParticle* aParticle) {

  double z = 9999.*Gaudi::Units::m;

  const SmartRefVector<LHCb::MCVertex>& vertices = aParticle->endVertices();
  SmartRefVector<LHCb::MCVertex>::const_iterator iterVert = vertices.begin();
  for (; iterVert < vertices.end(); ++iterVert){
    const double zTest = (*iterVert)->position().z();
    if ((zTest < z)&&(realInteraction((*iterVert)->type()))){
      z = zTest;
    } // if
  } // iterVert

  return z;
}

bool LHCb::MC::realInteraction(const LHCb::MCVertex::MCVertexType& type){
  
  bool real = true;
  if ((type == LHCb::MCVertex::PhotoElectric)||
      (type == LHCb::MCVertex::RICHPhotoElectric) ||
      (type == LHCb::MCVertex::Cerenkov) ||
      (type == LHCb::MCVertex::DeltaRay)) {
   real = false;
  }
  return real;
}
