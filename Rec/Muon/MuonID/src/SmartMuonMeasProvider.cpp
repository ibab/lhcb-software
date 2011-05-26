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
// Standard constructor, initializes variables
//=============================================================================
SmartMuonMeasProvider::SmartMuonMeasProvider(  ) {
  for (int i = 0; i <5; i++)  m_meas.push_back( MeasPairList() );
}

//=============================================================================
// Destructor
//=============================================================================
SmartMuonMeasProvider::~SmartMuonMeasProvider() 
{
  for (std::vector<MeasPairList>::iterator it = m_meas.begin();
       it != m_meas.end(); ++it) {
    for (MeasPairList::iterator it2 = it->begin(); it2 != it->end(); ++it2) {
      delete it2->first;
      delete it2->second;
    }
  }
}


//=============================================================================
// Function idsInRange

//given an state, find corresponding closest station and get hits in n sigmas
std::vector<LHCb::LHCbID>  SmartMuonMeasProvider::idsInRange(LHCb::State& state, 
                                                             double nsigmas,int discriminationValue) {
  std::vector<LHCb::LHCbID> ids;
  std::vector< LHCbIDWeight > oids;

  //find closest muon station to state
  int istation=findClosestStation(state.z());
  //std::cout <<"CLOSEST POINT at STATION "<<istation<<std::endl;
  
  //get measPairList from corresponding station
  const MeasPairList& measList = atStation(istation);
  //loop over measPairList to decide wether they are in range or not, and sort them out
  for (MeasPairList::const_iterator it = measList.begin();
       it != measList.end(); ++it) {
    const MeasPair& meas = *it;
    
    double val;
    //decide value to sort out measures in range
    //chi2 x coordinate
    if (discriminationValue==2) val=chi2x(meas,state);
    
    //chi2 y coordinate
    else if (discriminationValue==3) val=chi2y(meas,state);
    
    //global geometrical distance
    else if (discriminationValue==4) val=distance(meas,state);
    
    //distance in x coordinate
    else if (discriminationValue==5) val=distx(meas,state);
    
    //distance in y coordinate
    else if (discriminationValue==6) val=disty(meas,state);
    
    //global chi2
    else val=chi2(meas,state);
    
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
      //std:cout<<"\nsmmp xmeas1="<<meas.first;
      //std:cout<<"\nsmmp &ID1="<<&(meas.first->lhcbID());
      //std:cout<<"\nsmmp channelID1="<<meas.first->lhcbID().channelID();
      oids.push_back( LHCbIDWeight(meas.first->lhcbID(), val) );
    }
    
  }
  
  if (oids.size() == 0) return  ids;
  std::sort(oids.begin(),oids.end(),OrderByValue());
  //once sorted out, add to output LHCbIDs
  //std:cout<<"\n";
  for (std::vector<LHCbIDWeight>::iterator it = oids.begin(); it!=oids.end();++it){
    //std:cout<<"\nsmmp &ID2="<<&(it->first);
    //std:cout<<"\nsmmp channelID2="<<it->first.channelID();
    ids.push_back(it->first);
  }
  //std:cout<<"\n";
  return ids;
}

//find closest station to given z. To be corrected!!!
int SmartMuonMeasProvider::findClosestStation(double z)
{
  int st=0;
  double smallest=1e6;
  double stations_z[] = {12110.0,15270.0,16470.0,17670.0,18870.0};

  for (int i=0;i<5;i++) 
  {
    double difz=fabs((stations_z[i]-z));
    //std::cout<<"st="<<i<<std::endl;
    //std::cout<<"DIFZ="<<difz<<std::endl;
    //std::cout<<"smallest="<<smallest<<std::endl;
    if (difz<smallest)
    {
      smallest=difz;
      st=i;
    }
  }

  return st;
}


//extrapolate lineally state to given z
std::vector<double> SmartMuonMeasProvider::linearExtrapolator(const LHCb::State& state,const double z)
{
  std::vector<double> xy;
  double z0 = state.position().z();
  double pz = state.momentum().z();
  if (pz==0) {
    xy.push_back(double(10e5));
    xy.push_back(double(10e5));
  }
  else {
    double sx=state.momentum().x()/pz;
    double sy=state.momentum().y()/pz;
    xy.push_back(state.position().x()+sx*(z-z0));
    xy.push_back(state.position().y()+sy*(z-z0));
  }
  
  return xy;
}



//get uncx from errors on pair and state
double SmartMuonMeasProvider::uncx(const MeasPair& meas, const LHCb::State& state)
{
  LHCb::Measurement& x_meas = *(meas.first);
  //std::cout <<"xmeas ok"<<std::endl;

  double errStateX=sqrt(state.errX2());
  //std::cout <<"errStateX "<<errStateX<<std::endl;
  //std::cout <<"errMeasureX "<<x_meas.errMeasure()<<std::endl;
  double uncX=sqrt(pow(errStateX,2)+pow(x_meas.errMeasure(),2));
  //std::cout <<"sigmaX="<<uncX<<std::endl;
  return uncX;
 
}


//get uncy from errors on pair and state
double SmartMuonMeasProvider::uncy(const MeasPair& meas, const LHCb::State& state)
{

  LHCb::Measurement& y_meas = *(meas.second);
  //std::cout <<"ymeas ok"<<std::endl;

  double errStateY=sqrt(state.errY2());
  //std::cout <<"errStateY "<<errStateY<<std::endl;
  //std::cout <<"errMeasureY="<<y_meas.errMeasure()<<std::endl;
  double uncY=sqrt(pow(errStateY,2)+pow(y_meas.errMeasure(),2));
  //std::cout <<"sigmaY="<<uncY<<std::endl;
  return uncY;  
}




