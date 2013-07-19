#include "GaussRICH/RichPhotInfo.h"

RichPhotInfo::RichPhotInfo() : 
  m_MotherChTrackPDGcode(0),
  m_CkvPhotonEnergyAtProd(0),
  m_CkvAngleThetaAtProd(0),
  m_CkvAnglePhiAtProd(0),  
  m_ChTrackMomAtProd(0) ,
  m_ChTrackMomentumVector(0),
  m_MotherChTrackPDGMass(0),
  m_ChTrackCkvPrestep(0), 
  m_ChTrackCkvPoststep(0),
  m_PhotonRayleighScatFlag(0), 
  m_AerogelExitPosition(0),
  m_Mirror1ReflPosition(0),
  m_Mirror2ReflPosition(0),
  m_Mirror1DetCopyNum(-1),
  m_Mirror2DetCopyNum(-1),
  m_VerbosePhotTagFlag(false),
  m_PhotProdRadiatorNum(-1),
  m_QW2PCreflFlag(0), //added by RWL 9.11.06
  m_QW2PCreflFlagSave(0),
  m_PhotonHpdReflectionFlag(0),
  m_HpdQWPhotIncidentPosition(0),
  m_PmtLensPhotIncidentPosition(0),
  m_PhotonSourceInfo(0)
   {; }

RichPhotInfo::~RichPhotInfo() {;}

