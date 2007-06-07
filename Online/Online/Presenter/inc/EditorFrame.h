#ifndef EDITORFRAME_H_
#define EDITORFRAME_H_

#include <TGFrame.h>
#include "../inc/presenter.h"

using namespace pres;

class TGTab;
class TRootEmbeddedCanvas;

class DimBrowser;
class PresenterMainFrame;
class TGListTree;

class EditorFrame : public TGCompositeFrame
{
  public:
    EditorFrame(TGWindow *p, UInt_t w = 1, UInt_t h = 1, MsgLevel v = SILENT);
    virtual ~EditorFrame();
    
    // slots
    void refreshHistoDBListTree(FilterCriteria filterCriteria);
    void refreshPagesDBListTree();
    
  //  EditorFrame (const EditorFrame & );
  //  EditorFrame & operator= (const EditorFrame &);  
    
  private:
     MsgLevel            m_verbosity;
     PresenterMainFrame  *m_mainFrame;
     TGListTree          *m_databaseHistogramTreeList;
     TGListTree          *m_pagesFromHistoDBListTree;
     DimBrowser          *m_dimBrowser;
//     TRootEmbeddedCanvas *m_mainEmbCanvas;
//      TGTab               *m_mainTab;
      
//      TGCompositeFrame    *m_onlineTab;
//      TGCompositeFrame    *m_offlineTab;
//      
//      TGVerticalFrame     *fF1;
//      TGCompositeFrame    *m_mainCanvasFrame;
//      TGLayoutHints       *m_mainCanvasLayout;
  
  ClassDef(EditorFrame, 0)
};

#endif /*EDITORFRAME_H_*/
