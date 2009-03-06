//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistPage.cpp,v 1.25 2009-03-06 09:26:54 ggiacomo Exp $

#include "OnlineHistDB/OnlineHistPage.h"
using namespace std;
using namespace OnlineHistDBEnv_constants;

OnlineHistPage::OnlineHistPage(OnlineHistDBEnv& Env,
			       std::string Name) :
  OnlineHistDBEnv(Env), m_name(Name), 
  m_folder(""), m_doc(""), m_pattern(""), m_syncWithDB(false)
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
  removeAllHistograms();
  text theFolder[VSIZE_FOLDER]="";
  sb2 folder_null;
  text theDoc[VSIZE_PAGEDOC]="";
  text thePattern[VSIZE_PGPATTERN]="";
  m_StmtMethod = "OnlineHistPage::load";
  OCIStmt *stmt=NULL;
  if ( OCI_SUCCESS == prepareOCITaggedStatement
       (stmt, "BEGIN :out := ONLINEHISTDB.GETPAGE(thePage => :2, theFolder => :3, theDoc => :4, thePattern => :5); END;",
        "GETPAGE") ) {
    myOCIBindInt   (stmt,":out", out);  
    myOCIBindString(stmt,":2"  , m_name);
    myOCIBindString(stmt,":3"  , theFolder, VSIZE_FOLDER, &folder_null);
    myOCIBindString(stmt,":4"  , theDoc, VSIZE_PAGEDOC, &m_doc_null);
    myOCIBindString(stmt,":5"  , thePattern, VSIZE_PGPATTERN, &m_pattern_null);
    myOCIStmtExecute(stmt);
    
    releaseOCITaggedStatement(stmt, "GETPAGE");
  }
  if (out>=0) { // page already exists, load its layout
    m_folder = std::string((const char *) theFolder);
    m_doc = m_doc_null ? "" : std::string((const char *) theDoc);
    m_pattern = m_pattern_null ? "" : std::string((const char *) thePattern);
    OCIStmt *lstmt=NULL;
    if ( OCI_SUCCESS == prepareOCITaggedStatement
         (lstmt, "SELECT VH.NAME,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y,INSTANCE,SHID,MOTHERH,IOVERLAP FROM SHOWHISTO SH,VIEWHISTOGRAM VH WHERE SH.PAGE=:1 AND VH.HID= SH.HISTO ORDER BY SH.MOTHERH DESC,SH.IOVERLAP,SH.INSTANCE",
          "LOADPAGE") ) {
      myOCIBindString(lstmt,":1"  , m_name);
      if (OCI_SUCCESS == myOCISelectExecute(lstmt) ) {
        text NAME[Nfetch][VSIZE_NAME];
        for (int k=0;k<Nfetch;k++)
          NAME[k][0]='\0';
        float PADMIN_X[Nfetch];
        float PADMIN_Y[Nfetch];
        float PADMAX_X[Nfetch];
        float PADMAX_Y[Nfetch];
        int INSTANCE[Nfetch];
        int SHID[Nfetch];
        int MOTHERH[Nfetch];
        int IOVERLAP[Nfetch];
        myOCIDefineString(lstmt, 1, NAME[0]  ,VSIZE_NAME);
        myOCIDefineFloat (lstmt, 2, PADMIN_X[0]);
        myOCIDefineFloat (lstmt, 3, PADMIN_Y[0]);
        myOCIDefineFloat (lstmt, 4, PADMAX_X[0]);
        myOCIDefineFloat (lstmt, 5, PADMAX_Y[0]);
        myOCIDefineInt   (lstmt, 6, INSTANCE[0]);
        myOCIDefineInt   (lstmt, 7, SHID[0]);
        myOCIDefineInt   (lstmt, 8, MOTHERH[0]);
        myOCIDefineInt   (lstmt, 9, IOVERLAP[0]);
        int xf = Nfetch;
        while ( xf == Nfetch) {
          xf = myOCIFetch(lstmt, Nfetch);
          m_h.reserve(xf);
          for(int j=0; j<xf ; j++) {
            OnlineHistogram* histo = m_HStorage->getHistogram(std::string((const char*) NAME[j]),
                                                              m_name,
                                                              INSTANCE[j]);
            histo->setDebug(debug());
            histo->setExcLevel(excLevel());

            OnlineHistoOnPage* newh = new OnlineHistoOnPage(histo);
            m_h.push_back( newh );
            newh->setPadSize(PADMIN_X[j], PADMIN_Y[j], PADMAX_X[j], PADMAX_Y[j]);

            if (MOTHERH[j]>0) {
              for (int k=0; k<j;k++) {
                if (SHID[k] == MOTHERH[j]) {
                  m_h[j]->setOverlap(m_h[k], IOVERLAP[j]);
                  break;
                }
              }
            }
          }      
        }
        myOCIFetch(lstmt, 0);
        m_syncWithDB = true;
      }
      releaseOCITaggedStatement(lstmt, "LOADPAGE");
    }
  }
}




