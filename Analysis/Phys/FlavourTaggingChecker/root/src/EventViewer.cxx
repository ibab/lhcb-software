#include <iostream>
#include "EventViewer.h"

#include "TEveWindowManager.h"
#include "TEveManager.h"
#include "TEveGeoNode.h"
#include "TEveBrowser.h"

#include "TGeoManager.h"
#include "TFile.h"
#include "TGLViewer.h"
#include "TSystem.h"
#include "TGLUtil.h"
#include "TRandom.h"
#include "TEveStraightLineSet.h"
#include "TMath.h"

#include <TRint.h>
#include <TROOT.h>


EventViewer::EventViewer( ) {

  declareProperty( "ViewOptionLHCb",  m_ViewOption = -1 );
  event_list.clear();

}
EventViewer::~EventViewer( ) {
  if (gEve && gEve->GetBrowser()) gEve->GetBrowser()->UnmapWindow();
  // TEveManager::Terminate();
  // gApplication->Terminate(0);
  exit(0); 
}

template < typename T > T* clone(const T& orig) {
  T* temp = new T();
  *temp = orig;
  return temp;
}

void EventViewer::storeClone(Event& event) { 

  Event * eventcopy = new Event(event); 
  Vertices vert_copy(0);

  int n = event.signalParts().size();
  eventcopy->clearSignalParts();
  for(int i=0; i<n; ++i){
    Particle * b = event.signalParts().at(i);
    Particle * a = new Particle(*b); 
    //cout<<a->ID()<<endl;
    //Particle * a = clone(**i); 
    eventcopy->addToSignalParts(a);
  }
  //  Particles::iterator i;
  for(unsigned int i=0; i<event.particles().size(); ++i){
    //    for(i=event.particles().begin();i!=event.particles().end(); ++i){
    Particle * a = new Particle(*(event.particles()[i])); 
    eventcopy->addToParticles(a);
  }
  //Vertices::iterator iv;
  for(unsigned int i=0; i<event.getVertices().size(); ++i){
    // for(iv=event.getVertices().begin();iv!=event.getVertices().end(); ++iv){
    //cout<<" "<<event.getVertices().size()<<endl;

    Vertex * a = new Vertex(*(event.getVertices()[i])); 
    //cout<<a->position().z()<<endl;
    vert_copy.push_back(a);
  }
  if(event.oppositeB()) {
    Particle * a = new Particle(*(event.oppositeB())); 
    eventcopy->setOppositeB(a);
  }

  eventcopy->setVertices(vert_copy);

  event_list.push_back(eventcopy);

}//< deep copy of event object

