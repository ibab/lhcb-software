// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Presenter/gui/presenter/PageEditorMainWindow.ui.h,v 1.2 2007-03-30 12:51:28 psomogyi Exp $

/****************************************************************************
 ** ui.h extension file, included from the uic-generated form implementation.
 **
 ** If you want to add, delete, or rename functions or slots, use
 ** Qt Designer to update this file, preserving your code.
 **
 ** You should not define a constructor or destructor in this file.
 ** Instead, write your code in functions called init() and destroy().
 ** These will automatically be called by the form's constructor and
 ** destructor.
 ***************************************************************************/
#include <boost/shared_ptr.hpp>  // BOOST
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include "foreach.hpp"

#include "OnlineHistDB/OnlineHistDB.h"  // LHCb
#include "dic.hxx"

#include "DimProxy.h"
#include "DatabaseLoginDialog.h"

//#include "algorithm"

#include <qstatusbar.h> //Qt
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qlistview.h>
#include <qtimer.h>
//#include <qfont.h>
#include <qguardedptr.h> // for QObjects

#include <TCanvas.h>              // ROOT
//#include <TRootEmbeddedCanvas.h>
#include <TPad.h>
#include <TH1.h>
#include <TH2.h>

#include <math.h> //std

//using boost::shared_ptr;
//using std::string;
//using std::cout;
//using std::vector;
//using boost::lexical_cast;

DimBrowser   dimBrowser;
OnlineHistDB *histogramDB = NULL; // deleted in a.destructor()
QTimer       *autoUpdate  = NULL; // Qt will get rid of this upon exit.
bool         offsetData   = false;
bool         startStop    = false;

DatabaseLoginDialog *loginDialog = NULL;

boost::char_separator<char> slashSeparator("/");
boost::char_separator<char> underscoreSeparator("_"); // "|;-"
typedef boost::tokenizer< boost::char_separator<char> > Tokens;

struct HistogramH1D
{
  TH1F     *rootH1D;
  TH1F     *rootOffsetH1D;  
  DimProxy *dimProxy;
};

struct HistogramH2D
{
  TH2F     *rootH2D;
  TH2F     *rootOffsetH2D;  
  DimProxy *dimProxy;
};

// vector of histograms to be displayed: to see with QPtrList...
std::vector<HistogramH2D> displayedH2DHistograms;
std::vector<HistogramH1D> displayedH1DHistograms;
std::vector<HistogramH1D> displayedHPDHistograms;

QPixmap PageEditorMainWindow::uicLoadPixmap(const QString &name)
{
  const QMimeSource *m = QMimeSourceFactory::defaultFactory()->data(name);
  if (!m) return QPixmap();
  QPixmap pix;
  QImageDrag::decode(m, pix);
  return pix;
}

void PageEditorMainWindow::dimContextMenu()
{
  QPopupMenu *dimContextMenu = new QPopupMenu(this);
  Q_CHECK_PTR(dimContextMenu);
//  dimContextMenu->insertTearOffHandle();
  if(m_connectedToHistogramDatabase){
    dimContextMenu->insertItem(tr("&Add selection to Database"), this,
                               SLOT(addHistogramsToDatabase()));
  }
    dimContextMenu->insertItem(tr("&Add selection to page"), this,
                             SLOT(addSelectedHistogramsFromDIMToPage()));                               

//  dimContextMenu->insertSeparator();
  dimContextMenu->exec(QCursor::pos());
  delete dimContextMenu;
}

void PageEditorMainWindow::fileOpen()
{

}

void PageEditorMainWindow::fileSave()
{

}

void PageEditorMainWindow::fileSaveAs()
{

}

void PageEditorMainWindow::filePrint()
{

}

void PageEditorMainWindow::helpIndex()
{

}

void PageEditorMainWindow::helpContents()
{

}

void PageEditorMainWindow::helpAbout()
{

}

void PageEditorMainWindow::init()
{
  m_connectedToHistogramDatabase = false;
  
  autoUpdate = new QTimer(this, "Page refresh handler");
  connect(autoUpdate, SIGNAL(timeout()), this, SLOT(refreshPage()));
  autoUpdate->start(50, FALSE);
    
  // get msg logging     
  refreshDIMSVCListView();
  
  pageHistogramsView->hide();
  
  // TODO: work w/o HistoDB...
  if (!loginDialog) {
    loginDialog = new DatabaseLoginDialog(this);
    connect(loginDialog, SIGNAL(login(const QString &,
                                      const QString &,
                                      const QString &)),
            this, SLOT(connectToDatabase(const QString &,
                                         const QString &,
                                         const QString &)));
  }

  loginDialog->show();
//  loginDialog->raise();
//  loginDialog->setActiveWindow();
}

void PageEditorMainWindow::destroy()
{
  if (histogramDB) delete histogramDB;
}

