// $Id: SmartMuonMeasProvider.cpp,v 1.3 2009-07-09 11:23:12 cattanem Exp $
// Include files
// local
//#include "MuonID/MuonMeasurementProvider.h"
#include "SmartMuonMeasProvider.h"
#include <iostream>



//-----------------------------------------------------------------------------
// Implementation file for class : SmartMuonMeasurementProvider
//
// 2008-07-16 : Jose Angel Hernando Morata
//              Xabier Cid Vidal
//-----------------------------------------------------------------------------



//=============================================================================
// Function idsInRange

//given an state, find corresponding closest station and get hits in n sigmas
std::vector<LHCb::LHCbID>  SmartMuonMeasProvider::idsInRange(const LHCb::State& state,
                                                             double nsigmas,int discriminationValue) {
  std::vector<LHCb::LHCbID> ids;
  std::vector< LHCbIDWeight > oids;

  //find closest muon station to state
  int istation=findClosestStation(state.z());

  //get measPairList from corresponding station
  const auto& measList = atStation(istation);
  //loop over measPairList to decide wether they are in range or not, and sort them out
  for (const auto& meas : measList ) {
    double val;
    //decide value to sort out measures in range
    switch (discriminationValue) {
    //chi2 x coordinate
        case 2 : val=chi2x(meas,state); break;
    //chi2 y coordinate
        case 3 : val=chi2y(meas,state); break;
    //global geometrical distance
        case 4 : val=distance(meas,state); break;
    //distance in x coordinate
        case 5 : val=distx(meas,state); break;
    //distance in y coordinate
        case 6 : val=disty(meas,state); break;
    //global chi2
        default: val=chi2(meas,state);
    }

    //decide if meas is in range.
    //Calculate distance in x and y and compare to number of preset sigmas.
    //Sigma depending on errors of measure and extrapolated state.
    //Turns out to be field of interest on this method
    double difx = distx(meas,state);
    double dify = disty(meas,state);

    double uncert_x=uncx(meas,state);
    double uncert_y=uncy(meas,state);

    //if condition fulfilled for this measure, add to list LHCbID and val to sort out
    if ((difx <= nsigmas*uncert_x) && (dify <=nsigmas*uncert_y)) {
      oids.emplace_back( meas.first->lhcbID(), val );
    }
  }

  std::sort(oids.begin(),oids.end(),OrderByValue());
  //once sorted out, add to output LHCbIDs
  for (const auto& i : oids) ids.push_back(i.first);
  return ids;
}

//find closest station to given z. To be corrected!!!
int SmartMuonMeasProvider::findClosestStation(double z)
{
  double smallest=1e6;
  std::array<double,5>  stations_z = {12110.0,15270.0,16470.0,17670.0,18870.0};

  int st=0;
  for (int i=0;i<5;i++) {
    double difz=fabs((stations_z[i]-z));
    if (difz<smallest) {
      smallest=difz;
      st=i;
    }
  }
  return st;
}

//extrapolate lineally state to given z
std::pair<double,double> SmartMuonMeasProvider::linearExtrapolator(const LHCb::State& state,double z) const
{
  auto pos = state.position();
  auto mom = state.momentum();
  auto z0 = pos.z();
  auto pz = mom.z();
  return pz !=0 ? std::make_pair(pos.x()+(z-z0)*mom.x()/pz,
                                 pos.y()+(z-z0)*mom.y()/pz)
                : std::make_pair(1.e6,1.e6);
}
