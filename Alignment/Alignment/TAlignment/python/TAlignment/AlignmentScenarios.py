from TAlignment.Alignables import *
from TAlignment.SurveyConstraints import *
from Configurables import TAlignment

#class AlignmentScenario():
#    __slots__ = {
#        "Name"                 : "" # name of the scenario
#        ,"ElementsToAlign"     : [] # list with alignment elements
#        ,"Constraints"         : [] # list with lagrange constraints
#        ,"SurveyConstraints"   : SurveyConstraints() # survey constraints
#        ,"WriteCondSubDetList" : [] # list of subdetectors written to xml
#        }
#    def __init__( aName ) :
#        Name = aName
        
# scenario used for prompt alignment

def configurePromptAlignment( fixQOverPBias = True ) :
    TAlignment().WriteCondSubDetList += ['Velo','TT','IT','OT','MUON']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("Tx")
    elements.VeloLeft("Tx")
    elements.IT("None")
    elements.ITBoxes("TxTyTzRxRyRz")
    elements.ITLayers("Tx")
    elements.OT("None")
    elements.OTCFrames("Tx")
    elements.OTCFrameLayers("Tz")
    elements.TT("None")
    elements.TTLayers("None")
    elements.TTModules("TxRz")
    elements.Tracker("None")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx ")
    # fix the q/p scale by not moving T in X. note that you do not
    # want to do this if you use D0 in the alignment
    if fixQOverPBias:
        constraints.append("OT3X : OT/T3X1U.Side : Tx")
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]


# define additional scenarios below
def configureVeloHalfAlignment(): 
    TAlignment().WriteCondSubDetList  += ['Velo']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("TxTyTzRxRyRz")
    elements.VeloLeft("TxTyTzRxRyRz")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx Ty Tz Rx Ry Rz")
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()


def configureVeloSensorAlignment():
    TAlignment().WriteCondSubDetList  += ['Velo']

    # To align at the sensor level, we align 'modules' and
    #'phi-sensors'. The R-sensor is always equal to the modules. (We
    #keep its 'deltas' equal to zero.) However, because of a feature
    #in the way we keep track of mother-daughter relations, it is
    #important to create alignables for the R-sensors as well: use
    #"None" to specify an alignable without active degrees of
    #freedom.

    # Note also that it doesn't make sense to align R modules for
    # rotations around the z-axis. Rather than aligning the phi sensor
    # for this rotation, we put the rotation at the module level.

    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("TxTyTzRxRyRz")
    elements.VeloLeft("TxTyTzRxRyRz")
    elements.VeloModules("TxTyTzRxRyRz")
    elements.VeloPhiSensors("TxTyTz")
    elements.VeloRSensors("None")
    TAlignment().ElementsToAlign += list(elements)

    # make sure that the velo stays where it was. Important note: the
    # dofs here must match the ones that we actually align for. If you
    # specify too many, things will go rather wrong.
    TAlignment().Constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx Ty Tz Rx Ry Rz")
    
    # now constrain the total shearing and z-scale in each Velo half
    # to get the z-axis nominal. we'll do this by tightening the
    # survey errors.

    # we need to align the relative twist of the two halves. the
    # C-side (minus X, Right) is known to have a corkscrew ('twist')
    # in the base plate. so, for the right half we don't fix the Rz rotation:
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloLeft/Module(00|40) : 0.0001 0.0001 0.0001 0.000001 0.000001 0.000001' ]
    ## for the right half, we fix the shearing and z-scale, but not the Rz rotation 
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloRight/Module(01|41) : 0.0001 0.0001 0.0001 0.000001 0.000001 0.0001' ]

    # bad idea: release the shearing:
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloRight/Module(01|41) : 0.02 0.02 0.0001 0.000001 0.000001 0.0001' ]


def configureTEDVeloSensorAlignment():
    TAlignment().WriteCondSubDetList  += ['Velo']

    # To align at the sensor level, we align 'modules' and
    #'phi-sensors'. The R-sensor is always equal to the modules. (We
    #keep its 'deltas' equal to zero.) However, because of a feature
    #in the way we keep track of mother-daughter relations, it is
    #important to create alignables for the R-sensors as well: use
    #"None" to specify an alignable without active degrees of
    #freedom.

    # Note also that it doesn't make sense to align R modules for
    # rotations around the z-axis. Rather than aligning the phi sensor
    # for this rotation, we put the rotation at the module level.

    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("None")
    elements.VeloLeft("None")
    elements.VeloModules("None")
    elements.VeloPhiSensors("None")
    elements.VeloRSensors("None")
    # TAlignment().ElementsToAlign += list(elements)
    myDOFs = '/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/Module14:TxTy'
    TAlignment().ElementsToAlign = [ myDOFs ]
    

    # make sure that the velo stays where it was. Important note: the
    # dofs here must match the ones that we actually align for. If you
    # specify too many, things will go rather wrong.
    TAlignment().Constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx Ty Tz Rx Ry Rz")
    
    # now constrain the total shearing and z-scale in each Velo half
    # to get the z-axis nominal. we'll do this by tightening the
    # survey errors.

    # we need to align the relative twist of the two halves. the
    # C-side (minus X, Right) is known to have a corkscrew ('twist')
    # in the base plate. so, for the right half we don't fix the Rz rotation:
    # surveyconstraints = SurveyConstraints()
    # surveyconstraints.All()
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloLeft/Module(00|40) : 0.0001 0.0001 0.0001 0.000001 0.000001 0.000001' ]
    ## for the right half, we fix the shearing and z-scale, but not the Rz rotation 
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloRight/Module(01|41) : 0.0001 0.0001 0.0001 0.000001 0.000001 0.0001' ]

    # bad idea: release the shearing:
    #surveyconstraints.XmlUncertainties += [ 
    #    'Velo/VeloRight/Module(01|41) : 0.02 0.02 0.0001 0.000001 0.000001 0.0001' ]

