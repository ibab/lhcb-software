
// $Id: PageDescriptionTextView.h,v 1.4 2010-09-19 18:49:53 robbep Exp $
#ifndef PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H
#define PRESENTER_PAGEDESCRIPTIONTEXTVIEW_H 1

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGHtml.h>
#ifdef WIN32
#pragma warning( pop )
#endif

#include <string>

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
  bool retrieveListOfProblems( const std::string& pageName , 
			       const std::string& fileName ) ;

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
