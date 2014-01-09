// $Id: CreateTrendingHistogramDialog.cpp,v 1.4 2010-10-01 12:52:26 frankb Exp $

#include <iostream>

// ROOT
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGComboBox.h>

// Boost
#include <boost/filesystem.hpp>

// Online
#include "Trending/ITrendingTool.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistDBEnv.h"

// This class
#include "CreateTrendingHistogramDialog.h"

ClassImp(CreateTrendingHistogramDialog)

// acces to trending tool
namespace PresenterGaudi {
  extern ITrendingTool * trendingTool;
}
//===========================================================================
// Constructor
//===========================================================================
CreateTrendingHistogramDialog::CreateTrendingHistogramDialog( const TGWindow * p ,
                                                              const TGWindow * main ,
                                                              OnlineHistDB * histdb,
                                                              std::string partition,
                                                              std::string& result ) :
  TGTransientFrame( p , main , 800 , 250 ) ,
  m_rootFrame( p ) ,
  m_fileEntry( 0 ) ,
  m_histdb( histdb ),
  m_partition( partition ),
  m_output( &result )
{
  SetCleanup(kDeepCleanup);
  Connect( "CloseWindow()" , "CreateTrendingHistogramDialog" , this ,
           "DontCallClose()" ) ;
  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputSystemModal);
  build();
  MapWindow();
}


//===========================================================================
// Destructor
//===========================================================================
CreateTrendingHistogramDialog::~CreateTrendingHistogramDialog( ) {
  Cleanup();
}

//===========================================================================
// OK button pressed
//===========================================================================
void CreateTrendingHistogramDialog::ok() {

  // check if this is a valid trending file
  bool result = false ;

  std::string fileName ( m_fileEntry -> GetText() ) ;
  std::string fileNamePart = m_partition + "_" + fileName;
  result = PresenterGaudi::trendingTool -> openRead( fileNamePart ) ;

  std::string ratio ( m_tagEntry->GetText() );

  if ( ! result ) {
    std::cout << "Error opening file " << fileNamePart << std::endl;
    *m_output = "The selected file '"+fileNamePart+"' gave error in TrendingTool::openRead";
    m_fileEntry -> SetText( "" ) ;
  } else {
    std::vector< std::string > tags ;
    result = PresenterGaudi::trendingTool -> tags( tags ) ;
    PresenterGaudi::trendingTool->closeFile();
    if ( ( ! result ) || ( tags.empty() ) ) {
      std::cout << "Error getting tags from " << fileNamePart << std::endl;
      *m_output = "The selected file '"+fileNamePart+"'  does not contain valid tags";
      m_fileEntry -> SetText( "" ) ;
    } else {
      char message[100];
      while ( fileName.find( "/" ) != std::string::npos ) {
        fileName = fileName.substr( fileName.find("/")+1 );
      }
      bool ratioFound = false;
      std::vector< std::string >::iterator it ;
      int nbDeclared = 0;
      for ( it = tags.begin() ; it != tags.end() ; ++it ) {
        std::string tagName = *it;
        if ( tagName.size() < 80 ) {
          std::cout << ".. declaring '" << tagName << "' for " << fileName << std::endl;
          m_histdb->declareTrendingHistogram( tagName, fileName, tagName );
          if ( ratio != "" && tagName == ratio ) ratioFound = true;
          nbDeclared++;
        }
      }
      int nbRatio = 0;
      if ( ratioFound ) {
        for ( it = tags.begin() ; it != tags.end() ; ++it ) {
          if ( ratio != *it ) {
            std::string newTag = *it + "|" + ratio;
            if ( newTag.size() < 80 ) {
              m_histdb->declareTrendingHistogram( newTag, fileName, newTag );           
              nbRatio++;
            }
          }
        }
      }
      bool result = m_histdb -> commit() ;

      sprintf( message, "Committed %d for %d tags + %d ratios for trending file %s, status %d.", 
               nbDeclared, (int)tags.size(), nbRatio, fileName.c_str(), result );
      *m_output = std::string( message );
      std::cout << message << std::endl;
    }
  }
  CloseWindow();
}

