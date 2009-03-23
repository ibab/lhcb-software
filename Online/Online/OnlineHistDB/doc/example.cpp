// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/doc/example.cpp,v 1.12 2009-03-23 16:44:35 ggiacomo Exp $
#include <iostream>
#include <OnlineHistDB/OnlineHistDB.h>
#include <OnlineHistDB/OMAMessage.h>
using namespace std;

int main ()
{

  OnlineHistDB *HistDB = new OnlineHistDB("testhist2009",
                                          "LHCB_DEVMON_GIACOMO","devdb10");
  
  cout << "opened connection with "<<HistDB->nHistograms()<<" Histograms "
       << HistDB->nPages()<<" Pages "
       << HistDB->nPageFolders()<<" PageFolders "<<endl<<endl;
  
  bool ok=true;
  
  // messages from analysis
  std::vector<int> messID;
  std::cout <<" You have "<<HistDB->getMessages(messID)<<" messages from Analysis"<<std::endl;
  for (std::vector<int>::iterator im= messID.begin() ; im !=  messID.end() ; im++) {
    OMAMessage* message = new OMAMessage(*im, *HistDB);
    message->dump(&(std::cout));
    delete message;
  }
  
  ok &= HistDB->declareTask("EXAMPLE","MUON","GAS","",true,true,false);
  OnlineHistTask* mytask = HistDB->getTask("EXAMPLE");
  if (mytask)
    mytask->setSavingFrequency(3.5);
  
  if (ok) {
    string ServiceName="H1D/nodeMF001_EXAMPLE_01/SafetyCheck/Trips";
    HistDB->declareHistByServiceName(ServiceName);
    ServiceName="H1D/nodeMF001_EXAMPLE_01/SafetyCheck/Trips_after_use_of_CRack";
    HistDB->declareHistByServiceName(ServiceName);
    ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit_Map_$Region_M1R1";
    HistDB->declareHistByServiceName(ServiceName); 
    ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit_Map_$Region_M1R2";
    HistDB->declareHistByServiceName(ServiceName);
    ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit_Map_$Region_M3R1";
    HistDB->declareHistByServiceName(ServiceName);
    
    HistDB->declareHistogram("EXAMPLE","Timing","Coincidences",OnlineHistDBEnv::H1D);
    HistDB->declareHistogram("EXAMPLE","Timing","Time_of_flight",OnlineHistDBEnv::H1D);
    
  }



  OnlineHistogram* thisH = HistDB->getHistogram("EXAMPLE/Timing/Time_of_flight");
  if(thisH) {
    std::string mydns("H1D/nodeA01_Adder_01/EXAMPLE/Timing/Time_of_flight");
    thisH->setDimServiceName(mydns);
  } 
 

 

  OnlineHistogram* h1=HistDB->getHistogram
    ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M1R1");
  OnlineHistogram* h2=HistDB->getHistogram
    ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M1R2");
  OnlineHistogram* h3=HistDB->getHistogram
    ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M3R1");
  
  if (h1 && h2 && h3) {

    OnlineHistPage* pg=HistDB->getPage("/Examples/My Example Page");
    if(pg) {
      pg->declareHistogram(h1, 0.  ,0.5 ,0.5, 1. ); 
      pg->declareHistogram(h2, 0.5 ,0.5 ,1. , 1. ); 
      pg->declareHistogram(h3, 0.  ,0.  ,0.6 , 0.5);
      std::string pfile("mytask/myPatternFile");
      pg->setPatternFile(pfile);
      pg->save(); // needed to sync pg object with DB
      
      int lc=2, fs=7, fc=3;
      float ymax=20000.;
      h1->setDisplayOption("LINECOLOR",(void*) &lc);
      h1->setDisplayOption("FILLSTYLE",(void*) &fs);
      h1->setDisplayOption("FILLCOLOR",(void*) &fc); 
      h1->setDisplayOption("YMAX",(void*) &ymax); 
      h1->saveDisplayOptions();
      h1->dump();
      
      // second instance of h1
      OnlineHistogram* newh = pg->declareHistogram(h1,0.61, 0. ,1. ,0.5 ,2);
      pg->save();
      ymax=200000.;
      newh->setDisplayOption("YMAX",(void*) &ymax); 
      newh->unsetDisplayOption("FILLCOLOR");
      lc=4;
      newh->setDisplayOption("LINECOLOR",(void*) &lc);
      newh->saveDisplayOptions();
      newh->dump();
      
    }
  }

  
  std::vector<string> folders;
  std::vector<string> pages;
  std::vector<OnlineHistogram*> histos;  
  int nfold=HistDB->getPageFolderNames(folders);
  int i,j,k;
  for (i=0;i<nfold;i++) {
    cout << "Folder " << folders[i] <<endl;
    pages.clear();
    int np=HistDB->getPageNamesByFolder(folders[i],pages);
    for (j=0;j<np;j++) {
      cout << "     Page " << pages[j] <<endl;
      histos.clear();
      int nh=HistDB->getHistogramsByPage(pages[j],&histos);
      for (k=0;k<nh;k++) {
        cout << "           Histogram " << histos[k]->identifier() ;
        if (histos[k]->instance()>1)
          cout << " (Instance "<<histos[k]->instance()<<")";
        cout <<endl;
      }    
    }
  }
  
  if(0) {
    histos.clear();
    cout << "-------------Histos with associated page------------"<<endl;
    HistDB->getAllHistograms(&histos);
    std::vector<OnlineHistogram*>::iterator ih;
    for (ih=histos.begin() ; ih != histos.end() ; ih++) {
      if ( ((*ih)->page2display()).size() > 0)
        cout << "Histo "<<(*ih)->identifier() << " -> Page "<<
          (*ih)->page2display() << endl;
    }
  }


  std::vector<string> mylist;
  cout << "----------------------------------------"<<endl;
  int nss=HistDB->getSubsystems(mylist);
  for (i=0;i<nss;i++) {
    cout << "Subsys "<<mylist[i]<<endl;
  }
  mylist.clear();
  cout << "----------------------------------------"<<endl;
  nss=HistDB->getTasks(mylist);
  for (i=0;i<nss;i++) {
    cout << "Task "<<mylist[i]<<endl;
  }
  mylist.clear();
  cout << "----------------------------------------"<<endl;
  nss=HistDB->getAlgorithms(mylist);
  for (i=0;i<nss;i++) {
    cout << "Algorithm "<<mylist[i]<<endl;
  }
  
  // commit all changes only if there are no errors from histogram declarations
  if (ok) 
    HistDB->commit();
  else 
    cout << "commit aborted because of previous errors" <<endl;
  
  HistDB->setDebug(3);
  delete HistDB;
}

