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
    elements.VeloRight("Tx")
    elements.VeloLeft("Tx")
    TAlignment().ElementsToAlign = list(elements)

    # make sure that the velo stays where it was
    TAlignment().Constraints = constraints = []
    constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx ")
 
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
    elements.VeloRight("TxTy")
    elements.VeloLeft("TxTy")
    elements.VeloModules("TxTyTzRz")
    elements.VeloPhiSensors("TxTyTz")
    elements.VeloRSensors("None")
    TAlignment().ElementsToAlign += list(elements)

    # make sure that the velo stays where it was. Important note: the
    # dofs here must match the ones that we actually align for. If you
    # specify too many, things will go rather wrong.
    TAlignment().Constraints.append("VeloHalfAverage : Velo/Velo(Left|Right) : Tx Ty")
    
    # now constrain the total shearing and z-scale in each Velo half
    # to get the z-axis nominal. we'll do this by tightening the
    # survey errors:
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    surveyconstraints.XmlUncertainties += [ 
        'Velo/Velo(Left|Right)/Module(00|01|40|41) : 0.0001 0.0001 0.0001 0.000001 0.000001 0.000001' ]

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
def configure2012DataAlignment( fixQOverPBias = False ) :
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
 
    # tweak the survey a little bit to fix the z-scale to survey
    surveyconstraints = SurveyConstraints()
    surveyconstraints.All()
    # make sure we fix the z-scale
    surveyconstraints.XmlUncertainties += ["OT/T3X1U : 0.5 0.5 0.00001 0.0001 0.0001 0.0001" ]
