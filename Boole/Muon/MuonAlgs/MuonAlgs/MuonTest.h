#ifndef MUONALGS_MUONTEST_H
#define MUONALGS_MUONTEST_H 1
#include <cmath>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"       
#include "GaudiKernel/MsgStream.h"  
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"
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
#include "DetDesc/IReadOut.h"   
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/MuonChannel.h"

#include "MuonKernel/MuonTile.h"
#include "Event/MCMuonHit.h"   
#include "Event/MCMuonDigit.h"   
#include "Event/MuonDigit.h"   

#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonReadoutCond.h"  
#include "Event/MuonOriginFlag.h"
#include "GaudiKernel/INTupleSvc.h" 
#include "GaudiKernel/NTuple.h" 
#include "MuonUtils/MuonGeometryStore.h"

class MuonTest : public Algorithm {

 
public:
  // Constructor: A constructor of this form must be provided.
	MuonTest(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode locateMuonTileFromXYZ(double x,double y,double z,int&
	numberTileOutput, MuonTileID phChTileID[2],bool debug=false);
  private:
  IMuonTileXYZTool* m_pMuonTileXYZ ;

  IHistogram1D* m_histoTiming ;

  NTuple::Tuple*                  m_digiNtuple;
/// N-tuple items to be written away: Number of tracks
  NTuple::Item<long>              m_hit;
  NTuple::Array<float>            m_x;
  NTuple::Array<float>            m_y;
  NTuple::Array<float>            m_z;
  NTuple::Array<float>            m_time;
  NTuple::Array<long>             m_nature;
  NTuple::Array<long>             m_region;
  NTuple::Array<long>             m_spill;	
  NTuple::Array<long>             m_muon;	
  NTuple::Array<long>             m_fired;	
  NTuple::Array<long>             m_hitted;	
  NTuple::Array<long>             m_dead;	
  NTuple::Array<long>             m_notfound;	
  NTuple::Array<long>             m_mother;	

   NTuple::Tuple*                 m_digiError;
   NTuple::Item<long>             m_hitNoInDigit;
   NTuple::Item<long>             m_hitInInput;
   NTuple::Item<long>             m_hitWronglyPalcedX;
   NTuple::Item<long>             m_hitWronglyPalcedY;

   NTuple::Tuple*                 m_mcdigit;
   NTuple::Item<long>          	  m_mcdigitnumber;
   NTuple::Array<long>            m_mcmuondigitalive;	
   NTuple::Array<long>            m_mcmuondigitregion;	
   NTuple::Item<long>          	  m_mcmuonhit;
   NTuple::Array<long>            m_mcmuonhitnature;
   NTuple::Array<long>            m_mcmuonhitpartition;
   NTuple::Array<long>            m_mcmuonhitalive;
   NTuple::Array<long>            m_mcmuonhitefficiency;
   NTuple::Array<float>           m_mcmuonhittime;

   NTuple::Tuple*                 m_digit;
   NTuple::Item<long>          	  m_digitnumber;
   NTuple::Array<long>            m_digitpartition;
   NTuple::Array<long>           m_digittime;
   NTuple::Array<long>           m_digitnature;
   NTuple::Array<long>           m_hitindigit;

	 };
#endif
