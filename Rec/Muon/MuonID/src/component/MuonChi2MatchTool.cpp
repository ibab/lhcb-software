#include "MuonChi2MatchTool.h"
#include "MuonDet/DeMuonDetector.h"
#include "MuonID/CommonMuonHit.h"
#include "Event/Track.h"

#include "GaudiKernel/SystemOfUnits.h"
#include <TMatrixF.h>
#include <vector>
#include <algorithm>

/** @class MuonChi2MatchTool MuonChi2MatchTool.h component/MuonChi2MatchTool.h
 *
 *
 *  @author Violetta Cogoni, Marianna Fontana, Rolf Oldeman, ported here by Giacomo Graziani
 *  @date   2015-11-11
 */

// convention for station counters to avoid confusions:
//   S from  0 to nStations (real station number obtained from condDB)
//   s from  0 to nSt       (hardcoded to 4) for vectors defined for M2-M5
//   iO from 0 to m_ord     (number of matched stations on which the algorithm is run)


MuonChi2MatchTool::MuonChi2MatchTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_lasttrack(NULL), m_bestcomb(0), m_chisq_min(-1.)
{
  declareInterface<IMuonMatchTool>(this);
  declareProperty("OnlyCrossedHits",m_OnlyCrossedHits=false,
                  "use only crossed hits");
  declareProperty("MaxHitsPerStation",m_maxnhitinFOI=5,
                  "consider only the closest MaxHitsPerStation muon hits per station");
  declareProperty("InputMaxSigmaMatch", m_InputMaxSigmaMatch = 8.,
                  "at input, only considers muon hits within this number of sigmas in x and y from track extrapolation");
}

StatusCode MuonChi2MatchTool::initialize() {
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // initialize geometry
  m_mudet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  nStations = m_mudet->stations();
  nRegions = m_mudet->regions();

  m_myFirstStation = (nStations == 5) ? 1 : 0;  // upgrade-wise

  m_zm.resize(nSt);
  for (unsigned int s=0; s<nSt; s++) {
    m_zm[s] =  m_mudet->getStationZ( s+m_myFirstStation ); // upgrade-wise
  }


  m_xpadsize.resize(nStations);
  m_ypadsize.resize(nStations);
  for (unsigned int S=0; S<nStations; S++) {
    (m_xpadsize[S]).resize(nRegions);
    (m_ypadsize[S]).resize(nRegions);
     for (unsigned int R=0; R<nRegions; R++) {
       m_xpadsize[S][R] = m_mudet->getPadSizeX(S,R);
       m_ypadsize[S][R] = m_mudet->getPadSizeY(S,R);
     }
  }

  // Define radiation lenghts z/X0 wrt z position
  // TODO: remove hardcoding of these numbers
  //                           z pos   z/X0
  m_mcs.push_back(std::make_pair(12800.*Gaudi::Units::mm,28.));  // ECAL + SPD + PS
  m_mcs.push_back(std::make_pair(14300.*Gaudi::Units::mm,53.));  // HCAL
  m_mcs.push_back(std::make_pair(15800.*Gaudi::Units::mm,47.5)); // M23 filter
  m_mcs.push_back(std::make_pair(17100.*Gaudi::Units::mm,47.5)); // M34 filter
  m_mcs.push_back(std::make_pair(18300.*Gaudi::Units::mm,47.5)); // M45 filter


  // vectors used for the algorithm
  m_maxcomb =pow(m_maxnhitinFOI, nSt);
  m_nhit.resize(nSt);
  m_trkExtrap.resize(nSt);

  // vectors to store the output
  m_hitOnStation.resize(nStations);
  m_bestmatchedHits.resize(nStations);

  return StatusCode::SUCCESS;
}

void  MuonChi2MatchTool::initCombs() {
  m_xhit_comb.clear() ; m_xhit_comb.resize(m_maxcomb);
  m_yhit_comb.clear() ; m_yhit_comb.resize(m_maxcomb);
  m_muhit_comb.clear(); m_muhit_comb.resize(m_maxcomb);
}


