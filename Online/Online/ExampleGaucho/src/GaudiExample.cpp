// $Id: GaudiExample.cpp,v 1.5 2007-08-20 09:36:19 evh Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiExample.h"
#include "AIDA/IAxis.h"

#include <cstring>
#include <cmath>

#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep() win::Sleep(100) 
#else
# include <unistd.h>
# define mysleep() usleep(100000) 
#endif


// Static Factory declaration
DECLARE_ALGORITHM_FACTORY(GaudiExample)

// Constructor
//------------------------------------------------------------------------------
GaudiExample::GaudiExample(const std::string& name, ISvcLocator* ploc)
  : Algorithm(name, ploc) {}

//------------------------------------------------------------------------------
StatusCode GaudiExample::initialize() {
  //------------------------------------------------------------------------------
  StatusCode sc = Algorithm::initialize(); // must be executed first
  MsgStream msg(msgSvc(), name());

  sc = service("HistogramDataSvc", m_histosvc, true );
  if( !sc.isSuccess() )   {
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

  oldStatus=new char[20];
  
  std::string Histo1Da_ID;
  std::string Histo1Da_Title;
  std::string Histo1Db_ID;
  std::string Histo1Db_Title;
  std::string Histo2D_ID;
  std::string Histo2D_Title;
  std::string HistoProf_ID;
  std::string HistoProf_Title;

  //  msg << MSG::INFO << "Booking histograms" << endreq;
  Histo1Da_Title = "Event type";
  Histo1Db_Title = "Mass plot";
  Histo2D_Title = "Plot of position in XY plane";
  HistoProf_Title = "Profile  histo of y(x)"; \
  // rearrange this to make it easy to change the nb of histograms
  for (int i=0;i<=250;i++) {
     Histo1Da_ID = "1Da"+boost::lexical_cast<std::string>(i);
     Histo1Db_ID = "1Db"+boost::lexical_cast<std::string>(i);
     Histo2D_ID = "2D"+boost::lexical_cast<std::string>(i);
     HistoProf_ID= "Prof"+boost::lexical_cast<std::string>(i);
     
     my1Dhisto1 = m_histosvc->book(Histo1Da_ID, Histo1Da_Title, 5, 0.5, 5.5 );
     my1Dhisto2 = m_histosvc->book(Histo1Db_ID, Histo1Db_Title, 30, 2900., 3200. );
     my2Dhisto  = m_histosvc->book(Histo2D_ID, Histo2D_Title,10,0.,400.,10,0.,300.); 
     my1Dhprof  = m_histosvc->bookProf(HistoProf_ID, HistoProf_Title,10,0.,400.);
     declareInfo(Histo1Da_ID,my1Dhisto1,Histo1Da_Title);
     declareInfo(Histo1Db_ID,my1Dhisto2,Histo1Db_Title);
     declareInfo(Histo2D_ID,my2Dhisto,Histo2D_Title);
     declareInfo(HistoProf_ID, my1Dhprof, HistoProf_Title);
  }
  nbinEntries=0;
  sumOfWeights=0;
  sumOfSquaredWeights=0;
  
  // No need to instantiate explicitly the MonitorScv. The GaudKernel/Algorithm has
  // a method declareInfo that and do this automatically for you
  msg << MSG::INFO << "Declaring infos to be published" << endreq;
  declareInfo("counter1",counter1,"All events");
  declareInfo("counter2",counter2,"Event of type 1");
  
  msg << MSG::INFO << "Test: Try to publish a second info with a name already used. Not possible" << endreq;
  declareInfo("counter2",counter2,"Event type 1");// Try to publish a second info with a name already used. For testing
  
  declareInfo("efficiency1",efficiency1,"Ratio type 1/all");
  declareInfo("aboveRef",aboveRef,"#events is above reference");
  
  msg << MSG::INFO << "Test: No declareInfo for char* type. By default, will use bool instead!" << endreq; 
  declareInfo("oldStatus",oldStatus,"Previous Trigger status");
  
  declareInfo("status",status,"Trigger status");
  

  declareInfo("efficiency2",efficiency2,"Time rate 1-2");
  
  
  myEvent = new event;
  if( 0 == myEvent) {
    msg << MSG::FATAL << "Unable to allocate structure event"  << endreq;
    return StatusCode::FAILURE;
  }
  
  myEvent->evt = 0;
  declareInfo("Event", "I:1;D:2;C", myEvent, sizeof(event), "Event data" );
  
  
  time(&time_old);
  
  // 
  // use Random Number Service to generate trigger events
  //
  sc = random1.initialize(randSvc(), Rndm::Flat(0.,1.));
  if ( !sc.isSuccess() ) {
    return sc;
  }  
  sc = random2.initialize(randSvc(), Rndm::Gauss(3096.,10.));
  if ( !sc.isSuccess() ) {
    return sc;
  }  
  sc = random3.initialize(randSvc(), Rndm::Gauss(0.,4000.));
  if ( !sc.isSuccess() ) {
    return sc;
  }
  
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode GaudiExample::execute() {
  //------------------------------------------------------------------------------
  MsgStream         msg( msgSvc(), name() );
  //  msg << MSG::INFO << "executing...." << endreq;
  
  int eventtype;
  counter1++;
  float dice1=random1();
  if(dice1 > 0.5 )     {eventtype=1; counter2++;}
  else if(dice1 > 0.4)  eventtype=2;
  else if(dice1 > 0.25) eventtype=3;
  else if(dice1 > 0.2)  eventtype=4;
  else 		        eventtype=5;
//  my1Dhisto1->fill(eventtype);
  
  efficiency1= float(counter2)/counter1;
  
  status="Normal";
  if(efficiency1 > .55 ) {
    aboveRef = true;
    strcpy(oldStatus,status.c_str());
    status="High";
  }
  
  
  double mass;
  if(eventtype== 1)mass = random2();
  else             mass = 2800+random1()*500;
 // my1Dhisto2->fill(mass);
  
  position.first = random1()*400;
  position.second = random1()*300;
  //log << MSG::DEBUG <<  "position: " << position.first << " " << position.second << endreq;
  for (int i=0;i<=250;i++) {
     std::string Histo1Da = "1Da"+boost::lexical_cast<std::string>(i);
     std::string Histo1Db = "1Db"+boost::lexical_cast<std::string>(i);
     std::string Histo2D = "2D"+boost::lexical_cast<std::string>(i);
     std::string ProfHisto = "Prof"+boost::lexical_cast<std::string>(i);
     StatusCode sc = m_histosvc->retrieveObject(Histo1Da,my1Dhisto1);
     if ( my1Dhisto1 != 0) {
       my1Dhisto1->fill(eventtype);
     } 
     sc = m_histosvc->retrieveObject(Histo1Db,my1Dhisto2);
     if ( my1Dhisto2 != 0) { 
        my1Dhisto2->fill(mass);
     }
     sc = m_histosvc->retrieveObject(Histo2D,my2Dhisto);
     if ( my2Dhisto != 0) { 
        my2Dhisto->fill(position.first,position.second);
     }
     sc = m_histosvc->retrieveObject(ProfHisto,my1Dhprof);
     if ( my1Dhprof != 0) { 
        my1Dhprof->fill(position.first,position.second);
     }	
  }
  // Track calculations for bin 3 in my1Dhprof:
  int ixBin=3;
  if (int(position.first/400*10) == ixBin ){
    nbinEntries++;
    sumOfWeights += position.second;
    sumOfSquaredWeights += position.second*position.second;
    msg << MSG::DEBUG << "my1Dhprof: all entries: " << counter1 << " x: " << position.first 
        << " y: " << position.second <<  " bin: " << ixBin << " binEntries: " << nbinEntries << " sumOfWeights: " << sumOfWeights 
        << " sumOfSquaredWeights: " << sumOfSquaredWeights << endreq;
    msg << MSG::DEBUG << "my1Dhprof: ixBin " << ixBin << " entries " << my1Dhprof->binEntries(ixBin)
        << " mean " << my1Dhprof->binHeight(ixBin) << " rms " << my1Dhprof->binRms(ixBin)
        << " error " << my1Dhprof->binError(ixBin) << endreq;
  }
  
  if (counter1 % 50 == 0) {
    time(&time_new);
    efficiency2=float(counter2)/(time_new-time_old);
  }
  
  (myEvent->evt)++;
  myEvent->px = random3();
  myEvent->py = 2*random3();
  if(fabs(myEvent->py) > 8000) strcpy(myEvent->status, "SELECTED");
  else                         strcpy(myEvent->status, "REJECTED");
  if(counter1 % 100 == 0) {
    msg << MSG::DEBUG << "Structure myEvent: evt = " << myEvent->evt << " px = " << myEvent->px
        << " py = " << myEvent->py << " status = " << myEvent->status << endreq;
  }

  //  delay
  mysleep();
  
  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode GaudiExample::finalize() {
  //------------------------------------------------------------------------------
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "finalizing...." << endreq;
  
  // Print the final contents of the Profile1D histogram
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
