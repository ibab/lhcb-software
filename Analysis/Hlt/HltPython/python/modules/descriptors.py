# =============================================================================
""" @namespace descriptors
@brief Collection of decay descriptors and tools to run on different types of data
@author Hugo RUIZ hugo.ruiz@cern.ch
@author Elias LOPEZ elopez@ecm.ub.es
@author Antonio PEREZ-CALERO aperez@ecm.ub.es
@date 2008-01-29
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
    'semilept'         : '{[<Xb>]cc  -> [mu+]cc..., [<Xb>]cc -> ([tau+]cc ->[ mu+]cc...)...}',
    'Bd2DstarMuNu'     : '[B0]cc     ->[{mu+ ,tau+ }]cc [{nu_mu , nu_tau}]cc '+threeBodyDstar+'...',# ... neded if  pi0 emitted
    'Bs2DsMuNu'        : '[B_s0]cc   =>[{mu+ ,tau+ }]cc [{nu_mu , nu_tau}]cc '+threeBodyDs+'...',# ... neded if  pi0 emitted
    'Bd2D0Kstar'       : '[B0]cc     -> [D~0]cc '+KStar+someGammas , 
    'Bs2MuMu'          : '[B_s0]cc   -> [ mu+]cc [mu-]cc '+lotsOfGammas, # someGammas not enough
    'Bd2KPi'           : '[B0]cc     -> [K+]cc [pi-]cc '+someGammas,
    'Bd2PiPi'          : '[ B0 ]cc   -> [ pi+ ]cc [ pi- ]cc ' + someGammas,
    'Bd2KK'            : '[ B0 ]cc   -> [ K+ ]cc [ K- ]cc ' + someGammas,
    'Bs2PiPi'          : '[ B_s0 ]cc -> [ pi+ ]cc [ pi- ]cc ' + someGammas,
    'Bs2PiK'           : '[ B_s0 ]cc -> [ pi+ ]cc [ K- ]cc ' + someGammas,
    'Bs2KK'            : '[ B_s0 ]cc -> [ K+ ]cc [ K- ]cc ' + someGammas,
    'Bd2KstarD2KSPiPi' : '[B0]cc     -> '+threeBodyD0+KStar+someGammas,
    'Bs2DsDs'          : '[B_s0]cc   => '+threeBodyDs+threeBodyDs+someGammas, # => needed if Ds through Ds*
    'Bs2DsPi'          : '[B_s0]cc   -> '+threeBodyDs+' [pi-]cc'+someGammas,
    'Bs2PhiPhi'        : '[B_s0]cc   -> '+Phi+Phi+someGammas,
    'Bu2KD2KSPiPi'     : '[B+]cc     -> [K+]cc '+threeBodyD0+lotsOfGammas,
    'Bu2KMuMu'         : '[B+]cc     -> [K+]cc mu+ mu- '+lotsOfGammas, # someGammas not enough
    'Bd2MuMuKstar'     : '[B0]cc     ->  mu+ mu- '+KStar+lotsOfGammas, # someGammas not enough
    'Bd2PiPiPi0'       : '[B0]cc     -> [pi+]cc [pi-]cc pi0 '+someGammas,
    'Bs2PhiGamma'      : '[B_s0]cc   ->'+someGammas,
    'Bs2PsiPhi'        : '[B_s0]cc   -> '+Phi+JPsi+' '+someGammas,
    'Bu2Kee'           : '[B+]cc     -> [K+]cc e+ e- '+someGammas
    }



#---------------------------------------------------
def decayDescriptor ( sample ):
    """ Returns decay descriptor corresponding to a sample name.
    @param sample Standard name of the data sample
    @returns Corresponding decay descriptor
    @autor Hugo Ruiz, hugo.ruiz@cern.ch
    """
    return decayDescriptorDict[ sample ]


