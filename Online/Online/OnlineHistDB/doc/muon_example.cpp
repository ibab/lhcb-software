#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
#include <sstream>
#include <string>
using namespace std;

// PLEASE DON'T RUN THIS CODE AS IT IS!!!
// YOU WOULD OVERRIDE THE MUON MONITORING STUFF
//
// SHOULD BE USED ONLY AS A REFERENCE EXAMPLE



int main() {
  bool ok =true;
  bool sure=false;

  std::string options;
  int optioni;
  float optionf;

  // password should be asked to DB manager
  OnlineHistDB *HistDB = new OnlineHistDB("PASSWORD","HIST_WRITER","HISTDB");
  std::vector<OnlineHistogram*> list;
  HistDB->getHistogramsByTask("MuonDAQMon",&list);

  // default display options for all histograms
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih= list.begin(); ih!= list.end() ; ih++) {
    if ((*ih)->dimension() == 1){
      optioni=1111110; options="HIST";
    }
    else {
      optioni=10; options="COLZ";
    }
    (*ih)->setDisplayOption("STATS", &optioni);
    (*ih)->setDisplayOption("DRAWOPTS", &options);
    (*ih)->saveHistoSetDisplayOptions();
  }

  // set some special display options
  OnlineHistogram* h;
  h=HistDB->getHistogram("MuonDAQMon/MuonMonitor/TDC time by ODE_$Q1/M1R1/ODE1");
  if(h) {
    options="channel"; h->setDisplayOption("LABEL_X", &options);
    options="TDC time (ns)"; h->setDisplayOption("LABEL_Y", &options);
    h->saveHistoSetDisplayOptions();
  }
  h=HistDB->getHistogram("MuonDAQMon/MuonMonitor/Hits by ODE_$Q1/M1R1/ODE1");
  if(h) {
    options="channel"; h->setDisplayOption("LABEL_X", &options);
    h->saveHistoSetDisplayOptions();
  }

  // define some on-the-fly histograms through the HMerge algorithm
  std::vector<OnlineHistogram*> sources;
  for (int ist=1;ist<6;ist++) {
    sources.clear();
    stringstream nn;
    nn << "MuonDAQMon/MuonMonitor/Pads Map_$M"<<ist;
    for (int ir=1;ir<5;ir++) {
      stringstream nnr;
      nnr << nn.str()<< "R"<< ir;
      OnlineHistogram* hh=HistDB->getHistogram(nnr.str());
      sources.push_back(hh);
    }
    stringstream xname;
    xname << "Pads Map_$M" << ist;
    ok &= (HistDB->declareAnalysisHistogram(std::string("HMerge"),
                                            xname.str(),
                                            sources)
           != NULL);
  }
  if (!ok) abort();
  
  OnlineHistogram* hnew = HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M2");
  if(hnew) {
    optioni=0; hnew->setDisplayOption("STATS", &optioni);
    options="COLZ"; hnew->setDisplayOption("DRAWOPTS", &options);
    hnew->saveHistoSetDisplayOptions(); // options are saved for the whole histogram set
  }

  OnlineHistPage*  hitmap =  HistDB->getPage("/Muon Example/Pad Map Summary (fast)");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pads Map_$M2"),
                                           0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pads Map_$M3"),
                                           0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pads Map_$M4"),
                                           0.01, 0.01, 0.49, 0.49 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pads Map_$M5"),
                                           0.51, 0.01, 0.99, 0.49 ) );
  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();

  hitmap =  HistDB->getPage("/Muon Example/Pad Map Summary");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M2"),
                                           0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M3"),
                                           0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M4"),
                                           0.01, 0.01, 0.49, 0.49 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M5"),
                                           0.51, 0.01, 0.99, 0.49 ) );
  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();


  for (int is=1;is<6;is++) {
    stringstream nn,nh;
    nn << "/Muon Example/Pad Map M"<<is;
    nh << "MuonDAQMon_ANALYSIS/HMerge/Pads Map_$M"<<is;
    OnlineHistPage*  hitm =  HistDB->getPage(nn.str());
    hitm->removeAllHistograms();
    ok = true;
    ok &= (NULL != hitm->declareHistogram( HistDB->getHistogram(nh.str()),
                                           0.01, 0.01, 0.99, 0.99 ) );
    if(ok) 
      hitm->save();
    else
      abort();
    hitm->dump();
  }
  
  OnlineHistPage*  gen =  HistDB->getPage("/Muon Example/Multiplicity and Time spectrum");
  gen->removeAllHistograms();
  ok = true;
  ok &= (NULL != gen->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pad Multiplicity_$all"),
                                        0.01, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != gen->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/TDC time of Muon Pads"),
                                        0.01, 0.01, 0.99, 0.49 ) );
  if(ok) gen->save();
  else abort();
  gen->dump();
  
  OnlineHistPage*  pms =  HistDB->getPage("/Muon Example/Multiplicity per station");
  pms->removeAllHistograms();
  ok = true;
  ok &= (NULL != pms->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pad Multiplicity_$M2"),
                                        0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != pms->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pad Multiplicity_$M3"),
                                        0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != pms->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pad Multiplicity_$M4"),
                                        0.01, 0.01, 0.49, 0.49 ) );
  ok &= (NULL != pms->declareHistogram( HistDB->getHistogram("MuonDAQMon/MuonMonitor/Pad Multiplicity_$M5"),
                                        0.51, 0.01, 0.99, 0.49 ) );
  if(ok) pms->save();
  else abort();
  pms->dump();
  
  


  OnlineHistogram* allhits =  HistDB->getHistogram("MuonDAQMon/MuonMonitor/Expected Hits per station");
  OnlineHistogram* intimehits =  HistDB->getHistogram("MuonDAQMon/MuonMonitor/In-time Hits per station");
  OnlineHistogram* occu=NULL;
  sources.clear();
  sources.push_back(intimehits);
  sources.push_back(allhits);
  if (allhits && intimehits)
    occu = HistDB->declareAnalysisHistogram("Efficiency",
                                            "Average Occupancy per Station/Region",
                                            sources);
  
  OnlineHistPage*  pulse = HistDB->getPage("/Muon Example/Pulsed Run Summary");
  pulse->removeAllHistograms();
  ok = true;
  ok &= (NULL != pulse->declareHistogram( allhits,
                                          0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != pulse->declareHistogram( intimehits,
                                          0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != pulse->declareHistogram( occu,
                                          0.01, 0.01, 0.99, 0.49 ) );
  if(ok) pulse->save();
  else abort();
  pulse->dump();
  
  OnlineHistPage*  page;  
  for (int quad=1; quad<=4; quad++) {
    for (int s=1; s<=5; s++) {
      
      stringstream hmname,oldnm;
      hmname << "/Muon Example/Q"<<quad <<"/Pad Map M"<<s;
      oldnm << "/Muon Example/Q"<<quad <<"/Hit Map M"<<s;
      OnlineHistPage* oldpage = HistDB->getPage(oldnm.str());
      if(oldpage) HistDB->removePage(oldpage);
      page = HistDB->getPage(hmname.str());
      page->removeAllHistograms();
      ok = true;
      stringstream hmhname;
      hmhname << "MuonDAQMon/MuonMonitor/Pads Map_$M"<<s;
      ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hmhname.str()+"R1"),
                                             0.01, 0.51, 0.49, 0.99 ) );
      ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hmhname.str()+"R2"),
                                             0.51, 0.51, 0.99, 0.99 ) );
      ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hmhname.str()+"R3"),
                                             0.01, 0.01, 0.49, 0.49 ) );
      ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hmhname.str()+"R4"),
                                             0.51, 0.01, 0.99, 0.49 ) );
      if(ok) page->save();
      std::vector<OnlineHistogram*> hlist;
      page->getHistogramList(&hlist);
      std::string option("BOX");
      float bordo=0.;
      std::vector<OnlineHistogram*>::iterator ht;
      for (ht=hlist.begin() ; ht != hlist.end() ; ht++) {
        (*ht)->setDisplayOption("DRAWOPTS",(void*) &option);
        switch(quad) {
        case 1:
          (*ht)->setDisplayOption("XMIN",&bordo);
          (*ht)->setDisplayOption("YMIN",&bordo);
          break;
        case 2:
          (*ht)->setDisplayOption("XMIN",&bordo);
          (*ht)->setDisplayOption("YMAX",&bordo);
          break;
        case 3:
          (*ht)->setDisplayOption("XMAX",&bordo);
          (*ht)->setDisplayOption("YMAX",&bordo);
          break;
        case 4:
          (*ht)->setDisplayOption("XMAX",&bordo);
          (*ht)->setDisplayOption("YMIN",&bordo);
          break;
        }
        
        (*ht)->saveHistoPageDisplayOptions();
      }
      page->dump();
      
      int maxr= (s<4) ? 4 : 1;
      for (int r=1; r<= maxr ;r++) {
        stringstream pname;
        pname << "/Muon Example/Q"<<quad <<"/M"<<s<<"R"<<r<<" by ODE";
        page = HistDB->getPage(pname.str());
        page->removeAllHistograms();
        ok = true;
        stringstream hname,tname;
        hname<< "MuonDAQMon/MuonMonitor/Hits by ODE_$Q"<<quad 
             <<"/M"<<s<<"R"<<r<<"/ODE";
        ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hname.str()+"1"),
                                               0.01, 0.51, 0.49, 0.99 ) );
        ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hname.str()+"2"),
                                               0.51, 0.51, 0.99, 0.99 ) );
        tname<<"MuonDAQMon/MuonMonitor/TDC time by ODE_$Q"<<quad 
             <<"/M"<<s<<"R"<<r<<"/ODE";
        ok &= (NULL != page->declareHistogram( HistDB->getHistogram(tname.str()+"1"),
                                               0.01, 0.01, 0.49, 0.49 ) );
        ok &= (NULL != page->declareHistogram( HistDB->getHistogram(tname.str()+"2"),
                                               0.51, 0.01, 0.99, 0.49 ) );
        if(ok) page->save();
        page->dump();
      }
      if (s>3) {
        stringstream pname;
        pname << "/Muon Example/Q"<<quad <<"/M"<<s<<"R2-3-4 by ODE";
        page = HistDB->getPage(pname.str());
        page->removeAllHistograms();
        ok = true;
        double offset=0.005;
        for (int r=2; r<= 4 ;r++) {
          stringstream hname,tname;
          hname<< "MuonDAQMon/MuonMonitor/Hits by ODE_$Q"<<quad 
               <<"/M"<<s<<"R"<<r<<"/ODE1";
          tname<<"MuonDAQMon/MuonMonitor/TDC time by ODE_$Q"<<quad 
               <<"/M"<<s<<"R"<<r<<"/ODE1";
          ok &= (NULL != page->declareHistogram( HistDB->getHistogram(hname.str()),
                                                 offset+0.005, 0.51, offset+0.325, 0.99 ) );
          ok &= (NULL != page->declareHistogram( HistDB->getHistogram(tname.str()),
                                                 offset+0.005, 0.01, offset+0.325, 0.49 ) );
          offset += 0.33;
        }
        if(ok) page->save();
        page->dump();
      }
    }
  }

  // now pages for cosmics  
  for (int ist=1;ist<6;ist++) {
    sources.clear();
    stringstream nn;
    nn << "MuonDAQMon/CosmicMonitor/Track Map_$M"<<ist;
    for (int ir=1;ir<5;ir++) {
      stringstream nnr;
      nnr << nn.str()<< "R"<< ir;
      OnlineHistogram* hh=HistDB->getHistogram(nnr.str());
      sources.push_back(hh);
    }
    stringstream xname;
    xname << "Track Map_$M" << ist;
    ok &= (HistDB->declareAnalysisHistogram(std::string("HMerge"),
                                            xname.str(),
                                            sources)
           != NULL);
  }
  if (!ok) abort();
  
  hnew = HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Track Map_$M2");
  if(hnew) {
    optioni=0; hnew->setDisplayOption("STATS", &optioni);
    options="COLZ"; hnew->setDisplayOption("DRAWOPTS", &options);
    hnew->saveHistoSetDisplayOptions();
  }

  hitmap =  HistDB->getPage("/Muon Example/Cosmic Track Maps");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Track Map_$M2"),
                                           0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Track Map_$M3"),
                                           0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Track Map_$M4"),
                                           0.01, 0.01, 0.49, 0.49 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon_ANALYSIS/HMerge/Track Map_$M5"),
                                           0.51, 0.01, 0.99, 0.49 ) );
  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();
  cout << "committing changes"<<endl;

  
  hitmap =  HistDB->getPage("/Muon Example/Cosmic Summary");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track Multiplicity"),
                                           0.01, 0.51, 0.49, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Hits per track"),
                                           0.51, 0.51, 0.99, 0.99 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Hit time raw_$all"),
                                           0.01, 0.01, 0.49, 0.49 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Hit time TOF corrected_$all"),
                                           0.51, 0.01, 0.99, 0.49 ) );

  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();

  hitmap =  HistDB->getPage("/Muon Example/Cosmic Track Residuals");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Residuals x_$all"),
                                           0.01, 0.64, 0.49, 0.995 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Residuals y_$all"),
                                           0.51, 0.64, 0.99, 0.995 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/slope diff. XZ"),
                                           0.01, 0.32, 0.49, 0.635 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/slope diff. YZ"),
                                           0.51, 0.32, 0.99, 0.635 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track Chi2x"),
                                           0.01, 0.00, 0.49, 0.315 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track Chi2y"),
                                           0.51, 0.00, 0.99, 0.315 ) );

  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();
  
  
  hitmap =  HistDB->getPage("/Muon Example/Cosmic polar angles");
  hitmap->removeAllHistograms();
  ok = true;
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track phi wrt vertical"),
                                           0.01, 0.60, 0.49, 0.995 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track theta wrt vertical"),
                                           0.51, 0.60, 0.99, 0.995 ) );
  ok &= (NULL != hitmap->declareHistogram( HistDB->getHistogram("MuonDAQMon/CosmicMonitor/Track theta vs phi wrt vert"),
                                           0.01, 0.01, 0.99, 0.595 ) );

  if(ok) 
    hitmap->save();
  else
    abort();
  hitmap->dump();

  if(sure) {
    cout << "committing changes"<<endl;
    HistDB->commit();
  }
  delete HistDB;
}
