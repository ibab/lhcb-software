#ifndef MUONALGS_MUONCHAMBERRESPONSE_H
#define MUONALGS_MUONCHAMBERRESPONSE_H 1
#include <cmath>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "GaudiKernel/RndmGenerators.h"
#include "AIDA/IHistogram1D.h"
#include "CLHEP/Units/PhysicalConstants.h"
 
/// Detector description classes
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/DetectorElementException.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/SolidBox.h"  

#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonUtils/MuonGeometryStore.h" 
#include "MuonKernel/MuonTile.h"
#include "MuonAlgs/MuonPhChID.h"    
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/DeMuonGasGap.h"
 
#include "Event/MuonOriginFlag.h"
 

 
class MuonChamberResponse  {
public:
//constructor
MuonChamberResponse(Rndm::Numbers* flat,Rndm::Numbers* poisson,double mean );
 MuonChamberResponse(){};
~MuonChamberResponse(){};
int extractNoise();
private:
Rndm::Numbers* p_flat;
Rndm::Numbers* p_poisson;
double m_chamberNoiseRate;
 
 };

#endif