void PageEditorMainWindow::refreshHistogramListView()
{
  std::vector<std::string>      localDatabaseFolders;
  std::vector<std::string>      localDatabasePages;
  std::vector<OnlineHistogram*> localDatabaseHistograms;
  
  histoDBHistogramsView->clear();
  
  try {
    if (m_connectedToHistogramDatabase) {
      localDatabaseFolders.clear();
      histogramDB->getPageFolderNames(localDatabaseFolders);
      BOOST_FOREACH(string folder, localDatabaseFolders) {
        QListViewItem *folderItem = new QListViewItem(histoDBHistogramsView,
                                                      folder,
                                                      tr("Folder"));
        localDatabasePages.clear();
        histogramDB->getPageNamesByFolder(folder, localDatabasePages);
        BOOST_FOREACH(string page, localDatabasePages) {
          QListViewItem *pageItem = new QListViewItem(folderItem,
                                                      page,
                                                      tr("Page"));
          localDatabaseHistograms.clear();
          histogramDB->getHistogramsByPage(page, localDatabaseHistograms);
          BOOST_FOREACH(OnlineHistogram *histogram, localDatabaseHistograms)
          {
            QListViewItem *item1 = new QListViewItem(pageItem,
                                         histogram->identifier(),
                                         tr("Histogram"));
            if("H2D" == histogram->hstype()) {
              item1->setPixmap(0, uicLoadPixmap("h2_t.png"));
            } else if ("H1D" == histogram->hstype()) {
              item1->setPixmap(0, uicLoadPixmap("h1_t.png"));
            }   
          }
        }
      }
    }
  } catch (SQLException sqlException) {
    // TODO: add error logging backend
//  if (m_verbose > 1) e.getErrorCode()
    std::cout << sqlException.getMessage() << std::endl;
//    qDebug(sqlException.getMessage().);
    switch(QMessageBox::warning(this, tr("LHCb Presenter Page Editor"),
                            tr("Could get histograms from database:\n\n%1\n"
                               "\n\n").arg(sqlException.getMessage()),
                            QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
        // try again
        break;
      default:
        break;
    }
  }
}

void PageEditorMainWindow::refreshDIMSVCListView()
{
  // See what we can do about DIM services...
  std::string  dimServerName;
  std::string  dimServerNodeName;  
  std::string  dimServiceName;
  std::string  dimServiceFormat;  
  std::string  statusMessage;

  dimServicesView->clear();
  
  const int nDimServers = dimBrowser.getServers();

  // if some servers found, discover which services are there
  // - otherwise return "fail"
  if (nDimServers > 0) {
    const char *dimDnsServerNode = DimClient::getDnsNode();
    statusMessage = "Successfully connected to DIM via DIM_DNS_NODE \"" + 
                    boost::lexical_cast<std::string>(dimDnsServerNode) +"\"";
    std::cout << statusMessage << std::endl;
    statusBar()->message(statusMessage);
      
    char  *dimFormat;
    char  *dimServer;    
    char  *dimServerNode;
    char  *dimService;  
    int   dimType;
      
    std::string svcType;
    std::string utgidElement;
    std::string taskName;
    std::string algorithmName;
    std::string entityName; 

    while (dimBrowser.getNextServer(dimServer, dimServerNode)) {
    
      // get name of this server
      dimServerName     =  boost::lexical_cast<std::string>(dimServer);
      dimServerNodeName =  boost::lexical_cast<std::string>(dimServerNode);
      
      if (dimServerName.find("DIS_DNS",0) != std::string::npos) {
  //    if (m_verbose > 1)
        std::cout << "\tfound DIS_DNS server at node \"" <<
                     dimServerNodeName << "\" skipping..." << std::endl;
      } else {
        std::cout << "\tfound interesting DIM server \"" <<
                   dimServerName << "\" providing the following services:" <<
                   std::endl;    
        QListViewItem *item = new QListViewItem(dimServicesView,
                                                "DIM Services");  
        QListViewItem *thisDimServer = new QListViewItem(item,
                                                       dimServerName.c_str(),
                                                       "");
        
        // now loop get a list of all services offered by this server
        dimBrowser.getServerServices(dimServerName.c_str());        
        while(dimType = dimBrowser.getNextServerService(dimService,
                                                        dimFormat)) {
          dimServiceName = boost::lexical_cast<std::string>(dimService);
          dimServiceFormat  = boost::lexical_cast<std::string>(dimFormat);
          
          // only want DIM services
          if (dimType != DimSERVICE) continue;
  
          // thisDimServer ListView cols: Task, Algo, Name, Type, UTGID, DIM
          // replace this with an enum...
          // we tokenize the DIM SVC name and fill in the list...
          // H1D/node_Adder_1/Adder/Algorithmname/Mass
          Tokens tokenedDIMSVC(dimServiceName, slashSeparator);
          Tokens::iterator dimSVCNameElements = tokenedDIMSVC.begin();
          svcType = *dimSVCNameElements;
          // The first DIM SVC element is type, so checking against token:
          // TODO: Ask Giacomo to support HPD in HistoDB
          if ("H1D" == svcType ||
              "H2D" == svcType) { //||
  //          "HPD" == svcType) {
  //        if (m_verbose > 1)          
            std::cout << "\t->\tSVC: " << dimServiceName <<
                         " format "    << dimServiceFormat  << std::endl;
  
            // Parse Taskname:
            utgidElement  = *++dimSVCNameElements;
            Tokens tokenedUTGID(utgidElement, underscoreSeparator);
            Tokens::iterator utgidElements = tokenedUTGID.begin();          
            taskName      = *++utgidElements;          
            algorithmName = *++dimSVCNameElements;
            entityName    = *++dimSVCNameElements;

            // TODO: do a "GROUP BY" at some point, add ++ bounds checking...
            QListViewItem *item1 = new QListViewItem(thisDimServer,
                                                     entityName,
                                                     algorithmName,
                                                     taskName,
                                                     svcType,
                                                     utgidElement,
                                                     dimServiceName.c_str());
            if("H2D" == svcType) {
              item1->setPixmap(0, uicLoadPixmap("h2_t.png"));
            } else if ("H1D" == svcType) {
              item1->setPixmap(0, uicLoadPixmap("h1_t.png"));
            }          
          } else  {
  //      if (m_verbose > 1)          
            std::cout << "\t\tSVC: " << dimServiceName <<
                         " format "  << dimServiceFormat  << std::endl;
          }
        }
      }
    } 
  } else {
    std::cout << "Sorry, no DIM DNS found." << std::endl;
    switch(QMessageBox::warning(this, "LHCb Presenter Page Editor",
                          tr("Could not connect to a DIM DNS server.\n\n"
                             "This program can't function correctly.\n\n"),
                          QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
          // try again
        break;
      default:
        break;
      }    
    statusBar()->message(tr("Sorry, no DIM DNS found."));
  }
}

void PageEditorMainWindow::refreshPagesListView()
{
  std::vector<std::string> localDatabaseFolders;
  std::vector<std::string> localDatabasePages;
  histoDBPagesView->clear();
  try {
    if (m_connectedToHistogramDatabase) {
      localDatabaseFolders.clear();
      histogramDB->getPageFolderNames(localDatabaseFolders);
      BOOST_FOREACH(string folder, localDatabaseFolders) {
        QListViewItem *folderItem = new QListViewItem(histoDBPagesView,
                                                      folder,
                                                      "Folder");
        localDatabasePages.clear();
        histogramDB->getPageNamesByFolder(folder, localDatabasePages);
        BOOST_FOREACH(string page, localDatabasePages) {
        new QListViewItem(folderItem, page, "Page");
        }
      }
    }
  } catch (SQLException sqlException) {
// TODO: add error logging backend
// if (m_verbose > 1)      
    cout << sqlException.getMessage() << endl;
    switch (QMessageBox::warning(this, "LHCb Presenter Page Editor",
                            tr("Could get histograms from database:\n\n%1\n"
                               "\n\n").arg(sqlException.getMessage()),
                            QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
        // try again
        break;
      default:
        break;        
    }
  }
}

void PageEditorMainWindow::refreshFolderNameComboBox()
{
  std::vector<std::string> folders;
  folders.clear();
  folderNameComboBox->clear();
  try {
    if (m_connectedToHistogramDatabase) {
      folders.clear();
      histogramDB->getPageFolderNames(folders);
      BOOST_FOREACH(string folder, folders) {
        folderNameComboBox->insertItem(folder);
      }
    }
  } catch (SQLException sqlException) {
// TODO: add error logging backend
// if (m_verbose > 1)      
    cout << sqlException.getMessage() << endl;
    switch(QMessageBox::warning(this, tr("LHCb Presenter Page Editor"),
                      tr("Could get folders and pages from database:\n\n%1\n"
                         "\n\n").arg(sqlException.getMessage()),
                        QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
        // try again
        break;
      default:
        break;        
      }
  }
  folderNameComboBox->setEditText(tr("Type foldername here..."));
}

void PageEditorMainWindow::refreshPageNameComboBox()
{
  std::vector<std::string> localDatabasePages; 
  localDatabasePages.clear();
  pageNameComboBox->clear();
  histogramDB->getPageNamesByFolder(folderNameComboBox->currentText(),
                                    localDatabasePages);
  BOOST_FOREACH(string page, localDatabasePages) {
    pageNameComboBox->insertItem(page);
  }
  pageNameComboBox->setEditText(tr("Type pagename here..."));
}

void PageEditorMainWindow::histoDBPagesViewContextMenu()
{
  QPopupMenu *dimContextMenu = new QPopupMenu(this);
  Q_CHECK_PTR(dimContextMenu);
  // if item is selected and is a leaf, then handle given page
  // TODO: traversal in case of non-leaf nodes...
  QListViewItem *item = histoDBPagesView->selectedItem();
  if ( 0 == item->childCount()) {    
//  dimContextMenu->insertTearOffHandle();
  dimContextMenu->insertItem(tr("&Load selected page"), this,
                             SLOT(loadSelectedPageFromDB()));
//  dimContextMenu->insertSeparator();
//  dimContextMenu->insertItem("&Delete selected page", this,
//                             SLOT(deleteSelectedPageFromDB()));
  dimContextMenu->exec(QCursor::pos());
  } else {
//    dimContextMenu->insertItem("&Delete selected folder", this,
//                               SLOT(deleteFolderFromDB()));  
  }
  delete dimContextMenu;
}

void PageEditorMainWindow::histoDBHistogramsViewContexMenu()
{
  QPopupMenu *dimContextMenu = new QPopupMenu(this);
  Q_CHECK_PTR( dimContextMenu );
//  dimContextMenu->insertTearOffHandle();
  dimContextMenu->insertItem(tr("&Add selection to page"), this,
                             SLOT(addSelectedHistogramsFromDatabaseToPage()));
    //  dimContextMenu->insertSeparator(); 
  dimContextMenu->exec(QCursor::pos());
  delete dimContextMenu;
}

void PageEditorMainWindow::deleteSelectedPageFromDB()
{
// HistoDB does not support it by specification...
}

void PageEditorMainWindow::loadSelectedPageFromDB()
{
// OnlineHistPage* pg=histogramDB->getPage("Example Page","Examples")
}

void PageEditorMainWindow::savePage()
{
// h1->setHistoSetDisplayOption("LINECOLOR",(void*) &lc);
// h1->setHistoSetDisplayOption("FILLSTYLE",(void*) &fs);
// h1->setHistoSetDisplayOption("FILLCOLOR",(void*) &fc); 
// h1->setHistoSetDisplayOption("YMAX",(void*) &ymax);
// lc=3;
// h2->initDisplayOptionsFromSet();

  try {
    if (m_connectedToHistogramDatabase) {  
//   void GetPadPar(Double_t &xlow, Double_t &ylow, Double_t &xup, Double_t &yup)
//   Size_t            GetXsizeUser() const {return fXsizeUser;}
//   Size_t            GetYsizeUser() const {return fYsizeUser;}
//   Size_t            GetXsizeReal() const {return fXsizeReal;}
//   Size_t            GetYsizeReal() const {return fYsizeReal;}
// WNDC, HNDC, XlowNDC, YlowNDC, Abs~
  
      statusBar()->message(tr("Saving page to OnlineHistDB..."));
      
            // TODO: Sanity check, dialog - add length check too...
      if (tr("Type foldername here...") == folderNameComboBox->currentText() ||
          tr("Type pagename here...") == pageNameComboBox->currentText()) return;
      
      OnlineHistPage *page = histogramDB->
            getPage(pageNameComboBox->currentText(),
            folderNameComboBox->currentText());
          page->setFolder(folderNameComboBox->currentText());            
      
      BOOST_FOREACH(HistogramH1D displayedH1DHistogram,
                    displayedH1DHistograms) {
        std::cout << "Title: " << displayedH1DHistogram.rootH1D->GetTitle()
                  << std::endl;
        OnlineHistogram *h1 = histogramDB->
              getHistogram(displayedH1DHistogram.rootH1D->GetTitle());
        page->declareHistogram(h1,0. ,0. ,0.5,0.5);   
      }
      BOOST_FOREACH(HistogramH1D displayedHPDHistogram,
                    displayedHPDHistograms) {
        std::cout << "Title: " << displayedHPDHistogram.rootH1D->GetTitle()
                  << std::endl;
        OnlineHistogram *h1 = histogramDB->
              getHistogram(displayedHPDHistogram.rootH1D->GetTitle());
        page->declareHistogram(h1,0. ,0. ,0.5,0.5);        
      }
      BOOST_FOREACH(HistogramH2D displayedH2DHistogram,
                    displayedH2DHistograms) {
        std::cout << "Title: " << displayedH2DHistogram.rootH2D->
                                      GetTitle() << std::endl;
        OnlineHistogram *h1 = histogramDB->
              getHistogram(displayedH2DHistogram.rootH2D->GetTitle());
        page->declareHistogram(h1,0. ,0. ,0.5,0.5);        
      }
      
      histogramDB->commit();
      
      refreshPagesListView();
      refreshHistogramListView();
      refreshFolderNameComboBox();
      refreshPageNameComboBox();
      
      statusBar()->message(tr("Successfully saved page to OnlineHistDB."));                                               
    }
  } catch (SQLException sqlException) {
// TODO: add error logging backend
// if (m_verbose > 1)      
    cout << sqlException.getMessage() << endl;
    statusBar()->message(tr("Could not save page to OnlineHistDB."));
    switch(QMessageBox::warning(this, tr("LHCb Presenter Page Editor"),
                                  tr("Could save page to database:\n\n%1\n"
                                     "\n\n").arg(sqlException.getMessage()),
                                  QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
        // try again
        break;
      default:
        break;        
    }
  }                                      
}

void PageEditorMainWindow::deleteSelectedFolderFromDB()
{
// HistoDB does not support it by specification...
}

void PageEditorMainWindow::switchLayoutMode()
{
  // TODO: reset Page layout when switching...
  if (autoLayoutRadioButton->isChecked()) {
    histoDBHistogramsView->setSelectionMode(QListView::Extended);
  } else if (manualLayoutRadioButton->isChecked()) {
    histoDBHistogramsView->setSelectionMode(QListView::Single);
  }
}

void PageEditorMainWindow::addSelectedHistogramsFromDatabaseToPage()
{
  int    nBinsX = 0;
  int    nBinsY = 0;
  double xMin   = 0;
  double xMax   = 0;
  double yMin   = 0;
  double yMax   = 0;
  
  double value  = 0;
  
  int m_refreshTimeHisto = 1;
  int m_verbose = 1;
  
  int iHisto = 1;

  m_mainPageRootCanvas->GetCanvas()->SetEditable(true);
  m_mainPageRootCanvas->GetCanvas()->Divide(1,1);
  m_mainPageRootCanvas->Clear();
  
  
//  QPtrList<QListViewItem> lst;
//  QListViewItemIterator itS(histoDBHistogramsView);
//  while (itS.current()) {
//    if (itS.current()->isSelected()) {
//      lst.append(itS.current());
//  ++itS;
//  }
     
  
  QListViewItemIterator it(histoDBHistogramsView);  
  while (it.current()) {
    QListViewItem *item = it.current();
    if (item->isSelected() && "Histogram" == item->text(1)) {

      OnlineHistogram *h1 = histogramDB->getHistogram(item->text(0));
      
      std::string serviceNameFQ = h1->hstype() + "/nodeMF001_" + 
                                  h1->task() + "_01/" + h1->algorithm() +
                                  "/" + h1->hsname();

      std::cout << serviceNameFQ << std::endl;                         
      
      new QListViewItem(pageHistogramsView, item->text(0), h1->hstype());

      std::string serviceType = h1->hstype();   
//    if (m_verbose > 1)
        std::cout << "Reconstructing DIM SVC name: " << serviceNameFQ <<
                     std::endl;
                     
      iHisto++;
      std::string histoID = "histo_" +
                            boost::lexical_cast<std::string>(iHisto);                     
                                   
      if ("H1D" == serviceType.substr(0,3)) {
        HistogramH1D histogramH1D;
        // TODO: sharedptr: place delete...
        histogramH1D.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);        

        if (histogramH1D.dimProxy->serviceOK()) {    
          TH1 *tmp_h1D = histogramH1D.dimProxy->rootH1D();
          if (tmp_h1D) {
            nBinsX             = tmp_h1D->GetNbinsX();
            xMin               = tmp_h1D->GetXaxis()->GetXmin();
            xMax               = tmp_h1D->GetXaxis()->GetXmax();
            std::string hTitle = item->text(0);
//            tmp_h1D->GetTitle();
//            if (m_verbose > 3) {
              std::cout << " nBinsX " << nBinsX <<
                           " xMin "   << xMin   <<
                           " xMax "   << xMax   << std::endl;
//            }

            // book the 1D histo which is to be displayed
            // TODO: sharedptr - place delete
            histogramH1D.rootH1D = new TH1F(histoID.c_str(), hTitle.c_str(),
                                            nBinsX, xMin, xMax);
            histogramH1D.rootOffsetH1D = new TH1F("", "",
                                                  nBinsX, xMin, xMax);
            histogramH1D.rootH1D->SetMarkerStyle(22);
            histogramH1D.rootH1D->SetMarkerSize(0.9);
            displayedH1DHistograms.push_back(histogramH1D);
            
            // Fill w some data:
            nBinsX = histogramH1D.rootH1D->GetNbinsX();      
            value  = 0;

            double nEntries = histogramH1D.dimProxy->rootH1D()->GetEntries();
            
            for (int i=0; i< nBinsX+1; ++i) {
              value = histogramH1D.dimProxy->rootH1D()->GetBinContent(i);
              histogramH1D.rootH1D->SetBinContent(i, value);
              
              value = histogramH1D.dimProxy->rootH1D()->GetBinError(i);
              histogramH1D.rootH1D->SetBinError(i, value);
            }
  
            histogramH1D.rootH1D->SetEntries(nEntries);
            
            if (m_verbose > 0)
              std::cout << "DIM histo entries " << nEntries
                        << " histo entries "    <<
                           histogramH1D.rootH1D->GetEntries() << std::endl;            
          }
        }
      } else if ("HPD" == serviceType.substr(0,3)) {
        HistogramH1D histoProfile;
        // TODO: sharedptr - place delete     
        histoProfile.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);
        if (histoProfile.dimProxy->serviceOK()) {    
          TH1 *tmp_h1D = histoProfile.dimProxy->rootHPD();
          if (tmp_h1D) {
            nBinsX             = tmp_h1D->GetNbinsX();
            xMin               = tmp_h1D->GetXaxis()->GetXmin();
            xMax               = tmp_h1D->GetXaxis()->GetXmax();
            std::string hTitle = item->text(0);
//             tmp_h1D->GetTitle();
//          if (m_verbose > 3)
              std::cout << " nBinsX " << nBinsX << 
                           " xMin "   << xMin   <<
                           " xMax "   << xMax   << std::endl;

            // book the 1D histo which is to be displayed
            // TODO: sharedptr - place delete    
            histoProfile.rootH1D = new TH1F(histoID.c_str(),
                                            hTitle.c_str(),
                                            nBinsX, xMin, xMax);
            histoProfile.rootOffsetH1D = new TH1F("",
                                                  "",
                                                  nBinsX, xMin, xMax);
            histoProfile.rootH1D->SetMarkerStyle(22);
            histoProfile.rootH1D->SetMarkerSize(0.9);
            displayedHPDHistograms.push_back(histoProfile);
          }
        }
        
      } else if ("H2D" == serviceType.substr(0,3)) {              
        HistogramH2D histogramH2D;
        // TODO: sharedptr - place delete 
        histogramH2D.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);
        if (histogramH2D.dimProxy->serviceOK()) {
          TH2 *tmp_h2D = histogramH2D.dimProxy->rootH2D();
          if (tmp_h2D) {
            // dimProxy gives back a NULL pointer if something went wrong
            nBinsX             = tmp_h2D->GetNbinsX();
            nBinsY             = tmp_h2D->GetNbinsY();
            xMin               = tmp_h2D->GetXaxis()->GetXmin();
            xMax               = tmp_h2D->GetXaxis()->GetXmax();
            yMin               = tmp_h2D->GetYaxis()->GetXmin();
            yMax               = tmp_h2D->GetYaxis()->GetXmax();
            std::string hTitle = item->text(0);
//             tmp_h2D->GetTitle();            
//          if (m_verbose > 3)
            std::cout << " nBinsX " << nBinsX <<
                         " xMin "   << xMin   << " xMax " << xMax << 
                         " nBinsY " << nBinsY << " yMin " << yMin <<
                         " yMax "   << yMax   << std::endl;
            
            // book new TH2F - the one which is to be displayed
            // TODO: sharedptr - place delete       
            histogramH2D.rootH2D = new TH2F(histoID.c_str(), hTitle.c_str(), 
                                            nBinsX, xMin, xMax,
                                            nBinsY, yMin, yMax);
            histogramH2D.rootOffsetH2D = new TH2F("", "", 
                                            nBinsX, xMin, xMax,
                                            nBinsY, yMin, yMax);            
            displayedH2DHistograms.push_back(histogramH2D);
            
            // Fill with some data
            double nEntries = histogramH2D.dimProxy->rootH2D()->GetEntries();
                    
            nBinsX   = histogramH2D.rootH2D->GetNbinsX();
            nBinsY   = histogramH2D.rootH2D->GetNbinsY();      
            value    = 0.0;
                
            for (int i=0; i<= nBinsX+1; ++i) {
              for (int j=0; j<= nBinsY+1; ++j) {
                value = histogramH2D.dimProxy->rootH2D()->GetBinContent(i,j);
                if (0.0 == value) value = 0.00001; // to show in nice colour
                histogramH2D.rootH2D->SetBinContent(i, j, value);                
                value = histogramH2D.dimProxy->rootH2D()->GetBinError(i, j);
                histogramH2D.rootH2D->SetBinError(i, j, value);
              } 
            }
            
            histogramH2D.rootH2D->SetEntries(nEntries);
            
            if (m_verbose > 0)
              std::cout << "DIM histo entries " << nEntries
                        << " histo entries "    <<
                           histogramH2D.rootH2D->GetEntries() << std::endl;            
          }
        }
      }        
    }
    ++it;
  }
             
// setup the Canvas and draw the (empty) histograms
// choose a setup where #rows = #columns
// may lead to some unsused parts of the canvas, but 
// probably the easiest/best setup if no further
// knowledge about the contents displayed
              
  // #pads needed to display all information
  int nPads            = pageHistogramsView->childCount();  
  int m_nCanvasRows    = (int) ceil(sqrt((double)nPads));
  int m_nCanvasColumns = m_nCanvasRows;  

  m_mainPageRootCanvas->GetCanvas()->Divide(m_nCanvasColumns,m_nCanvasRows);
  int padCounter = 1;
  // now loop over all pads to set basic properties
  for (int i = 1; i<= m_nCanvasRows; ++i) {
    for (int j = 1; j <= m_nCanvasColumns; ++j) {
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetFillColor(10);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->Draw();      
      // m_mainPageRootCanvas->GetPad(padCounter)->SetTopMargin(0.5);
      // m_mainPageRootCanvas->GetPad(padCounter)->SetBottomMargin(0.5);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetLeftMargin(0.15);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetRightMargin(0.15);
      ++padCounter;      
    } 
  }
  
  // now loop over all histograms and draw them: 
  // order: 2D - 1D - Counters

  padCounter = 1;
  BOOST_FOREACH(HistogramH2D displayedH2DHistogram, displayedH2DHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);
//    displayedH2DHistogram.rootH2D->SetStats(kFALSE);
//    (*h2DIter).rootH2D -> Draw(m_2DDrawOption.c_str());
    displayedH2DHistogram.rootH2D->Draw("lego");
    ++padCounter;
  }

  BOOST_FOREACH(HistogramH1D displayedH1DHistogram, displayedH1DHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);    
//    displayedH1DHistogram.rootH1D->SetStats(kFALSE);
//    (*h1DIter).rootH1D -> Draw(m_1DDrawOption.c_str());      
    displayedH1DHistogram.rootH1D->Draw();
    ++padCounter;      
  }

  BOOST_FOREACH(HistogramH1D displayedHPDHistogram, displayedHPDHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);    
//    displayedHPDHistogram.rootH1D->SetStats(kFALSE);
//    (*hProfIter).rootH1D -> Draw(m_1DDrawOption.c_str());
    displayedHPDHistogram.rootH1D->Draw();      
    ++padCounter;      
  } 
  m_mainPageRootCanvas->Update();
  m_mainPageRootCanvas->GetCanvas()->SetEditable(false);
  statusBar()->message(tr("Canvas ready."));
}

