# =============================================================================
""" @namespace descriptors
@brief Collection of decay descriptors and tools to run on different types of data
@author Hugo RUIZ hugo.ruiz@cern.ch
@author Elias LOPEZ elopez@ecm.ub.es
@date 2007-12-16
"""

#---------------------------------------------------
# Definition of basic segments of decay descriptors
someGammas     = ' {,gamma}{,gamma}{,gamma} ' # More CPU efficient
lotsOfGammas   = ' {,gamma}{,gamma}{,gamma}{,gamma} ' # Safer
twoBodyD0      = ' ([D0]cc ->[K+ pi- '+someGammas+']cc) '
threeBodyD0    = ' ([D0]cc =>[KS0 pi- pi+ '+someGammas+']cc) ' # => Includes K* resonance
threeBodyDstar = ' ([D*(2010)-]cc -> '+twoBodyD0+' [pi-]cc {,gamma}{,gamma}) '
threeBodyDs    = ' ([D_s+]cc => [K+ K- pi+ '+someGammas+']cc) '
Phi            = ' (phi(1020) ->K- K+ '+someGammas+' )'
JPsi           = ' (J/psi(1S) -> mu+ mu- '+someGammas+' )'
KStar          = ' ([K*(892)0]cc -> [K+]cc [pi-]cc '+someGammas+' ) '


#---------------------------------------------------
# decayDescriptor dictionary
decayDescriptorDict = {
    'semilept'         : '{[<Xb>]cc   -> [mu+]cc..., [<Xb>]cc -> ([tau+]cc ->[ mu+]cc...)...}',
    'Bd2DstarMuNu'     : '[B0]cc ->[{mu+ ,tau+ }]cc [{nu_mu , nu_tau}]cc '+threeBodyDstar+'...',# ... neded if  pi0 emitted
    'Bd2D0Kstar'       : '[B0]cc   -> [D~0]cc '+KStar+someGammas , 
    'Bs2MuMu'          : '[B_s0]cc -> [ mu+]cc [mu-]cc '+lotsOfGammas, # someGammas not enough
    'Bd2KPi'           : '[B0]cc -> [K+]cc [pi-]cc '+someGammas,
    'Bd2PiPi'          : '[ B0 ]cc -> [ pi+ ]cc [ pi- ]cc ' + someGammas,
    'Bd2KK'            : '[ B0 ]cc -> [ K+ ]cc [ K- ]cc ' + someGammas,
    'Bs2PiPi'          : '[ B_s0 ]cc -> [ pi+ ]cc [ pi- ]cc ' + someGammas,
    'Bs2PiK'           : '[ B_s0 ]cc -> [ pi+ ]cc [ K- ]cc ' + someGammas,
    'Bs2KK'            : '[ B_s0 ]cc -> [ K+ ]cc [ K- ]cc ' + someGammas,
    'Bd2KstarD2KSPiPi' : '[B0]cc-> '+threeBodyD0+KStar+someGammas,
    'Bs2DsDs'          : '[B_s0]cc  :> '+threeBodyDs+threeBodyDs+someGammas, # => needed if Ds through Ds*
    'Bs2DsPi'          : '[B_s0]cc  -> '+threeBodyDs+' [pi-]cc'+someGammas,
    'Bs2PhiPhi'        : '[B_s0]cc  -> '+Phi+Phi+someGammas,
    'Bu2KD2KSPiPi'     : '[B+]cc -> [K+]cc '+threeBodyD0+lotsOfGammas,
    'Bu2KMuMu'         : '[B+]cc -> [K+]cc mu+ mu- '+lotsOfGammas, # someGammas not enough
    'Bd2MuMuKstar'     : '[B0]cc ->  mu+ mu- '+KStar+lotsOfGammas, # someGammas not enough
    'Bd2PiPiPi0'       : '[B0]cc -> [pi+]cc [pi-]cc pi0 '+someGammas,
    'Bs2PhiGamma'      : '[B_s0]cc ->'+someGammas,
    'Bs2PsiPhi'        : '[B_s0]cc -> '+Phi+JPsi+' '+someGammas,
    'Bu2Kee'           : '[B+]cc -> [K+]cc e+ e- '+someGammas
    }



#---------------------------------------------------
# Conversion to Hans preselection names
hansFileDict ={
    'Bd2DstarMuNu'    : 'Bd2DstMu',
    'Bd2D0Kstar'      : 'Bd2D0Kst',
    'Bs2MuMu'         : 'Bs2MuMu',
    'Bd2KPi'          : 'Bd2Kpi-decprodcut',
    'Bd2KstarD2KSPiPi': 'Bd2KstD2KSPiPi',
    'Bs2DsDs'         : 'Bs2DsDs',
    'Bs2DsPi'         : 'Bs2Dspi-Decprodcut',
    'Bs2PhiPhi'       : 'Bs2PhiPhi',
    'Bu2KD2KSPiPi'    : 'Bu2KD-KSPiPi',
    'Bu2KMuMu'        : 'Bu2Kmumu',
    'Bd2MuMuKstar'    : 'Bd2MuMuKst',
    'Bd2PiPiPi0'      : 'Bd2PiPiPi',
    'Bs2PhiGamma'     : 'Bs2phigamma',
    'Bs2PsiPhi'       : 'Bs2psiphi-decprodcut',
    'Bu2Kee'          : 'Bu2Kee'
}

#---------------------------------------------------
def decayDescriptor ( sample ):
    """ Returns decay descriptor corresponding to a sample name.
    @param sample Standard name of the data sample
    @returns Corresponding decay descriptor
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    return decayDescriptorDict[ sample ]


#---------------------------------------------------
def hansVolume( sample ):
    """ Returns number of volume where Hans put the corresponding data file in his presel stripping.
    @param sample Standard name of the data sample
    @returns Volume number where Hans has the corresponding file
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    filesInVol1 = ['Bd2D0Kst', 'Bd2DstMu', 'Bd2Kpi-decprodcut', 'Bd2KstD2KSPiPi', 'Bs2DsDs', 'Bs2Dspi-Decprodcut', 'Bs2MuMu', 'Bs2PhiPhi', 'Bu2KD-KSPiPi', 'Bu2Kmumu','MBL0-lumi2-1.dst','MBL0-lumi2-2.dst']
    filesInVol3 = ['Bd2MuMuKst', 'Bd2PiPiPi', 'Bs2phigamma', 'Bs2psiphi-decprodcut', 'Bu2Kee','MBL0-lumi2-3.dst','MBL0-lumi2-4.dst']

    if hansFileDict[sample] in filesInVol1: return 1
    elif hansFileDict[sample] in filesInVol2: return 2
    else:
        print 'SAMPLE IS NOT IN HANS PRESELECTIONS'
        raise RuntimeError
    return

#---------------------------------------------------
def optionForHansFile ( sample ):
    """ Returns an option including in the event selector the  corresponding data file in Hans's presel stripping.
    @param sample Standard name of the data sample
    @returns An option including in the event selector the  corresponding data file in Hans's presel stripping
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    
    out = 'EventSelector.Input   = {"DATAFILE=\'PFN:/afs/cern.ch/lhcb/group/trigger/vol'
    out += str(hansVolume( sample ))+'/dijkstra/Selections/'+hansFileDict[sample]+'-lum2.dst\' TYP=\'POOL_ROOTTREE\' OPT=\'READ\'"}'
    return out


