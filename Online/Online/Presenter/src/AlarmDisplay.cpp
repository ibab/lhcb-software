// $Id: AlarmDisplay.cpp,v 1.4 2010-10-19 13:25:41 ggiacomo Exp $

#include <vector>
#include <iostream>
#include <TGListTree.h>
#include <TGMsgBox.h>

#include "AlarmDisplay.h"
#include "PresenterMainFrame.h"
#include "PresenterInformation.h"
#include "PageDescriptionTextView.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OMAMessage.h"
#include "Elog.h"
#include "ShiftDB.h"

using namespace pres;

//-----------------------------------------------------------------------------
// Implementation file for class : AlarmDisplay
//
// 2010-05-24 : Peter Somogyi / Giacomo GRAZIANI
//-----------------------------------------------------------------------------

void AlarmDisplay::listAlarmsFromHistogramDB() {
  if ( m_enabled && m_mainFrame->isConnectedToHistogramDB() && 0 != m_listView) {

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
          TGListTreeItem* thisDet=NULL;
          std::string nodeName(message->msgtext());          
          if ( !message->isactive() ) {  
            TGListTreeItem* branch = archive;//message->isactive() ? treeRoot : archive;
            std::string subsys = message->concernedSystem();
            if (subsys.empty()) subsys = "OTHER";
            for (TGListTreeItem* detroot = branch->GetFirstChild(); detroot != NULL ; detroot=detroot->GetNextSibling()) {
              if (std::string(detroot->GetText()) == subsys) {
                thisDet = detroot;
                break;
              }
            }
            if (!thisDet) {
              thisDet=m_listView->AddItem(branch, subsys.c_str());
            }
          } else {
            thisDet = treeRoot;
            nodeName = std::string(message->concernedSystem()) + ":" + nodeName;
          }
          
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
  m_lastMsgId = -1;
}

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
      } else {
        m_lastMsgId = msgId;
        //== save context
        bool globalHistoryByRunFlag =  m_presenterInfo-> globalHistoryByRun();
        std::string previousSaveset = m_mainFrame->savesetFileName();
        const pres::PresenterMode prevPresenterMode = m_mainFrame->presenterMode();

        //== Set access parameters
        m_presenterInfo->setGlobalHistoryByRun(false) ;
        m_mainFrame->setPresenterModeVariable(History);
        m_mainFrame->setSavesetFileName(message.saveSet());
        m_mainFrame->setStatusBarText(message.saveSet().c_str(),2);

        //== Display it
        m_mainFrame->addHistoToPage( message.hIdentifier() );
        m_mainFrame->setDisplayModeVariable(Alarm);
        m_mainFrame->displaySimpleHistos();
        
        char header[100];
        sprintf( header, "%s message %3d", message.levelString(), msgId );
        m_mainFrame->setTopStatusBarText( header, 1);

        //== restore access parameters
        m_mainFrame->setPresenterModeVariable(prevPresenterMode);
        m_mainFrame->setSavesetFileName(previousSaveset); 
        m_presenterInfo->setGlobalHistoryByRun(globalHistoryByRunFlag) ;

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
  std::cout << "$$AlarmDisplay::InfoHandler: List of Analysis Alarms has changed, alarm refreshing enabled=" 
            << m_enabled <<std::endl;
  
  if ( m_enabled ) {
    m_mainFrame->reAccessPage();
  }
  std::cout << "$$AlarmDisplay::InforHandler teminated." << std::endl;
}


//=========================================================================
//  Clear the selected alarm
//=========================================================================
void AlarmDisplay::clearAlarm( ) {
  std::cout << "Clear alarm for message " << m_lastMsgId << std::endl;

  std::string dbPassword, dbUsername, dbName;
  m_mainFrame->getDatabaseWriter( dbPassword, dbUsername, dbName );
  std::cout << "Database " << dbName << " user " << dbUsername << " pass ******** " << std::endl;
  if ( dbName != "" ) {
    OnlineHistDB* writeDb = new OnlineHistDB( dbPassword, dbUsername, dbName );
  
    OMAMessage message( m_lastMsgId, *writeDb );
    if ( message.isactive () ) {
      std::cout << "Should clear alarm for " << message.hIdentifier() << std::endl;
      message.disable();
      message.store();
      writeDb->commit();
      std::cout << "Alarm cleared in the database" << message.hIdentifier() << std::endl;

      Elog myElog( m_mainFrame->logbookConfig(), 8080 );
      myElog.setCredential( "common", "Common!" );
      myElog.setLogbook( "Shift" );
      ShiftDB shiftdb ;
      std::string username =  shiftdb.getCurrentDataManager();
      myElog.addAttribute( "Author", username );
      std::string system = message.concernedSystem();
      if ( system.empty() ) system = "LHCb";
      myElog.addAttribute( "System", system );
      std::string body = "DM cleared Analysis message: " + std::string( message.msgtext() );
      myElog.addAttribute( "Run", "" );
      std::cout << "send to Elog " << std::endl;
      int number = myElog.submit( body );
      std::cout << "=== Elog produced entry " << number << std::endl;
    } else {
      std::cout << "Alarm is not active. Ignore" << std::endl;
    }
    delete writeDb;
  }
  listAlarmsFromHistogramDB();
}
