// Include files
#include "GaudiKernel/ToolFactory.h"
#include "Event/Particle.h"
// kernel
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticle2MCAssociator.h"
//#include "Kernel/Particle2MCLinker.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
// MC stuff
#include "Event/GenHeader.h"
#include "Event/MCHeader.h"

// From the MuonDetector:
//#include "MuonAlgs/MuonBasicGeometry.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonDetector.h"
#include "Event/MuonDigit.h"
#include "Event/MCMuonDigit.h"
#include "Event/MCMuonDigitInfo.h"
#include "Event/IntLink.h"

#include "TupleToolMuonIDCalib.h"
//#include "TMath.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMuonIDCalib
//
// 2011-09-23 : Fatima Soomro
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TupleToolMuonIDCalib )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolMuonIDCalib::TupleToolMuonIDCalib( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
    : TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty ( "NFoi"          ,m_NFoi  = 2 ) ;
  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );
  declareProperty( "isVerbose", m_doVerbose = false);
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolMuonIDCalib::~TupleToolMuonIDCalib() {}

//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode TupleToolMuonIDCalib::fill( const LHCb::Particle * /* top */,
                                       const LHCb::Particle *part,
                                       const std::string &   head,
                                       Tuples::Tuple &   tuple )
{
  // nothing for particles that are not Bs
  if( !part->isBasicParticle() || NULL==part->proto() )
    return StatusCode::SUCCESS;

  LoadMuonGeometry();

  if ( msgLevel(MSG::DEBUG) )
    debug () <<"Fatima: Loaded Geometry in TupleToolMuIDCalib"<< endmsg;

  StatusCode sc = fillCoordVectors();
  if(!sc) error()<<"couldnt fillCoorVectors!!!"<<endmsg;
  debug() <<"Fatima: fillCoordVectors in TupleToolMuIDCalib"<< endmsg;

  const std::string prefix=fullName(head);
  sc = fillVars( part, prefix, tuple );
  return sc;
}


//==========================================================================
// Load the geometry
//==========================================================================

void TupleToolMuonIDCalib::LoadMuonGeometry()
{
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  debug () << "stations and regions from MuonGeometry are "<< m_NStation<<" "<<m_NRegion<<endmsg;

  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endreq;
    i++;
  }
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  // set the size of the local vectors
  m_regionInnerX.resize(m_NStation);
  m_regionOuterX.resize(m_NStation);
  m_regionInnerY.resize(m_NStation);
  m_regionOuterY.resize(m_NStation);
  for(int station = 0 ; station < m_NStation ; station++ ){
    m_stationZ.push_back(m_mudet->getStationZ(station));
    m_regionInnerX[station] = m_mudet->getInnerX(station);
    m_regionOuterX[station] = m_mudet->getOuterX(station);
    m_regionInnerY[station] = m_mudet->getInnerY(station);
    m_regionOuterY[station] = m_mudet->getOuterY(station);
  }

}

//==========================================================================
// fill vectors of x,y,z positions for the MuonCoords
//==========================================================================

StatusCode TupleToolMuonIDCalib::fillCoordVectors(){

  //  m_HitInTrk.clear();
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_hitInFOIx.clear();
  m_hitInFOIdx.clear();
  m_hitInFOIy.clear();
  m_hitInFOIdy.clear();
  m_hitInFOIz.clear();
  m_hitInFOIdz.clear();
  m_hitInFOIID.clear();

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords =NULL;
  if(exist<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords))
    coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords )
  {
    err() << "Cannot retrieve MuonCoords " << endreq;
    return StatusCode::FAILURE;
  }

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }
    m_coordPos[station*m_NRegion+region].
      push_back(coordExtent_(x,dx,y,dy,z,dz,*iCoord));
    //      m_HitInTrk.push_back(false);
  }

  return StatusCode::SUCCESS;

}

