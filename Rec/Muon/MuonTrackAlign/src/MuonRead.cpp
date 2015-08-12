// Include files 

// from Gaudi
#include "GaudiKernel/NTuple.h"
#include "GaudiAlg/TupleObj.h" 
#include "GaudiKernel/INTupleSvc.h"
// local
#include "MuonRead.h"

//from LHCb
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
#include "Event/Track.h"
#include "Event/Node.h"

#include <boost/foreach.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : MuonRead
//
// 2006-11-13 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonRead )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRead::MuonRead( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_muonDet(NULL)
  , m_muonBuffer(NULL)
{
 declareProperty( "MuonTracksName" ,
                   m_muonTracksName="/Event/Rec/Muon/MuonsForAlignment");
}
//=============================================================================
// Destructor
//=============================================================================
MuonRead::~MuonRead() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonRead::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_muonDet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if (!m_muonBuffer) info()<<"error retrieving MuonRawBuffer "<<endmsg;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonRead::execute() {

  debug() << "==> Execute" << endmsg;


  Tuple Simone = nTuple(1,"Brunelmuonhit");

  LHCb::Tracks* muonTracks=get<LHCb::Tracks>(m_muonTracksName);

  debug()<<"muon track size "<<muonTracks->size()<<endmsg;

  
  StatusCode sc=m_muonBuffer->getTile(m_muonDigit);  //return a vector of MuonTileID
  if (sc.isFailure())return sc;
  LHCb::Tracks::const_iterator it;

  for(it=muonTracks->begin();it!=muonTracks->end();it++){

    double x,y,z,dx,dy,dz;
    std::vector<LHCb::LHCbID>  list_of_tile=(*it)->lhcbIDs () ;
    std::vector<LHCb::LHCbID>::const_iterator itTile;

       
    std::vector<double> Fx,Fy,Fz,Fdx,Fdy,Ftx,Fty,Fdtx,Fdty,Fp;
    std::vector<double> residual, residualerr, measurementerr,nodez ;

    std::vector<double> Vxm,Vym,Vzm,Vdxm,Vdym,Vdzm,region,station,Chamber;
    std::vector <double> MCID,MCmothID,MCp,MCtx,MCty,MCVx,MCVy,MCVz;
    double Fchi2;

    Fchi2=((*it)->chi2()); //it track iterator!!

    // loop over track nodes const std::vector<LHCb::Node*>
    int m = 1;
    BOOST_FOREACH( const LHCb::Node* node, (*it)->nodes() ) {
      debug()<<" node information "<< node->z();
      
      if ( node->z() > 11900 && node->type()!=LHCb::Node::Outlier  ) {
        debug() << " ----  in the Muon detector "<< endmsg;
        const LHCb::State& state =node->state();
	
        Fx.push_back(state.x());
        Fy.push_back(state.y());
        Fz.push_back(state.z());
        Fdx.push_back(sqrt(state.errX2()));
        Fdy.push_back(sqrt(state.errY2()));
        
        Ftx.push_back(state.tx());
        Fty.push_back(state.ty());
        
        Fdtx.push_back(sqrt(state.errTx2()));
        Fdty.push_back(sqrt(state.errTy2())); 
        Fp.push_back(state.qOverP());       
        
        nodez.push_back(node->z());
        residual.push_back(node->residual());
        residualerr.push_back(std::sqrt(node->errResidual2())) ;
        measurementerr.push_back(std::sqrt(node->errMeasure2())) ;
        m++;
        
      }      
    }
    if (m<=30){
      
      Simone->farray("Fx",Fx.begin(),Fx.end(),"Nnode",30);   // 30 is the maximum number of pads per track allowed
      Simone->farray("Fy",Fy.begin(),Fy.end(),"Nnode",30);
      Simone->farray("Fz",Fz.begin(),Fz.end(),"Nnode",30);
      Simone->farray("Fdx",Fdx.begin(),Fdx.end(),"Nnode",30);
      Simone->farray("Fdy",Fdy.begin(),Fdy.end(),"Nnode",30);
      Simone->farray("Ftx",Ftx.begin(),Ftx.end(),"Nnode",30);
      Simone->farray("Fty",Fty.begin(),Fty.end(),"Nnode",30);
      Simone->farray("Fdtx",Fdtx.begin(),Fdtx.end(),"Nnode",30);
      Simone->farray("Fdty",Fdty.begin(),Fdty.end(),"Nnode",30);
      Simone->farray("Fp",Fp.begin(),Fp.end(),"Nnode",30);
      
      Simone->farray("nodez",nodez.begin(),nodez.end(),"Nnode",30);
      Simone->farray("Fres",residual.begin(),residual.end(),"Nnode",30);
      Simone->farray("Freserr",residualerr.begin(),residualerr.end(),"Nnode",30);
      Simone->farray("Fmeaserr",measurementerr.begin(),measurementerr.end(),"Nnode",30);
      
      
      Simone->fill("Fchi2",Fchi2);  
      Simone->fill("ndof",float((*it)->nDoF())) ;
      Simone->fill("status",float((*it)->fitStatus())) ;
    } else info() << "More than 30 pads per track" << endmsg;

    int p = 1;
    
    for (itTile=list_of_tile.begin();itTile!=list_of_tile.end();itTile++){
      if ((*itTile).isMuon() == true) {
        LHCb::MuonTileID tile = itTile->muonID();
        std::vector<DeMuonChamber*> vchambers;

        m_muonDet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
        vchambers = m_muonDet->Tile2Chamber(tile);
        Chamber.push_back(float(vchambers[0]->chamberNumber()));
        debug()<<"*** tile position ***"<<tile<<endmsg;
        debug()<<" x = "<<x<<" y = "<<y<<" z = "<<z<<endmsg;
        debug()<<" dx = "<<dx<<" dy ="<<dy<<" dz = "<<dz<<endmsg;
        debug()<<" region "<<tile.region()<<" station "<<tile.station()<<endmsg;
        debug()<<"*********************"<<tile<<endmsg;
        
        Vxm.push_back(x);
        Vym.push_back(y);
        Vzm.push_back(z);
        Vdxm.push_back(dx);
        Vdym.push_back(dy);
        Vdzm.push_back(dz);
        region.push_back(double(tile.region()));
        station.push_back(double(tile.station()));
        p++;
        
        LHCb::MCParticle* pp=NULL;
        searchNature(tile,pp);
        if (pp!=NULL){
          debug() << "MC particle linked to tile " << pp->particleID().pid() << endmsg;
          MCID.push_back(double(pp->particleID().pid()));
          if (pp->mother()!=0) {
            debug() << " found a mother "<<endmsg;  
            MCmothID.push_back(double(pp->mother()->particleID().pid()));
          } else {
	    MCmothID.push_back(0);
	  }
          
          MCp.push_back(pp->p());
          debug() << "MCParticle momentum "<<pp->momentum()<<endmsg;
          
          MCtx.push_back(pp->momentum().px()/pp->momentum().pz());
          MCty.push_back(pp->momentum().py()/pp->momentum().pz());
          MCVx.push_back(pp->originVertex()->position().x());
          MCVy.push_back(pp->originVertex()->position().y());
          MCVz.push_back(pp->originVertex()->position().z());
        } else {
	  debug() << " No MC particle linked to tile" << endmsg;
	  
	  MCp.push_back(0);
	  MCtx.push_back(0);
	  MCty.push_back(0);
	  MCVx.push_back(0);
	  MCVy.push_back(0);
	  MCVz.push_back(0);
	  
        }
        
      } else   debug()<<" this LHCbID is not a MuonTile "<<endmsg;
      
    }
    
    if (p<=30){
      
      Simone->farray("x",Vxm.begin(),Vxm.end(),"Npad",30);
      Simone->farray("y",Vym.begin(),Vym.end(),"Npad",30);
      Simone->farray("z",Vzm.begin(),Vzm.end(),"Npad",30);
      Simone->farray("dx",Vdxm.begin(),Vdxm.end(),"Npad",30);
      Simone->farray("dy",Vdym.begin(),Vdym.end(),"Npad",30);
      Simone->farray("dz",Vdzm.begin(),Vdzm.end(),"Npad",30);
      Simone->farray("station",station.begin(),station.end(),"Npad",30);
      Simone->farray("R",region.begin(),region.end(),"Npad",30);
      Simone->farray("NCh",Chamber.begin(),Chamber.end(),"Npad",30);
      Simone->farray("MCID",MCID.begin(),MCID.end(),"Npad",30);
      Simone->farray("MCmothID",MCmothID.begin(),MCmothID.end(),"Npad",30);
      Simone->farray("MCtx",MCtx.begin(),MCtx.end(),"Npad",30);
      Simone->farray("MCty",MCty.begin(),MCty.end(),"Npad",30);
      Simone->farray("MCp",MCp.begin(),MCp.end(),"Npad",30);
      Simone->farray("MCVx",MCVx.begin(),MCVx.end(),"Npad",30);
      Simone->farray("MCVy",MCVy.begin(),MCVy.end(),"Npad",30);
      Simone->farray("MCVz",MCVz.begin(),MCVz.end(),"Npad",30);
    }
    

    float x_hit[5] = {0,0,0,0,0}, y_hit[5] = {0,0,0,0,0}, z_hit[5] = {0,0,0,0,0};
    float tx_hit[5] = {0,0,0,0,0}, ty_hit[5] = {0,0,0,0,0};
    int counter[5] = {0,0,0,0,0}; 
    int iMS = 0;
    
    LHCb::MCHits* muonhits = get< LHCb::MCHits > (LHCb::MCHitLocation::Muon );

    if (muonhits!=NULL){
      for ( LHCb::MCHits::const_iterator iMc = muonhits->begin(); iMc != muonhits->end(); ++iMc ) {
        if ((*iMc)->mcParticle()!=NULL){
          if (abs((*iMc)->mcParticle()->particleID().pid())==13) {
            
            if ((*iMc)->midPoint().z()>10000 && (*iMc)->midPoint().z()<14000) iMS=0;
            if ((*iMc)->midPoint().z()>15000 && (*iMc)->midPoint().z()<16000) iMS=1;
            if ((*iMc)->midPoint().z()>16000 && (*iMc)->midPoint().z()<17000) iMS=2;
            if ((*iMc)->midPoint().z()>17000 && (*iMc)->midPoint().z()<18000) iMS=3;
            if ((*iMc)->midPoint().z()>18000 && (*iMc)->midPoint().z()<20000) iMS=4;
            if (iMS<0||iMS>4) continue;
        
            x_hit[iMS] += (float) (*iMc)->midPoint().x();
            y_hit[iMS] += (float) (*iMc)->midPoint().y();
            z_hit[iMS] += (float) (*iMc)->midPoint().z();
            tx_hit[iMS] = (float) (*iMc)->dxdz(); 
            ty_hit[iMS] = (float) (*iMc)->dydz();
            
            counter[iMS]++;
          }
        }
      }
      
      for (int iMS=0;iMS<5;iMS++){
        if (counter[iMS]>0){
          x_hit[iMS] /=counter[iMS]; 
          y_hit[iMS] /=counter[iMS]; 
          z_hit[iMS] /=counter[iMS]; 
        }
      }
    
      
      Simone->array("x_hit",x_hit,5);
      Simone->array("y_hit",y_hit,5);
      Simone->array("z_hit",z_hit,5);
      Simone->array("tx_hit",tx_hit,5);
      Simone->array("ty_hit",ty_hit,5);
    }

    Simone->write();
    debug()<<" The muon track has "
	   << list_of_tile.size()<<" Tiles and "<<  (*it)->nodes().size()<<" nodes "<< endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode MuonRead::searchNature(LHCb::MuonTileID tile,LHCb::MCParticle*& pp)
{
  pp=NULL;
  
  //  unsigned int station=tile.station();
  //  unsigned int region=tile.region();
  
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>
    myLink( eventSvc(), msgSvc(),LHCb::MuonDigitLocation::MuonDigit);
  if (myLink.notFound()) debug()<<" my link not found "<<endmsg;

  debug()<<" my link found "<<endmsg;
  std::vector<LHCb::MuonTileID>::iterator iDigit;

  LHCb::MCParticle* ppfirst=NULL;

  bool first=true;
  bool second=false;

  for (iDigit = m_muonDigit.begin(); iDigit != m_muonDigit.end(); iDigit++){
    LHCb::MuonTileID digitile=(*iDigit);
    if ((digitile.intercept(tile)).isValid()){
      debug()<<" find the digit corresponding to tile "<<first<<endmsg;
       pp = myLink.first( digitile );   
       if (first){
         ppfirst=pp;
         first=false;
       } else {
         second=true;
         if (pp!=NULL&&pp==ppfirst)  return StatusCode::SUCCESS;
       }
    } else {
      debug()<<"digitile.intercept(tile)) is NOT Valid"<<endmsg;
    }
    
    
  }
  if (!second){
    if (ppfirst!=NULL){
      pp=ppfirst;
      return StatusCode::SUCCESS;
    }

  }
  debug() << "MCParticle not found" << endmsg;
  

           
  return StatusCode::SUCCESS;
}
