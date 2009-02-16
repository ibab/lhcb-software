// $Id: GaudiExample.cpp,v 1.13 2009-02-16 16:22:22 evh Exp $

// Include files
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "Gaucho/MonStatEntity.h"
#include "Gaucho/MonVectorI.h"
#include "Gaucho/MonVectorD.h"
#include "GaudiExample.h"

#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IAxis.h"

#include <cstring>
#include <cmath>

#include "GaudiUtils/Aida2ROOT.h"
#include "TH1D.h"

#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep(x) win::Sleep(x) 
#else
# include <unistd.h>
# define mysleep(x) usleep(x*1000000) 
#endif


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(GaudiExample)
  
  // Constructor
  //------------------------------------------------------------------------------
  GaudiExample::GaudiExample(const std::string& name, ISvcLocator* ploc)
    : GaudiHistoAlg(name, ploc)
      ,statEntity(msgSvc(), name,0) 
      ,monVectorInt(msgSvc(), name,0)
      ,monVectorDouble(msgSvc(), name,0)
      
{  
  declareProperty("SleepTime",sleepTime=1); // Sleeping time between events, in seconds
}

//------------------------------------------------------------------------------
StatusCode GaudiExample::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());
  
  sc = service("HistogramDataSvc", m_histosvc, true );
  if( !sc.isSuccess() ){
    msg << MSG::FATAL << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }
  
  counter1=0;   	// "All events"
  counter2=0;   	// "Event of type 1"
  efficiency1 = 0;	// "Ratio counter2/counter1"
  aboveRef = false;	// "true if efficiency > 0.55"
  efficiency2=0.0;	// "3090 < mass < 310"
  position.first = 0;
  position.second = 0;
  index.first = 0;
  index.second = 0;
  ix.first = 0.;
  ix.second = 0;
  
  oldStatus=new char[20];
  
  // For histos debugging
  nbinEntries=0;
  sumOfWeights=0;
  sumOfSquaredWeights=0;
  
  // No need to instantiate explicitly the MonitorScv. The GaudKernel/Algorithm has
  // a method declareInfo that and do this automatically for you
  msg << MSG::INFO << "Declaring infos to be published" << endreq;
  
  declareInfo("counter1",counter1,"All events");
  
  declareInfo("counter2",counter2,"Event of type 1");
  
  // msg << MSG::INFO << "Test: Try to publish info with a name already used. Not possible" << endreq;
  declareInfo("counter2",counter2,"Event type 1");
  
  declareInfo("efficiency1",efficiency1,"Ratio type 1/all");
  declareInfo("aboveRef",aboveRef,"#events is above reference");
  
  // msg << MSG::INFO << "Test: No declareInfo for char* type. By default, will use bool instead!" << endreq; 
  // declareInfo("oldStatus",oldStatus,"Previous Trigger status");
  
  declareInfo("status",status,"Trigger status");
  
  declareInfo("efficiency2",efficiency2,"Time rate 1-2");
  
  // It doesn't work because there are not pairs II nor pairs DI in IMonitorSvc 
  // The IMonitorSvc Interface Misunderstand and declare this two pairs as DD
  declareInfo("index",index,"ij indices (pair of ints)");
  declareInfo("ix",ix,"i -> x map (pair of double-int)");
  
  declareInfo("position",position,"xy position (pair of doubles)");
  
  // IMonitorSvc has no declareInfo methods for MonObjects. 
  // But we publish MonObjects using the obsolete declareInfo for void *
  // notice that the second and four arguments are obsoletes
  declareInfo("xyStatistics", "", &statEntity, 0, "xy statistics (StatEntity)");
  monVectorInt.setValue(m_vectInt);
  declareInfo("vectorInts", "", &monVectorInt, 0, "std::vector of ints"); 
  monVectorDouble.setValue(m_vectDouble);
  declareInfo("vectorDoubles",  "", &monVectorDouble, 0, "std::vector of doubles");
  
  // This is only for counters which will be monitorated as rates
  // Obs: the description should be "COUNTER_TO_RATE"
  declareInfo("COUNTER_TO_RATE[count1]", count_rate_1, "Counter number 1");
  declareInfo("COUNTER_TO_RATE[count2]", count_rate_2, "Counter number 2");
 // declareInfo("count2", count_rate_2);
  
  // For histograms, either declare explicitly here using  histosvc, or 
  // use plot methods from class GaudiHistos in GaudiExample::execute(), see my1Dhisto1 below
  effHisto =  m_histosvc->book("effHisto","Efficiency (counter2/counter1)",10,0.,1.);  
  declareInfo("effHisto",effHisto,"Efficiency (counter2/counter1)");
  
  time(&time_old);
  
  // use Random Number Service to generate events
  sc = random1.initialize(randSvc(), Rndm::Flat(0.,1.));
  if ( !sc.isSuccess() ) {
    return sc;
  }  
  sc = random2.initialize(randSvc(), Rndm::Gauss(3096.,10.));
  if ( !sc.isSuccess() ) {
    return sc;
  }  
  sc = random3.initialize(randSvc(), Rndm::Gauss(892.,50.3));
  if ( !sc.isSuccess() ) {
    return sc;
  }
  sc = random4.initialize(randSvc(), Rndm::Poisson(5));
  if ( !sc.isSuccess() ) {
    return sc;
  }
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode GaudiExample::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  msg << MSG::DEBUG << "******************************************" << endreq;
  msg << MSG::DEBUG << "execute method...." << endreq;
  
  int eventtype;
  counter1++;
  
  time(&time_new);
  msg << MSG::DEBUG << "Current event: " << counter1 << " Current time: " <<
    time_new << endreq;
  
  // For histograms, use plot methods from class GaudiHistos: 
  // E.g.: AIDA::IHistogram1D*  plot1D ( value, title, low, high, bins, weight );
  
  std::string Histo1Da_ID;
  std::string Histo1Da_Title;
  std::string Histo1Db_ID;
  std::string Histo1Db_Title;
  std::string Histo2D_ID;
  std::string Histo2D_Title;
  std::string HistoProf_ID;
  std::string HistoProf_Title;
  
  Histo1Da_Title = "Event type";
  Histo1Db_Title = "Mass plot";
  Histo2D_Title = "Plot of position in XY plane";
  HistoProf_Title = "Profile  histo of y(x)"; 
  // rearrange this to make it easy to change the nb of histograms
  unsigned int nHisto=2;
  for (unsigned int i=0;i<nHisto;i++) {
    Histo1Da_ID = "1Da"+boost::lexical_cast<std::string>(i);
    Histo1Db_ID = "1Db"+boost::lexical_cast<std::string>(i);
    Histo2D_ID = "2D"+boost::lexical_cast<std::string>(i);
    HistoProf_ID= "Prof"+boost::lexical_cast<std::string>(i);
    
    float dice1=random1();
    if(dice1 > 0.9 )      eventtype=0; // To test UNDERFLOW
    if(dice1 > 0.5 )      eventtype=1;
    else if(dice1 > 0.4)  eventtype=2;
    else if(dice1 > 0.25) eventtype=3;
    else if(dice1 > 0.2)  eventtype=4;
    else if(dice1 > 0.1)  eventtype=5;
    else 		   eventtype=15;  // Just to test OVERFLOW
    
    IHistogram1D * my1Dhisto1 = plot1D(eventtype,Histo1Da_ID,Histo1Da_Title,0.5, 5.5,5);
    
    msg << MSG::DEBUG << Histo1Da_ID << ": Bin entries: " << my1Dhisto1->binEntries(AIDA::IAxis::UNDERFLOW_BIN) << " -";
    for(int jxBin=0; jxBin<my1Dhisto1->axis().bins();++jxBin)  msg << MSG::DEBUG << " " <<  my1Dhisto1->binEntries(jxBin);
    msg << MSG::DEBUG << " - " <<  my1Dhisto1->binEntries(AIDA::IAxis::OVERFLOW_BIN) ;
    msg << MSG::DEBUG << " errors: "  << my1Dhisto1->binError(AIDA::IAxis::UNDERFLOW_BIN) << " -";
    for(int jxBin=0; jxBin<my1Dhisto1->axis().bins();++jxBin)  msg << MSG::DEBUG << " " << my1Dhisto1->binError(jxBin);
    msg << MSG::DEBUG << "  - " <<  my1Dhisto1->binError(AIDA::IAxis::OVERFLOW_BIN) << endreq;
    
    double mass; 
    if(1 == eventtype)      mass = random2();
     else if(2 == eventtype) mass = random3();
    else                    mass = 2800+random1()*500;
    
    IHistogram1D * my1Dhisto2 = plot1D(mass,Histo1Db_ID,Histo1Db_Title,2900., 3200., 30);
    
    TH1D *h = Gaudi::Utils::Aida2ROOT::aida2root(  my1Dhisto1 );
    if ( 0 != h ) {
      // get the axis:
      TAxis* axis = h->GetXaxis() ;
      
      if( 0 != axis ) 
      {
        
        // loop over bins:

     //   for ( int ibin = 1; ibin < 6 ; ++ibin ) {

     //     char * test = "Test label "+ibin;
     //     msg << MSG::DEBUG << " bin " << ibin << " heeft als naam " << test << endreq;
          axis-> SetBinLabel ( 1, "bin 1" );
	  axis-> SetBinLabel ( 2, "bin 2" );
	  axis-> SetBinLabel ( 3, "bin 3" );
	  axis-> SetBinLabel ( 4, "bin 4" );
	  axis-> SetBinLabel ( 5, "bin 5" );
						                  
    //    }
        

     }
    }
    
    
    
    
    position.first = random1()*400;
    position.second = random1()*300;
    IHistogram2D * my2Dhisto = plot2D(position.first, position.second, 
                                      Histo2D_ID, Histo2D_Title, 0., 400., 0., 300., 10, 10);
    IProfile1D * my1Dhprof = profile1D(position.first, position.second, 
                                     HistoProf_ID, HistoProf_Title, 0., 400., 10);
    
    // Debug info:
    msg << MSG::DEBUG << Histo1Db_ID << " All entries: " << my1Dhisto2->allEntries() << " mean: " << my1Dhisto2->mean() 
        << " rms: " << my1Dhisto2->rms() << endreq;
    msg << MSG::DEBUG << Histo2D_ID << " All entries: " << my2Dhisto->allEntries() << " meanX: " << my2Dhisto->meanX() 
        << " rmsX: " << my2Dhisto->rmsX() << " meanY: " << my2Dhisto->meanY() << " rmsY: " << my2Dhisto->rmsY() << endreq;
    msg << MSG::DEBUG << HistoProf_ID << " All entries: " << my1Dhprof->allEntries() << " mean: " << my1Dhprof->mean() 
        << " rms: " << my1Dhprof->rms() << endreq;
    
    // Track calculations for bin 3 in last my1Dhprof:
    ixBin=3;
    if (int(position.first/400*10) == ixBin ){
      sumOfWeights += position.second;
      sumOfSquaredWeights += position.second*position.second;
/*      msg << MSG::DEBUG << HistoProf_ID  << ": all entries: " << counter1 << " x: " << position.first
          << " y: " << position.second <<  " bin: " << ixBin << " binEntries: " << nbinEntries 
          << " sumOfWeights: " << sumOfWeights << " sumOfSquaredWeights: " << sumOfSquaredWeights << endreq;
      msg << MSG::DEBUG << HistoProf_ID  << ": ixBin " << ixBin << " entries " << my1Dhprof->binEntries(ixBin)
          << " mean " << my1Dhprof->binHeight(ixBin) << " rms " << my1Dhprof->binRms(ixBin)
          << " error " << my1Dhprof->binError(ixBin) << endreq;*/
    }
  }
 
  // (Track only last loop step above):
  if(1 == eventtype) counter2++;
  efficiency1= float(counter2)/counter1;
  effHisto->fill(efficiency1);
