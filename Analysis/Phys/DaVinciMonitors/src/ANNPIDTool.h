// $Id: ANNPIDTool.h,v 1.2 2009-02-15 22:03:51 jonrob Exp $
#ifndef DAVINCIMONITORS_ANNPIDTool_H
#define DAVINCIMONITORS_ANNPIDTool_H 1

// base class
#include "BasePlotTool.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class ANNPIDTool ANNPIDTool.h
 *
 *  Fills ANN PID Training Tuple
 *
 *  @author Chris Jones
 *  @date   09-02-2011
 */
class ANNPIDTool : public BasePlotTool
{

public:

  /// Standard constructor
  ANNPIDTool( const std::string& type,
              const std::string& name,
              const IInterface* parent );

  virtual ~ANNPIDTool( ); ///< Destructor

protected:

  /// Fill final state PID plots - overwrites BasePlotTools method
  StatusCode fillImpl( const LHCb::Particle* p,
                       const std::string& trailer = "" );

};

#endif // DAVINCIMONITORS_ANNPIDTool_H