void PageEditorMainWindow::addHistogramsToDatabase()
{
  QString dimURL;
  QListViewItemIterator it(dimServicesView);
  while (it.current()) {
    QListViewItem *item = it.current();
    // if item is selected and is a leaf, then add to DB
    // TODO: traversal in case of non-leaf nodes...
    if (item->isSelected() && 0 == item->childCount() ) {
      // Ask Giacomo for HPD DIM SVC name patching on DB side
      try {
        if (m_connectedToHistogramDatabase) {
//        if (m_verbose > 1) {
//          std::cout <<  "Histogram: " << item->text(5) <<
//                        " Task: " << item->text(0) << std::endl;
//        }
          // Declare task...      
//          histogramDB->declareTask(item->text(0),"","","",true,true,false);
          // Declare Histogram
          // TODO: comment magic column numbers...
          histogramDB->declareHistByServiceName(item->text(5));
          histogramDB->commit();
          std::string histoName = item->text(2) + "/" + item->text(1) +
                                  "/" + item->text(0);

          OnlineHistogram* h3=histogramDB->getHistogram(histoName);
          OnlineHistPage* pg=histogramDB->getPage("yPage", "unsorted");
          pg->setFolder("unsorted");
          pg->declareHistogram(h3, 0.0, 0.0, 0.5, 1.0);
//           histogramDB->commit();  
          // TODO: col-enum
          // This should also work ...find out why it doesn't.
//          histogramDB->declareHistogram(item->text(2), // taskName
//                                   item->text(1), // algorithmName
//                                   item->text(0), // entityName
//                                   0);            // Dimension
//
//          std::cout <<  "Task: "  << item->text(2) <<
//                        " Algo: " << item->text(1) <<
//                        " Name: " << item->text(0) << std::endl;
                               
          statusBar()->message(tr("Successfully added selected histogram(s)"
                                "to queue for declaration."));
        } else {
          statusBar()->message(tr("Could not add selected histogram(s) to "
                               "queue for declaration."));
        }
      } catch (SQLException sqlException) {
        // TODO: add error logging backend - MsgStream?
        statusBar()->message(tr("Could not add selected histogram(s) to "
                             "OnlineHistDB for declaration."));
        switch(QMessageBox::warning(this, tr("LHCb Presenter Page Editor"),
          tr("Could not queue selected histogram(s) to OnlineHistDB:\n\n%1\n"
             "Sorry.\n\n").arg(sqlException.getMessage()),
          QMessageBox::Ok, QMessageBox::NoButton)) {
            case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
              // try again
              break;
            default:
              break;
          }
//      if (m_verbose > 1)          
        std::cout << sqlException.getMessage();                                 
      }
    }
    ++it;
  }
  // TODO: Add Confirm dialog w listing of histos to be added.
  switch(QMessageBox::question(this, tr("LHCb Presenter Page Editor"),
      tr("Really add selected histograms to Histogram Database?\n\n"),
         QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton)) {
    case QMessageBox::Yes: // The user clicked the Ok/pressed Enter        
      if (m_connectedToHistogramDatabase) {
        statusBar()->message(tr("Saving selected histogram(s) to"
                             "OnlineHistDB..."));
        try {
          histogramDB->commit();
          statusBar()->message(tr("Successfully declared selected histograms "
                               "in OnlineHistDB."));
//        if (m_verbose > 1) std::cout << "declared: " <<  std::endl;                                  
        } catch (SQLException sqlException) {
        // TODO: add error logging backend - MsgStream?
        // TODO: Ask Giacomo to add clear statement queue to API.
          switch(QMessageBox::warning(this, tr("LHCb Presenter Page Editor"),
              tr("Could not declare selected histograms in OnlineHistDB:\n\n"
                 "%1\nSorry.\n\n").arg(sqlException.getMessage()),
              QMessageBox::Ok, QMessageBox::NoButton,
              QMessageBox::NoButton)) {
            case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
              // try again
              break;
            default:
              break;
          }
        statusBar()->message(tr("Could not declare selected histograms in "
                             "OnlineHistDB"));
//        if (m_verbose > 1)
        std::cout << sqlException.getMessage();                         
        }
      }
      break;
    default:
      break;
  }
  dimServicesView->clearSelection();
  
  // TODO:
  if (m_connectedToHistogramDatabase){    
    //TODO: refactor refresh* methods to use only 1 DB readout...
    refreshPagesListView();
    refreshHistogramListView();
    refreshFolderNameComboBox();
    refreshPageNameComboBox();
  }

}

