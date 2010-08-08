// $Id: CreateTrendingHistogramDialog.h,v 1.1 2010-08-08 15:12:33 robbep Exp $
#ifndef PRESENTER_CREATETRENDINGHISTOGRAMDIALOG_H
#define PRESENTER_CREATETRENDINGHISTOGRAMDIALOG_H 1

// Include files
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGFrame.h>
#ifdef WIN32
#pragma warning( pop )
#endif

// forward declarations
class TGTextEntry ;
class TGComboBox ;
class OnlineHistDB ;

/** @class CreateTrendingHistogramDialog CreateTrendingHistogramDialog.h
 *
 *  Dialog window to create trending histograms from the presenter
 *
 *  @author Patrick Robbe
 *  @date   2010-08-04
 */
class CreateTrendingHistogramDialog : public TGTransientFrame {
 public:
  /// Standard constructor
  CreateTrendingHistogramDialog( const TGWindow * p , 
				 const TGWindow * main , 
				 OnlineHistDB   * histdb ) ;

  virtual ~CreateTrendingHistogramDialog() ; ///< Destructor
  
  /// Function called when the OK button is pressed
  void ok();

  /// Build window
  void build();

  /// Function called when the window is closed
  void CloseWindow();

  /// Open a file dialog window to choose the name of the trend file
  void OpenFileDialog() ;

 private:
  const TGWindow *    m_rootFrame ; ///< Root frame of this window
  TGTextEntry * m_nameEntry ; ///< Entry for the histo name
  TGTextEntry * m_fileEntry ; ///< Entry for the file name
  TGComboBox  * m_tagEntry ;  ///< Entry for the tag name

  OnlineHistDB * m_histdb ; ///< Pointer to histogram DB object

  ClassDef( CreateTrendingHistogramDialog , 0 ) 
};

#endif // PRESENTER_CREATETRENDINGHISTOGRAMDIALOG_H
