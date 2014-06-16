#!/usr/bin/env ganga
# =============================================================================
# $Id$ 
# =============================================================================
# @file BenderGanga.py
#
# Ganga script to submit Bender job 
#
# This file is a part of BENDER project:
# ``Python-based Interactive Environment for Smart and Friendly Physics Analysis''
#
# The project has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#
# And it is based on the 
# LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''
# 
# By usage of this code one clearly states the disagreement 
# with the smear campaign of Dr.O.Callot et al.: 
# ``No Vanya's lines are allowed in LHCb/Gaudi software.''
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2014-05-16
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""Ganga script to submit Bender Job

oooooooooo.                              .o8                     
`888'   `Y8b                            \"888                     
 888     888  .ooooo.  ooo. .oo.    .oooo888   .ooooo.  oooo d8b 
 888oooo888' d88' `88b `888P\"Y88b  d88' `888  d88' `88b `888\"\"8P 
 888    `88b 888ooo888  888   888  888   888  888ooo888  888     
 888    .88P 888    .o  888   888  888   888  888    .o  888     
o888bood8P'  `Y8bod8P' o888o o888o `Y8bod88P\" `Y8bod8P' d888b    

This file is a part of BENDER project:
``Python-based Interactive Environment for Smart and Friendly Physics Analysis''

The project has been designed with the kind help from
Pere MATO and Andrey TSAREGORODTSEV. 

And it is based on the 
LoKi project: ``C++ ToolKit for Smart and Friendly Physics Analysis''

By usage of this code one clearly states the disagreement 
with the smear campaign of Dr.O.Callot et al.: 
``No Vanya's lines are allowed in LHCb/Gaudi software.''

"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-02-03"
__version__ = "$Revision$" 
# =============================================================================
print __doc__
# =============================================================================
my_area  = '/afs/cern.ch/user/i/ibelyaev/cmtuser'
my_opts  = my_area + "/Bender_v23r2/Phys/VBWork/work/work_Bc2rho/Bc.py"
#
## prepare job :
j = Job (
    #
    name         = 'MyJob',
    #
    application  = Bender (
    version      = 'v23r4'   ,
    module       = my_opts 
    ) ,
    #
    outputfiles  = [ SandboxFile ( '*.root' ) , SandboxFile ( '*.xml'  ) ] ,
    #
    backend      = Dirac () ,
    #
    splitter     = SplitByFiles ( filesPerJob   =     5 ,
                                  bulksubmit    =  True , 
                                  ignoremissing = False ) 
    )
#
## get data from BK:
q = BKQuery ( '/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping20/WGBandQSelection7/90000000/PSIX.MDST'   )
dataset = q.getDataset()

#
## set input data to job 
j.inputdata = dataset

#
## submit the job 
j.submit()


# =============================================================================
# The END 
# =============================================================================

