//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistPage.cpp,v 1.13 2007-10-16 12:16:10 ggiacomo Exp $

#include "OnlineHistDB/OnlineHistPage.h"

OnlineHistPage::OnlineHistPage(OnlineHistDBEnv& Env,
			       OnlineHistogramStorage* Storage,
			       std::string Name) :
  OnlineHistDBEnv(Env), m_Hstorage(Storage), m_name(Name), 
  m_folder(""), m_doc(""), m_syncWithDB(false)
{
  std::string outname;
  // check page name syntax
  outname = PagenameSyntax(m_name, m_folder);
  if (outname != m_name) {
    errorMessage("Page name was changed from "+m_name+" to "+outname);
    m_name = outname;
  }

  // check if page exists already in DB
  int out=0;
  m_h.clear();
  Statement *astmt=m_conn->createStatement 
    ("begin :out := OnlineHistDB.GetPage(thePage => :2, theFolder => :3, theDoc => :4); end;");
  try{
    astmt->registerOutParam(1, OCCIINT); 
    astmt->setString(2, m_name);
    astmt->registerOutParam(3,OCCISTRING,300);
    astmt->registerOutParam(4,OCCISTRING,100);
    astmt->execute();
    out=astmt->getInt(1);
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistPage::OnlineHistPage");
    }
  if (out>=0) { // page already exists
    m_folder = astmt->getString(3);
    m_doc = astmt->getString(4);
    m_conn->terminateStatement (astmt); 

    ResultSet *rset=0;
    astmt=m_conn->createStatement
      ("select VH.NAME,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y,INSTANCE FROM SHOWHISTO SH,VIEWHISTOGRAM VH WHERE PAGEFULLNAME(SH.PAGE,SH.PAGEFOLDER)=:1 and VH.HID= SH.HISTO ORDER BY SH.INSTANCE");
    try{
      astmt->setString(1, m_name);
      rset =  astmt->executeQuery ();
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistPage::OnlineHistPage");
      }
    while ( rset->next () ) {
      cout << "calling getHistogram "<<rset->getString(1)<< " in page "<<m_name<<
	" ("<<rset->getInt(6)<<")"<<endl;
      OnlineHistogram* newh= m_Hstorage->getHistogram(rset->getString(1),
						      m_name,
						      rset->getInt(6));
      cout<< " histogram is on page "<< newh->page() <<endl;
      newh->setDebug(debug());
      newh->setExcLevel(excLevel());
      m_h.push_back(newh);
      m_cx.push_back(rset->getFloat(2));
      m_cy.push_back(rset->getFloat(3));
      m_sx.push_back(rset->getFloat(4));
      m_sy.push_back(rset->getFloat(5));
    }
    astmt->closeResultSet (rset);
    m_syncWithDB = true;
  }
  m_conn->terminateStatement (astmt);
}




OnlineHistPage::~OnlineHistPage(){}


OnlineHistogram* OnlineHistPage::declareHistogram(OnlineHistogram* h,
						  float Cx,
						  float Cy,
						  float Sx,
						  float Sy,
						  unsigned int instance) {
  OnlineHistogram* outh=0;
  if (h->isAbort() == false) {
    bool knownHisto=false;
    int ih=findHistogram(h,instance,knownHisto);
    
    if (-1 == ih ) { // new histogram
      instance = newHistogramInstance(h);
      m_h.push_back(h);
      m_cx.push_back(Cx);
      m_cy.push_back(Cy);
      m_sx.push_back(Sx);
      m_sy.push_back(Sy);
      ih=m_h.size()-1;
    }
    else {
      m_cx[ih] = Cx;
      m_cy[ih] = Cy;
      m_sx[ih] = Sx;
      m_sy[ih] = Sy;
    }
    if(knownHisto) {
      // object has been already used in a previous instance: create new object 
      outh = m_Hstorage->getNewHistogram(h->identifier());
      m_h[ih] = outh;
    }
    else 
      outh=h;
    m_syncWithDB = false;
  }
  return outh;
}

