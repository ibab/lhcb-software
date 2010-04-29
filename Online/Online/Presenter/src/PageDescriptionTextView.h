
// $Id: PageDescriptionTextView.h,v 1.2 2010-04-29 14:24:08 robbep Exp $
#ifndef PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H
#define PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H 1

#include <TGHtml.h>
#include <string>

class TGVScrollBar ;
namespace boost {
  namespace posix_time { 
    class ptime ;
  };
};

/** @class PageDescriptionTextView PageDescriptionTextView.h
 *
 *  ROOT Gui widget to display description of pages
 *
 *  @author Patrick Robbe
 *  @date   2010-04-22
 */

class PageDescriptionTextView: public TGHtml {
 public:
  /// Standard constructor
  PageDescriptionTextView( const TGWindow * p , UInt_t w, UInt_t h,
			   const std::string & address ) ;

  virtual ~PageDescriptionTextView( ); ///< Destructor

  /// Retrieve list of problems from problemDB and put them in display
  bool retrieveListOfProblems( const std::string& pageName ) ;

  /// Add text to display (but does not erase previous text)
  virtual Bool_t LoadBuffer(const char *txtbuf) ;

  /// For backward compatibility
  virtual void DataChanged( ) { Redraw() ; } ;

  /// Handle click in text
  virtual Bool_t HandleButton(Event_t* event) ;

protected:
  void ComputeVirtualSize() ;

private:
  /// name of problem db web server
  std::string m_problemDbServerAddress ;
};
#endif // PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H
