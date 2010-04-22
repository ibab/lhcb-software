// $Id: KnownProblemList.h,v 1.1 2010-04-22 10:18:00 robbep Exp $
#ifndef PRESENTER_KNOWNPROBLEMLIST_H
#define PRESENTER_KNOWNPROBLEMLIST_H 1

#include <TGListBox.h>
#include <string>

/** @class KnownProblemList KnownProblemList.h
 *
 *  ROOT Gui widget to display list of known problems
 *  taken from problem database
 *
 *  @author Patrick Robbe
 *  @date   2010-04-19
 */

class KnownProblemList: public TGListBox {
 public:
  /// Standard constructor
  KnownProblemList( const TGWindow * p , const std::string & address ) ;

  virtual ~KnownProblemList( ); ///< Destructor

  void retrieveListOfProblems() ;

protected:

private:
  std::string m_problemDbServerAddress ;
};
#endif // PRESENTER_KNOWNPROBLEMLIST_H
