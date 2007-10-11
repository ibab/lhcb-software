#ifndef EDITORFRAME_H_
#define EDITORFRAME_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class vector;

class TGTab;
class TRootEmbeddedCanvas;
class TCanvas;
class TGListTree;
class TGListTreeItem;
class TGPopupMenu;
class TGComboBox;
class TPad;
class TObject;
class TList;
class TMap;

class DbRootHist;
class TH1;

class DimBrowser;
class OnlineHistDB;
class OnlineHistogram;
class PresenterMainFrame;

enum Command
    {
      X_ENIGMA_COMMAND,
      M_AddDimToDB_COMMAND,
      M_AddDimToPage_COMMAND,
      M_AddDBHistoToPage_COMMAND,
      M_DeleteDBHisto_COMMAND,
      M_SetHistoPropertiesInDB_COMMAND,
      M_RefreshHistoDBListTree_COMMAND,
      M_RefreshHistoDIMListTree_COMMAND,
      M_RefreshDBPagesListTree_COMMAND,
      M_LoadPage_COMMAND
    };

class EditorFrame : public TGCompositeFrame
{
  public:
    EditorFrame(TGWindow* p, UInt_t w, UInt_t h, DimBrowser& dim, MsgLevel v);
    virtual ~EditorFrame();
    
    // slots
    void refreshHistoDBListTree(FilterCriteria filterCriteria);
    void refreshPagesDBListTree();
    void refreshDimSvcListTree();
    void hideDBTools();
    void showDBTools();
    
    void enableClearHistos();
    void disableClearHistos();
    
    void enablePageRefresh();
    void disablePageRefresh();
    
    void enableLoadPage();
    
    void autoCanvasLayout();

    void clickedDimTreeElement(TGListTreeItem* entry, MouseButton btn, Int_t x, Int_t y);
    void clickedHistoDBTreeElement(TGListTreeItem* entry, MouseButton btn, Int_t x, Int_t y);
    void clickedPageTreeElement(TGListTreeItem* entry, MouseButton btn, Int_t x, Int_t y);
    
    TGPopupMenu* getDimContextMenu() const { return m_dimContextMenu; }
    void handleCommand(Command cmd);
    void addDimSvcToHistoDB();
    void addDimSvcToPage();
    void addDbHistoToPage();
    void setHistoPropertiesInDB();
    void deleteHistoFromDB();
    void loadPage();
    void setHistoParFromDB(TH1* histogram, OnlineHistogram* onlineHistogram);
    void refreshPage();
    bool paintHist(DbRootHist* histogram);
    
    void clearHistosOnPage();
    void EventInfo(int event, int px, int py, TObject *selected);
    
    TCanvas* editorCanvas;
    TRootEmbeddedCanvas* editorEmbCanvas;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::const_iterator m_DbHistosOnPageIt;

  private:
    EditorFrame (const EditorFrame & );
    EditorFrame & operator= (const EditorFrame &);  
    
    void buildGUI();

    MsgLevel             m_verbosity;
    PresenterMainFrame*  m_mainFrame; 
    DimBrowser*          m_dimBrowser;    
    OnlineHistDB*        m_histogramDB;

    TGCompositeFrame*    m_editorCanvasMainFrame;
    
    int m_mainCanvasWidth;
    int m_mainCanvasHeight;
    int m_histoPadOffset;
    
    TString m_drawOption; // bulk DO
    TString m_1ddrawOption; // bulk DO
    TString m_2ddrawOption; // bulk DO
    
    TGListTree*          m_databaseHistogramTreeList;
    TGListTree*          m_pagesFromHistoDBListTree;
    TGListTree*          m_dimSvcListTree;
    TGPopupMenu*         m_dimContextMenu;
    TGPopupMenu*         m_histoDBContextMenu;
    TGPopupMenu*         m_pagesContextMenu;
    int                  m_msgBoxReturnCode;
    std::string          m_message;
    TGViewPort*          m_histoDBCanvasViewPort;
    TGViewPort*          m_pagesFromHistoDBViewPort;
    TGCanvas*            m_dimSvcListTreeContainterCanvas;
    TGCanvas*            m_histoDBCanvas;
    TGVerticalFrame*     m_histoDBFilterFrame;
    TGComboBox*          m_histoDBFilterComboBox;
 
    std::vector<TString>      m_knownDimServices;
    std::vector<TString>::const_iterator m_knownDimServicesIt;
    
    std::vector<OnlineHistogram*>      m_onlineHistosOnPage;
    std::vector<OnlineHistogram*>::const_iterator m_onlineHistosOnPageIt;
  
  ClassDef(EditorFrame, 0)
};

#endif /*EDITORFRAME_H_*/