// Calculate Chi2
double MuonChi2MatchTool::calcChi2(std::vector<std::pair<double,double> > combination,double vextraM[],double p){

  //construct covariance matrix
  double chisq = 0.;
  TMatrixF var(m_ord,m_ord);
  TMatrixF ivar(m_ord,m_ord);

  double zj = 0;
  double zk = 0;
  double deltaj = 0;
  double deltak = 0;

  for(int j = 0; j < m_ord; j++){
    int sj=m_matchedStationIndex[j];
    var[j][j] += pow(combination[j].second,2);
    zj = m_zm[sj];
    for(int k = 0; k < m_ord; k++){
      int sk=m_matchedStationIndex[k];
      zk = m_zm[sk];
      for(unsigned int i = 0; i < m_mcs.size() ; i++){
        if(m_mcs[i].first < std::min(zj,zk)){
          var[j][k] += (zj - m_mcs[i].first)*(zk - m_mcs[i].first)*pow((13.6*Gaudi::Units::MeV/p),2)*m_mcs[i].second ;
        }
      }//end i
    }// end k

  }// end j

  //take the inverse of Var
  ivar = var.Invert();

  //calculate the chisquare
  for (int j = 0; j < m_ord; j++){
    deltaj = combination[j].first - vextraM[j];
    for (int k = 0; k < m_ord; k++){
      deltak = combination[k].first - vextraM[k];
      chisq += deltaj * ivar[j][k] * deltak;
    }//end k
  }//end j

  return chisq;
}


void MuonChi2MatchTool::getResidual(TrackMuMatch& match) {
  const CommonMuonHit* muhit = std::get<0>(match);
  float matchSigma = std::get<1>(match);
  if(matchSigma > m_InputMaxSigmaMatch) return;
  int s =  muhit->station() - m_myFirstStation; // index for our M2-M5 vectors
  if(s <0 || s>(nSt-1)) return; // do not use M1
  if(m_OnlyCrossedHits && muhit->uncrossed()) return;
  if(m_hitOnStation[muhit->station()] == false) m_missedStations--;
  m_hitOnStation[muhit->station()] = true;
  debug() <<"MuonChi2MatchTool  found match in M"<<muhit->station()+1<<" uncrossed? "<<muhit->uncrossed()<<endmsg;
  m_nhit[s]++;

  double extraX = std::get<2>(match);
  double extraY = std::get<3>(match);
  m_trkExtrap[s] = std::make_pair(extraX, extraY);
  m_dhit[s].push_back(std::make_pair(muhit,
                                     pow(extraX - muhit->x(),2) +  pow(extraY - muhit->y(),2) ) );
}


/// makes combinations among the variable number of stations m_ord through an iterative call
void MuonChi2MatchTool::makeComb(std::vector<int>* c , int& comb) {
  int s,iO,i;
  // make this combination
  verbose() <<"MuonChi2MatchTool combination #"<<comb<<" :";
  for (iO=0; iO<m_ord; iO++)
    verbose() <<" M"<<m_matchedStationIndex[iO]+2<<" "<<(*c)[iO];
  verbose()<<endmsg;

  for (iO=0; iO<m_ord; iO++) {
    i=c->at(iO);
    s=m_matchedStationIndex[iO];
    const CommonMuonHit* muhit = m_dhit[s][i].first;
    m_xhit_comb[comb].push_back(std::make_pair(muhit->x(), m_xpadsize[muhit->station()][muhit->region()]/sqrt(12)));
    m_yhit_comb[comb].push_back(std::make_pair(muhit->y(), m_ypadsize[muhit->station()][muhit->region()]/sqrt(12)));
    m_muhit_comb[comb].push_back(muhit);
  }
  comb++;

  // now call next combination
  bool notLastComb = false;
  iO=m_ord-1;
  while (iO>-1) {
    s=m_matchedStationIndex[iO];
    if( (*c)[iO] < (m_nhit[s]-1) ) { // increase this index and go on
      (*c)[iO] ++;
      notLastComb =true;
      break;
    }
    else {
      (*c)[iO]=0; // roll-back
      iO--; // continue loop on previous dimension
    }
  }
  if(notLastComb) makeComb(c, comb);
}


