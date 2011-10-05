// Include files
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Particle.h"  
// kernel 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticle2MCAssociator.h"
//#include "Kernel/Particle2MCLinker.h"
#include <Kernel/DVAlgorithm.h>
#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
#include "Kernel/IOnOffline.h"
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
#include "TMath.h"


//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMuonIDCalib
//
// 2011-09-23 : Fatima Soomro 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TupleToolMuonIDCalib );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMuonIDCalib::TupleToolMuonIDCalib( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent) 
  : TupleToolBase ( type, name , parent )
  , m_dva(0)
					 

{
  declareInterface<IParticleTupleTool>(this);
  declareProperty ( "NFoi"          ,m_NFoi  = 2 ) ;
  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );
  
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolMuonIDCalib::~TupleToolMuonIDCalib() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TupleToolMuonIDCalib::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  
  //std::cout<<"Fatima: initialize TupleToolMuIDCalib"<<std::endl;

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode TupleToolMuonIDCalib::fill( const LHCb::Particle *top, const LHCb::Particle *part,
                                          const std::string &  	head, Tuples::Tuple &  	tuple	) {
  

  // nothing for particles that are not Bs
  if( part->isBasicParticle()  || abs(part->particleID().pid())<500  ) 
    return StatusCode::SUCCESS; 
  
  
  // check if the B had only two muon daughters, i.e. its B->2 body
  int countDaughters =0; 
  const SmartRefVector< LHCb::Particle > Bdaughters = part->daughters();
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = Bdaughters.begin() ; idau != Bdaughters.end() ; ++idau)
    {
      if ( (*idau )->isBasicParticle() && abs((*idau )->particleID().pid()) == 13 )
	countDaughters++;  
    }
  
  if (countDaughters != 2) {
    debug ()<<"For particle with ID "<< part->particleID().pid() <<"  the number of muon daughters is "<<countDaughters<<" !!! Not filling the variables" <<endmsg;
    return StatusCode::SUCCESS; 

  }
  else debug()<<"For particle with ID "<< part->particleID().pid() <<"  the number of muon daughters is "<<countDaughters<<" good!" <<endmsg;
  
    
  LoadMuonGeometry();
  debug () <<"Fatima: Loaded Geometry in TupleToolMuIDCalib"<< endmsg;
  
  StatusCode sc;
  
  sc=fillCoordVectors();
  if(!sc) error()<<"couldnt fillCoorVectors!!!"<<endmsg;
  debug() <<"Fatima: fillCoordVectors in TupleToolMuIDCalib"<< endmsg;
  
  debug()<<"Looping over "<<countDaughters<<"  daughters and filling muID variables "  <<endmsg;
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = Bdaughters.begin() ; idau != Bdaughters.end() ; ++idau)
    {
      const LHCb::Particle *p = (*idau);
      std::string  name = "muplus";
      if( p->charge() <0) name = "muminus";
      
      debug () << "for particle " << p->particleID().pid() << "  the prefix is  " <<name<<endmsg;
      sc = fillVars( p, name , tuple );
      
    }
  
  if (!sc) return sc;
  
  return StatusCode::SUCCESS;
  
}


//==========================================================================
// Load the geometry
//==========================================================================

void TupleToolMuonIDCalib::LoadMuonGeometry(){
  
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
  return;
  
}



//==========================================================================
// fill vectors of x,y,z positions for the MuonCoords
//==========================================================================

