// $Id: $
#ifndef DUMMYDOC_H 
#define DUMMYDOC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "PresenterDoc/IPresenterDoc.h"            // Interface


/** @class DummyDoc DummyDoc.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2012-03-30
 */
class DummyDoc : public GaudiTool, virtual public IPresenterDoc {
public: 
  /// Standard constructor
  DummyDoc( const std::string& type, 
            const std::string& name,
            const IInterface* parent);

  virtual ~DummyDoc( ); ///< Destructor
  
  virtual std::string document( std::string& histoName, int binX, int binY );
  
};
#endif // DUMMYDOC_H
