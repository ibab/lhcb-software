// $Id: MuonBasicGeometry.h,v 1.1 2002-10-21 20:38:03 asatta Exp $
#ifndef MUONDET_MUONBASICGEOMETRY_H 
#define MUONDET_MUONBASICGEOMETRY_H 1

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include <GaudiKernel/ObjectVector.h>  
#include <GaudiKernel/IDataProviderSvc.h>
#include "GaudiKernel/IHistogramSvc.h"


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
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/MuonParameters.h"
#include "MuonDet/DeMuonGasGap.h"
#include "MuonDet/MuonReadoutCond.h"



// Include files

/** @class MuonBasicGeometry MuonBasicGeometry.h MuonAlgs/MuonBasicGeometry.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2002-10-07
 */
class MuonBasicGeometry {
public:
  /// Standard constructor
  MuonBasicGeometry(IDataProviderSvc* detSvc , IMessageSvc * msgSvc );
  ~MuonBasicGeometry(){}; ///< Destructor
  int getStations();
  int getRegions();
  int getPartitions();
  std::string  findName(std::string allname,std::string rootName);
  std::string getStationName(int station);
  
protected:

private:
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;
  std::vector<std::string> numsta;  
};
#endif // MUONALGS_MUONBASICGEOMETRY_H
