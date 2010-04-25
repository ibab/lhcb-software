// $Id: KnownProblemList.h,v 1.2 2010-04-25 18:08:44 robbep Exp $
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

  bool retrieveListOfProblems( const std::string & system ) ;

protected:

private:
  std::string m_problemDbServerAddress ;
};
#endif // PRESENTER_KNOWNPROBLEMLIST_H
