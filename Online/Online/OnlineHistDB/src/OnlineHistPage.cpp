//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistPage.cpp,v 1.21 2008-04-30 13:29:16 ggiacomo Exp $

#include "OnlineHistDB/OnlineHistPage.h"
using namespace std;
using namespace OnlineHistDBEnv_constants;

OnlineHistPage::OnlineHistPage(OnlineHistDBEnv& Env,
			       std::string Name) :
  OnlineHistDBEnv(Env), m_name(Name), 
  m_folder(""), m_doc(""), m_syncWithDB(false)
{
  // check page name syntax
  std::string outname = PagenameSyntax(m_name, m_folder);
  if (outname != m_name) {
    warningMessage("Page name was changed from "+m_name+" to "+outname);
    m_name = outname;
  }
  load();
}

void OnlineHistPage::load() {
  // check if page exists already in DB
  const int Nfetch=100;
  int out=0;
  m_h.clear();
  text theFolder[VSIZE_FOLDER]="";
  sb2 folder_null;
  text theDoc[VSIZE_PAGEDOC]="";
  m_StmtMethod = "OnlineHistPage::load";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCITaggedStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.GETPAGE(thePage => :2, theFolder => :3, theDoc => :4); END;",
	"GETPAGE") ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":2"  , m_name);
    myOCIBindString(stmt,":3"  , theFolder, VSIZE_FOLDER, &folder_null);
    myOCIBindString(stmt,":4"  , theDoc, VSIZE_PAGEDOC, &m_doc_null);
    myOCIStmtExecute(stmt);
    
    releaseOCITaggedStatement(stmt, "GETPAGE");
  }
  if (out>=0) { // page already exists, load its layout
    m_folder = std::string((const char *) theFolder);
    m_doc = m_doc_null ? "" : std::string((const char *) theDoc);
    OCIStmt *lstmt=NULL;
    if ( OCI_SUCCESS == prepareOCITaggedStatement
	 (lstmt, "SELECT VH.NAME,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y,INSTANCE FROM SHOWHISTO SH,VIEWHISTOGRAM VH WHERE SH.PAGE=:1 AND VH.HID= SH.HISTO ORDER BY SH.INSTANCE",
	  "LOADPAGE") ) {
      myOCIBindString(lstmt,":1"  , m_name);
      if (OCI_SUCCESS == myOCISelectExecute(lstmt) ) {
	text NAME[Nfetch][VSIZE_NAME];
	for (int k=0;k<Nfetch;k++)
	  NAME[k][0]='\0';
	float CENTER_X[Nfetch];
	float CENTER_Y[Nfetch];
	float SIZE_X[Nfetch];
	float SIZE_Y[Nfetch];
	int INSTANCE[Nfetch];
	myOCIDefineString(lstmt, 1, NAME[0]  ,VSIZE_NAME);
	myOCIDefineFloat (lstmt, 2, CENTER_X[0]);
	myOCIDefineFloat (lstmt, 3, CENTER_Y[0]);
	myOCIDefineFloat (lstmt, 4, SIZE_X[0]);
	myOCIDefineFloat (lstmt, 5, SIZE_Y[0]);
	myOCIDefineInt   (lstmt, 6, INSTANCE[0]);

	int xf = Nfetch;
	while ( xf == Nfetch) {
	  xf = myOCIFetch(lstmt, Nfetch);
	  int newsize=m_h.size()+xf;
	  m_h.reserve(newsize);
	  m_cx.reserve(newsize);
	  m_cy.reserve(newsize);
	  m_sx.reserve(newsize);
	  m_sy.reserve(newsize);
	  for(int j=0; j<xf ; j++) {
	    OnlineHistogram* newh= m_HStorage->getHistogram(std::string((const char*) NAME[j]),
							    m_name,
							    INSTANCE[j]);
	    newh->setDebug(debug());
	    newh->setExcLevel(excLevel());
	    m_h.push_back(newh);
	    m_cx.push_back(CENTER_X[j]);
	    m_cy.push_back(CENTER_Y[j]);
	    m_sx.push_back(SIZE_X[j]);
	    m_sy.push_back(SIZE_Y[j]);
	  }      
	}
	myOCIFetch(lstmt, 0);
	m_syncWithDB = true;
      }
      releaseOCITaggedStatement(lstmt, "LOADPAGE");
    }
  }
}




OnlineHistPage::~OnlineHistPage(){}




OnlineHistogram* OnlineHistPage::declareHistogram(OnlineHistogram* h,
						  double Cx,
						  double Cy,
						  double Sx,
						  double Sy,
						  unsigned int instance) {
  OnlineHistogram* outh=0;
  if (h->isAbort() == false) {
    bool knownHisto=false;
    int ih=findHistogram(h,instance,knownHisto);
    
    if (-1 == ih ) { // new histogram
      instance = newHistogramInstance(h);
      m_h.push_back(h);
      m_cx.push_back((float) Cx);
      m_cy.push_back((float) Cy);
      m_sx.push_back((float) Sx);
      m_sy.push_back((float) Sy);
      ih=m_h.size()-1;
    }
    else {
      m_cx[ih] = (float) Cx;
      m_cy[ih] = (float) Cy;
      m_sx[ih] = (float) Sx;
      m_sy[ih] = (float) Sy;
    }
    if(knownHisto) {
      // object has been already used in a previous instance: create new object 
      outh = m_HStorage->getNewHistogram(h->identifier());
      m_h[ih] = outh;
    }
    else 
      outh=h;
    m_syncWithDB = false;
  }
  return outh;
}

