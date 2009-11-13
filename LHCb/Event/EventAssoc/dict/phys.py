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
    
classes += [ Rels.RelW2D( 'LHCb::CaloHypo'      , 'LHCb::Track'       , 'float'  ) ]
classes += [ Rels.RelW2D( 'LHCb::CaloCluster'   , 'LHCb::Track'       , 'float'  ) ]
classes += [ Rels.Rel1D ( 'LHCb::Track'                               , 'float'  ) ]
classes += [ Rels.RelW1D( 'LHCb::Particle'      , 'LHCb::VertexBase'  , 'double' ) ]
classes += [ Rels.Rel1D( 'LHCb::Particle'       , 'LHCb::VertexBase'             ) ]
classes += [ Rels.Rel1D( 'LHCb::Particle'       , 'int'             ) ]
classes += [ Rels.Rel1D( 'LHCb::Particle'       , 'double'             ) ]

    
classes += [ Rels.RelW1D( 'LHCb::Particle' , 'LHCb::Particle'  , 'double' ) ]

includes += ['Event/Particle.h'      ]
includes += ['Event/Track.h'         ]
includes += ['Event/CaloCluster.h'   ]
includes += ['Event/CaloHypo.h'      ]
includes += ['Event/VertexBase.h'    ]

__all__ = ( classes, includes )

# =============================================================================
# The END 
# =============================================================================
