// $Id: RichRecToolBase.h,v 1.9 2003-11-02 21:46:02 jonrob Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

// Base class
#include "RichUtils/RichToolBase.h"

/** @class RichRecToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 *
 *  @todo  Review if this class is needed
 */

class RichRecToolBase : public RichToolBase {

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase() {};

  /// Initialize method
  virtual StatusCode initialize();

  /// Finalize method
  virtual StatusCode finalize();

protected:   // Protected methods

private:   // Private data

};

#endif // RICHRECALGS_RICHRECTOOLBASE_H
