// $Id: MuonPIDCloner.h,v 1.1 2010-08-13 13:21:08 jpalac Exp $
#ifndef MICRODST_MUONPIDCLONER_H
#define MICRODST_MUONPIDCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneMuonPID.h>            // Interface
#include <MicroDST/Functors.hpp>

/** @class MuonPIDCloner MuonPIDCloner.h src/MuonPIDCloner.h
 *
 *  Clone an LHCb::MuonPID. At the moment this does nothing other than
 *  performing a simple clone and storing the clone in the appropriate
 *  TES location. It is a plalce holder for future, more complex cloning.
 *
 *  @author Juan PALACIOS
 *  @date   2010-08-13
 */
class MuonPIDCloner : public extends1<MicroDSTTool, ICloneMuonPID>
{

public:

  /// Standard constructor
  MuonPIDCloner( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  virtual ~MuonPIDCloner( ); ///< Destructor

  virtual LHCb::MuonPID* operator() (const LHCb::MuonPID* pid);

private:

  LHCb::MuonPID* clone(const LHCb::MuonPID* pid);

private:

  typedef MicroDST::BasicCopy<LHCb::MuonPID> BasicMuonPIDCloner;
  typedef MicroDST::BasicItemCloner<LHCb::Track> BasicTrackCloner;

};

#endif // MICRODST_MUONPIDCLONER_H
