#ifndef DISPLVERTICES_PERPVOFFSETJETS20P3_H
#define DISPLVERTICES_PERPVOFFSETJETS20P3_H 1

#include "GaudiAlg/GaudiTool.h"

#include "Kernel/IParticleReFitter.h"

class TH3D;

/**
 * Jet Energy Correction for Displaced Vertex jets in Stripping20r*p3
 *
 * @author Victor Coco
 * @author Pieter David
 * @date   2013-12-19
 */
class PerPVOffsetJECS20p3 : public GaudiTool, virtual public IParticleReFitter
{
public:
  PerPVOffsetJECS20p3( const std::string& type
                , const std::string& name
                , const IInterface* parent );

  virtual ~PerPVOffsetJECS20p3();

  StatusCode initialize();

  // interface methods
  StatusCode reFit( LHCb::Particle& jet ) const;
private:
  bool m_apply;
  std::string m_histo_path;
  std::vector<TH3D*> m_histos;
  float m_shiftJEC;
};
#endif // DISPLVERTICES_PERPVOFFSETJETS20P3_H
