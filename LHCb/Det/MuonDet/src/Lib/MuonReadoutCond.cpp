// $Id: MuonReadoutCond.cpp,v 1.10 2008-07-12 06:15:40 asatta Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
#define MUONDET_MUONREADOUTCOND_CPP

// Include files
#include "MuonDet/MuonReadoutCond.h"
#include <cmath>
#include <iostream>

/** @file MuonReadoutCond.cpp
 *
 * Implementation of class : MuonReadoutCond
 * This is the Muon readout information, with electronic and chamber
 * effects required to convert GEANT information into fired channels
 *
 * @author David Hutchcroft, David.Hutchcroft@cern.ch
 *
 */

// Standard Constructors
MuonReadoutCond::MuonReadoutCond():
  Condition(), m_RList()
{
}

/// Copy constructor
MuonReadoutCond::MuonReadoutCond( MuonReadoutCond& obj ) :
  IValidity(),
  Condition( (Condition&)obj ),
  m_RList(obj.getRList())
{
}

/// update constructor, do a deep copy of all
/// except for the properties of a generic DataObject
void MuonReadoutCond::update( MuonReadoutCond& obj ){
  Condition::update((Condition&)obj);
  m_RList = obj.getRList();
}

/// Standard Destructor
MuonReadoutCond::~MuonReadoutCond()
{
}

StatusCode MuonReadoutCond::initialize(){
  /// Set CumProbX and CumProbY from cluserX and clusterY
  /// and make the randomnumber generators
  std::vector<_readoutParameter>::iterator iRout;
  for(iRout = m_RList.begin() ; iRout != m_RList.end() ; iRout++){

    if(iRout->ClusterX.size() == 0 || iRout->ClusterY.size() == 0){
      return StatusCode::FAILURE;
    }

    // start with X
    iRout->CumProbX.clear(); // remove old probabilties
    const int maxX=maxCluster(*iRout,'x');

    // make enough entries in the vector
    iRout->CumProbX.insert( iRout->CumProbX.begin(), maxX, 0.);

    // fill CumProbX in position i with value p from cluster
    std::vector<_clus>::const_iterator probIt;
    for( probIt = iRout->ClusterX.begin();
         probIt != iRout->ClusterX.end(); ++probIt){
      iRout->CumProbX[(*probIt).clSize-1]=(*probIt).clProb;
    }

    double rsum=0.;
    std::vector<double>::iterator cumIt;
    for( cumIt = iRout->CumProbX.begin();
         cumIt != iRout->CumProbX.end() ; ++cumIt){
      rsum += *cumIt;
      *cumIt=rsum;
    }

    // alright paranoia here, normalise the cumlative sum
    for( cumIt = iRout->CumProbX.begin();
         cumIt != iRout->CumProbX.end() ; ++cumIt){
      *cumIt /= rsum;
    }

    // same for Y
    iRout->CumProbY.clear(); // remove old probabilties
    const int maxY=maxCluster(*iRout,'y');

    // make enough entries in the vector
    iRout->CumProbY.insert( iRout->CumProbY.begin(), maxY, 0.);

    // fill CumProbY in position i with value p from cluster
    for( probIt = iRout->ClusterY.begin();
         probIt != iRout->ClusterY.end(); ++probIt){
      iRout->CumProbY[(*probIt).clSize-1]=(*probIt).clProb;
    }

    rsum=0.;
    for( cumIt = iRout->CumProbY.begin();
         cumIt != iRout->CumProbY.end() ; ++cumIt){
      rsum += *cumIt;
      *cumIt=rsum;
    }

    // alright paranoia here, normalise the cumlative sum
    for( cumIt = iRout->CumProbY.begin();
         cumIt != iRout->CumProbY.end() ; ++cumIt){
      *cumIt /= rsum;
    }
  }

  return StatusCode::SUCCESS;
}



/// Used by XML to set readout type from std::string to int
StatusCode MuonReadoutCond::addReadout(const std::string &rType, int &index){

  _readoutParameter tmpParameters;

  if(rType == "Anode"){
    tmpParameters.ReadoutType=0;
  }else if(rType == "Cathode"){
    tmpParameters.ReadoutType=1;
  }else{
    index=-1;
    return StatusCode::FAILURE;
  }
  tmpParameters.Efficiency=0.;
  tmpParameters.SyncDrift=0.;
  tmpParameters.ChamberNoise=0.;
  tmpParameters.ElectronicsNoise=0.;
  tmpParameters.MeanDeadTime=0.;
  tmpParameters.RMSDeadTime=0.;
  tmpParameters.TimeGateStart=0.;
  tmpParameters.PadEdgeSizeX=0.;
  tmpParameters.PadEdgeSigmaX=0.;
  tmpParameters.PadEdgeSizeY=0.;
  tmpParameters.PadEdgeSigmaY=0.;
  tmpParameters.JitterMin=0;
  tmpParameters.JitterMax=0;

  m_RList.push_back(tmpParameters);
  index = m_RList.size() - 1;
  return StatusCode::SUCCESS;
}

