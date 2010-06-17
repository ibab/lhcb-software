// $Id: AlarmDisplay.cpp,v 1.2 2010-06-17 10:44:19 ggiacomo Exp $

#include <vector>
#include <iostream>
#include <TGListTree.h>
#include <TGMsgBox.h>

#include "AlarmDisplay.h"
#include "PresenterMainFrame.h"
#include "PageDescriptionTextView.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OMAMessage.h"

using namespace pres;

//-----------------------------------------------------------------------------
// Implementation file for class : AlarmDisplay
//
// 2010-05-24 : Peter Somogyi / Giacomo GRAZIANI
//-----------------------------------------------------------------------------

void AlarmDisplay::listAlarmsFromHistogramDB() {
  if (m_enabled && m_mainFrame->isConnectedToHistogramDB() && 0 != m_listView) {

    m_listView->UnmapWindow();

    TGListTreeItem* treeRoot = m_listView->GetFirstItem();
    m_listView->DeleteChildren(treeRoot);
    treeRoot->SetOpen(1);
    TGListTreeItem* archive=m_listView->AddItem(treeRoot, "archive");
    m_nAlarms=m_nWarning=m_nArchived=0; 
    m_alarmMessageIDs.clear();
    try {
      m_mainFrame->histogramDB()->getMessages(m_alarmMessageIDs);
      std::vector<int>::const_iterator alarmMessageIDsIt;
      for (alarmMessageIDsIt = m_alarmMessageIDs.begin();
           alarmMessageIDsIt != m_alarmMessageIDs.end();
           alarmMessageIDsIt++) {

        OMAMessage* message = new OMAMessage(*alarmMessageIDsIt, *(m_mainFrame->histogramDB()));
        if (false == message->isAbort()) {
          TGListTreeItem* branch = message->isactive() ? treeRoot : archive;
          std::string subsys = message->concernedSystem();
          if (subsys.empty()) subsys = "OTHER";
          TGListTreeItem* thisDet=NULL;
          for (TGListTreeItem* detroot = branch->GetFirstChild(); detroot != NULL ; detroot=detroot->GetNextSibling()) {
            if (std::string(detroot->GetText()) == subsys) {
              thisDet = detroot;
              break;
            }
          }
          if (!thisDet) {
            thisDet=m_listView->AddItem(branch, subsys.c_str());
          }
          
          std::string nodeName(message->msgtext());
          TGListTreeItem* treeNode = m_listView->AddItem(thisDet, nodeName.c_str());
          m_mainFrame->setTreeNodeType(treeNode, message->levelString());
          m_listView->SetCheckBox(treeNode, false);
          treeNode->SetUserData((void*)&(*alarmMessageIDsIt));
          if (message->isactive()) {
            if (message->level() == OMAMessage::ALARM) m_nAlarms++;
            if (message->level() == OMAMessage::WARNING) m_nWarning++;
          }
          else {
            m_nArchived++;
          }
        }
        delete message;
      }
    } catch (std::string sqlException) {
      if (m_mainFrame->verbosity() >= Verbose) { std::cout << sqlException << std::endl; }
      if (Batch != m_mainFrame->presenterMode() ) {
        m_mainFrame->setStatusBarText(sqlException.c_str());
        new TGMsgBox(m_mainFrame->GetClient()->GetRoot(), m_mainFrame , "Database Error",
                     Form("OnlineHistDB server:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
    m_listView->RenameItem(treeRoot, Form("%d Alarms %d Warnings %d archived", m_nAlarms, m_nWarning, m_nArchived));
    m_listView->SortChildren(treeRoot);
    m_listView->MapWindow();
  } else if (!(m_mainFrame->isConnectedToHistogramDB())) {
    std::string textmessage = "Alarms not read from Database.";
    m_mainFrame->setStatusBarText(textmessage.c_str(), 2);
    if (m_mainFrame->verbosity() >= Verbose) { std::cout << textmessage << std::endl; }
  }
  if (Batch != m_mainFrame->presenterMode()) {
    m_mainFrame->GetClient()->NeedRedraw(m_listView);
  }
};


void AlarmDisplay::loadSelectedAlarmFromDB(int msgId) {
  bool error=false;
  std::string errorText="Database sync error when loading analsyis message";
  if (m_enabled && m_mainFrame->isConnectedToHistogramDB()) {
    m_mainFrame->unclearHistosIfNeeded();
    m_mainFrame->removeHistogramsFromPage();

    try {
      OMAMessage message(msgId, *(m_mainFrame->histogramDB()));
      if (message.isAbort()) {
        error=true;
      }
      else {
        std::string previousSaveset = m_mainFrame->savesetFileName();
        m_mainFrame->setSavesetFileName(message.saveSet());
        
        m_mainFrame->setStatusBarText(message.saveSet().c_str(),2);
        const pres::PresenterMode prevPresenterMode = m_mainFrame->presenterMode();
        m_mainFrame->setPresenterModeVariable(History);
        m_mainFrame->addHistoToPage(message.hIdentifier(), separate);
        m_mainFrame->autoCanvasLayout();
        m_mainFrame->setDisplayModeVariable(Alarm);
        
        char header[100];
        sprintf( header, "%s message %3d", message.levelString(), msgId );
        m_mainFrame->setTopStatusBarText( header, 1);
        
        m_mainFrame->setPresenterModeVariable(prevPresenterMode);
        m_mainFrame->setSavesetFileName(previousSaveset); 
        
        m_mainFrame->pageDescriptionView()->Clear();
        std::string messageText = message.msgtext();
        messageText += "\n----------------------------------------------------\n";
        messageText += "Created on "; 
        messageText += message.humanTime(); 
        std::stringstream scount;
        scount << "Occured "<< message.noccur() <<" times, solved "<< message.nsolved()
               <<" times, retriggered "<<message.nretrig() <<" times\n";
        messageText += scount.str();
        if (message.isactive()) {
          messageText += "Last Seen on ";
        }
        else {
          messageText += "Disappeared on ";
        }
        messageText += message.humanlastTime(); 
        messageText += "Analysis ";
        messageText += message.ananame();
        
        messageText += "\nSaveset is ";
        messageText += message.saveSet();
        
        m_mainFrame->pageDescriptionView()->LoadBuffer(messageText.c_str());
        m_mainFrame->pageDescriptionView()->DataChanged();
        
        if (m_mainFrame->referencesOverlayed() ) {
          m_mainFrame->enableReferenceOverlay();
        } else {
          m_mainFrame->disableReferenceOverlay();
        }
      }
    } catch (std::string sqlException) {
      if (m_mainFrame->verbosity() >= Verbose) { std::cout << sqlException << std::endl; }
      m_mainFrame->setStatusBarText(sqlException.c_str());
      errorText += Form(":\n%s\n",sqlException.c_str());
      error=true;
    }
    if (error && Batch != m_mainFrame->presenterMode() ) {
      new TGMsgBox(m_mainFrame->GetClient()->GetRoot(), m_mainFrame , "Database Error",
                   errorText.c_str(),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  }
}


// called every time a new alarm/warning is published or cleared
void AlarmDisplay::infoHandler() {  
  if (m_mainFrame->verbosity() >= Verbose) { 
    std::cout << "List of Analysis Alarms has changed, alarm refreshing enabled=" << 
      m_enabled <<std::endl;
  }
  if(m_enabled) {
    std::string prevstatusText;
    if (m_mainFrame->displayMode() == Alarm) { // clear page if an alarm was displayed
      m_mainFrame->removeHistogramsFromPage();
      prevstatusText ="List of Analysis Alarms has been updated";
    }
    else {
      prevstatusText = m_mainFrame->getStatusBarText(2);
    }
    m_mainFrame->setStatusBarText("Analysis Alarms have changed! reloading them...", 2);
    sleep(3); // give time to analysis to end
    listAlarmsFromHistogramDB();
    m_mainFrame->histogramDB()->refresh();
    m_mainFrame->clearAlarmPages();
    m_mainFrame->setStatusBarText(prevstatusText.c_str(),2);
  }
}