OnlineHistogram* OnlineHistPage::addHistogram(OnlineHistogram* h,
					      double Cx,
					      double Cy,
					      double Sx,
					      double Sy) {
  OnlineHistogram* outh;
  outh=declareHistogram(h,Cx,Cy,Sx,Sy,999999);
  return outh;
}

bool OnlineHistPage::getHistLayout(OnlineHistogram* h,
				   double &Cx,
				   double &Cy,
				   double &Sx,
				   double &Sy,
				   unsigned int instance) const {
  bool found=false;
  bool known;
  int jh=findHistogram(h,instance,known);
  if (-1 != jh) {
    found = true;
    Cx = (double) m_cx[jh];
    Cy = (double) m_cy[jh];
    Sx = (double) m_sx[jh];
    Sy = (double) m_sy[jh];
  }
  return found;
}

bool OnlineHistPage::removeHistogram(OnlineHistogram* h,
				     unsigned int instance) {
  bool out=true;
  bool known;
  int jh=findHistogram(h,instance,known);
  if (-1 != jh) {
    m_h.erase(m_h.begin()+jh);
    m_cx.erase(m_cx.begin()+jh);
    m_cy.erase(m_cy.begin()+jh);
    m_sx.erase(m_sx.begin()+jh);
    m_sy.erase(m_sy.begin()+jh);
    m_syncWithDB = false;
  }
  return out;
}

bool OnlineHistPage::removeAllHistograms() {
  m_h.clear();
  m_cx.clear();
  m_cy.clear();
  m_sx.clear();
  m_sy.clear();
  m_syncWithDB = false;
  bool out = true;
  return out;
}

bool OnlineHistPage::remove() {
  int out=0;
  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistPage::remove";
  
  if ( OCI_SUCCESS == prepareOCIStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.DELETEPAGE(:1); END;") ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":1"  , m_name);
    myOCIStmtExecute(stmt);
    releaseOCIStatement(stmt);
  }
  return (out > 0);  
}

void OnlineHistPage::rename(std::string NewName) {
  OCIStmt *stmt=NULL;
  if(m_syncWithDB) {
    m_StmtMethod = "OnlineHistPage::Rename";
    if ( OCI_SUCCESS == prepareOCIStatement(stmt,"BEGIN :out := ONLINEHISTDB.RENAMEPAGE(:old,:new,:fold); END;" ) ) {
      text Name[VSIZE_PAGENAME];
      text Fold[VSIZE_FOLDER];
      myOCIBindString(stmt,":old", m_name);
      myOCIBindString(stmt,":new", NewName);
      myOCIBindString(stmt,":out", Name, VSIZE_PAGENAME);    
      myOCIBindString(stmt,":fold", Fold, VSIZE_FOLDER);    
      if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
	m_name = std::string((const char *) Name);
	m_folder = std::string((const char *) Fold);
      }
      releaseOCIStatement(stmt);
    }
  }
  else {
    m_name =PagenameSyntax(NewName, m_folder);
  }
}


int OnlineHistPage::findHistogram(OnlineHistogram* h,
				  unsigned int instance,
				  bool &knownHisto) const {
  int ih=-1;
  unsigned int ii=0;
  knownHisto=false;
   std::vector<OnlineHistogram*>::iterator ix;
  for (unsigned int jh=0; jh<m_h.size() ; jh++) {
    if ((m_h[jh])->hid() == h->hid()) { // this histogram is already on page
      if (m_h[jh] == h) knownHisto = true; // the same object is already on page
      if (++ii == instance) {
	ih=jh;
	break;
      }
    }
  }
  return ih;
}

unsigned int OnlineHistPage::newHistogramInstance(OnlineHistogram* h) {
  unsigned int ih=1;
  std::vector<OnlineHistogram*>::iterator jh;
  for (jh = m_h.begin(); jh != m_h.end(); ++jh) {
    if (h->hid() == (*jh)->hid()) 
      ih++;
  }
  return ih;
}

unsigned int OnlineHistPage:: HistogramInstance(unsigned int xh) const {
  int ih=1;
  if (xh>= m_h.size())
    ih=0;
  else {
    for (unsigned int jh=0; jh<xh ; jh++) {
      if ((m_h[jh])->hid() == (m_h[xh])->hid()) 
	ih++;
    }
  }
  return ih;
}

