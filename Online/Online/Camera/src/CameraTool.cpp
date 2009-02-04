// $Id: CameraTool.cpp,v 1.2 2009-02-04 13:00:01 rogers Exp $
// Include files

// local
#include "CameraTool.h"
#include "client.h"

// ROOT
#include "TH1.h"
#include "TH2.h"

// boost
#include "boost/lexical_cast.hpp"

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
  declareProperty("ServerPort",m_servport=12345);
  declareProperty("Enabled",m_dosend=true);
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
    m_camc = new client(m_servername.c_str(),m_servport);
    if (m_camc->m_healthy==0) 
    {
      Warning( "Failed to setup CAMERA" ).ignore();
      delete m_camc;
      m_camc = NULL;
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

int CameraTool::SendAndClear(int l,const std::string& who,const std::string& what)
{

  if ( ( m_dosend == true) && m_camc ) {

    std::stringstream ss;
    std::stringstream s;

    ss << who<<"/"<<l<<"/"<<what<<"\n";

    if (m_out.entries()>0) m_out.tostream(s);

    if (m_camc->Connect()>0) {
      char buf[3];
      while (  m_camc->rd(buf,2)==-2){};
      buf[2] = 0;

      if (strncmp(buf,"GO",2) == 0 )
      {
        // if (m_out.entries()<1) m_camc->nowait();
        m_camc->wr(ss.str().c_str(),ss.str().length());

        if (m_out.entries()>0)
          m_camc->wr(s.str().c_str(),s.str().length());
      }
      else 
      {
        error() << "DANGER! All threads of camserv are busy!" << endreq;
        error() << " -> Aborting message '" << ss.str() << "'" << endreq;
      }

      m_camc->new_sock();

    }

    m_out.reset();

    m_lastHistoNum = 0;
  }

  return 1;
}


int CameraTool::SendAndClearTS(int l,const std::string& who,const std::string& what)
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
