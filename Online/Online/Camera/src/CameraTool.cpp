// $Id: CameraTool.cpp,v 1.11 2010-01-14 14:39:52 nmangiaf Exp $
// Include files

// local
#include "CameraTool.h"
#include "client.h"

// ROOT
#include "TH1.h"
#include "TH2.h"

// boost
#include "boost/lexical_cast.hpp"

#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : CameraTool
//
// 2007-05-30 : Claus Buszello
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CameraTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CameraTool::CameraTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_camc(NULL)
    , m_out()
    , m_lastHistoNum ( 0 )
{
  declareInterface<ICameraTool>(this);
  declareProperty("ServerName",m_servername="127.0.0.1");
  declareProperty("ServerPort",m_servport=45123);
  declareProperty("ServerNames",m_servernames);
  declareProperty("ServerPorts",m_servports);
  declareProperty("Enabled",m_dosend=true);
  numErrBZ = 0;
  numErrCN = 0;
}

//=============================================================================
// Destructor
//=============================================================================
CameraTool::~CameraTool() {}

//=============================================================================
StatusCode CameraTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
 
  if (m_dosend) {
    if (m_servernames.size() > 0){
      unsigned int num = m_servernames.size();
      if (m_servports.size()<num) num = m_servports.size();
      
      for (unsigned int i = 0;i<num;++i){
        client * c = new client(m_servernames[i].c_str(), m_servports[i]);
        if (c->m_healthy==0) {
          Warning( "Failed to setup CAMERA client" ).ignore();
          delete c;
          c = NULL;
        }
        else {
          m_clients.push_back(c);
        }
      }
      
    }
    else{
      m_camc = new client(m_servername.c_str(),m_servport);
      if (m_camc->m_healthy==0) {
        Warning( "Failed to setup CAMERA client" ).ignore();
        delete m_camc;
        m_camc = NULL;

      }
    
      info() << "Made CAMERA client to "<<m_servername.c_str()<<":"<<m_servport<<endmsg;
      m_clients.push_back(m_camc);
      
    }
  }// if(m_dosend)
  else
  {
    debug() << "Camera set to NOT SEND." << endreq;
  }
  
  debug() << "Setup of CameraTool is done"<<endreq;
  return sc;
}

//=============================================================================
StatusCode CameraTool::finalize()
{
  if (NULL != m_camc) {
    delete m_camc;
    m_camc = NULL;
  }// if(NULL!=m_camc)
  
  return GaudiTool::finalize();
}

//=============================================================================

