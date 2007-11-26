//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineRootHist.cpp,v 1.16 2007-11-26 17:36:44 ggiacomo Exp $
#include "OnlineHistDB/OnlineRootHist.h"
#include <TFile.h>
#include <TPaveStats.h>
#include <TCanvas.h>
#include <TStyle.h>

OnlineRootHist::OnlineRootHist(std::string Identifier,
			       OnlineHistDB *Session,
			       std::string Page,
			       int Instance) :
  m_identifier(Identifier) , m_session(Session), m_dbHist(NULL), m_rootHist(NULL),
  m_refOption("NOREF"), m_reference(NULL), m_startrun(1), m_DataType("default")
{
  if (Session) connectToDB(Session, Page, Instance);
}
	

OnlineRootHist::OnlineRootHist(OnlineHistogram*  oh) :
  m_identifier("") , m_session(NULL), m_dbHist(oh), m_rootHist(NULL),
  m_reference(NULL), m_startrun(1), m_DataType("default")
{
  if(m_dbHist) 
    m_identifier = m_dbHist->identifier();
}

OnlineRootHist::~OnlineRootHist() {
  if(m_reference) {
    //    m_reference->SetBit(kCanDelete);
    delete  m_reference;
  }
}

bool OnlineRootHist::setdbHist(OnlineHistogram*  oh) {
  bool out=false;
  bool isInit = (NULL == m_dbHist);
  if(oh) {
    if (oh->identifier() == m_identifier && false == oh->isAbort()) {
      m_dbHist = oh;
      if (m_rootHist && isInit) setTH1FromDB();
      out=true;
    }
    else {
      oh->warningMessage("provided OnlineHistogram object is not compatible");
    }
  }
  return out;
}


bool OnlineRootHist::setrootHist(TH1*  rh) {
  bool out=false;
  bool isInit = (NULL == m_rootHist);
  if(rh) {
    m_rootHist = rh;
    if (m_dbHist && isInit) setTH1FromDB();
    out=true;
  }
  return out;
}

bool OnlineRootHist::connectToDB(OnlineHistDB* Session,
		   std::string Page,
		   int Instance)
{
  bool out;
  if(Session) m_session = Session;
  if (m_session)
    out= setdbHist(m_session->getHistogram(m_identifier, Page, Instance));
  else
    out = false;
  return out;
}