void PageEditorMainWindow::refreshPageToggle(){
  
  if (startStopButton->isOn()) {
    //std::cout << "Hi." << std::endl;
    startStop = true;
    startStopButton->setText(tr("Stop"));    
  } else {
    //std::cout << "isOn." << std::endl;
    startStop = false;
    startStopButton->setText(tr("Start"));  
  }
}

void PageEditorMainWindow::refreshPage()
{
  if (startStop) {
    bool   updateCanvas = false;
    bool   m_histoOK    = true;
    
    int    nBinsX   = 0;
    int    nBinsY   = 0;
    double value    = 0;
    
//    if (m_verbose > 2 )
//      std::cout << "loop over 1D histograms and update if necessary" << std::endl;

    BOOST_FOREACH(HistogramH1D displayedH1DHistogram,
                  displayedH1DHistograms) {
//      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
      if (displayedH1DHistogram.dimProxy->serviceOK()) {      
        if (displayedH1DHistogram.dimProxy->serviceUpdated()) {
          // histogram has received new data, need to update
          updateCanvas = true;
          displayedH1DHistogram.dimProxy->resetServiceUpdated();
          
          displayedH1DHistogram.rootH1D->Reset();      
          nBinsX   = displayedH1DHistogram.rootH1D->GetNbinsX();      
          value    = 0;
          
          double nEntries = displayedH1DHistogram.dimProxy->
              rootH1D()->GetEntries();
          
//            if (statOption == HPDGui::stat1D ||
//                statOption == HPDGui::stat1D2D) {
//              (*h1DIter).h1D->SetStats(kTRUE);
//            } else {
//              (*h1DIter).h1D->SetStats(kFALSE);
//            } // if statOption
          
          for (int i=0; i< nBinsX+1; ++i) {
            value = displayedH1DHistogram.dimProxy->
                rootH1D()->GetBinContent(i);
            displayedH1DHistogram.rootH1D->SetBinContent(i, value);
            
            value = displayedH1DHistogram.dimProxy->
                rootH1D()->GetBinError(i);
            displayedH1DHistogram.rootH1D->SetBinError(i, value);
          }

          displayedH1DHistogram.rootH1D->SetEntries(nEntries);
          
//            if (m_verbose > 0)
            std::cout << "DIM histo entries " << nEntries << 
                         " histo entries "    <<
                         displayedH1DHistogram.rootH1D->GetEntries() <<
                         std::endl;  
        }        
      } else  {
        m_histoOK = false;
        std::string hTitle = displayedH1DHistogram.rootH1D->GetTitle();      
//        if (m_verbose > 1)
          std::cout << "H1D not OK " << hTitle << std::endl;
//        HPDGui::Pause(); // pause updates
        std::string message = "1D histogram " + hTitle +
                              " not OK, stopping update";      
        statusBar()->message(message.c_str());
        return;      
      }  
    }

    BOOST_FOREACH(HistogramH1D displayedHPDHistogram,
                  displayedHPDHistograms) {
//      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
      if (displayedHPDHistogram.dimProxy->serviceOK()) {      
        if (displayedHPDHistogram.dimProxy->serviceUpdated()) {
          // histogram has received new data, need to update
          updateCanvas = true;
          displayedHPDHistogram.dimProxy->resetServiceUpdated();
          
          displayedHPDHistogram.rootH1D->Reset();      
          nBinsX   = displayedHPDHistogram.rootH1D->GetNbinsX();      
          value    = 0;
          
          double nEntries = displayedHPDHistogram.dimProxy->
              rootH1D()->GetEntries();
          
//            if (statOption == HPDGui::stat1D ||
//                statOption == HPDGui::stat1D2D) {
//              (*h1DIter).h1D->SetStats(kTRUE);
//            } else {
//              (*h1DIter).h1D->SetStats(kFALSE);
//            } // if statOption
          
          for (int i=0; i< nBinsX+1; ++i) {
            value = displayedHPDHistogram.dimProxy->
                rootH1D()->GetBinContent(i);
            displayedHPDHistogram.rootH1D->SetBinContent(i, value);
            
            value = displayedHPDHistogram.dimProxy->
                rootH1D()->GetBinError(i);
            displayedHPDHistogram.rootH1D->SetBinError(i, value);
          }

          displayedHPDHistogram.rootH1D->SetEntries(nEntries);
          
//            if (m_verbose > 0)
            std::cout << "DIM histo entries " << nEntries << 
                         " histo entries "    << 
                         displayedHPDHistogram.rootH1D->GetEntries() <<
                         std::endl;  
        }        
      } else  {
        m_histoOK = false;
        std::string hTitle = displayedHPDHistogram.rootH1D->GetTitle();      
//        if (m_verbose > 1)
          std::cout << "H1D not OK " << hTitle << std::endl;
//        HPDGui::Pause(); // pause updates
        std::string message = "1D histogram " + hTitle +
                              " not OK, stopping update";      
        statusBar()->message(message.c_str());
        return;      
      }  
    }

//  if (m_verbose > 2)
//    std::cout << "loop over 2D histograms and update if necessary" << std::endl;
    BOOST_FOREACH(HistogramH2D displayedH2DHistogram,
                  displayedH2DHistograms) {
      if (displayedH2DHistogram.dimProxy->serviceOK()) {        
        if (displayedH2DHistogram.dimProxy->serviceUpdated()) {  
          // histogram has received new data, need to update
          updateCanvas = true;
          displayedH2DHistogram.dimProxy->resetServiceUpdated(); // reset flag
          displayedH2DHistogram.rootH2D->Reset();      
          
          double nEntries = displayedH2DHistogram.dimProxy->
              rootH2D()->GetEntries();
          
  //        if (statOption == HPDGui::stat2D ||
  //            statOption == HPDGui::stat1D2D) {
  //          (*rootH2DIter).rootH2D->SetStats(kTRUE);
  //        } else {
  //          (*rootH2DIter).rootH2D->SetStats(kFALSE);
  //        } // if statOption
        
          nBinsX   = displayedH2DHistogram.rootH2D->GetNbinsX();
          nBinsY   = displayedH2DHistogram.rootH2D->GetNbinsY();      
          value    = 0.0;
        
          for (int i=0; i<= nBinsX+1; ++i) {
            for (int j=0; j<= nBinsY+1; ++j) {
              value = displayedH2DHistogram.dimProxy->rootH2D()->
                    GetBinContent(i, j);
              if (value == 0.0) value = 0.00001; // to show in nice colour
              displayedH2DHistogram.rootH2D->SetBinContent(i,j,value);
              
              value = displayedH2DHistogram.dimProxy->rootH2D()->
                  GetBinError(i, j);
              displayedH2DHistogram.rootH2D->SetBinError(i, j, value);
            }
          }        
          displayedH2DHistogram.rootH2D->SetEntries(nEntries);
        
//        if (m_verbose > 0)
          std::cout << "DIM histo entries " << nEntries << 
                       " histo entries "    <<
                       displayedH2DHistogram.rootH2D->GetEntries()  <<
                       std::endl;
        }
      } else {
        m_histoOK = false;
        std::string hTitle = displayedH2DHistogram.rootH2D->GetTitle();      
//        if (m_verbose > 1)
          std::cout << "H2D not OK " << hTitle << std::endl;
//        HPDGui::Pause(); // pause updates
        std::string message = "2D histogram " + hTitle +
                              " not OK, stopping update";
        statusBar()->message(message.c_str());
        return;      
      }
    }
 
    int padCounter = 1;
    if (updateCanvas) {
      m_mainPageRootCanvas->GetCanvas()->SetEditable(true);    
      BOOST_FOREACH(HistogramH2D displayedH2DHistogram,
                    displayedH2DHistograms) {
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(true);
  //      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
  //          SetFillColor(10);
  //      displayedH2DHistogram.rootH2D->SetStats(kFALSE);
    //    displayedH2DHistogram.rootH2D -> Draw(m_2DDrawOption.c_str());
       if (offsetData) {
         displayedH2DHistogram.rootH2D->
            Add(displayedH2DHistogram.rootOffsetH2D, -1.);
       }  
        displayedH2DHistogram.rootH2D->Draw("lego");
        ++padCounter;
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(false);
      }
    
      BOOST_FOREACH(HistogramH1D displayedH1DHistogram,
                    displayedH1DHistograms) {
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(true);
  //      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
  //          SetFillColor(10);
  //      displayedH1DHistogram.rootH1D->SetStats(kFALSE);
    //    (*h1DIter).rootH1D -> Draw(m_1DDrawOption.c_str());
       if (offsetData) {
         displayedH1DHistogram.rootH1D->
            Add(displayedH1DHistogram.rootOffsetH1D, -1.);
       }
        displayedH1DHistogram.rootH1D->Draw();
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(false);      
        ++padCounter;      
      }
    
      BOOST_FOREACH(HistogramH1D displayedHPDHistogram,
                    displayedHPDHistograms) {
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(true);
  //      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
  //          SetFillColor(10);;          
  //      displayedHPDHistogram.rootH1D->SetStats(kFALSE);
    //    (*hProfIter).rootH1D -> Draw(m_1DDrawOption.c_str());
       if (offsetData) {
         displayedHPDHistogram.rootH1D->
             Add(displayedHPDHistogram.rootOffsetH1D, -1.);
//            displayedHPDHistogram.rootH1D->Inspect();
//            displayedHPDHistogram.rootOffsetH1D->Inspect();
       }  
        displayedHPDHistogram.rootH1D->Draw();
        m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
            SetEditable(false);     
        ++padCounter;      
      }
      m_mainPageRootCanvas->Update();
    }
    m_mainPageRootCanvas->GetCanvas()->SetEditable(false);       
  }
}

