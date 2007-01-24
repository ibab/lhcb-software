// $Id: ParentAlg.cpp,v 1.3 2007-01-24 20:42:24 frankb Exp $

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "ParentAlg.h"

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
DECLARE_ALGORITHM_FACTORY(ParentAlg)

// Constructor
//------------------------------------------------------------------------------
ParentAlg::ParentAlg(const std::string& name, ISvcLocator* ploc)
  : Algorithm(name, ploc), m_publishsvc() {
  //	, m_monitorsvc(){
  //------------------------------------------------------------------------------
  m_publishsvc = 0;
  //  m_monitorsvc = 0;
  //  histoSvc=Algorithm::histoSvc();
}

//------------------------------------------------------------------------------
StatusCode ParentAlg::initialize() {
  //------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  StatusCode sc;

  log << MSG::INFO << "creating sub-algorithms...." << endreq;

  sc =  createSubAlgorithm( "SubAlg", "SubAlg1", m_subalg1);
  if( sc.isFailure() ) {
    log << "Error creating Sub-Algorithm SubAlg1" << endreq;
    return sc;
  }

  sc =  createSubAlgorithm( "SubAlg", "SubAlg2", m_subalg1);
  if( sc.isFailure() ) {
    log << "Error creating Sub-Algorithm SubAlg1" << endreq;
    return sc;
  }

  sc = service("HistogramDataSvc", m_histosvc, true );
  if( !sc.isSuccess() )   {
    log << MSG::FATAL << "Unable to locate HistogramSvc" << endreq;
    return sc;
  }

  sc = serviceLocator()->service("MonitorSvc", m_publishsvc, true );
  if( !sc.isSuccess() )   {
    log << MSG::FATAL << "Unable to locate IPublish interface" << endreq;
    return sc;
  }
  //  sc = serviceLocator()->service("MonitorSvc", m_monitorsvc, true );
  //  if( !sc.isSuccess() )   {
  //    log << MSG::FATAL << "Unable to locate IMonitor interface" << endreq;
  //    return sc;
  //  }

  log << MSG::DEBUG << "MonitorSvc retrieved successfully" << endreq;
  counter1=0;
  counter2=0;
  counter3=0;
  counter4=0;
  aboveRef = false;
  frac1=0.0;
  frac2=0.0;
  position.first = 0;
  position.second = 0;
  VELOeff = 0;
  //  int len = strlen("initializing");
  status=new char[20];
  status=strcpy(status,"initializing");
  oldStatus = "";
  //  log << MSG::INFO << "Booking histograms" << endreq;
  myhisto = m_histosvc->book("1", "eventtype", 5, 0.5, 5.5 );
  myhisto2 = m_histosvc->book("2", "VELO efficiency", 80, 0., 400. );
  myhisto3 = m_histosvc->book("3", "OT efficiency", 60, 0., 300. );
  myhisto4 = m_histosvc->book("4", "ECAL hitmap", 50, 0., 50. );
  myhisto5 = m_histosvc->book("5", "HCAL hitmap", 50, 0., 50. );
  my2Dhisto = m_histosvc->book("6", "Plot of position in XY plane",10,0.,400.,10,0.,300.); 

  log << MSG::INFO << "Declaring Infos to be published" << endreq;
  m_publishsvc->declareInfo("counter1",counter1,"All events",this);
  m_publishsvc->declareInfo("counter2",counter2,"Event type 1",this);
  m_publishsvc->declareInfo("counter3",counter3,"Event type 2",this);
  log << MSG::INFO << "Try to publish a second info with a name already used. Not possible" << endreq;
  m_publishsvc->declareInfo("counter3",counter2,"Event type 1",this);// Try to publish a second info with a name already used. For testing
  m_publishsvc->declareInfo("counter4",counter4,"Event type 3",this);
  m_publishsvc->declareInfo("aboveRef",aboveRef,"#events is above reference",this);
  m_publishsvc->declareInfo("fraction1",frac1,"Ratio 2/3",this);
  m_publishsvc->declareInfo("fraction2",frac2,"Time rate 1-2",this);
  m_publishsvc->declareInfo("xyPosition",position,"Position in XY plane",this);
  log << MSG::INFO << "Warning: No declareInfo for char* type. Will use bool instead!" << endreq; 
  m_publishsvc->declareInfo("status",status,"Trigger status",this);
  m_publishsvc->declareInfo("oldStatus",oldStatus,"Previous Trigger status",this);
  m_publishsvc->declareInfo("eventtype",myhisto,"Event type",this);
  m_publishsvc->declareInfo("VELOefficiency",VELOeff,"VELO efficiency value",this);
  m_publishsvc->declareInfo("VELOefficiency",myhisto2,"VELO efficiency",this);
  m_publishsvc->declareInfo("OTefficiency",myhisto3,"OT efficiency",this);
  m_publishsvc->declareInfo("ECALhitmap",myhisto4,"ECAL hit map",this);
  m_publishsvc->declareInfo("HCALhitmap",myhisto5,"HCAL hit map",this);
  m_publishsvc->declareInfo("xyPositionPlot", my2Dhisto, "Plot of position in XY plane",this);
  //  m_publishsvc->DimStart();
  time(&time_old);

  // 
  // use Random Number Service to generate trigger events
  //
  sc = random.initialize(randSvc(), Rndm::Flat(0.,1.));
  if ( !sc.isSuccess() ) {
    return sc;
  }  

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode ParentAlg::execute() {
  //------------------------------------------------------------------------------
  MsgStream         log( msgSvc(), name() );
  StatusCode sc;
  //  log << MSG::INFO << "executing...." << endreq;

  std::vector<Algorithm*>::const_iterator it  = subAlgorithms()->begin();
  std::vector<Algorithm*>::const_iterator end = subAlgorithms()->end();
  for ( ; it != end; it++) {
    sc = (*it)->execute();
    if( sc.isFailure() ) {
      log << "Error executing Sub-Algorithm" << (*it)->name() << endreq;
    }
  }
  int eventtype=0;
  counter1++;
  if(counter1 > 100 ) aboveRef = true;
  // 
  // use Random Number Service to get generate trigger events
  //
  // 
  float dice1=random();
  float dice2=random();
  float tfdice;
  float bincons[80];
  int binnr,i;
  //eventtype histo
  if (dice1<0.5) {
    counter2++;
    myhisto->fill(1.0);
    eventtype = 1;
  }
  else if(dice1<0.95){
    counter3++;
    myhisto->fill(2.0);
    eventtype = 2;
  }
  else if(dice1<1.0){
    counter4++;
    myhisto->fill(3.0);
    eventtype = 3;
  }
  //"VELO efficiency"
  tfdice=dice1*400.;
  binnr=(int) tfdice/5;
  for (i=0;i<80;i++) bincons[i]=myhisto2->binHeight(i);
  if (tfdice>15. && tfdice<390.) bincons[binnr]=90.+10.*dice2;
  myhisto2->reset();
  for (i=0;i<80;i++) myhisto2->fill(2.5+i*5.,bincons[i]);
  VELOeff = 90.+10.*dice2;
  //"OT efficiency"
  tfdice=dice2*300.;
  binnr=(int) tfdice/5;
  for (i=0;i<60;i++) bincons[i]=myhisto3->binHeight(i);
  if (tfdice>20.) bincons[binnr]=90.+10.*dice1;
  myhisto3->reset();
  for (i=0;i<60;i++) myhisto3->fill(2.5+i*5.,bincons[i]);
  //  if (tfdice>20.) myhisto3->fill(90.+10.*dice1);
  //"ECAL hitmap"
  tfdice=dice1*50.;
  myhisto4->fill(tfdice);
  //"HCAL hitmap"
  tfdice=dice2*50.;
  myhisto5->fill(tfdice);

  position.first = dice1*400;
  position.second = dice2*300;
  //log << MSG::DEBUG <<  "position: " << position.first << " " << position.second << endreq;
  my2Dhisto->fill(position.first,position.second);

  //  counter1=counter1+1;
  if (counter1 % 50 == 0) {
    //	counter2=counter2+1;
    status=strcpy(status,"trigger1");
    oldStatus = std::string(status);
    //	status="trigger1";
  }
  if (counter1 % 100 == 0) {
    //	counter3=counter3+1;
    status=strcpy(status,"trigger2");
    oldStatus = std::string(status);
    // 	status="trigger2";
  }

  frac1=float(counter2)/float(counter3);
  if (counter1 % 50 == 0) {
    time(&time_new);
    frac2=(counter1-counter2)/(time_new-time_old);
  }

  //  delay
  mysleep();

  return StatusCode::SUCCESS;
}


//------------------------------------------------------------------------------
StatusCode ParentAlg::finalize() {
  //------------------------------------------------------------------------------
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "finalizing...." << endreq;

  //  m_publishsvc->undeclareAll( this );
  //  return StatusCode::SUCCESS;

  m_publishsvc->undeclareInfo("counter1",this);
  m_publishsvc->undeclareInfo("counter2",this);
  //  m_publishsvc->undeclareInfo("counter3",this);// Skip and check if undeclareAll will do  it.
  //  m_publishsvc->undeclareInfo("counter4",this);
  m_publishsvc->undeclareInfo("aboveRef",this);
  m_publishsvc->undeclareInfo("fraction1",this);
  m_publishsvc->undeclareInfo("fraction2",this);
  m_publishsvc->undeclareInfo("dummy",this);  // Name not declared, for test
  m_publishsvc->undeclareInfo("xyPosition",this);
  m_publishsvc->undeclareInfo("status",this);
  m_publishsvc->undeclareInfo("oldStatus",this);
  m_publishsvc->undeclareInfo("eventtype",this);
  m_publishsvc->undeclareInfo("VELOefficiency",this);
  m_publishsvc->undeclareInfo("OTefficiency",this);
  m_publishsvc->undeclareAll( this );
  m_publishsvc->undeclareInfo("ECALhitmap",this);
  m_publishsvc->undeclareInfo("HCALhitmap",this);

  //  if ( 0 != m_publishsvc )   {
  //	m_publishsvc->finalize();
  //    m_publishsvc->release();
  //    m_publishsvc = 0;
  //  }
  //  if ( 0 != m_monitorsvc )   {
  //    m_monitorsvc->release();
  //    m_monitorsvc = 0;
  //  }

  //  m_monitorsvc->finalize();

  log << MSG::INFO << "finalized successfully" << endreq;
  
  return StatusCode::SUCCESS;
}