void OnlineRootHist::setTH1FromDB() {
  if (m_dbHist && m_rootHist) {
    int iopt;
    float fopt;
    std::string sopt;
    if(m_dbHist->getDisplayOption("LABEL_X",   &sopt)) m_rootHist->SetXTitle (sopt.data());
    if(m_dbHist->getDisplayOption("LABEL_Y",   &sopt)) m_rootHist->SetYTitle (sopt.data());
    if(m_dbHist->getDisplayOption("LABEL_Z",   &sopt)) m_rootHist->SetZTitle (sopt.data());
    double bxmin=m_rootHist->GetXaxis()->GetXmin(),bxmax=m_rootHist->GetXaxis()->GetXmax();
    if(m_dbHist->getDisplayOption("XMIN",      &fopt)) bxmin=fopt;
    if(m_dbHist->getDisplayOption("XMAX",      &fopt)) bxmax=fopt;
    m_rootHist->GetXaxis()->SetRangeUser(bxmin,bxmax);
    if(m_dbHist->dimension() <2) { // 1d histograms
      if(m_dbHist->getDisplayOption("YMIN",      &fopt)) m_rootHist->SetMinimum(fopt);
      if(m_dbHist->getDisplayOption("YMAX",      &fopt)) m_rootHist->SetMaximum(fopt);
    }
    else {  // 2d histograms
      double bymin=m_rootHist->GetYaxis()->GetXmin(),bymax=m_rootHist->GetYaxis()->GetXmax();
      if(m_dbHist->getDisplayOption("YMIN",      &fopt)) bymin=fopt;
      if(m_dbHist->getDisplayOption("YMAX",      &fopt)) bymax=fopt;
      m_rootHist->GetYaxis()->SetRangeUser(bymin,bymax);
      if(m_dbHist->getDisplayOption("ZMIN",      &fopt)) m_rootHist->SetMinimum(fopt);
      if(m_dbHist->getDisplayOption("ZMAX",      &fopt)) m_rootHist->SetMaximum(fopt);
    }
    if(m_dbHist->getDisplayOption("STATS",     &iopt)) m_rootHist->SetStats(true);
    if(m_dbHist->getDisplayOption("FILLSTYLE", &iopt)) m_rootHist->SetFillStyle(iopt);
    if(m_dbHist->getDisplayOption("FILLCOLOR", &iopt)) m_rootHist->SetFillColor(iopt);
    if(m_dbHist->getDisplayOption("LINESTYLE", &iopt)) m_rootHist->SetLineStyle(iopt);
    if(m_dbHist->getDisplayOption("LINECOLOR", &iopt)) m_rootHist->SetLineColor(iopt);
    if(m_dbHist->getDisplayOption("LINEWIDTH", &iopt)) m_rootHist->SetLineWidth(iopt);
    if(m_dbHist->getDisplayOption("TIT_X_SIZE",&fopt)) m_rootHist->GetXaxis()->SetTitleSize(fopt);
    if(m_dbHist->getDisplayOption("TIT_X_OFFS",&fopt)) m_rootHist->GetXaxis()->SetTitleOffset(fopt);
    if(m_dbHist->getDisplayOption("LAB_X_SIZE",&fopt)) m_rootHist->GetXaxis()->SetLabelSize(fopt);
    if(m_dbHist->getDisplayOption("LAB_X_OFFS",&fopt)) m_rootHist->GetXaxis()->SetLabelOffset(fopt);
    if(m_dbHist->getDisplayOption("TIT_Y_SIZE",&fopt)) m_rootHist->GetYaxis()->SetTitleSize(fopt);
    if(m_dbHist->getDisplayOption("TIT_Y_OFFS",&fopt)) m_rootHist->GetYaxis()->SetTitleOffset(fopt);
    if(m_dbHist->getDisplayOption("LAB_Y_SIZE",&fopt)) m_rootHist->GetYaxis()->SetLabelSize(fopt);
    if(m_dbHist->getDisplayOption("LAB_Y_OFFS",&fopt)) m_rootHist->GetYaxis()->SetLabelOffset(fopt);    
    if(m_dbHist->getDisplayOption("TIT_Z_SIZE",&fopt)) m_rootHist->GetZaxis()->SetTitleSize(fopt);
    if(m_dbHist->getDisplayOption("TIT_Z_OFFS",&fopt)) m_rootHist->GetZaxis()->SetTitleOffset(fopt);
    if(m_dbHist->getDisplayOption("LAB_Z_SIZE",&fopt)) m_rootHist->GetZaxis()->SetLabelSize(fopt);
    if(m_dbHist->getDisplayOption("LAB_Z_OFFS",&fopt)) m_rootHist->GetZaxis()->SetLabelOffset(fopt);    
    if(m_dbHist->getDisplayOption("DRAWOPTS",  &sopt)) m_rootHist->SetOption(sopt.c_str());
    if(m_dbHist->getDisplayOption("REF",       &sopt)) m_refOption = sopt;
  }
}

