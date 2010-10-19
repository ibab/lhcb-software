// $Id: ROFarmMonitor.cpp,v 1.8 2010-10-19 14:20:10 ocallot Exp $
// Include files
// C++ include files

#ifdef _MSC_VER
//#include <windows.h>
#include <io.h>
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <cstdlib>
#include <iostream>
#include "GaudiKernel/Timing.h"
// local
#define MBM_IMPLEMENTATION
#include "ROMon/ROMon.h"

#include "ROFarmMonitor.h"

typedef ROMon::Nodeset::Nodes      Nodes;
typedef ROMon::Node::Buffers       Buffers;
typedef ROMon::MBMBuffer::Clients  Clients;
typedef ROMon::Node::Tasks         Tasks;
#define BUILDER_TASK 'M'
#define SENDER_TASK  'D'
#define MOORE_TASK   'G'
#define MEP_BUFFER   'M'
#define RES_BUFFER   'R'
#define EVT_BUFFER   'E'
#define SND_BUFFER   'S'

#define MAXLINE 53
#define MAXTASK 60
//-----------------------------------------------------------------------------
// Implementation file for class : ROFarmMonitor
//
// 2008-03-17 : Olivier Callot
//-----------------------------------------------------------------------------

static void help() {
  std::cout <<"  farm_monitor -option [-option]" << std::endl
            <<"       -d[elay]=<number>   Time delay in millisecond between 2 updates. " << std::endl
            <<"       -p[rint]=<level>    Verbosity of print. 0 = nothing, higher = more. " << std::endl
            << std::endl;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ROFarmMonitor::ROFarmMonitor( int argc, char** argv) : DimClient() {
  RTL::CLI cli(argc,argv,help);
  cli.getopt("delay",         1, m_delay       = 2000 );
  cli.getopt("print",         1, m_print       = 0    );
}

//=============================================================================
// Destructor
//=============================================================================
ROFarmMonitor::~ROFarmMonitor() {
  for ( std::vector<int>::iterator itI = m_svc.begin(); m_svc.end() != itI; ++itI ) {
    dic_release_service( *itI );
  }
}

//=========================================================================
//  Initialization, connects to DIM services
//=========================================================================
void ROFarmMonitor::initialize ( ) {
  DimBrowser dbr;
  char* service, *format;

  m_shifter = new ROShifter();
  char fmt[10] = "C";
  m_dimShiftLeader = new DimService( "FarmMonitor/Shifter/ShiftLeader", fmt, m_shiftLeader, 80 );
  m_dimDataManager = new DimService( "FarmMonitor/Shifter/DataManager", fmt, m_dataManager, 80 );
  
  
  dbr.getServices( "/RO/*/ROpublish" );
  while ( dbr.getNextService( service, format ) ) {
    std::string myService( service );
    m_names.push_back( myService );
  }
  std::cout << "Found " << m_names.size() << " */ROpublist services" << std::endl;
  std::sort( m_names.begin(), m_names.end() );
  for ( std::vector<std::string>::iterator itS = m_names.begin(); m_names.end() != itS; ++itS ) {
    Descriptor* dummy = new Descriptor();
    std::cout << "Connect to Dim service : " << (*itS) << std::endl;
    int svc = dic_info_service((char*)(*itS).c_str(),MONITORED,0,0,0,nodeInfoHandler,(long)dummy,0,0);
    m_datas.push_back( dummy );
    m_svc.push_back( svc );
  }
  
  dbr.getServices( "RunInfo/*/partId" );
  while ( dbr.getNextService( service, format ) ) {
    std::string myService( service );
    std::string part = myService.substr(  myService.find( "/" )+1 );
    part = part.substr( 0, part.find("/") );
    if ( "LHCb" != part && "FEST" != part ) continue;
    
    PartitionDesc* myPart = new PartitionDesc( part );

    if ( 1 < m_print ) std::cout << "Connect to service " << service << std::endl;
    myPart->dimPartitionID = new DimInfo( service, 0, this );

    myService = "RunInfo/"+part+"/HLTnodeList";
    if ( 1 < m_print ) std::cout << "Connect to service " << myService << std::endl;
    myPart->dimHltNodes = new DimInfo( myService.c_str(), 0, this );

    myService = "RunInfo/"+part+"/CALIBnodeList";
    if ( 1 < m_print ) std::cout << "Connect to service " << myService << std::endl;
    myPart->dimCalibNodes = new DimInfo( myService.c_str(), 0, this );

    //-- Create counters, just with the number, 0 to MAXLINE-1
    for ( int kk = 0; MAXLINE >= kk ; ++kk ) {
      RODimLineService* dim = new RODimLineService( part, kk );
      myPart->lineServices.push_back( dim );
    }

    //-- Create counters, just with the number, 0 to MAXTASK-1
    for ( int kk = 0; MAXTASK >= kk ; ++kk ) {
      RODimLineService* dim = new RODimLineService( part, kk, "_TASK" );
      myPart->moniServices.push_back( dim );
    }

    //-- Create counters, just with the number, 0 to 19
    for ( int ll = 0; 20 >= ll ; ++ll ) {
      RODimFsmService* dim = new RODimFsmService( part, ll );
      myPart->fsmServices.push_back( dim );
    }
    myPart->moniNodes.clear();
    myPart->recoNodes.clear();
    if ( myPart->name == "FEST" || myPart->name == "LHCb" ) {
      myPart->moniNodes.push_back( "mona0802" );
      myPart->moniNodes.push_back( "mona0803" );
      myPart->moniNodes.push_back( "mona0804" );
      myPart->moniNodes.push_back( "mona0805" );
      myPart->moniNodes.push_back( "mona0806" );
      myPart->moniNodes.push_back( "mona0807" );
      myPart->moniNodes.push_back( "mona0808" );
      myPart->moniNodes.push_back( "mona0809" );
      myPart->moniNodes.push_back( "mona0810" );
      myPart->recoNodes.push_back( "mona0902" );
      myPart->recoNodes.push_back( "mona0903" );
      myPart->recoNodes.push_back( "mona0904" );
      myPart->recoNodes.push_back( "mona0905" );
      myPart->recoNodes.push_back( "mona0906" );
      myPart->recoNodes.push_back( "mona0907" );
      myPart->recoNodes.push_back( "mona0908" );
      myPart->recoNodes.push_back( "mona0909" );
      myPart->recoNodes.push_back( "mona0910" );
      myPart->recoNodes.push_back( "mona0911" );
      myPart->recoNodes.push_back( "mona0912" );
      myPart->recoNodes.push_back( "mona0913" );
      myPart->recoNodes.push_back( "mona0914" );
      myPart->recoNodes.push_back( "mona0915" );
      myPart->recoNodes.push_back( "mona0916" );
      myPart->recoNodes.push_back( "mona0917" );
      myPart->recoNodes.push_back( "mona0918" );
      myPart->recoNodes.push_back( "mona0919" );
      myPart->recoNodes.push_back( "mona0920" );
    }
    m_partitions.push_back( myPart );
  }
  
  // Start the DimServer providing the services
  m_dimServer = new DimServer();
  std::string serverName = "FarmMonitor";
  m_dimServer->start( serverName.c_str() );

  m_stateName.push_back( "Unknown" );
  m_stateName.push_back( "NOT_READY" );
  m_stateName.push_back( "READY" );
  m_stateName.push_back( "RUNNING" );
  m_stateName.push_back( "STOPPED" );
  m_stateName.push_back( "ERROR" );

  m_lastTime = System::currentTime( System::microSec );
}

//=========================================================================
//  Handles the update of the DimInfo services, i.e. partition informations
//=========================================================================
void ROFarmMonitor::infoHandler ( ) {
  if (1 < m_print ) std::cout << "infoHandler called" << std::endl;
  DimInfo* curr = getInfo();
  for ( std::vector<PartitionDesc*>::iterator itP = m_partitions.begin(); m_partitions.end() != itP; ++itP ) {
    if ( (*itP)->dimPartitionID == curr ) {
      int partID = curr->getInt();
      std::cout << "Update partitionID for " << (*itP)->name;
      printf( " ID %6d = hexa %4x\n", partID, partID );
      (*itP)->partitionID = partID;
    } else if ( (*itP)->dimHltNodes == curr ) {
      std::cout << "Update HltNodes for " << (*itP)->name << std::endl;
      // Update the list of nodes
      char* nodeList;
      (*itP)->hltNodes.clear();
      nodeList = curr->getString();
      int totL = curr->getSize();
      while ( 0 < totL ) {
        std::string address = nodeNameFromAddress( nodeList );
        std::cout << "HLT node " << nodeList << " is " << address << std::endl;
        (*itP)->hltNodes.push_back( address );
       
        int ll = strlen( nodeList ) + 1;
        nodeList += ll;
        totL -= ll;
      }
    } else if ( (*itP)->dimCalibNodes == curr ) {
      std::cout << "Found CalibNodes for " << (*itP)->name << std::endl;
      // Update the list of nodes
      char* nodeList;
      (*itP)->calibNodes.clear();
      nodeList = curr->getString();
      int totL = curr->getSize();
      while ( 0 < totL ) {
        std::string address = nodeNameFromAddress( nodeList );
        std::cout << "Calib node " << nodeList << " is " << address << std::endl;
        (*itP)->calibNodes.push_back( address );
       
        int ll = strlen( nodeList ) + 1;
        nodeList += ll;
        totL -= ll;
      }
    }
  }
}
//=========================================================================
//  Convert a node address in a C string to a node name
//=========================================================================
std::string ROFarmMonitor::nodeNameFromAddress ( char* add ) {
#ifdef _MSC_VER
  return "?";
#else
  char nodeName[100];
  int nodeLen = 99;
  struct sockaddr_in sa;
  int saLen = sizeof(struct sockaddr);
  sa.sin_family = AF_INET;
  inet_aton( add, &sa.sin_addr );
  getnameinfo( (const sockaddr*)&sa, saLen, nodeName, nodeLen, 0, 0, 0 );
  std::string address(nodeName);
  address = address.substr( 0, address.find( "-" ) );
  return address;
#endif
}
//=========================================================================
/// Routine called when one of the node's information is updated
//=========================================================================
void ROFarmMonitor::nodeInfoHandler(void* tag, void* address, int* size) {
  if ( address && tag && size && *size>0 ) {
    size_t len = *size;
    ROMon::ROMonData data(address);
    if ( 2 != data.type() )  return;
    Descriptor* d = *(Descriptor**)tag;    
    if ( d->length < len ) {
      d->length = len;
      d->reserve( size_t(1.2*len) );
    }
    d->actual = len;
    ::memcpy(d->data<char>(),address,d->actual);
  }
}

//=========================================================================
/// Update all displays
//=========================================================================
void ROFarmMonitor::update( )   {
  dim_lock(); // Avoid being interrupted now...


  //== Check the shift crew
  if ( m_shifter->hasChanged() ) {
    strcpy( m_shiftLeader, m_shifter->getShiftLeader().c_str() );
    strcpy( m_dataManager, m_shifter->getDataManager().c_str() );
    m_dimShiftLeader->updateService();
    m_dimDataManager->updateService();
    
    std::cout << "Shift Leader " << m_shiftLeader << std::endl;
    std::cout << "DataManager " << m_dataManager << std::endl;
  }

  longlong now = System::currentTime( System::microSec );
  float dt = .000001 * float( now - m_lastTime );
  m_lastTime = now;
  if ( 1 < m_print ) std::cout << "dt " << dt << std::endl;  

  const char* fmt = " %-12s   Mep%10d Evt%10d (Con%10d) Snd%10d (Acc%10d) Tsk%5d \n";

  for ( std::vector<PartitionDesc*>::iterator itP = m_partitions.begin(); 
        m_partitions.end() != itP; ++itP ) {
    int myPartID = (*itP)->partitionID;
    std::vector<std::string>& hltNodes = (*itP)->hltNodes;
    std::vector<std::string>& calNodes = (*itP)->calibNodes;
    std::vector<std::string>& monNodes = (*itP)->moniNodes;
    std::vector<std::string>& recNodes = (*itP)->recoNodes;
    std::vector<const ROMon::FSMTask*> notReadyTasks;
    std::vector<const ROMon::FSMTask*> readyTasks;
    std::vector<const ROMon::FSMTask*> runningTasks;
    std::vector<const ROMon::FSMTask*> stoppedTasks;
    std::vector<const ROMon::FSMTask*> errorTasks;
    std::vector<const ROMon::FSMTask*> otherTasks;
    std::string partName = (*itP)->name + "_";
    std::string eventBuffer = "Events_" + (*itP)->name;
    
    if ( 2 < m_print ) std::cout << "Starting part = " << (*itP)->name << std::endl;
    
    RONodeCounter sumHlt( "Total HLT" );
    RONodeCounter sumCal( "Calibration" );
    RONodeCounter sumMon( "Total Monit" );
    RONodeCounter sumRec( "Total Reco" );
    std::vector<RONodeCounter> hltCounters;
    std::vector<RONodeCounter> calCounters;
    std::vector<RONodeCounter> monCounters;
    std::vector<RONodeCounter> recCounters;

    std::vector<RONodeCounter> prevCounters = (*itP)->prevCounters;
    unsigned int lPrefx = (*itP)->name.length()+7;  // task name prefixed by partition + _MONA08
    
    for ( std::vector<Descriptor*>::iterator itD = m_datas.begin(); m_datas.end() != itD; ++itD ) {
      const ROMon::Nodeset* ns = (*itD)->data<const ROMon::Nodeset>();
      if ( 0 != ns ) {
        for (Nodes::const_iterator n=ns->nodes.begin(); n!=ns->nodes.end(); n=ns->nodes.next(n))  {
          //== is this node in the partition's list ?
          bool isHlt = false;
          bool isCalib = false;
          bool isMoni  = false;
          bool isReco  = false;
          if ( hltNodes.end() != std::find( hltNodes.begin(), hltNodes.end(), (*n).name ) ) isHlt   = true;
          if ( calNodes.end() != std::find( calNodes.begin(), calNodes.end(), (*n).name ) ) isCalib = true;
          if ( monNodes.end() != std::find( monNodes.begin(), monNodes.end(), (*n).name ) ) isMoni  = true;
          if ( recNodes.end() != std::find( recNodes.begin(), recNodes.end(), (*n).name ) ) isReco  = true;
          if ( !isHlt && !isCalib && !isMoni && ! isReco ) continue;
          if ( 1 < m_print ) std::cout << "Part " << (*itP)->name << " node " << (*n).name << std::endl;

          double newTime = double( (*n).time -1236100000 ) + double( (*n).millitm ) * 0.001;
          if ( 2 < m_print ) std::cout << "   Update time " << newTime << std::endl;

          if ( isHlt || isCalib ) {
            
            RONodeCounter cntr( (*n).name );          
            const Buffers& buffs = *(*n).buffers();
            for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
              const Clients& clients = (*ib).clients;
              char b = (*ib).name[0];
              
              if ( 1 < m_print ) std::cout << "  Buffer " << (*ib).name << " prod " << (*ib).ctrl.tot_produced << std::endl;
              
              switch (b) {
              case MEP_BUFFER : cntr.updateMep( (*ib).ctrl.tot_produced );  break;
              case EVT_BUFFER : cntr.updateEvt( (*ib).ctrl.tot_produced );  break;
              case SND_BUFFER : cntr.updateSnd( (*ib).ctrl.tot_produced );  break;
              default: break;
              }
              for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
                int  pID = (*ic).partitionID;
                if ( myPartID != pID ) continue;
                if ( 1 < m_print ) {
                  printf( "   Node %s  Buffer %s  Task %s Events %d  Part %4x\n", 
                          (*n).name, (*ib).name, (*ic).name, (*ic).events, pID );
                }
                char* p = strchr((*ic).name,'_');
                if ( p ) {
                  switch(*(++p)) {
                  case SENDER_TASK:
                    if( b == SND_BUFFER ) cntr.updateAcc( (*ic).events );
                    break;
                  case MOORE_TASK:
                    if( b == EVT_BUFFER ||
                        b == MEP_BUFFER) {
                      cntr.updateCon( (*ic).events );
                      cntr.updateTsk( 1 );
                    }
                    break;
                  default:
                    break;
                  }
                }
              }
            }
            if ( 0 < cntr.tsk() ) {
              if ( 0 < m_print ) {
                printf( fmt, (*n).name,  cntr.mep(), cntr.evt(), cntr.con(), cntr.snd(), cntr.acc(), cntr.tsk() );
              }
              //== Compute rates
              std::vector<RONodeCounter>::iterator itN;
              for ( itN = prevCounters.begin(); prevCounters.end() != itN; ++itN ) {
                if ( (*itN).name() == cntr.name() ) cntr.increment( *itN, newTime );
              }
              
              if ( isHlt ) {
                sumHlt.sum( cntr );
                hltCounters.push_back( cntr );
              } else if ( isCalib ) {
                sumCal.sum( cntr );
                calCounters.push_back( cntr );
              }
            }
          } else {
            const Buffers& buffs = *(*n).buffers();
            for(Buffers::const_iterator ib=buffs.begin(); ib!=buffs.end(); ib=buffs.next(ib))  {
              const Clients& clients = (*ib).clients;
              if ( 0 != eventBuffer.compare( (*ib).name ) ) continue;
              
              if ( 1 < m_print ) std::cout << "  Buffer " << (*ib).name << " prod " << (*ib).ctrl.tot_produced << std::endl;
              
              for (Clients::const_iterator ic=clients.begin(); ic!=clients.end(); ic=clients.next(ic))  {
                int  pID = (*ic).partitionID;
                if ( myPartID != pID ) continue;
                if ( 1 < m_print ) {
                  printf( "   Node %s  Buffer %s  Task %s Events %d  Part %4x\n", 
                          (*n).name, (*ib).name, (*ic).name, (*ic).events, pID );
                }
                if ( isMoni || isReco ) {              // only consumers, except the receiver
                  std::string taskName( (*ic).name );
                  if ( std::string::npos == taskName.find( "_RCV" ) ) {  // all tasks but the receiver  
                    RONodeCounter cntr( taskName.substr( lPrefx ).c_str() );          
                    cntr.updateCon( (*ic).events );
                    cntr.updateTsk( 1 );
                    std::vector<RONodeCounter>::iterator itN;
                    for ( itN = prevCounters.begin(); prevCounters.end() != itN; ++itN ) {
                      if ( (*itN).name() == cntr.name() ) cntr.increment( *itN, newTime );
                    }
                    if ( isMoni ) {
                      sumMon.sum( cntr );
                      monCounters.push_back( cntr );
                    } else if ( isReco ) {
                      sumRec.sum( cntr );
                      recCounters.push_back( cntr );
                    }
                  }
                }
              }
            }
          }
          
          //== Count tasks, assign them to a proper vector.
          const Tasks& tasks = *(*n).tasks();
          for( Tasks::const_iterator it=tasks.begin(); it!= tasks.end(); it = tasks.next(it))  {
            if ( isMoni || isReco ) {
              std::string taskName( (*it).name );
              if ( taskName.find( partName ) != 0 ) continue;
            }
            if ( 1 < m_print ) std::cout << "Node " << (*n).name << " task " << (*it).name 
                                         << " state " << (*it).state << std::endl;
            
            if ( 'F' == (*it).metaState ) {
              errorTasks.push_back( &(*it) );
            } else if ( 'N' == (*it).state ) {
              notReadyTasks.push_back( &(*it) );
            } else if ( 'r' == (*it).state ) {
              readyTasks.push_back( &(*it) );
            } else if ( 'R' == (*it).state ) {
              runningTasks.push_back( &(*it) );
            } else if ( 'S' == (*it).state ) {
              stoppedTasks.push_back( &(*it) );
            } else if ( 'E' == (*it).state ) {
              errorTasks.push_back( &(*it) );
            } else {
              otherTasks.push_back( &(*it) );
              std::cout << "UNKNOWN task on Node " << (*n).name << " task '" << (*it).name 
                        << "' state :" << (*it).state << std::endl;
            }
          }
        }
      }
    }

    if ( 0 < m_print ) {
      std::string title = "Total "+ (*itP)->name;
      printf( fmt, title.c_str(), sumHlt.mep(), sumHlt.evt(), sumHlt.con(), sumHlt.snd(), sumHlt.acc(), sumHlt.tsk() );
      if ( 0 < sumCal.tsk() ) {
        title = "Calib "+ (*itP)->name;
        printf( fmt, title.c_str(), sumCal.mep(), sumCal.evt(), sumCal.con(), sumCal.snd(), sumCal.acc(), sumCal.tsk() );
      }
      std::cout << "            FSM : " << notReadyTasks.size() << " NOT_READY, " << readyTasks.size() << " READY " 
                << runningTasks.size() << " RUNNING, " << stoppedTasks.size() << " STOPPED, "
                << errorTasks.size() << " ERROR, " 
                << otherTasks.size() << " other." << std::endl;
    }

    //=============================================================
    //== Update the services for nodes
    //=============================================================
    (*itP)->prevCounters = hltCounters;  //== save current counters for next iteration.
    (*itP)->prevCounters.insert( (*itP)->prevCounters.end(), calCounters.begin(), calCounters.end() );
    (*itP)->prevCounters.insert( (*itP)->prevCounters.end(), monCounters.begin(), monCounters.end() );
    (*itP)->prevCounters.insert( (*itP)->prevCounters.end(), recCounters.begin(), recCounters.end() );

    if ( 2 < m_print ) std::cout << "Update the services " << std::endl;

    std::vector<RODimLineService*>::iterator itS = (*itP)->lineServices.begin();
    std::vector<RONodeCounter>::iterator itN;
    RONodeCounter blank( "" );
    (*itS++)->update( sumHlt );
    //== If too many lines, compress: sum nodes in sub-farms.
    if ( MAXLINE-2 < hltCounters.size() ) {
      if ( 2 < m_print ) std::cout << "Compress by farm " << std::endl;
      std::vector<RONodeCounter> farmCounters;
      std::string farmName = "";
      for ( itN = hltCounters.begin(); hltCounters.end() != itN; ++itN ) {
        if ( (*itN).name().substr(0,6) != farmName ) {
          farmName = (*itN).name().substr(0,6);
          RONodeCounter farm( farmName.c_str() );
          farmCounters.push_back( farm );
        }
        farmCounters.back().sum( *itN);
      }
      hltCounters = farmCounters;
    }

    if ( 2 < m_print ) std::cout << "Update services " << std::endl;
    
    for ( itN = hltCounters.begin(); hltCounters.end() != itN; ++itN ) {
      if ( (*itP)->lineServices.end() != itS ) (*itS++)->update( *itN );
    }
    if ( 0 < sumCal.tsk() ) {
      if ( (*itP)->lineServices.end() != itS ) (*itS++)->update( blank );
      for ( itN = calCounters.begin(); calCounters.end() != itN; ++itN ) {
        if ( (*itP)->lineServices.end() != itS ) (*itS++)->update( *itN );
      }
    }
    while ( (*itP)->lineServices.end() != itS ) (*itS++)->update( blank );

    itS = (*itP)->moniServices.begin();
    if ( 0 < sumMon.tsk() ) {
      if ( (*itP)->moniServices.end() != itS ) (*itS++)->update( sumMon );
      for ( itN = monCounters.begin(); monCounters.end() != itN; ++itN ) {
        if ( (*itP)->moniServices.end() != itS ) (*itS++)->update( *itN );
      }
    }

    if ( 0 < sumRec.tsk() ) {
      if ( (*itP)->moniServices.end() != itS ) (*itS++)->update( blank );
      if ( (*itP)->moniServices.end() != itS ) (*itS++)->update( sumRec );
      for ( itN = recCounters.begin(); recCounters.end() != itN; ++itN ) {
        if ( (*itP)->moniServices.end() != itS ) (*itS++)->update( *itN );
      }
    }
    while ( (*itP)->moniServices.end() != itS ) (*itS++)->update( blank );
    //=============================================================
    //== Update the services for Tasks
    //=============================================================

    if ( 2 < m_print ) std::cout << "Update for tasks " << std::endl;
    
    std::vector<RODimFsmService*>::iterator itF = (*itP)->fsmServices.begin();

    unsigned int maxTask = 0;
    std::vector<const ROMon::FSMTask*>* best = 0;
    int level = 0;
    
    if ( maxTask < notReadyTasks.size() ) {
      maxTask = notReadyTasks.size();
      best = &notReadyTasks;
      level = 1;
    }
    if ( maxTask < readyTasks.size() ) {
      maxTask = readyTasks.size();
      best = &readyTasks;
      level = 2;
    }
    if ( maxTask < runningTasks.size() ) {
      maxTask = runningTasks.size();
      best = &runningTasks;
      level = 3;
    }
    if ( maxTask < stoppedTasks.size() ) {
      maxTask = stoppedTasks.size();
      best = &stoppedTasks;
      level = 4;
    }

    if ( 2 < m_print ) std::cout << "Max level " << level << " maxTask " << maxTask << std::endl;

    char message[200];
    if ( 0 < maxTask ) {
      sprintf( message, "%4d tasks", int(best->size()) );
      (*itF++)->update( level, m_stateName[level], message );
    }

    if ( 2 < m_print ) std::cout << "Report bad tasks " << std::endl;

    char timeStamp[60];
    std::vector<const ROMon::FSMTask*>::iterator itT;
    unsigned int maxSame = 20;
    time_t myTime;
    
    if ( best != &notReadyTasks || maxSame > maxTask ) {
     if ( 2 < m_print ) std::cout << "NotReady tasks " << std::endl; 
      for ( itT = notReadyTasks.begin(); notReadyTasks.end() != itT; ++itT ) {
        if ( itF != (*itP)->fsmServices.end() ) {
          myTime =  (*itT)->doneCmd;
          strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
          (*itF++)->update( 1, m_stateName[1], std::string( (*itT)->name ) + " since " + std::string(timeStamp) );
        }
      }
    }
    if ( best != &readyTasks || maxSame > maxTask ) {
     if ( 2 < m_print ) std::cout << "Ready tasks " << std::endl;
      for ( itT = readyTasks.begin(); readyTasks.end() != itT; ++itT ) {
        if ( itF != (*itP)->fsmServices.end() ) {
          myTime =  (*itT)->doneCmd;
          strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
          (*itF++)->update( 2, m_stateName[2], std::string( (*itT)->name ) + " since "+std::string(timeStamp) );
        }
      }
    }
    if ( best != &runningTasks || maxSame > maxTask ) {
     if ( 2 < m_print ) std::cout << "Running tasks " << std::endl;
      for ( itT = runningTasks.begin(); runningTasks.end() != itT; ++itT ) {
        if ( itF != (*itP)->fsmServices.end() ) {
          myTime =  (*itT)->doneCmd;
          strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
          (*itF++)->update( 3, m_stateName[3], std::string( (*itT)->name ) + " since "+std::string(timeStamp) );
        }
      }
    }
    if ( best != &stoppedTasks || maxSame > maxTask ) {
     if ( 2 < m_print ) std::cout << "Stopped tasks " << std::endl;
      for ( itT = stoppedTasks.begin(); stoppedTasks.end() != itT; ++itT ) {
        if ( itF != (*itP)->fsmServices.end() ) {
          myTime =  (*itT)->doneCmd;
          strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
          (*itF++)->update( 4, m_stateName[4], std::string( (*itT)->name ) + " since "+std::string(timeStamp) );
        }
      }
    }
    
    if ( 2 < m_print ) std::cout << "Update error tasks" << std::endl;

    for ( itT = errorTasks.begin(); errorTasks.end() != itT; ++itT ) {
      myTime =  (*itT)->doneCmd;
      strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
      if ( 1 < m_print )std::cout << " ERROR state since " << timeStamp
                                  << " FSM state " << (*itT)->state 
                                  << " target " << (*itT)->targetState 
                                  << " meta " << (*itT)->metaState  
                                  << " for task " << (*itT)->name 
                                  << std::endl;
      if ( itF != (*itP)->fsmServices.end() ) {
        (*itF++)->update( 5, m_stateName[5], std::string( (*itT)->name ) + " since "+std::string(timeStamp) );
      }
    }
    for ( itT = otherTasks.begin(); otherTasks.end() != itT; ++itT ) {
      myTime =  (*itT)->doneCmd;
      strftime( timeStamp, 40, "%d-%b-%Y %H:%M:%S", localtime( &myTime ) );
      if ( 1 < m_print ) std::cout << " OTHER  state since " << timeStamp
                                   << " FSM state " << (*itT)->state 
                                   << " target " << (*itT)->targetState 
                                   << " meta " << (*itT)->metaState  
                                   << " for task " << (*itT)->name 
                                   << std::endl;
      if ( itF != (*itP)->fsmServices.end() ) {
        (*itF++)->update( 5, m_stateName[0], std::string( (*itT)->name ) + " state " + (*itT)->state );
      }
    }
    while ( (*itP)->fsmServices.end() != itF ) (*itF++)->update( 0, "", "" );
    
    
  }
  dim_unlock();
  
  lib_rtl_sleep(m_delay);
}

extern "C" int farm_monitor(int argc,char** argv) {
  ROFarmMonitor disp(argc,argv);
  disp.initialize();
  while(1) {
    disp.update();
  }
  return 1;
}

//=============================================================================
