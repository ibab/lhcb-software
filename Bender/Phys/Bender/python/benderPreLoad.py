# $Id: benderPreLoad.py,v 1.3 2005-05-20 10:55:19 ibelyaev Exp $
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ 
# =============================================================================


# =============================================================================
# @file
#
# Collection of "typical particle PreLoading" consfigurations
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2004-03-16
# =============================================================================

import gaudimodule as gaudi

def Charged( **args ) :
    name      = args.get( 'Name'   , 'Charged' )
    appMgr    = args.get( 'appMgr' , gaudi.AppMgr()  )
    # 
    preload = appMgr.algorithm( name )
    preload.OutputLevel = 4
    
    if args.has_key('Location') : preload.OutputLocation = args.get('Location')
    
    desktop = appMgr.property( name + '.PhysDesktop')
    desktop.ParticleMakerType = 'CombinedParticleMaker'
    maker   = appMgr.property( name + '.PhysDesktop.CombinedParticleMaker' )
    
    maker.ExclusiveSelection = args.get ( 'Exclusive' , 1>2 )
    
    # get particles
    particles = args.get ( 'Particles' , [ 'muon' , 'electron' ,
                                           'kaon' , 'proton'   , 'pion'  ] )
    if 'muon'     in particles :
        maker.MuonSelection      = args.get ( 'Muons'     , [ "det='MUON' mu-pi='-10.0' " ] )
    if 'electron' in particles :
        maker.ElectronSelection  = args.get ( 'Electrons' , [ "det='CALO'  e-pi='-5.0'  " ] )
    if 'kaon'     in particles :
        maker.KaonSelection      = args.get ( 'Kaons'     , [ "det='RICH'  k-pi='-5.0' k-p='-5.0' " ] )
    if 'proton'   in particles :
        maker.ProtonSelection    = args.get ( 'Protons'   , [ "det='RICH'  p-pi='-5.0'  " ] )
    if 'pion'     in particles :
        maker.PionSelection      = args.get ( 'Pions'     , [ "det='RICH'  pi-k='-5.0'  " ] )

    maker.Particles = particles 

    return


def Hadrons( **args ) :
    name      = args.get( 'Name'   , 'Hadrons' )
    appMgr    = args.get( 'appMgr' , gaudi.AppMgr()  )
    #
    preload = appMgr.algorithm( name )
    
    print preload
    print appMgr.DLLs 
    print appMgr.topAlg
    
    preload.OutputLevel = 3
    
    if args.has_key('Location') : preload.OutputLocation = args.get('Location')
    
    desktop = appMgr.property( name + '.PhysDesktop')
    desktop.ParticleMakerType = 'CombinedParticleMaker'
    
    maker   = appMgr.property( name + '.PhysDesktop.CombinedParticleMaker' )
    
    maker.ExclusiveSelection = args.get ( 'Exclusive' , 1>2 )
    particles                = args.get ( 'Particles' , [ 'kaon' , 'proton' , 'pion' ] )

    if 'kaon'   in particles :
        maker.KaonSelection   = args.get ( 'Kaons'   , [ "det='RICH'  k-pi='-5.0' k-p='-5.0' " ] )
    if 'proton' in particles : 
        maker.ProtonSelection = args.get ( 'Protons' , [ "det='RICH'  p-pi='-5.0' " ] )
    if 'pion'   in particles :
        maker.PionSelection   = args.get ( 'Pions'   , [ "det='RICH'  pi-k='-5.0' " ] )
    
    maker.Particles = particles 

    return

def Leptons( **args ) :
    name      = args.get( 'Name'   , 'Leptons' )
    appMgr    = args.get( 'appMgr' , gaudi.AppMgr()  )
    #
    preload = appMgr.algorithm( name )
    preload.OutputLevel = 4
    
    if args.has_key('Location') : preload.OutputLocation = args.get('Location')
    
    desktop = appMgr.property( name + '.PhysDesktop')
    desktop.ParticleMakerType = 'CombinedParticleMaker'
    maker   = appMgr.property( name + '.PhysDesktop.CombinedParticleMaker' )
    
    maker.ExclusiveSelection = args.get ( 'Exclusive' , 1>2 )
    particles                = args.get ( 'Particles' , [ 'muon' , 'electron' ] )
    
    if 'muon'     in particles :
        maker.MuonSelection      = args.get ( 'Muons'     , [ "det='MUON' mu-pi='-10.0'  " ] )
    if 'electron' in particles :
        maker.ElectronSelection  = args.get ( 'Electrons' , [ "det='CALO'  e-pi='-5.0'   " ] )

    maker.Particles = particles
    
    return


# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.2  2004/10/13 17:51:04  ibelyaev
#  update for Tutorial
#
# =============================================================================
# The END 
# =============================================================================