void OnlineRootHist::setDrawOptionsFromDB(TPad* &Pad) {
  if(m_dbHist && m_rootHist) {
    int iopt=0;
    float fopt=0.;
    std::string sopt="";

    // TPaveStats is obtained after a pad->Draw(), but changing OptStat
    // doesn't resize the Pave.. using gStyle->SetOptStat in Draw() instead
    // if(m_dbHist->getDisplayOption("STATS", &iopt)) {
    //   TPaveStats* stats =  (TPaveStats*)m_rootHist->GetListOfFunctions()->FindObject("stats");
    //   if (stats) stats->SetOptStat(iopt);

    if(m_dbHist->getDisplayOption("LOGX", &iopt)) Pad->SetLogx(1);
    if(m_dbHist->getDisplayOption("LOGY", &iopt)) Pad->SetLogy(1);
    int gridx=gStyle->GetPadGridX(),gridy=gStyle->GetPadGridY();
    if(m_dbHist->getDisplayOption("GRIDX", &iopt)) gridx=iopt;
    if(m_dbHist->getDisplayOption("GRIDY", &iopt)) gridy=iopt;
    Pad->SetGrid(gridx,gridy);
    if (m_dbHist->dimension() > 1) {
      if(m_dbHist->getDisplayOption("LOGZ", &iopt)) Pad->SetLogz(1);
      if(m_dbHist->getDisplayOption("THETA", &fopt)) Pad->SetTheta(fopt);
      if(m_dbHist->getDisplayOption("PHI", &fopt)) Pad->SetPhi(fopt);
    }

  }
}



bool OnlineRootHist::updateDBOption(std::string opt, void *value, bool isdefault)
{ 
  // update option to current value only if is already set or if it's different from default
  bool update=false, out=true;
  if (m_dbHist && m_rootHist) {
    if (m_dbHist->isSetDisplayOption(opt) ) {
      update=true;
    }
    else {
      if (!isdefault)  update=true;
    }
    if(update)
      out = m_dbHist->setDisplayOption(opt, value);
  }
  return out;
}




