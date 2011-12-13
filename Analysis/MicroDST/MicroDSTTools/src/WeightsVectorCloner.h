// $Id: $
#ifndef MICRODST_WEIGHTSVECTORCLONER_H
#define MICRODST_WEIGHTSVECTORCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneWeightsVector.h>            // Interface
#include <MicroDST/Functors.hpp>

/** @class WeightsVectorCloner WeightsVectorCloner.h src/WeightsVectorCloner.h
 *
 *  Clone an LHCb::WeightsVector. At the moment this does nothing other than
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location.
 *
 *  @author Juan PALACIOS
 *  @date   2010-10-03
 */

class WeightsVectorCloner : public extends1<MicroDSTTool, ICloneWeightsVector>
{

public:

  /// Standard constructor
  WeightsVectorCloner( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~WeightsVectorCloner( ); ///< Destructor

  virtual LHCb::WeightsVector* operator() (const LHCb::WeightsVector* track);

private:

  LHCb::WeightsVector* clone(const LHCb::WeightsVector* track);

private:

  typedef MicroDST::BasicCopy<LHCb::WeightsVector> BasicWeightsVectorCloner;

};

#endif // MICRODST_WEIGHTSVECTORCLONER_H