void PageEditorMainWindow::cloneCanvas()
{
  std::cout << "Hi." << std::endl;
  boost::shared_ptr<TCanvas> clonedCanvas(new 
      (TCanvas)(m_mainPageRootCanvas->GetCanvas()->Clone("Investigate")));
  clonedCanvas->GetCanvas()->SetEditable(true);    
  clonedCanvas->GetCanvas()->Draw();
}

void PageEditorMainWindow::clearIntegrateToggle()
{
    if (clearIntegratePagePushButton->isOn()) {
    offsetData = true;
    bool   updateCanvas = false;
    
    int    nBinsX   = 0;
    int    nBinsY   = 0;
    double value    = 0;
 
   BOOST_FOREACH(HistogramH1D displayedH1DHistogram,
                 displayedH1DHistograms) {
//      if (displayedH1DHistogram.dimProxy->serviceOK()) {      
//        if (displayedH1DHistogram.dimProxy->serviceUpdated()) {
          updateCanvas = true;
//          displayedH1DHistogram.dimProxy->resetServiceUpdated();
          
//          dynamic_cast<TH1F*>
//          TH1F *hist_new=(TH1F*)hist_1->Clone();
          
//          displayedH1DHistogram.rootOffsetH1D = dynamic_cast<TH1F*>(
//            displayedH1DHistogram.rootH1D->Clone());
          double nEntries = displayedH1DHistogram.rootH1D->GetEntries();
            
            for (int i=0; i< nBinsX+1; ++i) {
              value = displayedH1DHistogram.rootH1D->GetBinContent(i);
              displayedH1DHistogram.rootOffsetH1D->SetBinContent(i, value);
              
              value = displayedH1DHistogram.rootH1D->GetBinError(i);
              displayedH1DHistogram.rootOffsetH1D->SetBinError(i, value);
            }
  
            displayedH1DHistogram.rootOffsetH1D->SetEntries(nEntries);
    }

   BOOST_FOREACH(HistogramH1D displayedHPDHistogram,
                 displayedHPDHistograms) {
//      if (displayedHPDHistogram.dimProxy->serviceOK()) {      
//        if (displayedHPDHistogram.dimProxy->serviceUpdated()) {
          updateCanvas = true;          
          displayedHPDHistogram.rootOffsetH1D = dynamic_cast<TH1F*>(
            displayedHPDHistogram.rootH1D->Clone());
    }


//  if (m_verbose > 2)
//    std::cout << "loop over 2D histograms and update if necessary" << std::endl;
    BOOST_FOREACH(HistogramH2D displayedH2DHistogram,
                  displayedH2DHistograms) {
//      if (displayedH2DHistogram.dimProxy->serviceOK()) {        
//        if (displayedH2DHistogram.dimProxy->serviceUpdated()) {  
          // histogram has received new data, need to update
          updateCanvas = true;
//          displayedH2DHistogram.dimProxy->resetServiceUpdated(); // reset flag
//                    
//          displayedH2DHistogram.rootOffsetH2D = dynamic_cast<TH2F*>(
//            displayedH2DHistogram.rootH2D->Clone());
          double nEntries = displayedH2DHistogram.rootH2D->GetEntries();
                    
            nBinsX   = displayedH2DHistogram.rootH2D->GetNbinsX();
            nBinsY   = displayedH2DHistogram.rootH2D->GetNbinsY();
            value    = 0.0;

            for (int i=0; i<= nBinsX+1; ++i) {
              for (int j=0; j<= nBinsY+1; ++j) {
                value = displayedH2DHistogram.rootH2D->GetBinContent(i,j);
                if (0.0 == value) value = 0.00001; // to show in nice colour
                displayedH2DHistogram.rootOffsetH2D->SetBinContent(i, j,
                                                                   value);                
                value = displayedH2DHistogram.rootH2D->GetBinError(i, j);
                displayedH2DHistogram.rootOffsetH2D->SetBinError(i, j,
                                                                 value);
              } 
            }
            
            displayedH2DHistogram.rootOffsetH2D->SetEntries(nEntries);
 
    }
    clearIntegratePagePushButton->setText(tr("Integrate"));
  } else {
    //std::cout << "isOn." << std::endl;
    offsetData = false;
    clearIntegratePagePushButton->setText(tr("Clear"));  
  }
}

