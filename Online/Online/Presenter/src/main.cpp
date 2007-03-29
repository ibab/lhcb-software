// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Presenter/src/main.cpp,v 1.1 2007-03-29 08:56:11 psomogyi Exp $

#include <TQApplication.h>  // QtGSI
#include <TQRootApplication.h>
#include <qguardedptr.h> // for QObjects

#include <qsplashscreen.h> 

#include <TBrowser.h>  //ROOT

// compile with: /EHsc  //STL
//#include <iostream>
//#include <exception>

#include <boost/shared_ptr.hpp>  // BOOST
//#include <boost/tokenizer.hpp>
//#include <boost/lexical_cast.hpp>
// This is the BOOST foreach macro: vc71+ and gcc34+ are compliant enough.
// Qt4 ships with one too. Qt3 uic doesn't like it nested in ui.-s 
#include "foreach.hpp"

#include "OnlineHistDB/OnlineHistDB.h"  // LHCb
#include "PageEditorMainWindow.h"

//using std::string;
//using std::exception;

//using boost::shared_ptr;
//using boost::lexical_cast;
//using boost::bad_lexical_cast;

//#include <qlistview.h>

int main( int argc, char **argv )
{
  // Init applications  
  boost::shared_ptr<TQApplication> app(new TQApplication("tq", &argc, argv));
  TQRootApplication a(argc, argv, 0);
  
//  QPixmap pixmap(QPixmap::fromMimeSource("splash2.png"));
//  QSplashScreen* splash = new QSplashScreen( pixmap );
//  splash->message("Loading LHCb Presenter...");
  
  
  // TODO: Put startup parameter logic here: either Presenter or Editor
  QGuardedPtr<PageEditorMainWindow> mainWin(new PageEditorMainWindow);

  a.setMainWidget( mainWin );
//   splash->show();
  mainWin->show();
  mainWin->raise();
  mainWin->setActiveWindow();
//  splash->finish( mainWin );
//  delete splash;
//  new TBrowser();
  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
//  sleep(3);
//  delete splash;
  return a.exec();
}