/// Utility routine to find the largest posible size of cluster
int MuonReadoutCond::maxCluster(const _readoutParameter &readout,
                                const char &xy){

  const std::vector<_clus> *cluster=0;
  if('X' == xy || 'x' == xy){
    cluster = &readout.ClusterX;
  }else if('Y' == xy || 'y' == xy){
    cluster = &readout.ClusterY;
  }

  int max=0;
  std::vector<_clus>::const_iterator it;
  for( it = (*cluster).begin();
       it != (*cluster).end();
       ++it){
    if( max < (*it).clSize ){
      max=(*it).clSize;
    }
  }
  return max;
}

/// Return the size of cluster corresponding to a random number (0->1) in X
int MuonReadoutCond::singleGapClusterX(const double &randomNumber,
                                       const double &xDistPadEdge,
                                       const int &i){
  return this->singleGapCluster(0,randomNumber,xDistPadEdge,i);
}

/// Return the size of cluster corresponding to a random number (0->1) in Y
int MuonReadoutCond::singleGapClusterY(const double &randomNumber,
                                       const double &yDistPadEdge,
                                       const int &i){
  return this->singleGapCluster(1,randomNumber,yDistPadEdge,i);
}

/// Used by singleGapClusterX and  singleGapClusterY to calculate value
int MuonReadoutCond::singleGapCluster(const int &xy,
                                      const double &randomNumber,
                                      const double &xpos,
                                      const int &i) {

  // Apologies for the high level of indirection, I decided that
  // having one routine that picked from X or Y was perferable to
  // two routines.

  if(randomNumber>1. || randomNumber<0.){
    return 1;
  }

  std::vector<double> *cumProb;
  std::vector<_clus> *cluster;
	std::vector<_clus> local_cluster;
	std::vector<double> local_cumProb;
  double *padEdgeSize;
  double *padEdgeSigma;
  if(0 == xy){
    cumProb = &(m_RList[i].CumProbX);
    cluster = &(m_RList[i].ClusterX);
    padEdgeSize = &(m_RList[i].PadEdgeSizeX);
    padEdgeSigma = &(m_RList[i].PadEdgeSigmaX);
		for(int size=0;size<(int)m_RList[i].CumProbX.size();size++){
		  local_cluster.push_back(m_RList[i].ClusterX[size]);
		  local_cumProb.push_back(m_RList[i].CumProbX[size]);
		}
  }else{
    cumProb = &(m_RList[i].CumProbY);
    cluster = &(m_RList[i].ClusterY);
    padEdgeSize = &(m_RList[i].PadEdgeSizeY);
    padEdgeSigma = &(m_RList[i].PadEdgeSigmaY);
		for(int size=0;size<(int)m_RList[i].CumProbY.size();size++){
		  local_cluster.push_back(m_RList[i].ClusterY[size]);
		  local_cumProb.push_back(m_RList[i].CumProbY[size]);
		}		
  }

  // now should have a cumlative sum in *cumProb
  int icsize;
   if( (*cumProb).size() >= 3  &&  randomNumber > (*cumProb)[1] ){
    // do _not_ do the correction for the pad edge effect
    // (cluster size at least 3 anyway
    icsize=1;
    while( (*cumProb)[icsize] < randomNumber ){
      ++icsize;
    }
  }else{
    // must correct for pad edge effect
     if( (*padEdgeSigma) > 0.  &&  (*padEdgeSize) < 0.){
      // tested that there _is_ an effect
       double edgeEffect = (*padEdgeSize) *
        safe_exponential( -0.5 * ( xpos*xpos) 
            / ( (*padEdgeSigma)*(*padEdgeSigma) ));
      local_cumProb[0] += edgeEffect;
      local_cumProb[1] -= edgeEffect;
     }
    icsize=0;
    while( (local_cumProb)[icsize] < randomNumber ){
      ++icsize;
    }
  }
  return icsize+1;
}


/// takes pairs: eg 3,0.2 and adds them to the X cluster size/prob list
void MuonReadoutCond::addClusterX(const int &size, const double &prob,
                                  const int &i){
  _clus tmp;
  tmp.clSize=size;
  tmp.clProb=prob;
  m_RList[i].ClusterX.push_back(tmp);
}

/// takes pairs: eg 3,0.2 and adds them to the Y cluster size/prob list
void MuonReadoutCond::addClusterY(const int &size, const double &prob,
                                  const int &i){
  _clus tmp;
  tmp.clSize=size;
  tmp.clProb=prob;
  m_RList[i].ClusterY.push_back(tmp);
}

void MuonReadoutCond::setTimeJitter(const std::vector<double> &jitterVec,
                                    const double &min,
                                    const double &max,
                                    const int &i){
  m_RList[i].JitterVector=jitterVec;
  m_RList[i].JitterMin=min;
  m_RList[i].JitterMax=max;
}

