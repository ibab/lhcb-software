// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/doc/example.cpp,v 1.5 2007-07-17 15:54:13 ggiacomo Exp $
#include <stdio.h>
#include <OnlineHistDB/OnlineHistDB.h>
int main ()
{
 OnlineHistDB *HistDB = new OnlineHistDB(PASSWORD,
					  OnlineHistDBEnv_constants::ACCOUNT,
					  OnlineHistDBEnv_constants::DB);
  
 bool ok=true;
  
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
   
   ok &= HistDB->sendHistBuffer(); // needed to send histogram buffer to DB
 }



 OnlineHistogram* thisH = HistDB->getHistogram("EXAMPLE/Timing/Time_of_flight");
 if(thisH)
   thisH->setDimServiceName("H1D/nodeA01_Adder_01/EXAMPLE/Timing/Time_of_flight");
 
 HistDB->declareCreatorAlgorithm("Subtraction",2,OnlineHistDBEnv::H1D,0,NULL,
				 "bin-by-bin subtraction");

 OnlineHistogram* s1=HistDB->getHistogram("EXAMPLE/SafetyCheck/Trips");
 OnlineHistogram* s2=HistDB->getHistogram("EXAMPLE/SafetyCheck/Trips_after_use_of_CRack");
 OnlineHistogram* htrips=0;
 std::vector<OnlineHistogram*> sources;
 if(s1 && s2) {
   sources.push_back(s1);
   sources.push_back(s2);
   htrips=HistDB->declareAnalysisHistogram("Subtraction",
					   "Trips_due_to_CRack",
					   sources);
 }

 std::string mypar[1]={"Max"};
 bool algok=HistDB->declareCheckAlgorithm("CheckMax",1,mypar,
			       "Checks all bins to be smaller than Max");
 
 if (htrips && algok) {
   std::vector<float> warn(1,100.);
   std::vector<float> alarm(1,500.);
   htrips->declareAnalysis("CheckMax", &warn, &alarm);
 } 


 OnlineHistogram* h1=HistDB->getHistogram
   ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M1R1");
 OnlineHistogram* h2=HistDB->getHistogram
   ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M1R2");
 OnlineHistogram* h3=HistDB->getHistogram
   ("EXAMPLE/OccupancyMap/Hit_Map_$Region_M3R1");

 if (h1 && h2 && h3) {
   std::string hcpar[2]={"w1","w2"};
   algok = HistDB->declareCreatorAlgorithm("Weighted mean",
					   2,
					   OnlineHistDBEnv::H1D,
					   2,
					   hcpar,
				"weighted mean of two histograms with weights w1 and w2");
   sources.clear();
   sources.push_back(h1);
   sources.push_back(h2);
   std::vector<float> weights(2,1.);
   weights[1]=0.5;
   if(algok) HistDB->declareAnalysisHistogram("Weighted mean",
					      "Silly plot",
					      sources,
					      &weights);

   OnlineHistPage* pg=HistDB->getPage("My Example Page","Examples/My examples");
   if(pg) {
     pg->declareHistogram(h1,0. ,0. ,0.5,0.5);
     pg->declareHistogram(h2,0. ,0.5,0.5,0.5);
     pg->declareHistogram(h3,0.5,0.5,0.5,0.4);
   
     int lc=2, fs=7, fc=3;
     float ymax=20000.;
     h1->setDisplayOption("LINECOLOR",(void*) &lc);
     h1->setDisplayOption("FILLSTYLE",(void*) &fs);
     h1->setDisplayOption("FILLCOLOR",(void*) &fc); 
     h1->setDisplayOption("YMAX",(void*) &ymax); 

     h1->dump();

     // second instance of h1
     OnlineHistogram* newh = pg->declareHistogram(h1,0.5,0. ,0.5,0.4,2);
     ymax=200000.;
     newh->setDisplayOption("YMAX",(void*) &ymax); 
     newh->unsetDisplayOption("LINECOLOR");

     newh->dump();
   }
 }



 // commit all changes only if there are no errors from histogram declarations
 ok &= HistDB->sendHistBuffer();
 if (ok) 
   HistDB->commit();
 else 
   cout << "commit aborted because of previous errors" <<endl;


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
       cout << "           Histogram " << histos[k]->identifier() <<endl;
     }    
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

 HistDB->setDebug(3);
 delete HistDB;
}

