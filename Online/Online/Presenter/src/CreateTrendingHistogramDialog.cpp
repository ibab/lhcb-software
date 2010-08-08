// $Id: CreateTrendingHistogramDialog.cpp,v 1.2 2010-08-08 15:37:20 robbep Exp $

#include <iostream>

// ROOT
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGButton.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TGComboBox.h>
#ifdef WIN32
#pragma warning( pop )
#endif

// Boost
#include <boost/filesystem.hpp>

// Online
#include "Trending/ITrendingTool.h"
#include "OnlineHistDB/OnlineHistDB.h" 
#include "OnlineHistDB/OnlineHistDBEnv.h"

// This class
#include "CreateTrendingHistogramDialog.h"

ClassImp( CreateTrendingHistogramDialog ) 

// acces to trending tool
namespace PresenterGaudi {
  extern ITrendingTool * trendingTool ;
};
//===========================================================================
// Constructor
//===========================================================================
CreateTrendingHistogramDialog::CreateTrendingHistogramDialog( const TGWindow * p , 
							      const TGWindow * main , 
							      OnlineHistDB * histdb ) :
  TGTransientFrame( p , main , 500 , 250 ) , 
  m_rootFrame( p ) , 
  m_nameEntry( 0 ) ,
  m_fileEntry( 0 ) ,
  m_tagEntry( 0 ) ,
  m_histdb( histdb ) {
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
  if ( 0 != m_tagEntry ) {
    TGTextLBEntry * tag_entry = 
      dynamic_cast< TGTextLBEntry * >( m_tagEntry -> GetSelectedEntry() ) ;
    
    if ( 0 != tag_entry ) { 
      std::string histoName( m_nameEntry -> GetText() ) ;
      std::string tagName  ( tag_entry -> GetText() -> Data() ) ;
      std::string fileName ( m_fileEntry -> GetText() ) ;

      // task name = histo name
      // algorithm name = file name
      // histogram name = tag name
      m_histdb -> declareHistogram( histoName , tagName , fileName , 
				    OnlineHistDBEnv::TRE ) ;
      bool result = m_histdb -> commit() ;
      int retCode ;
      if ( result ) {    
	new TGMsgBox( m_rootFrame , this , "Success" ,
		      "New histogram created in HistDB" ,
		      kMBIconExclamation, kMBOk, &retCode);
      } else {
	new TGMsgBox( m_rootFrame , this , "Failed" ,
		      "Cannot create the new histogram in HistDB" ,
		      kMBIconExclamation, kMBOk, &retCode);	
      }
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
  TGMainFrame *fMainFrame = 
    new TGMainFrame(this,10,10,kMainFrame | kVerticalFrame);
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
    TGLabel * warningLabel = new TGLabel( fMainFrame , 
					  "No histogram DB available !" ) ;
    warningLabel -> SetTextJustify( 33 ) ;
    fMainFrame -> AddFrame( warningLabel , 
			    new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    warningLabel -> MoveResize( 15 , 65 , 185 , 20 ) ;
  } 
  
  
  else if ( 0 != m_histdb ) {
    if ( ! m_histdb -> canwrite() ) {
      TGLabel * warningLabel = new TGLabel( fMainFrame , 
					    "You must login as HISTWRITER first !" ) ;
      warningLabel -> SetTextJustify( 33 ) ;
      fMainFrame -> AddFrame( warningLabel , 
			      new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      warningLabel -> MoveResize( 15 , 65 , 250 , 20 ) ;
      
    } else {
      // Name of histogram label
      TGLabel *nameLabel = new TGLabel(fMainFrame,"Name of the histogram:");
      nameLabel->SetTextJustify(33);
      fMainFrame->AddFrame(nameLabel, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      nameLabel->MoveResize( 15 , 25 , 185 , 20 ) ;
      
      // File name
      TGLabel *fileLabel = new TGLabel(fMainFrame,"Name of the trending file: ");
      fileLabel->SetTextJustify(33);
      fMainFrame->AddFrame(fileLabel, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      fileLabel->MoveResize( 15 , 65 , 185 , 20 ) ;
      
      // Button to open the file browser
      TGTextButton *fileButton = new TGTextButton( fMainFrame , "..." ) ;
      fileButton -> SetTextJustify(36);
      fileButton -> Resize( 30 , 20 ) ;
      fMainFrame->AddFrame(fileButton, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      fileButton->MoveResize(420 , 65 , 30 , 20 ) ;
      fileButton -> Connect( "Clicked()" , "CreateTrendingHistogramDialog" , 
			     this , "OpenFileDialog()" ) ;
      
      // Name of tag
      TGLabel *tagName = new TGLabel(fMainFrame,"Name of the tag: ");
      tagName->SetTextJustify(33);
      fMainFrame->AddFrame(tagName, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      tagName->MoveResize( 15 , 105 , 185 , 20 ) ;
      
      // OK button
      TGTextButton *OKButton = new TGTextButton(fMainFrame,"OK");
      OKButton->SetTextJustify(36);
      OKButton->Resize(90,25);
      fMainFrame->AddFrame(OKButton, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      OKButton->MoveResize(240,200,90,25);
      OKButton -> Connect( "Clicked()" , "CreateTrendingHistogramDialog" , 
			   this , "ok()" ) ;
      
      
      // Name entry
      m_nameEntry = new TGTextEntry(fMainFrame, new TGTextBuffer(15));
      m_nameEntry->SetMaxLength(4096);
      m_nameEntry->SetAlignment(kTextLeft);
      m_nameEntry->SetText("");
      m_nameEntry->Resize( 240 , m_nameEntry->GetDefaultHeight() ) ;
      fMainFrame->AddFrame(m_nameEntry, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      m_nameEntry->MoveResize(170,25,240,22);
      
      // File entry
      m_fileEntry = new TGTextEntry(fMainFrame, new TGTextBuffer(15));
      m_fileEntry->SetMaxLength(4096);
      m_fileEntry->SetAlignment(kTextLeft);
      m_fileEntry->SetText("");
      m_fileEntry->Resize(240,m_fileEntry->GetDefaultHeight());
      fMainFrame->AddFrame(m_fileEntry, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      m_fileEntry->MoveResize(170,65,240,22);
      
      // Tag entry
      m_tagEntry = new TGComboBox(fMainFrame);
      m_tagEntry->Resize(240,m_tagEntry->GetDefaultHeight());
      fMainFrame->AddFrame(m_tagEntry, 
			   new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
      m_tagEntry->MoveResize(170,105,240,22);
      
    }
  }
  AddFrame( fMainFrame , 
	    new TGLayoutHints( kLHintsExpandX | kLHintsExpandY ) ) ;
  
  fMainFrame -> MoveResize( 0 , 0 , 500 , 250 ) ;
  
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

//===========================================================================
// Open a file dialog window to choose the trend file name
//===========================================================================
void CreateTrendingHistogramDialog::OpenFileDialog() {
  TGFileInfo fileInfo ;
  new TGFileDialog( m_rootFrame , this , kFDOpen , 
		    &fileInfo ) ;
  m_fileEntry -> SetText( fileInfo.fFilename ) ;

  boost::filesystem::path filePath( fileInfo.fFilename ) ;

  // check if this is a valid trending file
  bool result = false ;
  int retCode ;

  // Check first the file name ends with ".trend"
  result = ( filePath.extension() == ".trend" ) ;
  
  if ( ! result ) {
    new TGMsgBox( m_rootFrame , this , "Invalid file" ,
		 "The selected file is not valid" ,
		 kMBIconExclamation, kMBOk, &retCode);
    m_fileEntry -> SetText( "" ) ;
    return ;
  }

  result = PresenterGaudi::trendingTool -> openRead( filePath.replace_extension().string() ) ;

  if ( ! result ) {
    new TGMsgBox( m_rootFrame , this , "Invalid file" ,
		 "The selected file is not valid" ,
		 kMBIconExclamation, kMBOk, &retCode);
    m_fileEntry -> SetText( "" ) ;
    return ;
  }

  std::vector< std::string > tags ;
  result = PresenterGaudi::trendingTool -> tags( tags ) ;

  if ( ( ! result ) || ( tags.empty() ) ) {
    new TGMsgBox( m_rootFrame , this , "Invalid tags" ,
		 "The selected file does not contain valid tags" ,
		 kMBIconExclamation, kMBOk, &retCode);
    m_fileEntry -> SetText( "" ) ;
    return ;    
  }

  std::vector< std::string >::iterator it ;
  int id = 0 ;
  for ( it = tags.begin() ; it != tags.end() ; ++it )
    m_tagEntry -> AddEntry( (*it).c_str() , ++id ) ;
}
