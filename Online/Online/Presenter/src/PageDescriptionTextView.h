// $Id: PageDescriptionTextView.h,v 1.1 2010-04-25 18:10:27 robbep Exp $
#ifndef PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H
#define PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H 1

#include <TGTextView.h>
#include <string>

/** @class PageDescriptionTextView PageDescriptionTextView.h
 *
 *  ROOT Gui widget to display description of pages
 *
 *  @author Patrick Robbe
 *  @date   2010-04-22
 */

class PageDescriptionTextView: public TGTextView {
 public:
  /// Standard constructor
  PageDescriptionTextView( const TGWindow * p , UInt_t w, UInt_t h,
			   const std::string & address ) ;

  virtual ~PageDescriptionTextView( ); ///< Destructor

  /// Retrieve list of problems from problemDB and put them in display
  bool retrieveListOfProblems( const std::string& pageName ) ;

  /// Add text to display (but does not erase previous text)
  virtual Bool_t AddToBuffer(const char *txtbuf) ;

protected:

private:
  std::string m_problemDbServerAddress ;
};
#endif // PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H