int CameraTool::SendAndClear(MessageLevel l,const std::string& who,const std::string& what)
{
  std::stringstream ss;
  std::stringstream s;

  ss << who<<"/"<<l<<"/"<<what<<"\n";

  bool success = false;
  //Loop over m_camc[] entries
  //for () {
  std::vector<client *>::iterator itc;
 
    
  if (m_dosend) {
    // loop over clients
    for (itc = m_clients.begin(); itc!= m_clients.end(); itc++){
      m_camc = (*itc);
      if (m_camc==NULL) continue;

      //put attached message into the stream s
      if(l != ICameraTool::CAM_COMMAND)if (m_out.entries()>0) m_out.tostream(s);
      if (m_camc->Connect()>0) {
        char buf[3];
        while (  m_camc->rd(buf,2)==-2){};
        buf[2] = 0;
        if (strncmp(buf,"GO",2) == 0 ){
          success =true;
          int ret=0;
          // if (m_out.entries()<1) m_camc->nowait();
          ret = m_camc->wr(ss.str().c_str(),ss.str().length());
          if (ret != (int)ss.str().length() ) success  = false;
          if(l != ICameraTool::CAM_COMMAND)if (m_out.entries()>0)
            ret = m_camc->wr(s.str().c_str(),s.str().length());
          if(l != ICameraTool::CAM_COMMAND)if (ret != (int)s.str().length() ) success  = false;
        }
        else {
          if (numErrBZ < 5){
            warning() << "All threads of camserv are busy!  -> Aborting message '" << ss.str() << endmsg;
            numErrBZ++;
          }
          if (numErrBZ==5){
            
            warning() << "All threads of camserv are busy!  -> Aborting message '" << ss.str()<<endmsg;
            warning() << "Above message repeated "<<numErrBZ<<" times. Aborting further messaging of this type."<<endmsg;
            numErrBZ++;
            
          }
        }
	
        m_camc->new_sock();
        // if (success) break; // break from the for loop over possible servers.
        if (success) break;
      }
    }    // end the for loop
    
    if (!success) {
      if (numErrCN < 5){
        warning() << "Could not connect to any camera server! -> Aborting message '" << ss.str() << endmsg;
        numErrCN++;
      }
      else if (numErrCN==5) {
        warning() << "Could not connect to any camera server!  -> Aborting message '" << ss.str()<<endmsg;
        warning() << "Above message repeated 5 times. Aborting further messaging of this type."<<endmsg;
        numErrCN++;
      }
      // No else, just silently not print the message, after the error.
    }
  }// if(m_dosend&&m_camc)
  
 
  else {
    if (msgLevel(MSG::DEBUG))
      debug() << "Sending to CAMERA disabled. Message " << ss.str() << endmsg;
  } // else(m_dosend&&m_camc)
  
    m_out.reset();
    m_lastHistoNum = 0;
  
  return 1;
}


int CameraTool::SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what)
{ 
  m_out.add("TEXT","Time of report: ");
  time_t t = time(NULL);
  m_out.add("CTIME",ctime(&t));
  return  SendAndClear(l,who,what);
}


int CameraTool::Append(const char * T,const char * C)
{
  if (m_dosend)
    m_out.add(T,C);
  return 1;
}

int CameraTool::Append(const char * T,void * C, int sz){
  if (m_dosend)
    m_out.add(T,C,sz);
  return 1;
}

int CameraTool::Append(const char * C){
  if (m_dosend)
    m_out.add("TEXT",C);
  return 1;
}

int CameraTool::Append(const std::string &C){
  if (m_dosend)
    m_out.add("TEXT",C.c_str());
  return 1;
}

int CameraTool::Append(TH1D * H, const char * opts)
{
  if (m_dosend) {
    if ( !H ) { Warning("Null TH1D pointer"); return 0; }
    const std::string s = ( opts != NULL ? (std::string)"."+opts : "" );
    int nXBins = H->GetNbinsX();
    int size = (5+2*(nXBins+2));
    float * data = new float[size];
    data[0] = 1;
    data[1]=nXBins;
    data[2]=(H->GetBinLowEdge(1));
    data[3]=(H->GetBinLowEdge(nXBins+1));
    data[4]=H->GetEntries();
    for (int i=5; i<2*(nXBins+2); i++) data[i]=0.0;
    int  iData = 5;
    data[iData++]= H->GetBinContent(0);
    for (int ixBin=1; ixBin<=nXBins; ixBin++)
      data[iData++]=  H->GetBinContent(ixBin);
    data[iData++]= H->GetBinContent(nXBins+1);

    for (int ixBin=0; ixBin<=nXBins+1; ixBin++)
      data[iData++]= H->GetBinError(ixBin);

    m_out.add("DIM1D"+s,data,size*sizeof(float));

    delete data;

    // CRJ : Title hack
    // should go away when MonObjects are used
    const std::string& rootH = "H" + boost::lexical_cast<std::string>(m_lastHistoNum++);
    const std::string& title = rootH+"->SetTitle(\"" + std::string(H->GetTitle()) + "\");";
    this->Append( "EVAL", title.c_str() );
  }// if(m_dosend)
  return 1;
}

