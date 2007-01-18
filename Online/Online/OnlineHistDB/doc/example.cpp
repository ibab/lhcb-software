#include <OnlineHistDB/OnlineHistDB.h>
int main ()
{
 std::string password="ask_to_DB_manager";
 OnlineHistDB *HistDB = new OnlineHistDB(password);

 HistDB->declareTask("EXAMPLE","MUON","GAS","",true,true,false);

 string ServiceName="H1D/nodeMF001_EXAMPLE_01/SafetyCheck/Trips";
 HistDB->declareHistByServiceName(ServiceName);
 ServiceName="H1D/nodeMF001_EXAMPLE_01/SafetyCheck/Trips after use of CRack";
 HistDB->declareHistByServiceName(ServiceName);
 ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit Map_$$_Region M1R1";
 HistDB->declareHistByServiceName(ServiceName); 
 ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit Map_$$_Region M1R2";
 HistDB->declareHistByServiceName(ServiceName);
 ServiceName="H2D/nodeMF001_EXAMPLE_01/OccupancyMap/Hit Map_$$_Region M3R1";
 HistDB->declareHistByServiceName(ServiceName);
 HistDB->commit(); // needed to send histogram buffer to DB

 
 
 HistDB->declareCreatorAlgorithm("Subtraction",2,
				 "bin-by-bin subtraction");
 std::vector<OnlineHistogram*> sources;
 sources.push_back(HistDB->getHistogram("EXAMPLE/SafetyCheck/Trips"));
 sources.push_back(HistDB->getHistogram("EXAMPLE/SafetyCheck/Trips after use of CRack"));
 OnlineHistogram* htrips=HistDB->declareAnalysisHistogram("Subtraction",
							  "Trips due to CRack",
							  sources);


 std::string mypar[1]={"Max"};
 HistDB->declareCheckAlgorithm("CheckMax",1,mypar,
			       "Checks all bins to be smaller than Max");
 
 if (htrips) {
   std::vector<float> warn(1,100.);
   std::vector<float> alarm(1,500.);
   htrips->declareAnalysis("CheckMax", &warn, &alarm);
 } 


 OnlineHistogram* h1=HistDB->getHistogram("EXAMPLE/OccupancyMap/Hit Map_$$_Region M1R1");
 OnlineHistogram* h2=HistDB->getHistogram("EXAMPLE/OccupancyMap/Hit Map_$$_Region M1R2");
 OnlineHistogram* h3=HistDB->getHistogram("EXAMPLE/OccupancyMap/Hit Map_$$_Region M3R1");

 OnlineHistPage* pg=HistDB->getPage("Example Page","Examples");
 pg->declareHistogram(h1,0. ,0. ,0.5,0.5);
 pg->declareHistogram(h2,0. ,0.5,0.5,0.5);
 pg->declareHistogram(h3,0. ,0. ,0.5,1. );
 pg->save();

 int lc=2, fs=7, fc=3;
 float ymax=20000.;
 h1->setHistoSetDisplayOption("LINECOLOR",(void*) &lc);
 h1->setHistoSetDisplayOption("FILLSTYLE",(void*) &fs);
 h1->setHistoSetDisplayOption("FILLCOLOR",(void*) &fc); 
 h1->setHistoSetDisplayOption("YMAX",(void*) &ymax); 
 
 
 lc=3;
 h2->initDisplayOptionsFromSet();
 h2->setDisplayOption("LINECOLOR",(void*)  &lc);


 fc=4;
 h3->initHistoPageDisplayOptionsFromSet("Example Page");
 h3->setHistoPageDisplayOption("FILLCOLOR",(void*) &fc);

 HistDB->commit();
 delete HistDB;
}

