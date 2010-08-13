// $Id: CaloHypoCloner.h,v 1.1 2010-08-13 13:21:08 jpalac Exp $
#ifndef MICRODST_CALOHYPOCLONER_H 
#define MICRODST_CALOHYPOCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneCaloHypo.h>            // Interface
#include <MicroDST/Functors.hpp>

/** @class CaloHypoCloner CaloHypoCloner.h src/CaloHypoCloner.h
 *  
 *  Clone an LHCb::CaloHypo. At the moment this does nothing other than 
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location. It is a plalce holder for future, more complex cloning.
 *  
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */

namespace LHCb {
  class CaloDigit;
  class CaloCluster;
}


class CaloHypoCloner : public extends1<MicroDSTTool, ICloneCaloHypo> {
public: 
  /// Standard constructor
  CaloHypoCloner( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~CaloHypoCloner( ); ///< Destructor

  StatusCode initialize();

  virtual LHCb::CaloHypo* operator() (const LHCb::CaloHypo* hypo);

private:

  LHCb::CaloHypo* clone(const LHCb::CaloHypo* hypo);

private:

  typedef MicroDST::BasicItemCloner<LHCb::CaloHypo> BasicCaloHypoCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloDigit> BasicCaloDigitCloner;
  typedef MicroDST::BasicItemCloner<LHCb::CaloCluster> BasicCaloClusterCloner;

};
#endif // MICRODST_CALOHYPOCLONER_H