void PageEditorMainWindow::connectToDatabase(const QString &dbPassword, const QString &dbUsername, const QString &dbName)
{
  try {
    if (histogramDB) delete histogramDB; //TODO:
    histogramDB = new OnlineHistDB(dbPassword, dbUsername, dbName);
//  if (m_verbose > 1)
    std::cout << "Successfully connected to OnlineHistDB." << std::endl;

    showHistogramDatabaseTools();
    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
    m_connectedToHistogramDatabase = true;    
 
    //TODO: refactor refresh* methods to use only 1 DB readout...
    refreshPagesListView();
    refreshHistogramListView();
    refreshFolderNameComboBox();
    refreshPageNameComboBox();
    
  } catch (SQLException sqlException) {
    // TODO: add error logging backend - MsgStream?
//  if (m_verbose > 1)
    std::cout << sqlException.getMessage();
    switch(QMessageBox::warning(this, "LHCb Presenter Page Editor",
                  tr("Could not connect to the OnlineHistDB server:\n\n%1\n"
                   "This program can't function correctly "
                   "without the server.\n\n").arg(sqlException.getMessage()),
                  QMessageBox::Ok, QMessageBox::NoButton)) {
      case QMessageBox::Ok: // The user clicked the Ok/pressed Enter
          // try again
        break;
      default:
        break;
    }
    
    hideHistogramDatabaseTools();    
    statusBar()->message(sqlException.getMessage());
    m_connectedToHistogramDatabase = false;
  }
}

