#!/usr/bin/env python
# =============================================================================
# $Id: selection.py,v 1.3 2006-11-27 17:01:02 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ 
# =============================================================================
## @file
#  Simple script to create proper selection files to buils relations
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date   2006-11-26
# =============================================================================
""" Simple script to create proper selection files to buils relations """
# =============================================================================
__author__ = 'Vanya BELYAEV ibelyaev@physics.syr.edu'

if "__main__" == __name__ :
    import Relations

    classes  = []
    includes = []
    
    classes += [ Relations.RelW1D('LHCb::ProtoParticle','LHCb::MCParticle','double') ]
    classes += [ Relations.RelW1D('LHCb::CaloDigit'    ,'LHCb::MCParticle','float' ) ]
    classes += [ Relations.RelW1D('LHCb::CaloCluster'  ,'LHCb::MCParticle','float' ) ]
    classes += [ Relations.RelW2D('LHCb::CaloHypo'     ,'LHCb::Track'     ,'float' ) ]
    classes += [ Relations.RelW2D('LHCb::CaloCluster'  ,'LHCb::Track'     ,'float' ) ]
    classes += [ Relations.Rel1D ('LHCb::Track'                           ,'float' ) ]

    # $DAVINCIMCKERNELROOT/Kernel/HepMC2MC.h:
    classes += [ Relations.Rel2D  ( 'HepMC::GenParticle*' , 'LHCb::MCParticle'   ) ]
    # $DAVINCIMCKERNELROOT/Kernel/MC2Collision.h:
    classes += [ Relations.Rel1D  ( 'LHCb::MCVertex'      , 'LHCb::GenCollision' ) ]
    # $DAVINCIMCKERNELROOT/Kernel/PC2MC.h:
    classes += [ Relations.RelW2D ('LHCb::RecVertex','LHCb::GenCollision','std::pair<unsigned int,float>') ]
    classes += [ Relations.RelW2D ('LHCb::RecVertex','LHCb::MCVertex'    ,'std::pair<unsigned int,float>') ]
    # $DAVINCIMCKERNELROOT/Kernel/Track2MC.h:
    classes += [ Relations.RelW2D ('LHCb::Track'    , 'LHCb::MCParticle' ,'double' ) ]
    # $DAVINCIMCKERNELROOT/Kernel/RC2HepMC.h:
    classes += [ Relations.Rel2D  ('LHCb::Particle' , 'HepMC::GenParticle*' ) ]
    # $DAVINCIMCKERNELROOT/Kernel/Particle2MC.h:
    classes += [ Relations.RelW2D ('LHCb::Particle' , 'LHCb::MCParticle' ,'double' ) ]
    classes += [ Relations.Rel2D  ('LHCb::Particle' , 'LHCb::MCParticle'           ) ]
    
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
    
    # make XML and C++ selction files:
    Relations.prepare ( classes , includes ) 


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.2  2006/11/25 19:01:15  ibelyaev
#  add the types from DaVinciMCKernel
#
# Revision 1.1  2006/06/11 15:36:15  ibelyaev
#  enable dictionaries on Win32
#
# =============================================================================
# The END 
# =============================================================================
