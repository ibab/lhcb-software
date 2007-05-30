#ifndef EDITORFRAME_H_
#define EDITORFRAME_H_

#include <TGFrame.h>
#include "../inc/presenter.h"

using namespace pres;

class TRootEmbeddedCanvas;
class TGTab;

class EditorFrame : public TGCompositeFrame
{
public:
  EditorFrame(const TGWindow *p, UInt_t w = 1, UInt_t h = 1, MsgLevel v = SILENT);
  virtual ~EditorFrame();
  
//  EditorFrame (const EditorFrame & );
//  EditorFrame & operator= (const EditorFrame &);  
  
private:
   MsgLevel            m_verbosity;
   TRootEmbeddedCanvas *m_mainEmbCanvas;
    TGTab               *m_mainTab;
    
    TGCompositeFrame    *m_onlineTab;
    TGCompositeFrame    *m_offlineTab;
    
    TGVerticalFrame     *fF1;
    TGCompositeFrame    *m_mainCanvasFrame;
    TGLayoutHints       *m_mainCanvasLayout;
  
  ClassDef(EditorFrame, 0)
};

#endif /*EDITORFRAME_H_*/
