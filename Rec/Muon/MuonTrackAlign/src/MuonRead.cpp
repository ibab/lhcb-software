// $Id: MuonRead.cpp,v 1.2 2007-07-23 09:37:23 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Event/Track.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiAlg/TupleObj.h" 
#include "GaudiKernel/INTupleSvc.h"
// local
#include "MuonRead.h"
using namespace LHCb;
//from LHCb
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
//-----------------------------------------------------------------------------
// Implementation file for class : MuonRead
//
// 2006-11-13 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonRead );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRead::MuonRead( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
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
  if(!m_muonBuffer)info()<<"error retriveing the toll "<<endreq;


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonRead::execute() {

  debug() << "==> Execute" << endmsg;


  Tuple Simone = nTuple(1,"Brunelmuonhit");

  Tracks* muonTracks=get<Tracks>(m_muonTracksName);

  info()<<"muon track size "<<muonTracks->size()<<endreq;

  m_muonDigit=m_muonBuffer->getTile();  //Restituisce un vettore di MuonTileID
  Tracks::const_iterator it;

  for(it=muonTracks->begin();it!=muonTracks->end();it++){
    info()<<" a muon track "<<endreq;

    double x,y,z,dx,dy,dz;
    std::vector<LHCbID >  list_of_tile=(*it)->lhcbIDs () ;
    std::vector<LHCbID>::const_iterator itTile;

    std::vector<State* >  list_of_states=(*it)->states () ;
    std::vector<State*>::const_iterator iStates;
    
       
    std::vector<double> Fx,Fy,Fz,Fdx,Fdy,Ftx,Fty,Fdtx,Fdty,Fp;
    std::vector<double> residual, residualerr, measurementerr ;

    std::vector<double> Vxm,Vym,Vzm,Vdxm,Vdym,Vdzm,region,station;
    std::vector <double> MCID,MCmothID,MCp,MCtx,MCty,MCVx,MCVy,MCVz;
    double Fchi2;

    Fchi2=((*it)->chi2()); //it è un iteratore sulle tracce, punta alle tracce!!

    for(iStates=list_of_states.begin();iStates!=list_of_states.end();iStates++){
      Fx.push_back((*iStates)->x());
      Fy.push_back((*iStates)->y());
      Fz.push_back((*iStates)->z());
      Fdx.push_back(sqrt((*iStates)->errX2()));
      Fdy.push_back(sqrt((*iStates)->errY2()));

      Ftx.push_back((*iStates)->tx());
      Fty.push_back((*iStates)->ty());

      Fdtx.push_back(sqrt((*iStates)->errTx2()));
      Fdty.push_back(sqrt((*iStates)->errTy2())); 
      Fp.push_back((*iStates)->qOverP());       
    }
    Simone->farray("Fx",Fx.begin(),Fx.end(),"Nstate",10);
    Simone->farray("Fy",Fy.begin(),Fy.end(),"Nstate",10);
    Simone->farray("Fz",Fz.begin(),Fz.end(),"Nstate",10);
    Simone->farray("Fdx",Fdx.begin(),Fdx.end(),"Nstate",10);
    Simone->farray("Fdy",Fdy.begin(),Fdy.end(),"Nstate",10);
    Simone->farray("Ftx",Ftx.begin(),Ftx.end(),"Nstate",10);
    Simone->farray("Fty",Fty.begin(),Fty.end(),"Nstate",10);
    Simone->farray("Fdtx",Fdtx.begin(),Fdtx.end(),"Nstate",10);
    Simone->farray("Fdty",Fdty.begin(),Fdty.end(),"Nstate",10);
    Simone->farray("Fp",Fp.begin(),Fp.end(),"Nstate",10);

    // loop over track nodes const std::vector<LHCb::Node*>
    const std::vector<LHCb::Node*>& nodes = (*it)->nodes() ; 
    for( std::vector<LHCb::Node*>::const_iterator inode = nodes.begin() ;
	 inode != nodes.end() ; ++inode) {
      residual.push_back((*inode)->residual()) ;
      residualerr.push_back(sqrt((*inode)->errResidual2())) ;
      measurementerr.push_back(sqrt((*inode)->errMeasure2())) ;
    }
    Simone->farray("Fres",residual.begin(),residual.end(),"Nnode",10);
    Simone->farray("Freserr",residualerr.begin(),residualerr.end(),"Nnode",10);
    Simone->farray("Fmeaserr",measurementerr.begin(),measurementerr.end(),"Nnode",10);


    Simone->fill("Fchi2",Fchi2);  
    Simone->fill("ndof",float((*it)->nDoF())) ;
    Simone->fill("status",float((*it)->fitStatus())) ;

    for(itTile=list_of_tile.begin();itTile!=list_of_tile.end();itTile++){
      MuonTileID tile=itTile->muonID();
      
      m_muonDet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
      info()<<"*** tile position ***"<<tile<<endreq;
      info()<<" x = "<<x<<" y = "<<y<<" z = "<<z<<endreq;
      info()<<" dx = "<<dx<<" dy ="<<dy<<" dz = "<<dz<<endreq;
      info()<<"*********************"<<tile<<endreq;

      Vxm.push_back(x);
      Vym.push_back(y);
      Vzm.push_back(z);
      Vdxm.push_back(dx);
      Vdym.push_back(dy);
      Vdzm.push_back(dz);
      region.push_back(tile.region());
      station.push_back(tile.station());

      MCParticle* pp=NULL;
      searchNature(tile,pp);
      if(pp!=NULL){
        info()<<pp->particleID().pid()<<endreq;
        MCID.push_back(double(pp->particleID().pid()));
        if(pp->mother()!=0) {
          info()<< " found a mother "<<endreq;  
          MCmothID.push_back(double(pp->mother()->particleID().pid()));
        }
        
        MCp.push_back(pp->p());
        info()<<"MCParticle momentum "<<pp->momentum()<<endreq;
        
        MCtx.push_back(pp->momentum().px()/pp->momentum().pz());
        MCty.push_back(pp->momentum().py()/pp->momentum().pz());
        MCVx.push_back(pp->originVertex()->position().x());
        MCVy.push_back(pp->originVertex()->position().y());
        MCVz.push_back(pp->originVertex()->position().z());
      }else info()<<" no part "<<endreq;

      
    }

    Simone->farray("R",region.begin(),region.end(),"Npad",10);
    Simone->farray("x",Vxm.begin(),Vxm.end(),"Npad",10);
    Simone->farray("y",Vym.begin(),Vym.end(),"Npad",10);
    Simone->farray("z",Vzm.begin(),Vzm.end(),"Npad",10);
    Simone->farray("dx",Vdxm.begin(),Vdxm.end(),"Npad",10);
    Simone->farray("dy",Vdym.begin(),Vdym.end(),"Npad",10);
    Simone->farray("dz",Vdzm.begin(),Vdzm.end(),"Npad",10);
    Simone->farray("station",station.begin(),station.end(),"Npad",10);
    Simone->farray("MCID",MCID.begin(),MCID.end(),"Npad",10);
    Simone->farray("MCmothID",MCmothID.begin(),MCmothID.end(),"Npad",10);
    Simone->farray("MCtx",MCtx.begin(),MCtx.end(),"Npad",10);
    Simone->farray("MCty",MCty.begin(),MCty.end(),"Npad",10);
    Simone->farray("MCp",MCp.begin(),MCp.end(),"Npad",10);
    Simone->farray("MCVx",MCVx.begin(),MCVx.end(),"Npad",10);
    Simone->farray("MCVy",MCVy.begin(),MCVy.end(),"Npad",10);
    Simone->farray("MCVz",MCVz.begin(),MCVz.end(),"Npad",10);
   

    float x_hit[5]={0,0,0,0,0},y_hit[5]={0,0,0,0,0},z_hit[5] ={0,0,0,0,0};
    float tx_hit[5]={0,0,0,0,0},ty_hit[5]={0,0,0,0,0};
    int counter[5]={0,0,0,0,0}; 
    int iMS =0;

    LHCb::MCHits* muonhits = get< LHCb::MCHits > (LHCb::MCHitLocation::Muon );

    if(muonhits!=NULL){
      for( LHCb::MCHits::const_iterator iMc = muonhits->begin(); iMc != muonhits->end(); ++iMc ) {
        if((*iMc)->mcParticle()!=NULL){
          if(abs((*iMc)->mcParticle()->particleID().pid())==13) {
            
            if((*iMc)->midPoint().z()>8000 && (*iMc)->midPoint().z()<14000) iMS=0;
            if((*iMc)->midPoint().z()>15000 && (*iMc)->midPoint().z()<16000) iMS=1;
            if((*iMc)->midPoint().z()>16000 && (*iMc)->midPoint().z()<17000) iMS=2;
            if((*iMc)->midPoint().z()>17000 && (*iMc)->midPoint().z()<18000) iMS=3;
            if((*iMc)->midPoint().z()>18000 && (*iMc)->midPoint().z()<20000) iMS=4;
            if(iMS<0||iMS>4) continue;
        
            x_hit[iMS] +=   (*iMc)->midPoint().x();
            y_hit[iMS] +=   (*iMc)->midPoint().y();
            z_hit[iMS] +=   (*iMc)->midPoint().z();
	    tx_hit[iMS]= (*iMc)->dxdz(); 
	    ty_hit[iMS]=  (*iMc)->dydz();
            
            counter[iMS]++;
          }
        }
      }
      
      for(int iMS=0;iMS<5;iMS++){
        if(counter[iMS]>0){
          x_hit[iMS] /=counter[iMS]; 
          y_hit[iMS] /=counter[iMS]; 
          z_hit[iMS] /=counter[iMS]; 
        }
      }
    }

    Simone->array("x_hit",x_hit,5);
    Simone->array("y_hit",y_hit,5);
    Simone->array("z_hit",z_hit,5);
    Simone->array("tx_hit",tx_hit,5);
    Simone->array("ty_hit",ty_hit,5);
    
    Simone->write();

  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonRead::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

StatusCode MuonRead::searchNature(MuonTileID tile,MCParticle*& pp)
{
  pp=NULL;

  //  unsigned int station=tile.station();
  //  unsigned int region=tile.region();

  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>
    myLink( eventSvc(), msgSvc(),LHCb::MuonDigitLocation::MuonDigit);
  if(myLink.notFound()) debug()<<" my link not found "<<endmsg;

  debug()<<" my link found "<<endmsg;
  std::vector<LHCb::MuonTileID>::iterator iDigit;

  MCParticle* ppfirst=NULL;

  bool first=true;
  bool second=false;

  for(iDigit = m_muonDigit.begin(); iDigit != m_muonDigit.end(); iDigit++){
    MuonTileID digitile=(*iDigit);
    if((digitile.intercept(tile)).isValid()){
      debug()<<" find the digit corresponding to tile "<<first<<endreq;
       pp = myLink.first( digitile );   
       if(first){
         ppfirst=pp;
         first=false;
       }else{
         second=true;
         if(pp!=NULL&&pp==ppfirst)  return StatusCode::SUCCESS;
       }
    }else{
      debug()<<"digitile.intercept(tile)).is NOT Valid"<<endreq;
    }
    

  }
  if(!second){
    if(ppfirst!=NULL){
      pp=ppfirst;
      return StatusCode::SUCCESS;
    }

  }
  info()<< "MCParticle not found"<<endreq;
  

           
  return StatusCode::SUCCESS;
};
  