bool OnlineHistPage::save() {
  bool out=false;
  stringstream hlist,cx,cy,sx,sy;
  std::string outname;

  hlist << "ONLINEHISTDB.HISTOTLIST(";
  cx << "ONLINEHISTDB.FLOATTLIST(";
  cy << "ONLINEHISTDB.FLOATTLIST(";
  sx << "ONLINEHISTDB.FLOATTLIST(";
  sy << "ONLINEHISTDB.FLOATTLIST(";
  for (unsigned int jh=0; jh<m_h.size() ; jh++) {
    hlist << "'" << m_h[jh]->hid() << "'"; 
    cx << m_cx[jh] ;
    cy << m_cy[jh] ;
    sx << m_sx[jh] ;
    sy << m_sy[jh] ;
    if (jh < m_h.size()-1) {
      hlist << ",";
      cx <<  ",";
      cy <<  ",";
      sx <<  ",";
      sy <<  ",";
    }
  }
  hlist << ")";
  cx <<  ")";
  cy <<  ")";
  sx <<  ")";
  sy <<  ")";
  stringstream command;
  command << "BEGIN :1 := ONLINEHISTDB.DECLAREPAGE(theFullName => '"<< m_name<<
    "',theDoc => '"<< m_doc <<
    "',hlist => " << hlist.str() <<
    ",Cx => " << cx.str() <<
    ",Cy => " << cy.str() <<
    ",Sx => " << sx.str() <<
    ",Sy => " << sy.str() << 
    ",theName => :2 "<< 
    ",theFolder => :3"<< 
    "); END;";

  OCIStmt *stmt=NULL;
  m_StmtMethod = "OnlineHistPage::save";
  if ( OCI_SUCCESS == prepareOCIStatement(stmt, command.str().c_str()) ) {
    text fullName[VSIZE_PAGENAME]="";
    text theName[VSIZE_PAGENAME]="";
    text theFolder[VSIZE_FOLDER]="";
    myOCIBindString(stmt,":1", fullName, VSIZE_PAGENAME);  
    myOCIBindString(stmt,":2", theName, VSIZE_PAGENAME);
    myOCIBindString(stmt,":3", theFolder, VSIZE_FOLDER);
    if (OCI_SUCCESS == myOCIStmtExecute(stmt)) {
      out = true;
      std::string xfn((const char *) fullName);
      if (xfn != m_name)
	warningMessage("pagename "+m_name+" becomes "+xfn+
		     " in DeclarePage... something wrong?");
      m_shortname = std::string((const char *) theName);
    }
    releaseOCIStatement(stmt);
  }
  
  if (out) {
    m_syncWithDB = true;
    // attach histogram objects to this page
    for (unsigned int jh=0; jh<m_h.size() ; jh++) {
       out &= (m_h[jh])->setPage(m_name , HistogramInstance(jh));
    }
  }
  return out;
}


void OnlineHistPage::dump() {
  cout << "-------------------------------------------------" << endl <<
    "Page "<< m_name << " in folder "<<m_folder<<" has "<< m_h.size() << " histograms:"<<endl;
  std::vector<OnlineHistogram*>::iterator ih;
  double Cx,Cy,Sx,Sy;
  if(!m_h.empty() ) cout << "Histogram Identifier                              Cx    Cy    Sx    Sy"<<endl; 
  for (ih = m_h.begin();ih != m_h.end(); ++ih) {
    if ( getHistLayout(*ih, Cx, Cy, Sx, Sy, (*ih)->instance() )) {
      cout << (*ih)->identifier() << " (Instance " << (*ih)->instance() 
	   << ")  " << Cx << " " << Cy
	   << " " << Sx << " " << Sy <<endl; 
    }
  }
  cout << " --- page is ";
  if (!m_syncWithDB) cout << "NOT";
  cout << " in sync with DB --" <<endl;
  cout << "-------------------------------------------------" << endl;
}


OnlinePageStorage::~OnlinePageStorage() 
{
  if (m_Pagenv->debug() > 2) cout << "Deleting "<<
    m_myPage.size() << " OnlineHistPage objects"<<endl;
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) 
    delete *ip; 
}

OnlineHistPage* OnlinePageStorage::getPage(std::string Name) {
  std::string folder="_NONE_";
  OnlineHistPage* page = 0;
  // see if page object exists already
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) {
    if (folder=="_NONE_") Name = (*ip)->PagenameSyntax(Name, folder);
    if ((*ip)->name() == Name) {
      page = *ip;
      break;
    }
  }
  // otherwise create new one
  if ( 0 == page) {
    page = new OnlineHistPage(*m_Pagenv, Name);
    m_myPage.push_back(page);
  }
  return page;
}

bool OnlinePageStorage::removePage(OnlineHistPage* Page) {
  bool out=Page->remove();
  if (out) {
    std::vector<OnlineHistPage*>::iterator ip = m_myPage.begin();
    while (ip != m_myPage.end()) {
      if( (*ip)->name() == Page->name() ) {
	if (Page != (*ip)) delete *ip;
	ip=m_myPage.erase(ip);
      }
      else {
	ip++;
      }
    }
    delete Page;
  }
  return out;  
}

void OnlinePageStorage::reloadPages() {
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) {
     (*ip)->load();
  }
}
