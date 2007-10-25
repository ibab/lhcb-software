#ifndef PAGESAVEDIALOG_H_
#define PAGESAVEDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

using namespace pres;

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class OnlineHistDB;
class DbRootHist;
class TGTextBuffer;
class TGTextEntry;
class std::vector;

class PageSaveDialog : public TGTransientFrame
{
  public:
    PageSaveDialog(PresenterMainFrame* gui, Int_t Width, Int_t Height,
      MsgLevel v);
    virtual ~PageSaveDialog();

    void build();
    void ok();
    void updateTextFields(TGListTreeItem*, Int_t, Int_t, Int_t);
    void setOkButton();

  private:
    PresenterMainFrame* m_mainFrame;
    TGListTree* m_pageFolderListTree;
    TGTextButton *m_okButton;
    TGTextEntry *folderNameTextEntry;
    TGTextEntry *pageNameTextEntry;
    MsgLevel            m_verbosity;
    int                 m_msgBoxReturnCode;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::const_iterator m_DbHistosOnPageIt;

    ClassDef(PageSaveDialog, 0)
};

#endif /*PAGESAVEDIALOG_H_*/