// =============== MAIN FUNCTION ===============
StatusCode MuonChi2MatchTool::run(const LHCb::Track* track,
                                  std::vector<TrackMuMatch>* bestMatches,
                                  std::vector<TrackMuMatch>* spareMatches)
{
  m_lasttrack = track;
  m_lastMatchesBest = bestMatches;
  m_lastMatchesSpare = spareMatches;
  m_ord=0;
  m_chisq_min=-1.;
  m_bestcomb=-1;
  m_matchedHits.clear();
  for (unsigned int S=0; S< nStations ; S++) {
    m_hitOnStation[S]=false;
    m_bestmatchedHits[S]=NULL;
  }

  if(!bestMatches)  return  StatusCode::SUCCESS;
	if(bestMatches->size() <2) return  StatusCode::SUCCESS; // require at least 2 reasonable matches

  m_dhit.clear();
  m_dhit.resize(nSt);

  for (unsigned int s=0; s<nSt; s++) m_nhit[s]=0;
  m_missedStations=nSt;

  std::vector<TrackMuMatch>::iterator ih;
  if(bestMatches) {
    for (ih=bestMatches->begin(); ih != bestMatches->end(); ih++) {
      getResidual(*ih);
    }
  }

  if(m_missedStations>0 && spareMatches) { // also look among low quality muon hits if nothing better was found
    for (ih=spareMatches->begin(); ih != spareMatches->end(); ih++) {
      if(false == m_hitOnStation[(std::get<0>(*ih))->station()])
        getResidual(*ih);
    }
  }

  if(m_missedStations>2) return StatusCode::SUCCESS; // require at least 2 matched stations

	// Sort distsq from closest to farthest hits
	for(int s=0;s<nSt;s++ ){
    if(m_nhit[s]>0)
      std::sort(m_dhit[s].begin(), m_dhit[s].end(), [](const std::pair<const CommonMuonHit*,double> &left, const std::pair<const CommonMuonHit*,double> &right) {	
                  return left.second < right.second;
                });
	}

  m_ord=nSt-m_missedStations; // m_ord is the number of matched station (order of our covariance matrix)
  m_matchedStationIndex.resize(m_ord); // map dimension to station

  std::vector<int> c(m_ord); // vector with running indexes for the combinations
  int iO=0;
  for (int s=0; s<nSt; s++) {
    if(m_nhit[s] > 0){ // use this station
      m_nhit[s]=std::min(m_maxnhitinFOI, m_nhit[s]);
      m_matchedStationIndex[iO]=s;
      c[iO] =0;
      iO++;
    }
  }

	// Find all the hit combinations
  initCombs();
	int comb = 0;
  makeComb(&c, comb); // will call iteratively itself for all possible combinations
  int ncomb=comb;
  verbose() << "Found "<<ncomb<<" combinations"<<endmsg;

	// Calculate chisquare starting from hit combinations
	double chisq_x = 0;
	double chisq_y = 0;
	double chisq_sum = 0;

	m_chisq_min = 1e12;
	

	double vextraXsM[nSt];
	double vextraYsM[nSt];
  for (iO=0; iO<m_ord; iO++) {
    vextraXsM[iO] = m_trkExtrap[m_matchedStationIndex[iO]].first;
    vextraYsM[iO] = m_trkExtrap[m_matchedStationIndex[iO]].second;
  }
	
	for(int combs = 0; combs < ncomb; combs++){
	  if (m_xhit_comb[combs].size()!=0 && m_yhit_comb[combs].size()!=0){

	    chisq_x = calcChi2(m_xhit_comb[combs],vextraXsM,track->p());	
	    chisq_y = calcChi2(m_yhit_comb[combs],vextraYsM,track->p());
	    chisq_sum = chisq_x + chisq_y;

	    if (chisq_sum < m_chisq_min){ // choose the best chi square and store that combination
        m_bestcomb=combs;
	      m_chisq_min = chisq_sum;

	    }//end if chisq_sum
	  }//end if size!=0
	}//end loop on combs
	
  // and the winner is...m_bestcomb
  // store the best CommonMuonHits
  verbose() << "MuonChi2MatchTool best comb is " << m_bestcomb <<endmsg;
  for (iO=0; iO<m_ord; iO++) {
    const CommonMuonHit* bestHit= (m_muhit_comb[m_bestcomb])[iO];
    m_matchedHits.push_back(bestHit);
    m_bestmatchedHits[bestHit->station()] = bestHit;
  }
  debug() << "MuonChi2MatchTool output: chi2="<<m_chisq_min<<" with ndof="<<m_ord*2<<endmsg;

  return StatusCode::SUCCESS;
}

