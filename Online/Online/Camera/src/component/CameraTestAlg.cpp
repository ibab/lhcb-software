// $Id: CameraTestAlg.cpp,v 1.3 2010-10-01 13:31:34 frankb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from ROOT
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>

// Camera
#include "Camera/CameraTestAlg.h"
#include "Camera/ICameraTool.h"

// std
#include <sstream>
#include <math.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Implementation file for class : CameraTestAlg
//
// 2008-02-22 : Claus Buszello
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CameraTestAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CameraTestAlg::CameraTestAlg( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{  
}
//=============================================================================
// Destructor
//=============================================================================
CameraTestAlg::~CameraTestAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CameraTestAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  cam = tool<ICameraTool>("CameraTool");
  cam->SendAndClear(ICameraTool::INFO,name().c_str(),"Initialized");

  m_evt   = 0;
  Hflat   = new TH1D ("H","H",10,-1,1);
  Hangled = new TH1D ("H1","H1",10,-1,1);
  
  fflat = new TF1 ("F","[0]*x+[1]");
  H2d   = new TH2D("h2","h2",10,-3,3,10,-3,3);
  H2d2  = new TH2D("h2","h2",10,-1.5,1.5,10,-1.5,1.5);
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CameraTestAlg::execute() {
  
  m_evt++;
  info()<<"Event number "<<m_evt<<endreq;
  
  
  float r = (float)rand()/((float)RAND_MAX) *2.f -1.f;
  float a = (float)rand()/((float)RAND_MAX) *2.f *3.141f;

  Hflat->Fill(r);
  
  
  Hangled->Fill(acos(r)/3.141f - .5f);
  

  H2d->Fill(2.f*fabs(r)*sin(a),2.f*fabs(r)*cos(a));

  H2d2->Fill(sin(a),cos(a));
  


  if (m_evt%50==0){
    
    Hflat->Fit("F","LEM");

    std::stringstream ss;    
    ss<<fflat->GetParameter(0)<<" * x + "<<fflat->GetParameter(1);

    std::stringstream ss2;    
    ss2<<"slope: "<<fflat->GetParameter(0)<< "+-"<<fflat->GetParError(0);
    
    

    cam->Append("Summary plots after 50 Events.");
    cam->Append("Fit Result:");
    cam->Append(ss2.str().c_str());

    //Setting the titles before they are drawn ensures, that
    //they are drawn properly with the histos
    //Histos H0,H1,H2, ... already exist when this is called.
    cam->Append("EVAL","H0.SetTitle(\"Two histograms in one frame\")");   
    cam->Append("EVAL","H0.GetXaxis()->SetTitle(\"X-axis\")");
    cam->Append("EVAL","H2.SetTitle(\"A 2D Histo described by a circle\")");
    cam->Append("EVAL","H2.GetXaxis()->SetTitle(\"Columns\")");
    cam->Append("EVAL","H2.GetYaxis()->SetTitle(\"Rows\")");
    cam->Append("EVAL","H3.SetTitle(\"A copy of the same as LEGO\")");
    cam->Append("EVAL","H4.SetTitle(\"Another 2D Histo\")");

    cam->Append("EVAL","F0.SetRange(-1,1)");
    cam->Append("EVAL","F0.SetLineColor(kRed)");
    
    cam->Append(Hangled);
    cam->Append(Hflat,"SAME");
    cam->Append("TF1.SAME",ss.str().c_str());

    cam->Append(H2d,"COLZ");
    cam->Append("TTELLIPSE","0 0 2 2");

    cam->Append(H2d,"LEGO");

    cam->Append(H2d2,"SURF");

    cam->SendAndClearTS(ICameraTool::INFO,"CameraTestAlg","Updated Histograms sent");
    

  
  }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CameraTestAlg::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