// return the FOI in x in a station and region for momentum (in MeV/c)
// this is a simpler version of the parameterization:
// foi = par0 + par2*exp(-par3*p)
//=============================================================================
double TupleToolMuonIDCalib::foiX(const int &station, const int &region, const double &p,
                                  const double &dx){
  //=============================================================================
  const double temp_x = ( m_xfoiParam1[ station * m_NRegion + region ] +
                          m_xfoiParam2[ station * m_NRegion + region ]*
                          std::exp(-m_xfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dx;
  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function

  if ( msgLevel(MSG::VERBOSE) )
    verbose()<< "from FOIX "<<temp_x<< endmsg;

  return temp_x;
}


// return the FOI in y in a station and region for momentum (in MeV/c)
//=============================================================================
double TupleToolMuonIDCalib::foiY(const int &station,
                                  const int &region,
                                  const double &p,
                                  const double &dy)
{
  //=============================================================================
  const double temp_y = ( m_yfoiParam1[ station * m_NRegion + region ] +
                          m_yfoiParam2[ station * m_NRegion + region ]*
                          std::exp(-m_yfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dy;

  if ( msgLevel(MSG::VERBOSE) )
    verbose() <<"from FOIY "<<temp_y<< endmsg;

  return temp_y;
}


//=============================================================================
//  fill Xs, Ys and DistAve
//=============================================================================
StatusCode TupleToolMuonIDCalib::fillVars(  const LHCb::Particle *part,
                                            const std::string prefix, Tuples::Tuple &tuple) {


  if( (!part->isBasicParticle() ) ||  NULL==part->proto()   ){
    error () << "!! Tried to fill MuonIDCalib quantities for particle " <<part->particleID().pid()<< "!!!  This should not happen"<< endmsg;
    return StatusCode::SUCCESS;
  }

  const LHCb::Track*  track= part->proto()->track();
  if((track->checkFlag(LHCb::Track::Clone))  ||
     !( track->checkType(LHCb::Track::Long) || track->checkType(LHCb::Track::Downstream)
        || track->checkType(LHCb::Track::Ttrack) )){
    return StatusCode::SUCCESS ;
  }

  ///-------------------------------------------------------------------------------------
  // 6 March 2012, Add the muon detector hit positions related to the track, to the ntuple
  // one has to get the muon part of the track (from muonPIDs)and then get the lhcbIDs related to it
  ///-------------------------------------------------------------------------------------

  if(m_doVerbose){

    std::vector<float> m_x;
    std::vector<float> m_y;
    std::vector<float> m_z;

    LHCb::MuonPIDs* pMuids =NULL;
    if(exist<LHCb::MuonPIDs>(LHCb::MuonPIDLocation::Default)) pMuids=get<LHCb::MuonPIDs>(LHCb::MuonPIDLocation::Default);

    if (!pMuids){
      error() << " Failed to get pMuids" <<  LHCb::MuonPIDLocation::Default << endreq;
      return StatusCode::FAILURE;
    }
    if ( msgLevel(MSG::DEBUG) )
      debug() << " >> number of Mu tracks in the event " << pMuids->size() << endmsg;
    const LHCb::Track *mutrack=NULL;
    LHCb::MuonPIDs::const_iterator ip;
    for( ip = pMuids->begin(); ip != pMuids->end() ; ip++){
      if ((*ip)->idTrack() == track){
        mutrack = (*ip)->muonTrack();
        if (!mutrack) continue;
        debug()<<"found a mutrack "<<endmsg;
      }
    } // end of muonIDs loop
    if ( msgLevel(MSG::DEBUG) )
      debug()<<"Fatima, I out of mutrack loop"<<endreq;

    if(mutrack) {
      const std::vector< LHCb::LHCbID > lhcbIDs = mutrack->lhcbIDs();
      if ( msgLevel(MSG::DEBUG) )
        debug() <<" Starting to find the muon hit info, have   " <<lhcbIDs.size()<<" to loop over "<<endmsg;

      int countVelo(0), countIT(0), countOT(0), countTT(0), countMuon(0), countCALO(0), countRICH(0);
      std::vector< LHCb::LHCbID >::const_iterator myID = lhcbIDs.begin();
      for ( ; myID != lhcbIDs.end(); myID++ )
      {
        if ( ((*myID).isVelo()) ) countVelo++;
        if ( ((*myID).isIT()) ) countIT++;
        if ( ((*myID).isOT()) ) countOT++;
        if ( ((*myID).isTT()) ) countTT++;
        if ( ((*myID).isCalo()) ) countCALO++;
        if ( ((*myID).isRich()) ) countRICH++;

        if ( ((*myID).isMuon()) )
        {
          countMuon++;
          const LHCb::MuonTileID mutile = (*myID).muonID();
          double x,dx,y,dy,z,dz;
          StatusCode sc = m_mudet->Tile2XYZ(mutile,x,dx,y,dy,z,dz);
          if ( msgLevel(MSG::DEBUG) )
            debug()<<"Muon pad hit: "<<x<< " "<<dx<<" "<<y<<" "<<dy<<" "<<z<<" "<<dz<<". Stn: "<<mutile.station()<<" regn: "<<mutile.region()<<" qrtr: "<<mutile.quarter()<<" nX and nY "<<mutile.nX()<<" "<<mutile.nY()<<endmsg;
          m_x.push_back(x);
          m_y.push_back(y);
          m_z.push_back(z);
        }
      }
      if ( msgLevel(MSG::DEBUG) )
      {
        debug()<< "track history is " <<track->checkHistory(LHCb::Track::MuonID)  <<endmsg;
        debug()<<"=============== velo "<<countVelo<<" IT "<< countIT <<" OT "<< countOT 
               <<" TT "<< countTT<<" calo "<< countCALO<<" rich "<<countRICH<<" muon "<<countMuon
               <<endmsg;
      }
    }

    else 
    {
      if ( msgLevel(MSG::DEBUG) )
        debug()<<"While trying to get lhcbIDs, I didn't find a mutrack! "<<endmsg;
    }

    tuple->farray(prefix+"_LHCbID_X", m_x.begin(), m_x.end(), prefix+"_n", 100);
    tuple->farray(prefix+"_LHCbID_Y", m_y.begin(), m_y.end(), prefix+"_n", 100);
    tuple->farray(prefix+"_LHCbID_Z", m_z.begin(), m_z.end(), prefix+"_n", 100);

  }// m_doVerbose

  ///-------------------------------------------------------------------------------
  /// Project the state at M1 into the rest of the muon stations and store the x, y
  ///-------------------------------------------------------------------------------

  const LHCb::State *m_stateP = &(track->closestState(m_stationZ[0]));

  for(int station = 0; station < m_NStation ; station++){

    m_trackX[station] = m_stateP->x() + ( m_stateP->tx() * (m_stationZ[station] - m_stateP->z()) );
    m_trackY[station] = m_stateP->y() + ( m_stateP->ty() * (m_stationZ[station] - m_stateP->z()) );
    if ( msgLevel(MSG::DEBUG) )
      debug () <<"Station: "<<station<<".  "<< "m_trackX[station] " 
               << m_trackX[station] << " = "<<m_stateP->x() << " + " 
               << "  (" << m_stateP->tx() <<" * ("<< m_stationZ[station]
               <<" - "<<m_stateP->z()<<" ) )"<< endmsg;
  } // station

  bool test = true; 
  for(int station = 0; station < m_NStation ; station++)
  {
    std::stringstream ss;
    ss << station+1;
    test &= tuple->column(prefix+"_Xs"+ss.str(), m_trackX[station] );
    test &= tuple->column(prefix+"_Ys"+ss.str(), m_trackY[station] );
  }

  // ======================================
  // Calculate and fill DistAve and Xs, Ys
  // ======================================

  double m_momentum = m_stateP -> p();
  int nHits=0;
  int ic_hit = 0;
  //int ic_closesthit = 0;
  float dist_ave = 0;
  std::vector<double> small_dist(m_NStation);
  std::vector<double> smalldist_X(m_NStation);
  std::vector<double> smalldist_Y(m_NStation);
  std::vector<double> smalldist_Z(m_NStation);
  for ( int i = 0; i < m_NStation; ++i )
  {
    small_dist[i] = 100000.;
    smalldist_X[i] = 10000000.;
    smalldist_Y[i] = 10000000.;
    smalldist_Z[i] = 10000000.;
  }

  // store hit info if its in the FOI of the track


  for(int station = 0 ; station < m_NStation ; station++){

    for(int region = 0 ; region < m_NRegion ; region++){

      if( !m_coordPos[station*m_NRegion + region].empty() ){

        std::vector<coordExtent_>::const_iterator itPos;

        for(itPos = m_coordPos[station*m_NRegion + region].begin(); itPos != m_coordPos[station*m_NRegion + region].end(); itPos++){

          // hits in foi
          double x = itPos->m_x;
          double dx = itPos->m_dx;
          double y = itPos->m_y;
          double dy = itPos->m_dy;

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "Computing DistAve: x, dx, y, dy are   "<<x<<"  "<<dx<<"  "<<y<<"  "<<dy<< endmsg;

          double xdist = fabs(x-m_trackX[station])/dx;
          double ydist = fabs(y-m_trackY[station])/dy;
          double dist = std::sqrt( xdist*xdist + ydist*ydist  );

          if ( msgLevel(MSG::VERBOSE) )
            verbose() <<"xdist, ydist and dist are  "<<xdist<<" "<<ydist<<" "<<dist<< endmsg;

          // Look for the Closest hit to the track in any station:
          if (dist < small_dist[station])
          {
            small_dist[station] = dist;
            smalldist_X[station] = itPos->m_x;
            smalldist_Y[station] = itPos->m_y;
            smalldist_Z[station] = itPos->m_z;
            //ic_closesthit = ic_hit;
          }

          // define the FOI in which we have to evaluate the average
          double foiXDim = m_NFoi*foiX( station, region, m_momentum, dx);
          double foiYDim = m_NFoi*foiY( station, region, m_momentum, dy);

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << "Checking for FOI: "<< m_NFoi<<" "<<station<<" "<<region
                      <<" "<<m_momentum<<" "<<dx<< endmsg;

          // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  )
          {
            ++nHits;
            dist_ave = dist_ave + (xdist*xdist+ydist*ydist);

            if(m_doVerbose){
              m_hitInFOIx.push_back(x);
              m_hitInFOIdx.push_back(dx);
              m_hitInFOIy.push_back(y);
              m_hitInFOIdy.push_back(dy);
              m_hitInFOIz.push_back(itPos->m_z);
              m_hitInFOIdz.push_back(itPos->m_dz);
              m_hitInFOIID.push_back(station*m_NRegion + region);
            }

          }

          ++ic_hit;
        }// for itPos

      }// region not empty

    }// region

  } // station

  if(m_doVerbose){
    tuple->farray(prefix+"_hitInFOI_X",  m_hitInFOIx.begin(),  m_hitInFOIx.end(),  prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_dX", m_hitInFOIdx.begin(), m_hitInFOIdx.end(), prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_Y",  m_hitInFOIy.begin(),  m_hitInFOIy.end(),  prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_dY", m_hitInFOIdy.begin(), m_hitInFOIdy.end(), prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_Z",  m_hitInFOIz.begin(),  m_hitInFOIz.end(),  prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_dZ", m_hitInFOIdz.begin(), m_hitInFOIdz.end(), prefix+"_n_InFOI", 100);
    tuple->farray(prefix+"_hitInFOI_ID", m_hitInFOIID.begin(), m_hitInFOIID.end(), prefix+"_n_InFOI", 100);

    for(int station = 0; station < m_NStation ; ++station )
    {
      std::stringstream ss; ss<< station+1 ; const std::string& mychar2 = ss.str();
      test &= tuple->column(prefix+"Stn"+mychar2+"X", smalldist_X[station]);
      test &= tuple->column(prefix+"Stn"+mychar2+"Y", smalldist_Y[station]);
      test &= tuple->column(prefix+"Stn"+mychar2+"Z", smalldist_Z[station]);
    }
  }

  if (nHits>0)
  {
    dist_ave = dist_ave/nHits;
  }
  else dist_ave = dist_ave;
  
  if ( msgLevel(MSG::VERBOSE) )
    verbose() <<" So the dist_ave for track "<< track->key()<<"  ("<< track->charge() 
              << ")   will be: " << dist_ave<< endmsg;

  test &= tuple ->column ( prefix+"_DistAve", dist_ave ) ;
  return test;

} // fillVars