void MuonChi2MatchTool::getListofCommonMuonHits(CommonConstMuonHits& matchedMuonHits, int station, bool) {
  if (station<0) {
    matchedMuonHits=m_matchedHits;
  } else {
    matchedMuonHits.clear();
    if(station<(int)nStations && m_bestmatchedHits[station])
      matchedMuonHits.push_back(m_bestmatchedHits[station]);
  }
}

void MuonChi2MatchTool::getListofMuonTiles(std::vector<LHCb::MuonTileID>& matchedTiles, int station, bool) {
  matchedTiles.clear();
  if(station>-1 && station<(int)nStations && m_bestmatchedHits[station]) {
    matchedTiles.push_back(m_bestmatchedHits[station]->tile());
  } else {
    std::transform(m_matchedHits.begin(),m_matchedHits.end(),
                   std::back_inserter(matchedTiles),
                   [](const CommonMuonHit* hit) { return hit->tile(); } );
  }
}

double MuonChi2MatchTool::getChisquare(int& ndof) {
  ndof= 2*m_ord;
  return m_chisq_min;
}

IMuonMatchTool::MuonMatchType MuonChi2MatchTool::getMatch(int station) {
  if(!m_hitOnStation[station]) return IMuonMatchTool::NoMatch;
  if(m_bestmatchedHits[station]->uncrossed()) return IMuonMatchTool::Uncrossed;
  return IMuonMatchTool::Good;
}


double MuonChi2MatchTool::getMatchSigma(int station) {
  // retrieve the original match distance in sigma for this station
  verbose() << "requested MatchSigma for M"<<station+1<<" lastTrack is "<<m_lasttrack<<" match is "<<m_hitOnStation[station]<<endmsg;
  double out=999.;
  if(!m_hitOnStation[station]) return out;
  if(m_lasttrack) {
    std::vector< TrackMuMatch > * mvector=NULL;
    if(m_lastMatchesBest) mvector = m_lastMatchesBest;
    if(!mvector) {
      if(m_lastMatchesSpare) mvector = m_lastMatchesSpare;
    }
    if(mvector) {
      for (auto im=mvector->begin(); im!=mvector->end(); im++) {
        if( std::get<0>(*im) == m_bestmatchedHits[station]) {
          out=std::get<1>(*im);
          break;
        }
      }
    }
  }
  return out;
}

double MuonChi2MatchTool::muonMatchPropertyD(const char*, int ) {
  return -9999.;
}

int MuonChi2MatchTool::muonMatchPropertyI(const char* propertyName, int) {
  return std::string{propertyName} == "matchedStations" ? m_ord : -9999;
}

DECLARE_TOOL_FACTORY( MuonChi2MatchTool )
