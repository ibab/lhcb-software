#ifndef DISPLVERTICES_PERPVOFFSETJET_H
#define DISPLVERTICES_PERPVOFFSETJET_H 1

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
class PerPVOffsetJEC : public GaudiTool, virtual public IParticleReFitter
{
public:
  PerPVOffsetJEC( const std::string& type
                , const std::string& name
                , const IInterface* parent );

  virtual ~PerPVOffsetJEC();

  StatusCode initialize();

  // interface methods
  StatusCode reFit( LHCb::Particle& jet ) const;
private:
  bool m_apply;
  std::string m_histo_path;
  std::vector<TH3D*> m_histos;
  float m_shiftJEC;
};
#endif // DISPLVERTICES_PERPVOFFSETJET_H
