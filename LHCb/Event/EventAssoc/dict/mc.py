# =============================================================================
## @file
#  Simple script to create proper selection files to buils relations
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2006-11-26
# =============================================================================
""" Simple script to create proper selection files to buils relations """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'

import Relations.Rels as Rels 
    
classes  = []
includes = []
    
classes += [ Rels.RelW1D( 'LHCb::ProtoParticle' , 'LHCb::MCParticle'    , 'double' ) ]
classes += [ Rels.RelW1D( 'LHCb::MCParticle'    , 'LHCb::ProtoParticle' , 'double' ) ]

classes += [ Rels.RelW1D( 'LHCb::CaloDigit'     , 'LHCb::MCParticle'    , 'float'  ) ]
classes += [ Rels.RelW1D( 'LHCb::CaloCluster'   , 'LHCb::MCParticle'    , 'float'  ) ]
classes += [ Rels.RelW1D( 'LHCb::MCParticle'    , 'LHCb::CaloCluster'   , 'float'  ) ]

# $DAVINCIMCKERNELROOT/Kernel/HepMC2MC.h:
classes += [ Rels.Rel2D  ( 'HepMC::GenParticle*' , 'LHCb::MCParticle'   ) ]
# $DAVINCIMCKERNELROOT/Kernel/MC2Collision.h:
classes += [ Rels.Rel1D  ( 'LHCb::MCVertex'      , 'LHCb::GenCollision' ) ]
# $DAVINCIMCKERNELROOT/Kernel/PC2MC.h:
classes += [ Rels.RelW2D ('LHCb::RecVertex','LHCb::GenCollision','std::pair<unsigned int,float>') ]
classes += [ Rels.RelW2D ('LHCb::RecVertex','LHCb::MCVertex'    ,'std::pair<unsigned int,float>') ]
# $DAVINCIMCKERNELROOT/Kernel/Track2MC.h:
classes += [ Rels.RelW2D ('LHCb::Track'    , 'LHCb::MCParticle' ,'double' ) ]
# $DAVINCIMCKERNELROOT/Kernel/RC2HepMC.h:
classes += [ Rels.Rel2D  ('LHCb::Particle' , 'HepMC::GenParticle*' ) ]
# $DAVINCIMCKERNELROOT/Kernel/Particle2MC.h, Particle2MCParticle.h:
classes += [ Rels.RelW1D ('LHCb::Particle' , 'LHCb::MCParticle' ,'double' ) ]
classes += [ Rels.RelW2D ('LHCb::Particle' , 'LHCb::MCParticle' ,'double' ) ]
classes += [ Rels.Rel2D  ('LHCb::Particle' , 'LHCb::MCParticle'           ) ]
classes += [ Rels.Rel1D  ('LHCb::Particle' , 'LHCb::MCParticle'           ) ]
    
classes += [ Rels.RelW2D ('LHCb::L0CaloCandidate' , 'LHCb::MCParticle'  , 'double' ) ]
    
includes += ['Event/Particle.h'      ]
includes += ['Event/MCParticle.h'    ]
includes += ['Event/MCVertex.h'      ]
includes += ['Event/ProtoParticle.h' ]
includes += ['Event/Track.h'         ]
includes += ['Event/CaloDigit.h'     ]
includes += ['Event/CaloCluster.h'   ]
includes += ['Event/CaloHypo.h'      ]
includes += ['Event/GenCollision.h'  ]
includes += ['Event/RecVertex.h'     ]
includes += ['HepMC/GenParticle.h'   ]
includes += ['Event/L0CaloCandidate.h' ]

__all__ = ( classes, includes )

# =============================================================================
# The END 
# =============================================================================
