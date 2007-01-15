// $Id: MuonPad2MCTool.cpp,v 1.2 2007-01-15 16:42:36 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MuonPad2MCTool.h"
#include "Event/IntLink.h"
#include "Event/MCMuonDigit.h"
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"
#include "Event/MuonDigit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPad2MCTool
//
// 2006-12-06 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonPad2MCTool );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPad2MCTool::MuonPad2MCTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMuonPad2MCTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MuonPad2MCTool::~MuonPad2MCTool() {} 

//=============================================================================

StatusCode MuonPad2MCTool::initialize()
{
 GaudiTool::initialize();  

  m_muonDetector=getDet<DeMuonDetector>
    ("/dd/Structure/LHCb/DownstreamRegion/Muon");
  return StatusCode::SUCCESS;
  
}

StatusCode MuonPad2MCTool::finalize() {  
  return GaudiTool::finalize() ;
}
 




MCParticle* MuonPad2MCTool::PadNoXtalk2MC(LHCb::MuonTileID tile,
                                   std::vector<LHCb::MuonTileID>& list_digit)
{
  MCParticle* pp=NULL;
  MCParticle* pplink=NULL;
  MCParticle* ppfirst=NULL;
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>
    myLink( evtSvc(), msgSvc(),LHCb::MuonDigitLocation::MuonDigit);
  if(myLink.notFound())info()<<" my link not found "<<endmsg;
  std::vector<LHCb::MuonTileID>::iterator iDigit;
  
  bool first=true;
  bool second=false;
  
  for(iDigit = list_digit.begin(); iDigit != list_digit.end(); iDigit++){
    MuonTileID digitile=(*iDigit);
    if((digitile.intercept(tile)).isValid()){
      //     info()<<" find the digit corresponding to tile "<<first<<endreq;
      pplink = myLink.first( digitile );
      //info()<<pp<<endreq;
      if(first){
        ppfirst=pplink;
        first=false;
      }else{
        second=true;
        
        if(pplink!=NULL&&pplink==ppfirst) return pplink;       
      }
    }
  }
  
  if(!second){
    if(ppfirst!=NULL){
      pp=ppfirst; 
      return pp;
    }    
  }  
  return pp;
  
}

MCParticle* MuonPad2MCTool::Pad2MC(LHCb::MuonTileID tile,
                                   std::vector<LHCb::MuonTileID>& list_digit)
{
  MCParticle* pp=NULL;
  MCParticle* pplink=NULL;

  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>
    myLink( evtSvc(), msgSvc(),LHCb::MuonDigitLocation::MuonDigit);
  if(myLink.notFound())info()<<" my link not found "<<endmsg;
  std::vector<LHCb::MuonTileID>::iterator iDigit;
  
  
  for(iDigit = list_digit.begin(); iDigit != list_digit.end(); iDigit++){
    MuonTileID digitile=(*iDigit);
    if((digitile.intercept(tile)).isValid()){
        //   info()<<" find the digit corresponding to tile "<<endreq;
      pplink = myLink.first( digitile );
 //info()<<" find the digit corresponding to tile "<<pplink<<endreq;
      if(pplink!=NULL){
        if(pplink->particleID().abspid()==13)return pplink;
      }
      
      if(pplink!=NULL)pp=pplink;
      
    }
  }
  return pp;
  
}

bool MuonPad2MCTool::isXTalk(LHCb::MuonTileID tile,MCParticle*& pp,
                       std::vector<LHCb::MuonTileID>& list_digit){

   bool xt=false;   
   if(tile.station()==0){
     XtalkPad(tile,pp,list_digit);
     if(pp!=NULL) xt=true;
   }else if(tile.station()==3&&tile.region()==0){
     XtalkPad(tile,pp,list_digit);
     // if(pp!=NULL)info()<<"found a muon "<<pp->particleID().pid()<<endreq;
     if(pp!=NULL) xt=true;
     
   }else if(tile.station()==4&&tile.region()==0){
     XtalkPad(tile,pp,list_digit);
     if(pp!=NULL) xt=true;

   }else {
     XtalkStrip(tile,pp,list_digit);
     if(pp!=NULL) xt=true;
   }
   return xt;
}




