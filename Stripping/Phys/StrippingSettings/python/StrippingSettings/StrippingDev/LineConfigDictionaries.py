"""
Place holder for stripping dictionary configurations under development.
These will not be used for production, only for testing.
Dictionaries have the name of the line builder instance.
"""


_dict = {}

# standard Bs->Jpsi/Phi prescaled and detatched lines
_dict['Bs2JpsiPhiPrescaledAndDetatched'] = {'PhiPT' : 500.,
                                            'BsMassWin' : 300.,
                                            'BsLTFITCHI2' : 14.,
                                            'BsVCHI2' : 20.,
                                            'DetatchedBsLTIME' : 15.,
                                            'PrescaledLinePrescale' : 1.0,
                                            'PrescaledLinePostscale' : 0.1,
                                            'DetatchedLinePrescale' : 1.0,
                                            'DetatchedLinePostscale' : 1.0}

# Prescaled Bs->Jpsi/Phi prescaled and detatched lines
# Copy standard parameters, change pre-scales.
_dict['Bs2JpsiPhiPrescaledAndDetatchedPrescaled'] = dict(_dict['Bs2JpsiPhiPrescaledAndDetatched'])
_dict['Bs2JpsiPhiPrescaledAndDetatchedPrescaled']['PrescaledLinePrescale'] = 0.1
_dict['Bs2JpsiPhiPrescaledAndDetatchedPrescaled']['DetatchedLinePrescale'] = 0.1
def configurationDictionary(builderName) :
    return _dict[builderName]
