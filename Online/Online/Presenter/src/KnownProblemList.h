// $Id: KnownProblemList.h,v 1.3 2010-05-16 18:09:40 robbep Exp $
#ifndef PRESENTER_KNOWNPROBLEMLIST_H
#define PRESENTER_KNOWNPROBLEMLIST_H 1

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGListBox.h>
#ifdef WIN32
#pragma warning( pop )
#endif

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
