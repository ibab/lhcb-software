#ifndef PAGESAVEDIALOG_H_
#define PAGESAVEDIALOG_H_

#include <TGFrame.h>
#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class OnlineHistDB;
class DbRootHist;
class TGTextBuffer;
class TGTextEntry;
class TGTextEdit;

class PageSaveDialog : public TGTransientFrame
{
  public:
    PageSaveDialog(PresenterMainFrame* gui, int width, int height, pres::MsgLevel v);
    virtual ~PageSaveDialog();

    void build();
    void ok();
    void updateTextFields(TGListTreeItem* node, int, int, int);
    void setOkButton();
    void CloseWindow();

  private:
    PresenterMainFrame* m_mainFrame;
    TGListTree*     m_pageFolderListTree;
    TGTextButton*   m_okButton;
    TGTextButton*   m_cancelButton;
    TGTextEntry*    m_folderNameTextEntry;
    TGTextEntry*    m_pageNameTextEntry;
    TGTextEntry*    m_patternFileTextEntry;
    TGTextEdit*     m_pageDescriptionTextEditor;
    pres::MsgLevel  m_verbosity;
    int             m_msgBoxReturnCode;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::const_iterator m_DbHistosOnPageIt;

    ClassDef(PageSaveDialog, 0)
};

#endif /*PAGESAVEDIALOG_H_*/