StatusCode TupleToolMuonIDCalib::fillCoordVectors(){
  
  //m_HitInTrk.clear();
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  
  
  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords =NULL;
  if(exist<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords))
    coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
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
      //m_HitInTrk.push_back(false);
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
  double temp_x =  ( m_xfoiParam1[ station * m_NRegion + region ] +
		     m_xfoiParam2[ station * m_NRegion + region ]*
		     exp(-m_xfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dx;
  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
  
  verbose()<< "from FOIX "<<temp_x<< endmsg;
  
  return temp_x;
}


// return the FOI in y in a station and region for momentum (in MeV/c)
//=============================================================================
double TupleToolMuonIDCalib::foiY(const int &station, const int &region, const double &p, 
				  const double &dy){
  //=============================================================================
  double temp_y = ( m_yfoiParam1[ station * m_NRegion + region ] +
		    m_yfoiParam2[ station * m_NRegion + region ]*
		    exp(-m_yfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dy;
  
  verbose() <<"from FOIY "<<temp_y<< endmsg;
  
  return temp_y;
}


//=============================================================================
//  fill Xs, Ys and DistAve
//=============================================================================
StatusCode TupleToolMuonIDCalib::fillVars(  const LHCb::Particle *part,
					    const std::string prefix, Tuples::Tuple &tuple) {
  
  
  if( (!part->isBasicParticle() ) ||  NULL==part->proto() ||  abs(part->particleID().pid()) != 13 ) {
    error () << "!! Tried to fill MuonIDCalib quantities for particle " <<part->particleID().pid()<< "!!!  This should not happen"<< endmsg;
    return StatusCode::SUCCESS; 
  }
  
  const LHCb::Track*  track= part->proto()->track();
  if((track->checkFlag(LHCb::Track::Clone))  ||
     !( track->checkType(LHCb::Track::Long) || track->checkType(LHCb::Track::Downstream) 
	|| track->checkType(LHCb::Track::Ttrack) )){
    return StatusCode::SUCCESS ; 
  }
  
  const LHCb::State *m_stateP = &(track->closestState(m_stationZ[0]));
  
  // Project the state into the muon stations
  for(int station = 0; station < m_NStation ; station++){
    
    m_trackX[station] = m_stateP->x() + ( m_stateP->tx() * (m_stationZ[station] - m_stateP->z()) );
    m_trackY[station] = m_stateP->y() + ( m_stateP->ty() * (m_stationZ[station] - m_stateP->z()) );
    
    debug () <<"Station: "<<station<<".  "<< "m_trackX[station] " << m_trackX[station] << " = "<<m_stateP->x() << " + " << "  (" << m_stateP->tx() <<" * ("<< m_stationZ[station]<<" - "<<m_stateP->z()<<" ) )"<< endmsg;
  } // station
  
  bool test = true; char mychar[100];
  
  for(int station = 0; station < m_NStation ; station++){
    sprintf( mychar, "%i", station+1 );
    test &= tuple->column(prefix+"_Xs"+mychar, m_trackX[station] );
    test &= tuple->column(prefix+"_Ys"+mychar, m_trackY[station] );
    
  }
  
  // ======================================
  // Calculate and fill DistAve and Xs, Ys
  // ======================================
  
  debug() <<"Filled the X/Ys " << endmsg;
  double m_momentum = m_stateP -> p();
  int nHits=0;
  float dist_ave =0;
  double small_dist[m_NStation];
  for (int i=0;i<m_NStation;i++){
    small_dist[i] = 100000.;
  }
  
    
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
	  
	  verbose() << "Computing DistAve: x, dx, y, dy are   "<<x<<"  "<<dx<<"  "<<y<<"  "<<dy<< endmsg;
	  
          double xdist = fabs(x-m_trackX[station])/dx;
          double ydist = fabs(y-m_trackY[station])/dy;
          double dist = sqrt( xdist*xdist + ydist*ydist  );
	  
	  verbose() <<"xdist, ydist and dist are  "<<xdist<<" "<<ydist<<" "<<dist<< endmsg;
	  
	  // Look for the Closest hit to the track in any station:	  
          if (dist < small_dist[station]) {
	    small_dist[station] = dist;
	  }
	  
	  // define the FOI in which we have to evaluate the average          
          double foiXDim = m_NFoi*foiX( station, region, m_momentum, dx);          
          double foiYDim = m_NFoi*foiY( station, region, m_momentum, dy);
	  verbose() << "Checking for FOI: "<< m_NFoi<<" "<<station<<" "<<region<<" "<<m_momentum<<" "<<dx<< endmsg;
	  
	  // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  ) 
	    {          
	      nHits++;
	      dist_ave = dist_ave + (xdist*xdist+ydist*ydist);            
	    }
	  
	  
	}// for itPos

      }// region not empty
      
    }// region

  } // station
  
  if (nHits>0) {
    dist_ave = dist_ave/nHits;
  }
  else dist_ave = dist_ave;
  verbose() <<" So the dist_ave for track "<< track->key()<<"  ("<< track->charge() << ")   will be: " << dist_ave<< endmsg;
  
  test &= tuple ->column ( prefix+"_DistAve", dist_ave ) ;
  return test;
  
} // fillVars



//============================================================================= 
//    Finalize 
//============================================================================= 

StatusCode TupleToolMuonIDCalib::finalize() {
  return StatusCode::SUCCESS;
}