StatusCode MuonPad2MCTool::XtalkPad(MuonTileID tile,MCParticle*& pp,std::vector<LHCb::MuonTileID>& list_digit)
{
  IntLink* link=get<IntLink>(LHCb::MCMuonDigitLocation::MCMuonDigit + "Info");
  if(link){
    // info()<<" link found"<<endreq;
    int  stored=link->link(tile);
    //info()<<stored<<endreq;
    MCMuonDigitInfo mc;
    mc.setDigitInfo(stored);
    if(mc.isXTalkHit()){
      //info()<<" cross talk hits !!!"<<endreq;
      MuonTileID top=tile.neighbourID(0,1);
      MuonTileID bottom=tile.neighbourID(0,-1);
      MuonTileID left=tile.neighbourID(-1,0);
      MuonTileID rigth=tile.neighbourID(1,0);
      if(top.isValid())pp=Pad2MC(top,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(bottom.isValid())pp=Pad2MC(bottom,list_digit);
      if(pp!=NULL){   
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;  
      }                
      if(left.isValid()) pp=Pad2MC(left,list_digit);
      if(pp!=NULL){   
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(rigth.isValid())pp=Pad2MC(rigth,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
    }
  }

  return StatusCode::SUCCESS;
}




StatusCode MuonPad2MCTool::XtalkStrip(MuonTileID tile,MCParticle*& pp,std::vector<LHCb::MuonTileID>& list_digit)
{
  MuonTileID uno;
  MuonTileID due;
  MuonLayout layoutone(m_muonDetector->
                       getLayoutX(0,tile.station(),tile.region()),
                       m_muonDetector->
                       getLayoutY(0,tile.station(),tile.region()));
  MuonLayout layoutdue(m_muonDetector->
                       getLayoutX(1,tile.station(),tile.region()),
                       m_muonDetector->
                       getLayoutY(1,tile.station(),tile.region()));
  uno=tile.containerID(layoutone);
  IntLink* link=get<IntLink>(LHCb::MCMuonDigitLocation::MCMuonDigit + "Info");
  if(link){
//    info()<<" link found"<<endreq;
    int  storedone=link->link(uno);
//    info()<<storedone<<endreq;
    MCMuonDigitInfo mcone;
    mcone.setDigitInfo(storedone);
    if(mcone.isXTalkHit()){
//      info()<<" cross talk hits !!!"<<endreq;
      MuonTileID top=uno.neighbourID(0,1);
      MuonTileID bottom=uno.neighbourID(0,-1);
      MuonTileID left=uno.neighbourID(-1,0);
      MuonTileID rigth=uno.neighbourID(1,0);
      if(top.isValid())pp=Pad2MC(top,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(bottom.isValid())pp=Pad2MC(bottom,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(left.isValid())pp=Pad2MC(left,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(rigth.isValid())pp=Pad2MC(rigth,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
    }
    int  storeddue=link->link(due);
//    info()<<storeddue<<endreq;
    MCMuonDigitInfo mcdue;
    mcdue.setDigitInfo(storeddue);
    if(mcdue.isXTalkHit()){
//      info()<<" cross talk hits !!!"<<endreq;
      MuonTileID top=due.neighbourID(0,1);
      MuonTileID bottom=due.neighbourID(0,-1);
      MuonTileID left=due.neighbourID(-1,0);
      MuonTileID rigth=due.neighbourID(1,0);  
      if(top.isValid())pp=Pad2MC(top,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(bottom.isValid())pp=Pad2MC(bottom,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(left.isValid())pp=Pad2MC(left,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
      if(rigth.isValid())pp=Pad2MC(rigth,list_digit);
      if(pp!=NULL){
        if(abs(pp->particleID().pid())==13)return StatusCode::SUCCESS;
      }
    }
  }      
  return StatusCode::SUCCESS;
}