OnlineHistPage::~OnlineHistPage()
{
  removeAllHistograms();
}




OnlineHistogram* OnlineHistPage::mydeclareHistogram(OnlineHistogram* h,
						  double Xmin,
						  double Ymin,
						  double Xmax,
						  double Ymax,
						  unsigned int instance,
              OnlineHistogram* onTopOfHist,
              int onTopOfInst,       
              int OverlapIndex) {
  OnlineHistogram* outh=NULL;
  if (h->isAbort() == false) {
    bool knownHisto=false;
    OnlineHistoOnPage* ph=findHistogram(h,instance,knownHisto);
    
    if (!ph) { // new histogram
      unsigned int newinst = newHistogramInstance(h);
      if (newinst != instance) {
        if( instance<10 ) {
          std::stringstream wm;
          wm << "Instance of new histogram changed from" << instance <<
            " to "<< newinst;
          warningMessage(wm.str());
        }
        instance = newinst;
      }
      ph = new OnlineHistoOnPage(h);
      m_h.push_back( ph );
      ph->instance = instance;

    }
    ph->setPadSize((float)Xmin, (float)Ymin, (float)Xmax, (float)Ymax);
    if(knownHisto) {
      // object has been already used in a previous instance: create new object 
      outh = m_HStorage->getNewHistogram(h->identifier());
      ph->histo = outh;
    }
    else 
      outh=h;

    if(onTopOfHist) {
      OnlineHistoOnPage* pm=findHistogram(onTopOfHist,onTopOfInst,knownHisto);
      if (pm) {
        if (pm->isOverlap() == false) {
          ph->setOverlap(pm, OverlapIndex);
        }
        else {
          warningMessage("you cannot overplot on histogram "+onTopOfHist->identifier()+
                         " that is an overlap too");
        }
      }
      else {
        warningMessage("you cannot overplot on histogram "+onTopOfHist->identifier()+
                       " that is not on Page");
        ph->unsetOverlap();
      }
    }

    m_syncWithDB = false;
  }
  return outh;
}


OnlineHistogram* OnlineHistPage::declareHistogram(OnlineHistogram* h,
                                                  double Xmin,
                                                  double Ymin,
                                                  double Xmax,
                                                  double Ymax,
                                                  unsigned int instance) {
  m_StmtMethod = "OnlineHistPage::declareHistogram";
  return mydeclareHistogram(h,Xmin,Ymin,Xmax,Ymax,instance);
}

OnlineHistogram* OnlineHistPage::declareOverlapHistogram(OnlineHistogram* h,
                                                         OnlineHistogram* onTopOf,
                                                         unsigned int instance,
                                                         int onTopOfInst,
                                                         int OverlapIndex) {
  m_StmtMethod = "OnlineHistPage::declareOverlapHistogram";
  return mydeclareHistogram(h,0.,0.,0.1,0.1,instance,onTopOf,onTopOfInst,OverlapIndex);
}

OnlineHistogram* OnlineHistPage::addHistogram(OnlineHistogram* h,
                                              double Xmin,
                                              double Ymin,
                                              double Xmax,
                                              double Ymax) {
  m_StmtMethod = "OnlineHistPage::addHistogram";
  return declareHistogram(h,Xmin,Ymin,Xmax,Ymax,999999);
}

OnlineHistogram* OnlineHistPage::addOverlapHistogram(OnlineHistogram* h,
                                                     OnlineHistogram* onTopOf,
                                                     int onTopOfInst,
                                                     int OverlapIndex) {
  m_StmtMethod = "OnlineHistPage::addOverlapHistogram";
  return mydeclareHistogram(h,0.,0.,0.1,0.1,999999,onTopOf,onTopOfInst,OverlapIndex);
}


bool OnlineHistPage::removeHistogram(OnlineHistogram* h,
				     unsigned int instance) {
  for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) {
    if ((*m_ih)->histo->hid() == h->hid() &&
        (*m_ih)->instance == instance) {
      OnlineHistoOnPage* tbd=(*m_ih);
      m_h.erase(m_ih);
      delete tbd;
      m_syncWithDB = false;
      return true;
    }
  }
  return false;
}

