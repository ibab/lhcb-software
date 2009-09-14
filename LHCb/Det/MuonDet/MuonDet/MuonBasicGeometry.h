// $Id: MuonBasicGeometry.h,v 1.3 2009-09-14 08:58:35 jonrob Exp $
#ifndef MUONDET_MUONBASICGEOMETRY_H 
#define MUONDET_MUONBASICGEOMETRY_H 1

#include <string>
#include <vector>

class IDataProviderSvc;
class IMessageSvc;

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

private:
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;
  std::vector<std::string> numsta;  
};
#endif // MUONALGS_MUONBASICGEOMETRY_H
