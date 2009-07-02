# =============================================================================
""" @namespace pidtools.py
@brief Translation between pid as integer and as string, and some shortcut pid functions
@author Diego Martinez diego.martinez.santos@cern.ch
@date 2007-11-28
"""
# =============================================================================

#---------------------------------------------------
ParticleTranslator={
    'phi':333,333:'phi',
    'e-':11,11:'e-',
    'e+':-11,-11: 'e+',
    'neutrino_e':12,12:'neutrino_e',
    'antineutrino_e':-12,-12:'antineutrino_e',
    'mu-':13,13:'mu-',
    'mu+':-13,-13:'mu+',
    'neutrino_mu':14,14:'neutrino_mu',
    'antineutrino_mu':-14,-14:'antineutrino_mu',
    'tau-':15,15:'tau-',
    'tau+':-15,-15:'tau+',
    'neutrino_tau':16,16:'neutrino_tau',
    'antineutrino_tau':-16,-16:'antineutrino_tau',
    'g':21,21:'g',
    'photon':22,22:'photon',
    'Z':23,23:'Z',
    'W+':24,24:'W+',
    'W-':-24,-24:'W-',
    'pi0':111,111:'pi0',
    'anti_pi0':-111,-111:'anti_pi0',
    'pi+':211,211:'pi+',
    'pi-':-211,-211:'pi-',
    'rho0(770)':113,113:'rho0(770)',
    'anti_rho0(770)':-113,-113:'anti_rho0(770)',
    'rho+(770)':213,213:'rho+(770)',
    'rho-(770)':-213,-213:'rho-(770)',
    'eta':221,221:'eta',
    'anti_eta':-221,-221:'anti_eta',
    'eta"(958)':331,331:'eta"(958)',
    'anti_eta(958)':331,331:'anti_eta(958)',
    'w(782)':223,223:'w(782)',
    'anti_w(782)':-223,-223:'anti_w(782)',
    'phi(1020)':333,333:'phi(1020)',
    'anti_phi(1020)':-333,-333:'anti_phi(1020)',
    'K0l':130,130:'k0l',
    'anti_K0l':-130,-130:'anti_K0l',
    'K0s':310,310:'K0s',
    'anti_K0s':-310,-310:'anti_K0s',
    'K0':311,311:'K0',
    'anti_K0':-311,-311:'anti_K0',
    'K+':321,321:'K+',
    'K-':-321,-321:'K-',
    'K*0(892)':313,313:'K*0(892)',
    'anti_K*0(892)':-313,-313:'anti_K*0(892)',
    'K*+(892)':323,323:'K*+(892)',
    'K*-(892)':-323,-323:'K*-(892)',
    'D+':411,411:'D+',
    'D-':-411,-411:'D-',
    'D*+':413,413:'D*+',
    'D*-':-413,-413:'D*-',
    'D0':421,421:'D0',
    'anti_D0':-421,-421:'anti_D0',
    'Ds+':431,431:'Ds+',
    'Ds-':-431,-431:'Ds-',
    'D*_s+':433,433:'D*_s+',
    'D*_s-':-433,-433:'D*_s-',
    'B0':511,511:'B0',
    'anti_B0':-511,-511:'anti_B0',
    'B+':521,521:'B+',
    'B-':-521,-521:'B-',
    'B0s':531,531:'B0s',
    'anti_B0s':-531,-531:'anti_B0s',
    'eta_c(1S)':441,441:'eta_c(1S)',
    'J/Psi(1S)':443,443:'J/Psi(1S)',
    'anti_eta_c(1S)':-441,-441:'anti_eta_c(1S)',
    'anti_J/Psi(1S)':-443,-443:'anti_J/Psi(1S)',
    'eta_b(1S)':551,551:'eta_b(1S)',
    'anti_eta_b(1S)':-551,-551:'anti_eta_b(1S)',
    'Ypsilon(1S)':553,553:'Ypsilon(1S)',
    'anti_Ypsilon(1S)':-553,-553:'anti_Ypsilon(1S)',
    'p':2212,2212:'p',
    'anti_p':-2212,-2212:'anti_p',
    'n':2112,2112:'n',
    'anti_n':-2112,-2112:'anti_n',
    'Delta0':2114,2114:'Delta0',
    'Delta++':2224,2224:'Delta++',
    'Delta--':-2224,-2224:'Delta--',
    'Delta+':2214,2214:'Delta+',
    'Delta-':1114,1114:'Delta-',
    'anti_Delta+':-2214,-2214:'anti_Delta+',
    'anti_Delta-':-1114,-1114:'anti_Delta-',
    'lambda':3122,3122:'lambda',
    'anti_lambda': -3122,-3122:'anti_lambda',
    'sygma+':3222,3222:'sygma+',
    'anti_sygma+':-3222,-3222:'anti_sygma+',
    'sygma0':3212,3212:'sygma0',
    'anti_sygma0':-3212,-3212:'anti_sygma0',
    'sygma-':3112,3112:'sygma-',
    'anti_sygma-':-3112,-3112:'anti_sygma-',
    'OMG-':3334,3334:'OMG-',
    'OMG+':-3334,-3334:'OMG+',
    '=0':3322,3322:'=0',
    'anti_=0':-3322,-3322:'anti_=0',
    '=-':3312,3312:'=-',
    '=+':-3312,-3312:'=+',
    'BC+':541,541:'BC+',
    'BC-':-541,-541:'BC-',
    'b_1+':10213,10213:'b_1+',
    'b_1-':-10213,-10213:'b_1-',
    'K1(1270)0':10313,10313:'K1(1270)0',
    'anti_K1(1270)0':-10313,-10313:'anti_K1(1270)0',
    'K1(1270)+':10323,10323:'K1(1270)+',
    'K1(1270)-':-10323,-10323:'K1(1270)-',
    'sygma*-':3114,3114:'sygma*-',
    'anti_sygma*-':-3114,-3114:'anti_sygma*-',
    'sygma*+':3224,3224:'sygma*+',
    'anti_sygma*+':-3224,-3224:'anti_sygma*+',
    'Sigma_c0': 4112,4112: 'Sigma_c0',
    'lambda_c+':4122,4122:'lambda_c+',
    'lambda_c-':-4122,-4122:'lambda_c-',
    '=+_c':4232,4232:'=+_c',
    '=-_c':-4232,-4232:'=-_c',
    'lambda_b0':5122,5122:'lambda_b0',
    'anti_lambda_b0':-5122,-5122:'anti_lambda_b0',
    'b1(1235)0':10113,10113:'b1(1235)0',
    'anti_b1(1235)0':-10113,-10113:'anti_b1(1235)0',
    'a_10':20113,20113:'a_10',
    'a_1+':20213,20213:'a_1+',
    'a_1-':-20213,-20213:'a_1-',
    'sygma*0':3214,3214:'sigma*0',
    'anti_sygma*0':-3214,-3214:'anti_sygma*0',
    'D*(2007)0':423,423:'D*(2007)0',
    'anti_D*(2007)0':-423,-423:'anti_D*(2007)0',
    'Linker Error':0.5,0.5:'Linker Error'
    
    }

#---------------------------------------------------
def PTranslate(n):
    if (ParticleTranslator.has_key(n) == False): return str(n)
    return ParticleTranslator[n]
 
#---------------------------------------------------
def pid (a) :
    """ Returns pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return a.particleID().pid()

#---------------------------------------------------
def absPid (a) :
    """ Returns absolute value of pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return abs(pid(a))

#---------------------------------------------------
def pidName (a) :
    """ Returns pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return  PTranslate(a.particleID().pid())