#include <iostream>
using namespace std;
int CameraTool::Append(TH2D * H, const char * opts)
{
  if (m_dosend) {
    if ( !H ) { Warning("Null TH2D pointer"); return 0; }
    const std::string s = ( opts != NULL ? (std::string)"."+opts : "" );
    int nXBins = H->GetNbinsX();
    int nYBins = H->GetNbinsY();
    int size = (8+2*(nXBins+2)*(nYBins+2));
    float * data = new float[size];

    data[0]=2.;
    data[1]=nXBins;
    data[2]=(H->GetBinLowEdge(1));
    data[3]=(H->GetBinLowEdge(nXBins+1));
    data[4]=nYBins;
    data[5]=(H->GetYaxis())->GetXmin();
    data[6]=H->GetYaxis()->GetXmax();
    data[7]=H->GetEntries();
    for (int i=8; i<2*(nXBins+2)*(nYBins+2); i++) data[i]=0.0;

    int iData = 8;


    data[iData++]= H->GetBinContent(0,0);

    for (int iyBin=1; iyBin<=nYBins; iyBin++){
      data[iData++]= H->GetBinContent(0,iyBin);
    }
    data[iData++]= H->GetBinContent(0, nYBins+1);

    for (int ixBin=1; ixBin<=nXBins; ixBin++){
      data[iData++]= H->GetBinContent(ixBin,0);
      for (int iyBin=1; iyBin<=nYBins; iyBin++){
        data[iData++]= H->GetBinContent(ixBin,iyBin);
      }
      data[iData++]= H->GetBinContent(ixBin,nYBins+1);
    }

    data[iData++]= H->GetBinContent(nXBins+1,0);
    for (int iyBin=1; iyBin<=nYBins; iyBin++){
      data[iData++]= H->GetBinContent(nXBins+1,iyBin);
    }
    data[iData++]= H->GetBinContent(nXBins+1,nYBins+1);


    //===== ERRORS======


    data[iData++]= H->GetBinError(0,0);

    for (int iyBin=1; iyBin<=nYBins; iyBin++){
      data[iData++]= H->GetBinError(0,iyBin);
    }
    data[iData++]= H->GetBinError(0, nYBins+1);

    for (int ixBin=1; ixBin<=nXBins; ixBin++){
      data[iData++]= H->GetBinError(ixBin,0);
      for (int iyBin=1; iyBin<=nYBins; iyBin++){
        data[iData++]= H->GetBinError(ixBin,iyBin);
      }
      data[iData++]= H->GetBinError(ixBin,nYBins+1);
    }
    
    data[iData++]= H->GetBinError(nXBins+1,0);
    for (int iyBin=1; iyBin<=nYBins; iyBin++){
      data[iData++]= H->GetBinError(nXBins+1,iyBin);
    }
    data[iData++]= H->GetBinError(nXBins+1,nYBins+1);
    
    m_out.add("DIM2D"+s,(void *)data,size*sizeof(float));
    
    delete data;
    
    // CRJ : Title hack 
    // should go away when MonObjects are used
    const std::string& rootH = "H" + boost::lexical_cast<std::string>(m_lastHistoNum++);
    const std::string& title = rootH+"->SetTitle(\"" + std::string(H->GetTitle()) + "\");";
    this->Append( "EVAL", title.c_str() );
  }// if(m_dosend)
  return 1;
}


std::ostream& operator<<(std::ostream &os, ICameraTool::MessageLevel l) {
  switch (l) {
  case ICameraTool::NONE:
    os << 0;
    break;
  case ICameraTool::INFO:
    os << 1;
    break;
  case ICameraTool::WARNING:
    os << 2;
    break;
  case ICameraTool::ERROR:
    os << 3;
    break;
  case ICameraTool::CAM_COMMAND:
    os << 9;
    break;
    // Use the None case for default.
  default:
    os << 0;
    break;
  }// switch(1)
  return os;
}//operator<<(std::ostream &, ICameraTool::MessageLevel)