bool OnlineRootHist::saveTH1ToDB(TPad* Pad) {
  bool out = false;
  if (m_dbHist && m_rootHist) {
    int iopt=0;
    float fopt=0.;
    std::string sopt="";
    out = true;
    sopt = m_rootHist->GetXaxis()->GetTitle();
    out &= updateDBOption ("LABEL_X", &sopt, 
			   sopt == "");
    sopt = m_rootHist->GetYaxis()->GetTitle();
    out &= updateDBOption ("LABEL_Y",     &sopt,
			   sopt == "");
    // note: axis mimina and maxima should not be set in this way, but through the web interface

    TPaveStats* stats =  (TPaveStats*)m_rootHist->GetListOfFunctions()->FindObject("stats");
    if(stats) { // if histogram has not been plotted (or has been plotted without stats), do nothing
      iopt = (int) stats->GetOptStat();
      out &= updateDBOption ("STATS", &iopt,
			     iopt == 1110 ); // seems to be hardcoded in root
    }
    iopt=(int) m_rootHist->GetFillStyle();
    out &= updateDBOption ("FILLSTYLE", &iopt,
			   iopt == (int) gStyle->GetHistFillStyle() );
    iopt=(int) m_rootHist->GetFillColor();
    out &= updateDBOption ("FILLCOLOR", &iopt,
			   iopt == (int) gStyle->GetHistFillColor());
    iopt=(int) m_rootHist->GetLineStyle();
    out &= updateDBOption ("LINESTYLE", &iopt,
			   iopt == (int) gStyle->GetHistLineStyle());
    iopt=(int) m_rootHist->GetLineColor();
    out &= updateDBOption ("LINECOLOR", &iopt,
			   iopt == (int) gStyle->GetHistLineColor());
    iopt=(int) m_rootHist->GetLineWidth();
    out &= updateDBOption ("LINEWIDTH", &iopt,
			   iopt == (int) gStyle->GetHistLineWidth());
    sopt = m_rootHist->GetOption();
    out &= updateDBOption ("DRAWOPTS",  &sopt,
			   sopt == "");
    fopt = m_rootHist->GetXaxis()->GetTitleSize();
    out &= updateDBOption ("TIT_X_SIZE",  &fopt,
			   fopt == (float) gStyle->GetTitleSize("X") );
    fopt = m_rootHist->GetXaxis()->GetTitleOffset();
    out &= updateDBOption ("TIT_X_OFFS",  &fopt,
			   fopt == (float) gStyle->GetTitleOffset("X") );
    fopt = m_rootHist->GetXaxis()->GetLabelSize();
    out &= updateDBOption ("LAB_X_SIZE",  &fopt,
			   fopt == (float) gStyle->GetLabelSize("X") );
    fopt = m_rootHist->GetXaxis()->GetLabelOffset();
    out &= updateDBOption ("LAB_X_OFFS",  &fopt,
			   fopt == (float) gStyle->GetLabelOffset("X") );
    fopt = m_rootHist->GetYaxis()->GetTitleSize();
    out &= updateDBOption ("TIT_Y_SIZE",  &fopt,
			   fopt == (float) gStyle->GetTitleSize("Y") );
    fopt = m_rootHist->GetYaxis()->GetTitleOffset();
    out &= updateDBOption ("TIT_Y_OFFS",  &fopt,
			   fopt == (float) gStyle->GetTitleOffset("Y") );
    fopt = m_rootHist->GetYaxis()->GetLabelSize();
    out &= updateDBOption ("LAB_Y_SIZE",  &fopt,
			   fopt == (float) gStyle->GetLabelSize("Y") );
    fopt = m_rootHist->GetYaxis()->GetLabelOffset();
    out &= updateDBOption ("LAB_Y_OFFS",  &fopt,
			   fopt == (float) gStyle->GetLabelOffset("Y") );    
    if (m_dbHist->dimension() > 1) {
      sopt = m_rootHist->GetZaxis()->GetTitle();
      out &= updateDBOption ("LABEL_Z",     &sopt,
			     sopt == "");
      fopt = m_rootHist->GetZaxis()->GetTitleSize();
      out &= updateDBOption ("TIT_Z_SIZE",  &fopt,
			     fopt == (float) gStyle->GetTitleSize("Z") );
      fopt = m_rootHist->GetZaxis()->GetTitleOffset();
      out &= updateDBOption ("TIT_Z_OFFS",  &fopt,
			     fopt == (float) gStyle->GetTitleOffset("Z") );
      fopt = m_rootHist->GetZaxis()->GetLabelSize();
      out &= updateDBOption ("LAB_Z_SIZE",  &fopt,
			     fopt == (float) gStyle->GetLabelSize("Z") );
      fopt = m_rootHist->GetZaxis()->GetLabelOffset();
      out &= updateDBOption ("LAB_Z_OFFS",  &fopt,
			     fopt == (float) gStyle->GetLabelOffset("Z") );
    }

    
    // now options from Pad ... be sure we are in the right Pad
   if(Pad) {
     iopt = Pad->GetLogx();
     out &= updateDBOption ("LOGX", &iopt,
			     iopt == gStyle->GetOptLogx() );
     iopt = Pad->GetLogy();
     out &= updateDBOption ("LOGY", &iopt,
			    iopt == gStyle->GetOptLogy() );
     iopt = Pad->GetGridx() ? 1 : 0;
     out &= updateDBOption ("GRIDX", &iopt,
			    (iopt>0) == gStyle->GetPadGridX() );
     iopt = Pad->GetGridy() ? 1 : 0;
     out &= updateDBOption ("GRIDY", &iopt,
			    (iopt>0) == gStyle->GetPadGridY() );
     if (m_dbHist->dimension() > 1) {
       iopt = Pad->GetLogz();
       out &= updateDBOption ("LOGZ", &iopt,
			      iopt == gStyle->GetOptLogz() );
       fopt = (float)Pad->GetTheta();
       out &= updateDBOption ("THETA", &fopt,
			      (int)fopt == 30 ); // seems to be hardcoded in root
       fopt = (float)Pad->GetPhi();
       out &= updateDBOption ("PHI", &fopt,
			      (int)fopt == 30 ); // seems to be hardcoded in root
     }
   }
   if(out)
     out=m_dbHist->saveDisplayOptions();
  }
  return out;
}



