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

std_map_sinfo       = gbl.std.map('string', 'map<unsigned long,bool>')

# Decorate maps to bool with __call__
std_map_ulb          = gbl.std.map('unsigned long', 'bool')
pair_ulb             = gbl.std.pair('const unsigned long', 'bool')
std_map_ulb.__pair   = pair_ulb

std_map_sb          = gbl.std.map('string', 'bool')
pair_sb             = gbl.std.pair('const string', 'bool')
std_map_sb.__pair   = pair_sb

def set_map(self, k, v):
    p = self.__pair(k, v)
    it = self.find(k)
    if it != self.end():
        self.erase(it)
    self.insert(p)

std_map_ulb.__setitem__ = set_map
std_map_sb.__setitem__ = set_map