OnlineHistogram* OnlineHistPage::addHistogram(OnlineHistogram* h,
					      float Cx,
					      float Cy,
					      float Sx,
					      float Sy) {
  OnlineHistogram* outh;
  outh=declareHistogram(h,Cx,Cy,Sx,Sy,999999);
  return outh;
}

bool OnlineHistPage::getHistLayout(OnlineHistogram* h,
				   float &Cx,
				   float &Cy,
				   float &Sx,
				   float &Sy,
				   unsigned int instance) const {
  bool found=false;
  bool known;
  int jh=findHistogram(h,instance,known);
  if (-1 != jh) {
    found = true;
    Cx = m_cx[jh];
    Cy = m_cy[jh];
    Sx = m_sx[jh];
    Sy = m_sy[jh];
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
  bool out=true;
  string command = "begin :out := OnlineHistDB.DeletePage(:1); end;";
  Statement *dst=m_conn->createStatement(command);
  try{
    dst->registerOutParam(1, OCCIINT);
    dst->setString(2,m_name);
    dst->execute();
    out = (dst->getInt(1) == 1);
    removeAllHistograms();
    m_syncWithDB = false;
  }catch(SQLException ex) {
    dumpError(ex,"OnlineHistPage::remove for Page "+m_name);
    out=false;
  }
  m_conn->terminateStatement (dst);
  
  return out;  
}

int OnlineHistPage::findHistogram(OnlineHistogram* h,
				  unsigned int instance,
				  bool &knownHisto) const {
  int ih=-1;
  unsigned int ii=0;
  knownHisto=false;
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

unsigned int OnlineHistPage::newHistogramInstance(OnlineHistogram* h) const {
  int ih=1;
  for (unsigned int jh=0; jh<m_h.size() ; jh++) {
    if ((m_h[jh])->hid() == h->hid()) 
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
  bool out=true;
  stringstream hlist,cx,cy,sx,sy;
  std::string outname;

  hlist << "OnlineHistDB.histotlist(";
  cx << "OnlineHistDB.floattlist(";
  cy << "OnlineHistDB.floattlist(";
  sx << "OnlineHistDB.floattlist(";
  sy << "OnlineHistDB.floattlist(";
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
  command << "begin :1 := OnlineHistDB.DeclarePage(theFullName => '"<< m_name<<
    "',theDoc => '"<< m_doc <<
    "',hlist => " << hlist.str() <<
    ",Cx => " << cx.str() <<
    ",Cy => " << cy.str() <<
    ",Sx => " << sx.str() <<
    ",Sy => " << sy.str() << 
    ",theName => :2 "<< 
    ",theFolder => :3"<< 
    "); end;";
  Statement *astmt=m_conn->createStatement(command.str()); 
  try{
    astmt->registerOutParam(1,OCCISTRING,350);
    astmt->registerOutParam(2,OCCISTRING,50);
    astmt->registerOutParam(3,OCCISTRING,300);
    astmt->execute();
    if (astmt->getString(1) != m_name)
      errorMessage("pagename "+m_name+" becomes "+astmt->getString(1)+
		   " in DeclarePage... something wrong?");
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistPage::save for page "+m_name);
      out=false;
    }
  m_conn->terminateStatement (astmt); 
  
  if (out) {
    m_syncWithDB = true;
    // attach histogram objects to this page
    for (unsigned int jh=0; jh<m_h.size() ; jh++) {
      out &= (m_h[jh])->setPage(m_name , HistogramInstance(jh));
    }
  }
  return out;
}

// std::string OnlineHistPage::fullName(std::string simplename, std::string folder) {
//   std::string outname=folder;
//   if (folder != "/") outname+= "/";
//   outname += simplename;
//   return outname;
// }




OnlinePageStorage::OnlinePageStorage(OnlineHistDBEnv* Env, OnlineHistogramStorage* Hstorage) :
  m_Pagenv(Env), m_Hstorage(Hstorage) {}

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
    page = new OnlineHistPage(*m_Pagenv, m_Hstorage, Name);
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
