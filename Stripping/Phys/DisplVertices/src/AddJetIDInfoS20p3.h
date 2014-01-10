#ifndef DISPLVERTICES_ADDJETIDINFOS20P3_H
#define DISPLVERTICES_ADDJETIDINFOS20P3_H 1

#include <boost/foreach.hpp>

#include "GaudiAlg/GaudiTool.h"

#include "Kernel/IExtraInfoTool.h"

#include "Kernel/PFParticle.h"
#include "Kernel/JetEnums.h"

#include "LoKi/PhysTypes.h"

/**
 * IExtraInfoTool to fill JetID variables, Stripping20p3 version
 *
 * @author Victor Coco
 * @author Pieter David
 * @date   2013-12-19
 */
class AddJetIDInfoS20p3 : public GaudiTool, virtual public IExtraInfoTool
{
public:
  AddJetIDInfoS20p3( const std::string& type
              , const std::string& name
              , const IInterface* parent );

  virtual ~AddJetIDInfoS20p3();

  StatusCode initialize();
  StatusCode finalize();

  // interface methods
  StatusCode calculateExtraInfo( const LHCb::Particle* top, const LHCb::Particle* part );
  int getFirstIndex(void) { return LHCb::JetIDInfo::StartJetIDInfo+1; }
  int getNumberOfParameters(void) { return LHCb::JetIDInfo::MPT - LHCb::JetIDInfo::StartJetIDInfo; }
  int getInfo( int index, double& value, std::string& name );
private:
  // const versions of the above

  std::vector<double> m_cache;
  inline double getCache( int key ) { return m_cache[key-getFirstIndex()]; }
  inline void setCache( int key, const double& value ) { m_cache[key-getFirstIndex()] = value; }
  inline void clearCache() { m_cache = std::vector<double>(getNumberOfParameters(), -10.); }
  bool cacheValid() const;
  void dump();

  // functors
  LoKi::PhysTypes::Fun PFTYPE;
  LoKi::PhysTypes::Fun NSATECAL;
  LoKi::PhysTypes::Fun NSATHCAL;
  LoKi::PhysTypes::Fun NWITHPVINFO;
  LoKi::PhysTypes::Fun NWITHPVINFOCHI24;
  LoKi::PhysTypes::Cut HASPVINFO;
};
#endif // DISPLVERTICES_ADDJETIDINFOS20P3_H