/*  msg << MSG::DEBUG << "Event: " << counter1 << " counter2: " << counter2 
      << " efficiency1: " << efficiency1 << " effHisto content: ";
  for(int i=0; i<= effHisto->axis().bins();++i){ msg << MSG::DEBUG << effHisto->binEntries(i) << " ";}
  msg << MSG::DEBUG<<  endreq;*/
  
  status="Normal";
  
  if(efficiency1 > .55 ) {
    aboveRef = true;
    strcpy(oldStatus,status.c_str());
    status="High";
  }
  
  index.first = (int)(random1()*10);
  index.second = (int)(random1()*20);
  
  ix.first = random1()*50;
  ix.second  =  (int)(ix.first);
  
  statEntity.fill(position.first, position.second, 1.0);
  // Print content from statEntity MonObject to check
  if(MSG::DEBUG == msgSvc()->outputLevel(name())) statEntity.print();
  
  if (counter1 % 50 == 0) {
    efficiency2=float(counter2)/(time_new-time_old);
  }
  
  int nD=(int)random4();
  m_vectDouble.clear();
  for(int i=0;i<nD;++i)m_vectDouble.push_back(random1()*100);
/*  msg << MSG::DEBUG << "Filled a vector of doubles with " << m_vectDouble.size() << " elements: ";
  for(std::vector<double>::iterator iD=m_vectDouble.begin();iD != m_vectDouble.end();++iD) msg << MSG::DEBUG << *iD << " ";
  msg << MSG::DEBUG << endreq;*/
  monVectorDouble.setValue(m_vectDouble);
  // Print content from monVectorDouble  to check
  if(MSG::DEBUG == msgSvc()->outputLevel(name())) monVectorDouble.print();
  
  int nI=(int)random4();
  m_vectInt.clear();
  for(int i=0;i<nI;++i)m_vectInt.push_back((int)(random1()*50));
