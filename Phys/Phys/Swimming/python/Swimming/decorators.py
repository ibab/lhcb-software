from GaudiPython.Bindings import gbl

__all__ = ['LHCb',
           'SwimmingReports',
           'P2TPRelation',
           'SharedParticles',
           'std_map_ulb',
           'std_map_sb',
           'std_map_sinfo']

# Types
LHCb            = gbl.LHCb
SwimmingReports = gbl.KeyedContainer('LHCb::SwimmingReport', 'Containers::KeyedObjectManager<Containers::hashmap>')
P2TPRelation    = LHCb.Relation2D('LHCb::Particle', 'LHCb::SwimmingReport')
SharedParticles = gbl.SharedObjectsContainer('LHCb::Particle')
