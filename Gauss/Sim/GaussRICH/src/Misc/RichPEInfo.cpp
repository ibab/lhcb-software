#include "RichPEInfo.h"

RichPEInfo::RichPEInfo() :
 m_PhotOriginRadiatorNumber(0),
 m_MotherOfPhotonPDGcode(0),
 m_PhotonEmisPoint(0),
 m_MotherOfPhotonId(0),
 m_PhotonEnergyAtCkvProd(0), 
 m_CherenkovThetaAtProd(0),
 m_CherenkovPhiAtProd(0),
 m_MotherofPhotonMomAtProd(0),
 m_MotherofPhotonMomVectAtProd(0),
 m_MotherofPhotonPDGMass(0),
 m_MotherofPhotonCkvPreStep(0),
 m_MotherofPhotonCkvPostStep(0),
 m_PhotonRayleighScatteringFlag(0), 
 m_PhotonAerogelExitPos(0),
 m_VerbosePeTagFlag(false),
 m_OptPhotonId(0),
 m_Mirror1PhotReflPosition(0),
 m_Mirror2PhotReflPosition(0),
 m_Mirror1PhotDetCopyNum(0),
 m_Mirror2PhotDetCopyNum(0){; }

RichPEInfo::~RichPEInfo() {;}