/////////////////////////////
void EventViewer::DrawLHCb(int opts) {

  TEveManager::Create();
  gEve->Redraw3D();    

  //TFile::SetCacheFileDir("./output");
  gGeoManager = gEve->GetGeometry("./lib/lhcbfull.root");
  gGeoManager->DefaultColors();
 
  TObjArray* vols = gGeoManager->GetListOfVolumes();
  
  for (Int_t ia=0; ia<vols->GetEntriesFast(); ia++ ) {
    TGeoVolume * vol = (TGeoVolume*)vols->At(ia);
    TString name = vol->GetName();

    //Geometry->GetListOfVolumes()->Print()
    //if(name.BeginsWith("PipeSection")) vol->SetVisibility(kFALSE);

    //if(name.BeginsWith("VeloVacTank")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloVacTanklvV5")) vol->SetVisibility(kFALSE);
    //if(name.BeginsWith("VeloVacTanklvVTank4")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSensors")) vol->SetTransparency(15);
    if(name.BeginsWith("VeloSupports"))vol->SetTransparency(40);

    if(name.BeginsWith("VeloRFFoill")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloRFBox")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("Rich1lvRich1AerogelWrap")) vol->SetVisibility(kFALSE);
    //if(name.BeginsWith("VeloSupports")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvMSIL")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvCLA")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvMFL")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvSpBase")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvMBL")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvCF")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvCuLayer1")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("VeloSupportslvKapton")) vol->SetVisibility(kFALSE);

    if(name.BeginsWith("MagnetYokeVert")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("MagnetYokeHor")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("MagnetCoil")) vol->SetTransparency(30);
    if(name.BeginsWith("Rich1lvRich1Mgs")) vol->SetVisibility(kFALSE);
    if(name.BeginsWith("Rich2lvRich2MagSh")) vol->SetTransparency(40);
    if(name.BeginsWith("EcalModules")) vol->SetTransparency(40);
    if(name.BeginsWith("MuonlvMuFilter")) vol->SetVisibility(kFALSE);
  }

  if(m_ViewOption>0) {
    length = 22000.;
    TEveGeoTopNode* tn = new TEveGeoTopNode(gGeoManager, 
					    gGeoManager->GetTopNode());
    tn->SetVisLevel(4);
    gEve->AddGlobalElement(tn);
    //gGeoManager->GetTopVolume()->GetNodes()->Print();
  } else {
    length = 1000.;
    TGeoNode* node1 = gGeoManager->GetTopVolume()->FindNode("VelolvVelo_4842");
    TEveGeoTopNode* inn = new TEveGeoTopNode(gGeoManager, node1);
    inn->SetVisLevel(4);
    gEve->AddGlobalElement(inn);
    //node1->GetMotherVolume()->GetNodes()->Print();
  }

  if(opts==0) {
    gEve->FullRedraw3D(kTRUE);
    // EClipType not exported to CINT (see TGLUtil.h):
    // 0 - no clip, 1 - clip plane, 2 - clip box
    TGLViewer *v = gEve->GetDefaultGLViewer();
    //v->GetClipSet()->SetClipType(1);
    v->ColorSet().Background().SetColor(kMagenta+4);
    v->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
    v->RefreshPadEditor(v);
    v->CurrentCamera().RotateRad(-.7, 0.5);
    v->DoDraw();
    gApplication->Run(kTRUE);
  }

}

//////////////////////////////////////////////////////////////
void EventViewer::DrawStoredEvents() {

  DrawLHCb(1);

  for(unsigned int i=0; i!=event_list.size(); ++i){
    Event* event = event_list.at(i);
    DrawEvent(event);
  }

}

//////////////////////////////////////////////////////////////
void EventViewer::DrawEvent(Event* event) {
 
  event->Print();

  TEveStraightLineSet* tracksEtaPhi = new TEveStraightLineSet("StraightLinesEtaPhi");
  tracksEtaPhi->SetLineColor(kYellow);
  tracksEtaPhi->SetLineWidth(1);
  TEveStraightLineSet* tracksEtaPhi_B = new TEveStraightLineSet("StraightLinesEtaPhi");
  tracksEtaPhi_B->SetLineColor(kRed);
  tracksEtaPhi_B->SetLineWidth(1);
  Particles parts = event->particles();//candidate particles
  for(Particles::iterator i=parts.begin(); i!=parts.end(); ++i){
    (*i)->Print();
    Float_t trackEta = (*i)->rapidity();
    Float_t trackPhi = (*i)->Phi();

    TEveVector trackDir( GetTEveVector(trackEta, trackPhi) );
    TEveVector trackEnd = trackDir * length;
    if( (*i)->fromB() == 1 ) 
      tracksEtaPhi_B->AddLine(event->RecVertex().x(), 
			      event->RecVertex().y(), 
			      event->RecVertex().z(), 
			      trackEnd.fX, trackEnd.fY, trackEnd.fZ );
    else
      tracksEtaPhi->AddLine(event->RecVertex().x(), 
			    event->RecVertex().y(), 
			    event->RecVertex().z(), 
			    trackEnd.fX, trackEnd.fY, trackEnd.fZ );
  }

  TEveStraightLineSet* sigtracksEtaPhi = new TEveStraightLineSet("StraightLinesEtaPhi");
  sigtracksEtaPhi->SetLineColor(kGreen);
  sigtracksEtaPhi->SetLineWidth(2);
  Particles sigparts = event->signalParts();//candidate particles
  for(Particles::iterator i=sigparts.begin(); i!=sigparts.end(); ++i){

    (*i)->Print();
    Float_t trackEta = (*i)->rapidity();
    Float_t trackPhi = (*i)->Phi();
    TEveVector trackDir( GetTEveVector(trackEta, trackPhi) );
    TEveVector trackEnd = trackDir * length;
    sigtracksEtaPhi->AddLine(event->RecVertex().x(), 
			     event->RecVertex().y(), 
			     event->RecVertex().z(), 
			     trackEnd.fX, trackEnd.fY, trackEnd.fZ );
  }

  gEve->AddElement(tracksEtaPhi_B);
  gEve->AddElement(sigtracksEtaPhi);
  gEve->AddElement(tracksEtaPhi);
  gEve->FullRedraw3D(kTRUE);

  // EClipType not exported to CINT (see TGLUtil.h):
  // 0 - no clip, 1 - clip plane, 2 - clip box
  TGLViewer *v = gEve->GetDefaultGLViewer();
  //v->GetClipSet()->SetClipType(1);
  v->ColorSet().Background().SetColor(kMagenta+4);
  v->SetGuideState(TGLUtil::kAxesOrigin, kTRUE, kFALSE, 0);
  v->RefreshPadEditor(v);

  v->CurrentCamera().RotateRad(-.7, 0.5);
  v->DoDraw();
 
  cout <<"\n Click on grey area and Cntrl-W to quit."<<endl;
  gApplication->Run(kTRUE);
  //gApplication->Run(kFALSE);

  tracksEtaPhi_B->DestroyElements();
  tracksEtaPhi->DestroyElements();
  sigtracksEtaPhi->DestroyElements();
}

// ################################################################################
TEveVector  EventViewer::GetTEveVector( Float_t& eta, Float_t& phi ) {
  TEveVector vec( (Float_t) TMath::Cos ( (Double_t) phi)/ TMath::CosH( (Double_t) eta ),
		  (Float_t) TMath::Sin ( (Double_t) phi)/ TMath::CosH( (Double_t) eta ),
		  (Float_t) TMath::TanH( (Double_t) eta ) );
  return vec;
}
