//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/src/OnlineHistPage.cpp,v 1.4 2007-01-29 17:52:23 ggiacomo Exp $

#include "OnlineHistDB/OnlineHistPage.h"

OnlineHistPage::OnlineHistPage(std::string Name, 
			       std::string Folder,
			       Connection* Conn) :
  OnlineHistDBEnv(Conn), m_name(Name), m_folder(Folder)
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
    if (Folder != (m_folder=astmt->getString(3))) {
      cout << "warning from OnlineHistPage::OnlineHistPage:" <<
	" page "<< Name <<" already exists in folder "<<m_folder << 
	endl << "  Please explicitly use the setFolder method to change the folder" 
	   << endl;
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
      //cout << "found histogram " << rset->getString(1) <<" on page "<<Name <<endl;
      OnlineHistogram* newh= new OnlineHistogram(rset->getString(1),
						 m_conn,
						 Name,
						 rset->getInt(6));
      m_h.push_back(newh);
      m_privh.push_back(newh);
      m_cx.push_back(rset->getFloat(2));
      m_cy.push_back(rset->getFloat(3));
      m_sx.push_back(rset->getFloat(4));
      m_sy.push_back(rset->getFloat(5));
    }
    astmt->closeResultSet (rset);
    m_conn->terminateStatement (astmt);
  }
}




OnlineHistPage::~OnlineHistPage(){
  // delete all histograms created here
  std::vector<OnlineHistogram*>::iterator ih;
  for (ih = m_privh.begin();ih != m_privh.end(); ++ih) 
    delete *ih; 
}


void OnlineHistPage::declareHistogram(OnlineHistogram* h,
				  float Cx,
				  float Cy,
				  float Sx,
				  float Sy,
				  unsigned int instance) {
  if (h->isAbort() == false) {
    int ih=findHistogram(h,instance);
    
    if (-1 == ih ) { // new histogram
      m_h.push_back(h);
      m_cx.push_back(Cx);
      m_cy.push_back(Cy);
      m_sx.push_back(Sx);
      m_sy.push_back(Sy);
    }
    else {
      m_cx[ih] = Cx;
      m_cy[ih] = Cy;
      m_sx[ih] = Sx;
      m_sy[ih] = Sy;
    }
    save();
    h->setPage(m_name,instance);
  }
}

int OnlineHistPage::findHistogram(OnlineHistogram* h,
				  unsigned int instance) const {
  int ih=-1;
  unsigned int ii=0;
  for (unsigned int jh=0; jh<m_h.size() ; jh++) {
    if ((m_h[jh])->hid() == h->hid()) { 
      if (++ii == instance) {
	ih=jh;
	break;
      }
    }
  }
  return ih;
}

bool OnlineHistPage::getHistLayout(OnlineHistogram* h,
				   float &Cx,
				   float &Cy,
				   float &Sx,
				   float &Sy,
				   unsigned int instance) const {
  bool found=false;
  int jh=findHistogram(h,instance);
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
  int jh=findHistogram(h,instance);
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




bool OnlineHistPage::save() {
  bool out=true;
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
      dumpError(ex,"OnlineHistPage::save");
      out=false;
    }
  return out;
}