// TODO: merge the following 2 into 1 function...
void PageEditorMainWindow::hideHistogramDatabaseTools()
{
  histoDBHistogramsView->hide();
  histoDBPagesView->hide();
  layoutModeButtonGroup->hide();
  savePagePushButton->hide();
  folderNameComboBox->hide();
  histgramFilterNameTextLabel->hide();
  histogramFilterComboBox->hide();
  pageNameComboBox->hide();
  folderNameTextLabel->hide();
  pageNameTextLabel->hide();
}

void PageEditorMainWindow::showHistogramDatabaseTools()
{
  histoDBHistogramsView->show();
  histoDBPagesView->show();
  layoutModeButtonGroup->show();
  savePagePushButton->show();
  folderNameComboBox->show();
  histgramFilterNameTextLabel->show();
  histogramFilterComboBox->show();
  pageNameComboBox->show();
  folderNameTextLabel->show();
  pageNameTextLabel->show();
}

void PageEditorMainWindow::clearCanvas()
{
//  m_mainPageRootCanvas->GetCanvas()->SetEditable(true);
//  m_mainPageRootCanvas->GetCanvas()->Divide(1,1);
//  m_mainPageRootCanvas->Clear();
  }

void PageEditorMainWindow::addSelectedHistogramsFromDIMToPage()
{
  int    nBinsX = 0;
  int    nBinsY = 0;
  double xMin   = 0;
  double xMax   = 0;
  double yMin   = 0;
  double yMax   = 0;
  
  double value  = 0;
  
  int m_refreshTimeHisto = 1;
  int m_verbose = 1;
  
  int iHisto = 1;

  m_mainPageRootCanvas->GetCanvas()->SetEditable(true);
  m_mainPageRootCanvas->GetCanvas()->Divide(1,1);
  m_mainPageRootCanvas->Clear();
  
  
//  QPtrList<QListViewItem> lst;
//  QListViewItemIterator itS(histoDBHistogramsView);
//  while (itS.current()) {
//    if (itS.current()->isSelected()) {
//      lst.append(itS.current());
//  ++itS;
//  }
     
  
  QListViewItemIterator it(dimServicesView);  
  while (it.current()) {
    QListViewItem *item = it.current();
    if (item->isSelected()) {

//      OnlineHistogram *h1 = histogramDB->getHistogram(item->text(5));
      
      std::string serviceNameFQ = item->text(5);

      std::cout << serviceNameFQ << std::endl;                         
      
      new QListViewItem(pageHistogramsView, item->text(0), item->text(3));

      std::string serviceType = item->text(3);   
//    if (m_verbose > 1)
        std::cout << "Reconstructing DIM SVC name: " << serviceNameFQ <<
                     std::endl;
                     
      iHisto++;
      std::string histoID = "histo_" +
                            boost::lexical_cast<std::string>(iHisto);                     
                                   
      if ("H1D" == serviceType.substr(0,3)) {
        HistogramH1D histogramH1D;
        // TODO: sharedptr: place delete...
        histogramH1D.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);        

        if (histogramH1D.dimProxy->serviceOK()) {    
          TH1 *tmp_h1D = histogramH1D.dimProxy->rootH1D();
          if (tmp_h1D) {
            nBinsX             = tmp_h1D->GetNbinsX();
            xMin               = tmp_h1D->GetXaxis()->GetXmin();
            xMax               = tmp_h1D->GetXaxis()->GetXmax();
            std::string hTitle = item->text(0);
//            tmp_h1D->GetTitle();
//            if (m_verbose > 3) {
              std::cout << " nBinsX " << nBinsX <<
                           " xMin "   << xMin   <<
                           " xMax "   << xMax   << std::endl;
//            }

            // book the 1D histo which is to be displayed
            // TODO: sharedptr - place delete
            histogramH1D.rootH1D = new TH1F(histoID.c_str(), hTitle.c_str(),
                                            nBinsX, xMin, xMax);
            histogramH1D.rootOffsetH1D = new TH1F("", "",
                                                  nBinsX, xMin, xMax);
            histogramH1D.rootH1D->SetMarkerStyle(22);
            histogramH1D.rootH1D->SetMarkerSize(0.9);
            displayedH1DHistograms.push_back(histogramH1D);
            
            // Fill w some data:
            nBinsX = histogramH1D.rootH1D->GetNbinsX();      
            value  = 0;

            double nEntries = histogramH1D.dimProxy->rootH1D()->GetEntries();
            
            for (int i=0; i< nBinsX+1; ++i) {
              value = histogramH1D.dimProxy->rootH1D()->GetBinContent(i);
              histogramH1D.rootH1D->SetBinContent(i, value);
              
              value = histogramH1D.dimProxy->rootH1D()->GetBinError(i);
              histogramH1D.rootH1D->SetBinError(i, value);
            }
  
            histogramH1D.rootH1D->SetEntries(nEntries);
            
            if (m_verbose > 0)
              std::cout << "DIM histo entries " << nEntries
                        << " histo entries "    <<
                           histogramH1D.rootH1D->GetEntries() << std::endl;            
          }
        }
      } else if ("HPD" == serviceType.substr(0,3)) {
        HistogramH1D histoProfile;
        // TODO: sharedptr - place delete     
        histoProfile.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);
        if (histoProfile.dimProxy->serviceOK()) {    
          TH1 *tmp_h1D = histoProfile.dimProxy->rootHPD();
          if (tmp_h1D) {
            nBinsX             = tmp_h1D->GetNbinsX();
            xMin               = tmp_h1D->GetXaxis()->GetXmin();
            xMax               = tmp_h1D->GetXaxis()->GetXmax();
            std::string hTitle = item->text(0);
//             tmp_h1D->GetTitle();
//          if (m_verbose > 3)
              std::cout << " nBinsX " << nBinsX << 
                           " xMin "   << xMin   <<
                           " xMax "   << xMax   << std::endl;

            // book the 1D histo which is to be displayed
            // TODO: sharedptr - place delete    
            histoProfile.rootH1D = new TH1F(histoID.c_str(),
                                            hTitle.c_str(),
                                            nBinsX, xMin, xMax);
            histoProfile.rootOffsetH1D = new TH1F("",
                                                  "",
                                                  nBinsX, xMin, xMax);
            histoProfile.rootH1D->SetMarkerStyle(22);
            histoProfile.rootH1D->SetMarkerSize(0.9);
            displayedHPDHistograms.push_back(histoProfile);
          }
        }
        
      } else if ("H2D" == serviceType.substr(0,3)) {              
        HistogramH2D histogramH2D;
        // TODO: sharedptr - place delete 
        histogramH2D.dimProxy = new DimProxy(serviceNameFQ,
                                             m_refreshTimeHisto,
                                             m_verbose);
        if (histogramH2D.dimProxy->serviceOK()) {
          TH2 *tmp_h2D = histogramH2D.dimProxy->rootH2D();
          if (tmp_h2D) {
            // dimProxy gives back a NULL pointer if something went wrong
            nBinsX             = tmp_h2D->GetNbinsX();
            nBinsY             = tmp_h2D->GetNbinsY();
            xMin               = tmp_h2D->GetXaxis()->GetXmin();
            xMax               = tmp_h2D->GetXaxis()->GetXmax();
            yMin               = tmp_h2D->GetYaxis()->GetXmin();
            yMax               = tmp_h2D->GetYaxis()->GetXmax();
            std::string hTitle = item->text(0);
//             tmp_h2D->GetTitle();            
//          if (m_verbose > 3)
            std::cout << " nBinsX " << nBinsX <<
                         " xMin "   << xMin   << " xMax " << xMax << 
                         " nBinsY " << nBinsY << " yMin " << yMin <<
                         " yMax "   << yMax   << std::endl;
            
            // book new TH2F - the one which is to be displayed
            // TODO: sharedptr - place delete       
            histogramH2D.rootH2D = new TH2F(histoID.c_str(), hTitle.c_str(), 
                                            nBinsX, xMin, xMax,
                                            nBinsY, yMin, yMax);
            histogramH2D.rootOffsetH2D = new TH2F("", "", 
                                            nBinsX, xMin, xMax,
                                            nBinsY, yMin, yMax);            
            displayedH2DHistograms.push_back(histogramH2D);
            
            // Fill with some data
            double nEntries = histogramH2D.dimProxy->rootH2D()->GetEntries();
                    
            nBinsX   = histogramH2D.rootH2D->GetNbinsX();
            nBinsY   = histogramH2D.rootH2D->GetNbinsY();      
            value    = 0.0;
                
            for (int i=0; i<= nBinsX+1; ++i) {
              for (int j=0; j<= nBinsY+1; ++j) {
                value = histogramH2D.dimProxy->rootH2D()->GetBinContent(i,j);
                if (0.0 == value) value = 0.00001; // to show in nice colour
                histogramH2D.rootH2D->SetBinContent(i, j, value);                
                value = histogramH2D.dimProxy->rootH2D()->GetBinError(i, j);
                histogramH2D.rootH2D->SetBinError(i, j, value);
              } 
            }
            
            histogramH2D.rootH2D->SetEntries(nEntries);
            
            if (m_verbose > 0)
              std::cout << "DIM histo entries " << nEntries
                        << " histo entries "    <<
                           histogramH2D.rootH2D->GetEntries() << std::endl;            
          }
        }
      }        
    }
    ++it;
  }
             