# Early 2012 data alignment
def configureEarlyDataAlignment( fixQOverPBias = True ) :
    TAlignment().WriteCondSubDetList += ['Velo','TT','IT','OT','MUON']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("Tx")
    elements.VeloLeft("Tx")
    elements.IT("None")
    elements.ITBoxes("TxTzRz")
    elements.ITLayers("TxTz")
    elements.OT("None")
    elements.OTCFrames("Tx")
    elements.OTCFrameLayers("Tz")
    elements.TT("None")
    elements.TTLayers("None")
    elements.TTModules("TxRz")
    elements.Tracker("None")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx ")
    # fix the q/p scale by not moving T in X. note that you do not
    # want to do this if you use D0 in the alignment
    if fixQOverPBias:
        constraints.append("OT3X : OT/T3X1U.Side : Tx")
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]

# Alignment scenario used for 2012 data
def configureTrackerAlignment( fixQOverPBias = False, surveyVer = 'latest' ) :
    TAlignment().WriteCondSubDetList += ['TT','IT','OT']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("None")
    elements.VeloLeft("None")
    elements.IT("None")
    elements.ITBoxes("TxTzRz")
    elements.ITLayers("None")
    elements.OT("None")
    elements.OTCFrames("TxRz")
    elements.OTCFrameLayers("Tz")
    elements.OTModules("TxRz")
    elements.TT("None")
    elements.TTLayers("Tz")
    elements.TTModules("TxRz")
    elements.Tracker("None")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    constraints = []
    # fix the q/p scale by not moving T in X. note that you do not
    # want to do this if you use D0 in the alignment
    if fixQOverPBias:
        constraints.append("OT3X : OT/T3X1U.Side : Tx")
    TAlignment().Constraints = constraints
    
    if surveyVer == None: return
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All(surveyVer)
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]


    
# Alignment scenario used for 2012 data
def configure2012DataAlignment( fixQOverPBias = False, surveyVer = 'latest' ) :
    TAlignment().WriteCondSubDetList += ['Velo','TT','IT','OT','MUON']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("None")
    elements.VeloLeft("None")
    elements.IT("None")
    elements.ITBoxes("TxTzRz")
    elements.ITLayers("None")
    elements.OT("None")
    elements.OTCFrames("TxRz")
    elements.OTCFrameLayers("Tz")
    elements.OTModules("TxRz")
    elements.TT("None")
    elements.TTLayers("Tz")
    elements.TTModules("TxRz")
    elements.Tracker("None")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx ")
    # fix the q/p scale by not moving T in X. note that you do not
    # want to do this if you use D0 in the alignment
    if fixQOverPBias:
        constraints.append("OT3X : OT/T3X1U.Side : Tx")

    if surveyVer == None: return
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All(surveyVer)
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]


# Alignment scenario used for 2012 data
def configureMuonAlignment( surveyVer = 'latest' ) :
    TAlignment().WriteCondSubDetList += ['MUON']
  
    # define the alignment elements
    elements = Alignables()
    elements.Velo("None")
    elements.VeloRight("None")
    elements.VeloLeft("None")
    elements.IT("None")
    elements.ITBoxes("None")
    elements.ITLayers("None")
    elements.OT("None")
    elements.OTCFrames("None")
    elements.OTCFrameLayers("None")
    elements.OTModules("None")
    elements.TT("None")
    elements.TTLayers("None")
    elements.TTModules("None")
    elements.Tracker("None")
    elements.MuonHalfStations("TxTy")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    constraints = []
    TAlignment().Constraints = constraints
    
    if surveyVer == None: return
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All(surveyVer)
    """
    surveyconstraints.MUON()
    surveyconstraints.append("Muon/M1/M1ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M1/M1CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M2/M2ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M2/M2CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M3/M3ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M3/M3CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M4/M4ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M4/M4CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M5/M5ASide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    surveyconstraints.append("Muon/M5/M5CSide : 0 0 0 0 0 0 : 10 10 30 0.001 0.005 0.001")
    """
