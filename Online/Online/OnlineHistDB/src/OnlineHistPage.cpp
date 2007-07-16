//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistPage.cpp,v 1.10 2007-07-16 12:47:32 ggiacomo Exp $

#include "OnlineHistDB/OnlineHistPage.h"

OnlineHistPage::OnlineHistPage(OnlineHistDBEnv& Env,
			       OnlineHistogramStorage* Storage,
			       std::string Name, 
			       std::string Folder) :
  OnlineHistDBEnv(Env), m_Hstorage(Storage), m_name(Name), m_folder(Folder)
{
  // check if page exists already in DB
  int out=0;
  Statement *astmt=m_conn->createStatement 
    ("begin :out := OnlineHistDB.GetPage(theName => :2, theFolder => :3, theDoc => :4); end;");
  try{
    astmt->registerOutParam(1, OCCIINT); 
    astmt->setString(2, Name);
    astmt->registerOutParam(3,OCCISTRING,30);
    astmt->registerOutParam(4,OCCISTRING,100);
    astmt->execute();
    out=astmt->getInt(1);
  }catch(SQLException ex)
    {
      dumpError(ex,"OnlineHistPage::OnlineHistPage");
    }
  if (out>=0) { // page already exists
    m_folder=astmt->getString(3);
    if (Folder != "_DEFAULT_" &&  Folder != m_folder) {
      cout << "warning from OnlineHistPage::OnlineHistPage:" <<
	" page "<< Name <<" already exists in folder "<<m_folder << 
	endl ;
    }
    m_doc = astmt->getString(4);
    m_conn->terminateStatement (astmt); 

    ResultSet *rset=0;
    astmt=m_conn->createStatement
      ("select VH.NAME,CENTER_X,CENTER_Y,SIZE_X,SIZE_Y,INSTANCE FROM SHOWHISTO SH,VIEWHISTOGRAM VH WHERE SH.PAGE=:1 and VH.HID= SH.HISTO ORDER BY SH.INSTANCE");
    try{
      astmt->setString(1, Name);
      rset =  astmt->executeQuery ();
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistPage::OnlineHistPage");
      }
    while ( rset->next () ) {
      OnlineHistogram* newh= m_Hstorage->getHistogram(rset->getString(1),
						      Name,
						      rset->getInt(6));
      newh->setDebug(debug());
      newh->setExcLevel(excLevel());
      m_h.push_back(newh);
      m_cx.push_back(rset->getFloat(2));
      m_cy.push_back(rset->getFloat(3));
      m_sx.push_back(rset->getFloat(4));
      m_sy.push_back(rset->getFloat(5));
    }
    astmt->closeResultSet (rset);
    m_conn->terminateStatement (astmt);
  }
  else {
    if (m_folder == "_DEFAULT_") {
      std::string mes = "No folder set for page " + m_name +
	".  Please use the setFolder method";
      errorMessage(mes);
    }
    m_conn->terminateStatement (astmt); 
  }
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
    if (save()) {
      if(knownHisto) {
	// object has been already used in a previous instance: create new object with proper instance
	outh = m_Hstorage->getHistogram(h->identifier(), m_name, instance);
	m_h[ih] = outh;
      }
      else 
	outh=h;
      bool setp = outh->setPage(m_name,instance);
      if (!setp) 
	errorMessage("Could not setting page "+m_name+" on histogram "+outh->identifier());
    }
    else {
      outh=0;
    }
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
  bool out=false;
  bool known;
  int jh=findHistogram(h,instance,known);
  if (-1 != jh) {
    m_h.erase(m_h.begin()+jh);
    m_cx.erase(m_cx.begin()+jh);
    m_cy.erase(m_cy.begin()+jh);
    m_sx.erase(m_sx.begin()+jh);
    m_sy.erase(m_sy.begin()+jh);
    out = save();
  }
  return out;
}

bool OnlineHistPage::removeAllHistograms() {
  m_h.clear();
  m_cx.clear();
  m_cy.clear();
  m_sx.clear();
  m_sy.clear();
  bool out = save();
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

bool OnlineHistPage::save() {
  bool out=true;
  if (m_folder == "_DEFAULT_") {
    errorMessage("Page "+m_name+" has no folder and cannot be saved");
    out = false;
  }
  else {
    stringstream hlist,cx,cy,sx,sy;
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
    command << "begin OnlineHistDB.DeclarePage(theName => '"<< m_name <<
      "',theFolder => '"<< m_folder <<
      "',theDoc => '"<< m_doc <<
      "',hlist => " << hlist.str() <<
      ",Cx => " << cx.str() <<
      ",Cy => " << cy.str() <<
      ",Sx => " << sx.str() <<
      ",Sy => " << sy.str() << "); end;";
    
    Statement *astmt=m_conn->createStatement(command.str()); 
    try{
      astmt->execute();
    }catch(SQLException ex)
      {
	dumpError(ex,"OnlineHistPage::save for page "+m_name);
	out=false;
    }
    m_conn->terminateStatement (astmt); 
  }
  return out;
}

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

OnlineHistPage* OnlinePageStorage::getPage(std::string Name, 
					   std::string Folder) {
  OnlineHistPage* page = 0;
  // see if page object exists already
  std::vector<OnlineHistPage*>::iterator ip;
  for (ip = m_myPage.begin();ip != m_myPage.end(); ++ip) {
    if ((*ip)->name() == Name) {
      page = *ip;
      break;
    }
  }
  // otherwise create new one
  if ( 0 == page) {
    page = new OnlineHistPage(*m_Pagenv, m_Hstorage, Name, Folder);
    m_myPage.push_back(page);
  }
  return page;
}

