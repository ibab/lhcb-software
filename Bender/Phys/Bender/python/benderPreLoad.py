# $Id: benderPreLoad.py,v 1.1 2004-03-17 10:20:54 ibelyaev Exp $
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

from gaudimodule import AppMgr

def Charged( **args ) :
    name      = args.get( 'Name'   , 'Charged' )
    #
    preload = appMgr.algorithm( name )
    preload.OutputLevel = 4
    
    if args.has_key('Location') : preload.OutputLocation = args.get('Location')
    
    desktop = appMgr.property( name + '.PhysDesktop')
    desktop.ParticleMakerType = 'CombinedParticleMaker'
    maker   = appMgr.property( name + '.PhysDesktop.CombinedParticleMaker' )
    
    maker.ExclusiveSelection = args.get ( 'Exclusive' , 1>2 )
    maker.Particles          = args.get ( 'Particles' , [ 'muon' , 'electron' ,
                                                          'kaon' , 'proton' , 'pion' ] )
    maker.MuonSelection      = args.get ( 'Muons'     , [ "det='MUON' mu-pi='0.0'  " ] )
    maker.ElectronSelection  = args.get ( 'Electrons' , [ "det='CALO'  e-pi='0.0'  " ] )
    maker.KaonSelection      = args.get ( 'Kaons'     , [ "det='RICH'  k-pi='0.0' k-p='-5.0' " ] )
    maker.ProtonSelection    = args.get ( 'Protons'   , [ "det='RICH'  p-pi='0.0'  " ] )
    maker.PionSelection      = args.get ( 'Pions'     , [ "det='RICH'  pi-k='-5.0' " ] )
    
    return


def Hadrons( **args ) :
    name      = args.get( 'Name'   , 'Hadrons' )
    appMgr    = args.get( 'appMgr' , AppMgr()  )
    #
    print appMgr.DLLs 
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
    maker.Particles          = args.get ( 'Particles' ,
                                          [ 'kaon' , 'proton' , 'pion' ] )
    maker.KaonSelection     = args.get ( 'Kaons'     , [ "det='RICH'  k-pi='0.0' k-p='-5.0' " ] )
    maker.ProtonSelection   = args.get ( 'Protons'   , [ "det='RICH'  p-pi='0.0'  " ] )
    maker.PionSelection     = args.get ( 'Pions'     , [ "det='RICH'  pi-k='-5.0' " ] )
    
    return

def Leptons( **args ) :
    name      = args.get( 'Name'   , 'Leptons' )
    appMgr    = args.get( 'appMgr' , AppMgr()  )
    #
    preload = appMgr.algorithm( name )
    preload.OutputLevel = 4
    
    if args.has_key('Location') : preload.OutputLocation = args.get('Location')
    
    desktop = appMgr.property( name + '.PhysDesktop')
    desktop.ParticleMakerType = 'CombinedParticleMaker'
    maker   = appMgr.property( name + '.PhysDesktop.CombinedParticleMaker' )
    
    maker.ExclusiveSelection = args.get ( 'Exclusive' , 1>2 )
    maker.Particles          = args.get ( 'Particles' , [ 'muon' , 'electron' ] )
    maker.MuonSelection      = args.get ( 'Muons'     , [ "det='MUON' mu-pi='0.0'  " ] )
    maker.ElectronSelection  = args.get ( 'Electrons' , [ "det='CALO'  e-pi='0.0'  " ] )
    
    return


# =============================================================================
# $Log: not supported by cvs2svn $ 
# =============================================================================
# The END 
# =============================================================================