bool OnlineHistPage::removeAllHistograms() {
  for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) 
    delete (*m_ih);
  m_h.clear();

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
  if (out>0)
    removeAllHistograms();
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


OnlineHistoOnPage* OnlineHistPage::findHistogram(OnlineHistogram* h,
                                  unsigned int instance,
                                  bool &knownHisto) {
  OnlineHistoOnPage* out=NULL;
  knownHisto=false;
  for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) {
    if((*m_ih)->histo->hid() == h->hid()) { // this histogram is already on page
      if((*m_ih)->histo == h) knownHisto = true; // the same object is already on page
      if((*m_ih)->instance== instance)  out = (*m_ih);
    }
  }
  return out;
}



unsigned int OnlineHistPage::newHistogramInstance(OnlineHistogram* h) {
  unsigned int ih=1;
  for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) {
    if (h->hid() == (*m_ih)->histo->hid()) 
      ih++;
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
  m_ih=m_h.begin();
  while(m_ih != m_h.end()) {
    hlist << "'" << (*m_ih)->histo->hid() << "'"; 
    cx << (*m_ih)->xmin;
    cy << (*m_ih)->ymin;
    sx << (*m_ih)->xmax;
    sy << (*m_ih)->ymax;
    if (++ m_ih != m_h.end()) {
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
  command << "BEGIN :1 := ONLINEHISTDB.DECLAREPAGE(theFullName => '"<< m_name<<"'";
  if (!m_doc.empty())
    command << ",theDoc => '"<< m_doc <<"'";
  if (!m_pattern.empty())
    command << ",thePattern => '"<< m_pattern <<"'";
  command << ",hlist => " << hlist.str() <<
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
    stmt=NULL;
  }
  

  if (out) {
    // attach histogram objects to this page
    for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++)
      (*m_ih)->save(m_name);

    // declare overlaps
    int iok;
    for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) {
      if( (*m_ih)->isOverlap() ) {
        int ovIx;
        OnlineHistoOnPage* moth=(*m_ih)->getOverlap(&ovIx);

        if ( OCI_SUCCESS == prepareOCIStatement(stmt,
                           "BEGIN :1 := ONLINEHISTDB.DECLAREHOVERLAP(:pg,:hov,:iov,:hmo,:imo,:oix);END;") ) {
          myOCIBindInt(stmt,":1",iok);
          myOCIBindString(stmt,":pg",m_name);
          myOCIBindString(stmt,":hov",(*m_ih)->histo->hid() );
          int iov=(int) (*m_ih)->instance;
          myOCIBindInt(stmt,":iov",iov);
          myOCIBindString(stmt,":hmo",moth->histo->hid());
          int imo=(int) moth->instance;
          myOCIBindInt(stmt,":imo",imo);
          myOCIBindInt(stmt,":oix",ovIx);
          sword outcode=myOCIStmtExecute(stmt);
          if (OCI_SUCCESS != outcode || 0 == iok) {
            out=false;
            warningMessage("failed to define overlap for histogram "+(*m_ih)->histo->identifier());
          }
          releaseOCIStatement(stmt);
          stmt=NULL;
        }
      }
    }
  }
  if (out) m_syncWithDB = true;
  return out;
}


void OnlineHistPage::dump() {
  std::cout << "-------------------------------------------------" << std::endl <<
    "Page "<< m_name << " in folder "<<m_folder<<" has "<< m_h.size() << " histograms:"<<std::endl;
  if(! m_h.empty() ) 
    std::cout << "Histogram Identifier                             Xmin   Ymin   Xmax   Ymax  Ovlap"<<std::endl; 
  for (m_ih=m_h.begin() ; m_ih != m_h.end() ; m_ih++) {
    std::cout << (*m_ih)->histo->identifier() << " (Instance " << (*m_ih)->instance
              << ")  " << (*m_ih)->xmin << " " << (*m_ih)->ymin
              << " " << (*m_ih)->xmax << " " << (*m_ih)->ymax;
    if ((*m_ih)->isOverlap()) std::cout << "  YES";
    std::cout << std::endl;
  }
  std::cout << " --- page is ";
  if (!m_syncWithDB) cout << "NOT";
  std::cout << " in sync with DB --" <<std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
}


OnlinePageStorage::~OnlinePageStorage() 
{
  if (m_Pagenv->debug() > 2) std::cout << "Deleting "<<
    m_myPage.size() << " OnlineHistPage objects"<<std::endl;
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
