#include "GaussRICH/RichPEInfo.h"

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
 m_Mirror2PhotDetCopyNum(0),
 m_QW2PCreflFlagSave(0),
 m_BackscatteredPeFlag(0),
 m_PhotoElectricFlag(0),
 m_HpdPhotonReflectionFlag(0),
 m_HpdQWExtPhotIncidentPosition(0),
 m_HpdPeLocalOriginPosition(0),
 m_PhotonSourceInformation(0)
{; }

RichPEInfo::~RichPEInfo() {;}

