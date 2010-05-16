#ifndef PAGESAVEDIALOG_H_
#define PAGESAVEDIALOG_H_

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include "presenter.h"

class PresenterMainFrame;
class TGComboBox;
class TGTextButton;
class OnlineHistDB;
class DbRootHist;
class TGTextBuffer;
class TGTextEntry;
class TGTextEdit;

class DatabasePagePathDialog : public TGTransientFrame
{
  public:
    DatabasePagePathDialog(PresenterMainFrame* gui, int width, int height, pres::MsgLevel v);
    virtual ~DatabasePagePathDialog();

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
    std::string     m_folderName;
    TGTextEntry*    m_pageNameTextEntry;
    std::string     m_pageName;
    TGTextEntry*    m_patternFileTextEntry;
    TGTextEdit*     m_pageDescriptionTextEditor;
    pres::MsgLevel  m_verbosity;
    int             m_msgBoxReturnCode;

    std::vector<DbRootHist*>      m_DbHistosOnPage;
    std::vector<DbRootHist*>::const_iterator m_DbHistosOnPageIt;

    ClassDef(DatabasePagePathDialog, 0)
};

#endif /*PAGESAVEDIALOG_H_*/