/*  msg << MSG::INFO << "Filled a vector of integers with " << m_vectInt.size() << " elements: ";  
  for(std::vector<int>::iterator iI=m_vectInt.begin();iI != m_vectInt.end();++iI) msg << MSG::DEBUG << *iI << " ";
  msg << MSG::INFO << endreq;*/
  monVectorInt.setValue(m_vectInt);
  // Print content from monVectorInt to check
  if(MSG::DEBUG == msgSvc()->outputLevel(name())) monVectorInt.print();
  
  //  delay
  mysleep(sleepTime);
  
  msg << MSG::DEBUG << "******************************************" << endreq;
  
  count_rate_1 = counter1;
  count_rate_2 = counter2;
    
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode GaudiExample::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  
  // Print the final contents of the Profile1D histogram
  AIDA::IProfile1D* my1Dhprof = profile1D(std::string("Profile  histo of y(x)"));
  msg << MSG::DEBUG << "GaudiExample: final contents of the Profile1D histogram my1Dhprof:" << endreq;
  msg << MSG::DEBUG << "ixBin " << "UNDERFLOW" << " entries " << my1Dhprof->binEntries(AIDA::IAxis::UNDERFLOW_BIN) 
      << " mean " << my1Dhprof->binHeight(AIDA::IAxis::UNDERFLOW_BIN) 
      << " rms " << my1Dhprof->binRms(AIDA::IAxis::UNDERFLOW_BIN)  
      << " error " << my1Dhprof->binError(AIDA::IAxis::UNDERFLOW_BIN) << endreq;
  
  int nXBins = my1Dhprof->axis().bins();
  for (int ixBin=0; ixBin< nXBins; ixBin++){
    msg << MSG::DEBUG << "ixBin " << ixBin << " entries " << my1Dhprof->binEntries(ixBin)
        << " mean " << my1Dhprof->binHeight(ixBin) << " rms " << my1Dhprof->binRms(ixBin)
        << " error " << my1Dhprof->binError(ixBin) << endreq;
  }
  
   msg << MSG::DEBUG << "ixBin " << "OVERFLOW" << " entries " << my1Dhprof->binEntries(AIDA::IAxis::OVERFLOW_BIN)
       << " mean " << my1Dhprof->binHeight(AIDA::IAxis::OVERFLOW_BIN) 
       << " rms " << my1Dhprof->binRms(AIDA::IAxis::OVERFLOW_BIN)
       << " error " << my1Dhprof->binError(AIDA::IAxis::OVERFLOW_BIN) << endreq;
  return StatusCode::SUCCESS;
}