// setup the Canvas and draw the (empty) histograms
// choose a setup where #rows = #columns
// may lead to some unsused parts of the canvas, but 
// probably the easiest/best setup if no further
// knowledge about the contents displayed
              
  // #pads needed to display all information
  int nPads            = pageHistogramsView->childCount();  
  int m_nCanvasRows    = (int) ceil(sqrt((double)nPads));
  int m_nCanvasColumns = m_nCanvasRows;  

  m_mainPageRootCanvas->GetCanvas()->Divide(m_nCanvasColumns,m_nCanvasRows);
  int padCounter = 1;
  // now loop over all pads to set basic properties
  for (int i = 1; i<= m_nCanvasRows; ++i) {
    for (int j = 1; j <= m_nCanvasColumns; ++j) {
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetFillColor(10);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->Draw();      
      // m_mainPageRootCanvas->GetPad(padCounter)->SetTopMargin(0.5);
      // m_mainPageRootCanvas->GetPad(padCounter)->SetBottomMargin(0.5);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetLeftMargin(0.15);
      m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
          SetRightMargin(0.15);
      ++padCounter;      
    } 
  }
  
  // now loop over all histograms and draw them: 
  // order: 2D - 1D - Counters

  padCounter = 1;
  BOOST_FOREACH(HistogramH2D displayedH2DHistogram, displayedH2DHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);
//    displayedH2DHistogram.rootH2D->SetStats(kFALSE);
//    (*h2DIter).rootH2D -> Draw(m_2DDrawOption.c_str());
    displayedH2DHistogram.rootH2D->Draw("lego");
    ++padCounter;
  }

  BOOST_FOREACH(HistogramH1D displayedH1DHistogram, displayedH1DHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);    
//    displayedH1DHistogram.rootH1D->SetStats(kFALSE);
//    (*h1DIter).rootH1D -> Draw(m_1DDrawOption.c_str());      
    displayedH1DHistogram.rootH1D->Draw();
    ++padCounter;      
  }

  BOOST_FOREACH(HistogramH1D displayedHPDHistogram, displayedHPDHistograms) {
    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->cd();
//    m_mainPageRootCanvas->GetCanvas()->GetPad(padCounter)->
//        SetFillColor(10);    
//    displayedHPDHistogram.rootH1D->SetStats(kFALSE);
//    (*hProfIter).rootH1D -> Draw(m_1DDrawOption.c_str());
    displayedHPDHistogram.rootH1D->Draw();      
    ++padCounter;      
  } 
  m_mainPageRootCanvas->Update();
  m_mainPageRootCanvas->GetCanvas()->SetEditable(false);
  statusBar()->message(tr("Canvas ready."));
}
