#ifndef EDITORFRAME_H_
#define EDITORFRAME_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class std::vector;

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
  M_DimCheckAllChildren_COMMAND,
  M_DimUnCheckAllChildren_COMMAND,
  M_DimCollapseAllChildren_COMMAND,
  M_DeleteDBHisto_COMMAND,
  M_DBHistoCheckAllChildren_COMMAND,
  M_DBHistoUnCheckAllChildren_COMMAND,
  M_DBHistoCollapseAllChildren_COMMAND,
  M_SetHistoPropertiesInDB_COMMAND,
  M_RefreshHistoDBListTree_COMMAND,
  M_RefreshHistoDIMListTree_COMMAND,
  M_RefreshDBPagesListTree_COMMAND,
  M_LoadPage_COMMAND,
  M_DeletePage_COMMAND,
  M_DeleteFolder_COMMAND
};

class EditorFrame : public TGCompositeFrame
{
  public:
    EditorFrame(TGWindow* p, UInt_t w, UInt_t h, DimBrowser& dim,
      MsgLevel v);
    virtual ~EditorFrame();

    // slots
    void refreshHistoDBListTree();
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
    void deleteSelectedHistoFromCanvas();

    void clickedDimTreeItem(TGListTreeItem* item, MouseButton btn, Int_t x,
      Int_t y);
    void clickedHistoDBTreeItem(TGListTreeItem* item, MouseButton btn,
      Int_t x, Int_t y);
    void clickedPageTreeItem(TGListTreeItem* item, MouseButton btn, Int_t x,
      Int_t y);

    TGPopupMenu* getDimContextMenu() const { return m_dimContextMenu; }
    void handleCommand(Command cmd);
    void addDimSvcToHistoDB();
    void addDimSvcToPage();
    void addDbHistoToPage();
    void dimCheckAllChildren();
    void dimUnCheckAllChildren();
    void dimCollapseAllChildren();

    void setHistoPropertiesInDB();
    void dbHistoCheckAllChildren();
    void dbHistoUnCheckAllChildren();
    void dbHistoCollapseAllChildren();

    void deleteSelectedHistoFromDB();
    void loadSelectedPageFromDB();
    void deleteSelectedPageFromDB();
    void deleteSelectedFolderFromDB();
    void setHistoParFromDB(TH1* histogram, OnlineHistogram* onlineHistogram);
    void refreshPage();
    void paintHist(DbRootHist* histogram);

    void clearHistosOnPage();
    void EventInfo(int event, int px, int py, TObject *selected);

    TCanvas* editorCanvas;
    TRootEmbeddedCanvas* editorEmbCanvas;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::iterator m_DbHistosOnPageIt;

  private:
    EditorFrame (const EditorFrame & );
    EditorFrame & operator= (const EditorFrame &);

    void buildGUI();
    std::string convDimToHistoID(std::string dimSvcName);

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

    std::vector<std::string>      m_knownDimServices;
    std::vector<std::string>::const_iterator m_knownDimServicesIt;

    std::vector<OnlineHistogram*>      m_onlineHistosOnPage;
    std::vector<OnlineHistogram*>::const_iterator m_onlineHistosOnPageIt;

    std::vector<std::string>      m_histogramTypes;
    std::vector<std::string>::const_iterator m_histogramType;

    ClassDef(EditorFrame, 0)
};

#endif /*EDITORFRAME_H_*/