bool OnlineRootHist::setDisplayOption(std::string ParameterName, 
				      void* value) {
  bool out=false;
  if(m_dbHist) {
    out =m_dbHist->setDisplayOption(ParameterName,value);
    if(out && m_rootHist) setTH1FromDB();
  }
  return out;
}


void OnlineRootHist::Draw(TPad* &Pad) {
  if(m_rootHist) {
    int curStat=0,iopt;
    if (m_dbHist) {
      if(m_dbHist->getDisplayOption("STATS"  ,   &iopt)) {
	curStat = gStyle->GetOptStat();
	gStyle->SetOptStat(iopt);      
      }
      else {
	gStyle->SetOptStat(1110); // seems to be hardcoded in root
      }
    }
    m_rootHist->Draw(); 
    setDrawOptionsFromDB(Pad);

    if ("NOREF" != m_refOption) {
      if (getReference()) {
	drawReference();
      }
    }

  }
}




bool OnlineRootHist::setReference(TH1 *ref,
				  int startrun,
				  std::string DataType) {
  bool out=false;
  if(m_dbHist) {
    std::string refDir = m_dbHist->refRoot() +
      "/" + m_dbHist->task();
    std::string command = "mkdir -p " +refDir;
    system(command.c_str());
    std::stringstream refFile;
    refFile << refDir << "/" << DataType << std::string("_") << startrun;

    m_reference = (TH1*) ref->Clone(m_dbHist->hname().c_str());
    m_startrun = startrun;
    m_DataType = DataType;
    
    const char *file =  refFile.str().c_str();
    TFile* f = new TFile(file,"UPDATE");
    if(f) {
      if (false == f->IsZombie() ) {
	// keeps old versions.. use Write("",TObject::kOverwrite) to overwrite them
	m_reference->Write();
	f->Write();
	f->Close();
	out = true;
	m_dbHist->getTask()->setReference(refFile.str());
      }
      delete f;
    }
    m_reference->SetDirectory(0);

    if (!out)
      std::cout << "ERROR in OnlineRootHist::setReference : could not save reference into file " 
		<< refFile.str() << std::endl;
  }
  return out;
}


TH1* OnlineRootHist::getReference(int startrun,
				  std::string DataType) {
  TH1* out=0;
  if(m_dbHist) {
    if( NULL != m_reference && startrun == m_startrun
	&& DataType == m_DataType) {
      
      out = m_reference;
    }
    else {
      std::stringstream refFile;
      refFile << m_dbHist->refRoot() << "/"
	      << m_dbHist->task() << "/" << DataType << "_"
	      << startrun;
      TFile* f = new TFile(refFile.str().c_str(),"READ");
      if(f) {
	if (false == f->IsZombie() ) {
	  TH1* ref = (TH1*) f->Get(m_dbHist->hname().c_str());
	  if (ref) {
	    if(m_reference) delete m_reference;
	    ref->SetDirectory(0);
	    m_reference = out = ref;
	    m_startrun = startrun;
	    m_DataType = DataType;
	  }
	  f->Close();
	}
      }
      delete f;
    }
  }
  return out;
}


void OnlineRootHist::normalizeReference() {
  if( m_reference ) {
    if ("ENTR" == m_refOption) {
      m_reference->SetNormFactor(m_rootHist->GetSumOfWeights());
    }
    else if ("AREA" == m_refOption) {
      m_reference->SetNormFactor(m_rootHist->Integral());
    }
  }
}

void OnlineRootHist::drawReference() {
  if( m_reference ) {
    // standard plot style
    m_reference->SetLineStyle(2);
    m_reference->SetLineColor(16);
    // normalization
    normalizeReference();
    m_reference->Draw("SAME");
  }
}