//===========================================================================
// Build window with its different widgets
//===========================================================================
void CreateTrendingHistogramDialog::build() {
  SetLayoutBroken( true ) ;

  // main frame
  TGMainFrame *fMainFrame = new TGMainFrame(this,10,10,kMainFrame | kVerticalFrame);
  fMainFrame->SetLayoutBroken(kTRUE);

  // Cancel button
  TGTextButton *cancelButton = new TGTextButton(fMainFrame,"Cancel");
  cancelButton->SetTextJustify(36);
  cancelButton->Resize( 90 , 25 );
  fMainFrame->AddFrame(cancelButton,
                       new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  cancelButton->MoveResize(340 , 200 , 90 , 25 ) ;
  cancelButton -> Connect( "Clicked()" , "CreateTrendingHistogramDialog" ,
                           this , "CloseWindow()" ) ;

  if ( 0 == m_histdb ) {
    TGLabel * warningLabel = new TGLabel( fMainFrame , "No histogram DB available !" ) ;
    warningLabel -> SetTextJustify( 33 ) ;
    fMainFrame -> AddFrame( warningLabel , new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    warningLabel -> MoveResize( 15 , 65 , 185 , 20 ) ;
  } else if ( ! m_histdb -> canwrite() ) {
    TGLabel * warningLabel = new TGLabel( fMainFrame , "You must login as HISTWRITER first !" ) ;
    warningLabel -> SetTextJustify( 33 ) ;
    fMainFrame -> AddFrame( warningLabel , new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    warningLabel -> MoveResize( 15 , 65 , 250 , 20 ) ;
  } else {

    // File name
    std::string title = "Trending file: /hist/Trending/"+m_partition+"_";
    TGLabel *fileLabel = new TGLabel(fMainFrame, title.c_str());
    fileLabel->SetTextJustify(33);
    fMainFrame->AddFrame(fileLabel, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fileLabel->MoveResize( 15 , 65 , 250 , 20 ) ;

    // File entry
    m_fileEntry = new TGTextEntry(fMainFrame, new TGTextBuffer(15));
    m_fileEntry->SetMaxLength(4096);
    m_fileEntry->SetAlignment(kTextLeft);
    m_fileEntry->SetText("");
    m_fileEntry->Resize(150,m_fileEntry->GetDefaultHeight());
    fMainFrame->AddFrame(m_fileEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    m_fileEntry->MoveResize(260,65,150,22);

    TGLabel *fileLabel2 = new TGLabel(fMainFrame," .trend");
    fileLabel2->SetTextJustify(33);
    fMainFrame->AddFrame(fileLabel2, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fileLabel2->MoveResize( 420 , 65 , 50 , 20 ) ;


    // Ration name
    std::string title2 = "Create ratios of trends to variable (blank = not) ";
    TGLabel *fileLabel3 = new TGLabel(fMainFrame, title2.c_str());
    fileLabel3->SetTextJustify(33);
    fMainFrame->AddFrame(fileLabel3, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    fileLabel3->MoveResize( 15 , 105 , 250 , 20 ) ;

    // File entry
    m_tagEntry = new TGTextEntry(fMainFrame, new TGTextBuffer(15));
    m_tagEntry->SetMaxLength(80);
    m_tagEntry->SetAlignment(kTextLeft);
    m_tagEntry->SetText("");
    m_tagEntry->Resize(150,m_tagEntry->GetDefaultHeight());
    fMainFrame->AddFrame(m_tagEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    m_tagEntry->MoveResize(310,105,150,22);

    // OK button
    TGTextButton *OKButton = new TGTextButton(fMainFrame,"OK");
    OKButton->SetTextJustify(36);
    OKButton->Resize(90,25);
    fMainFrame->AddFrame(OKButton, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    OKButton->MoveResize(240,200,90,25);
    OKButton -> Connect( "Clicked()" , "CreateTrendingHistogramDialog" , this , "ok()" ) ;

  }
  AddFrame( fMainFrame , new TGLayoutHints( kLHintsExpandX | kLHintsExpandY ) ) ;

  fMainFrame -> MoveResize( 0 , 0 , 700 , 250 ) ;

  MapSubwindows();
  Resize(GetDefaultSize());
  MapWindow();
  Resize(500,250);
}

//===========================================================================
// Close window
//===========================================================================
void CreateTrendingHistogramDialog::CloseWindow() {
  DeleteWindow();
}
