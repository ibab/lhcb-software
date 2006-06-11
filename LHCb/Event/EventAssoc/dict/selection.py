#!/usr/bin/env python
# =============================================================================
# $Id: selection.py,v 1.1 2006-06-11 15:36:15 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
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

    includes += ['Event/MCParticle.h'    ]
    includes += ['Event/ProtoParticle.h' ]
    includes += ['Event/Track.h'         ]
    includes += ['Event/CaloDigit.h'     ]
    includes += ['Event/CaloCluster.h'   ]
    includes += ['Event/CaloHypo.h'      ]
    
    # make XML and C++ selction files:
    Relations.prepare ( classes , includes ) 



# =============================================================================
# $Log: not supported by cvs2svn $
# =============================================================================
# The END 
# =============================================================================
