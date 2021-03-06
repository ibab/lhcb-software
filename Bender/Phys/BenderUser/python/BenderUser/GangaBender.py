#!/usr/bin/env ganga
# -*- coding: utf-8 -*-
# =============================================================================
# $Id:$ 
# =============================================================================
# @file GangaBenderScript.py
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
#                   $Revision: 201065 $
# Last modification $Date: 2016-02-06 16:55:03 +0100 (Sat, 06 Feb 2016) $
#                by $Author: ibelyaev $
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
__version__ = "$Revision: 201065 $" 
# =============================================================================
print __doc__
# =============================================================================
my_module = "MyModule.py"
#
## prepare the job template 
#
t = JobTemplate (
    #
    name         = 'MyJob',
    #
    application  = Bender (
    version      = 'v28r4'   ,
    module       = my_module 
    ) ,
    #
    outputfiles  = [ LocalFile ( '*.root' ) ,
                     LocalFile ( '*.txt'  ) , 
                     LocalFile ( '*.db*'  ) , 
                     LocalFile ( '*.xml'  ) ] ,
    #
    backend      = Dirac () ,
    #
    splitter     = SplitByFiles ( filesPerJob   =     5 ,
                                  bulksubmit    =  True , 
                                  ignoremissing = False ) 
    )
#
## get data from BK:
#
qDown_2011 = BKQuery ( '/LHCb/Collision11/Beam3500GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping21r1/WGBandQSelection11/90000000/PSIX.MDST'     )
qUp_2011   = BKQuery ( '/LHCb/Collision11/Beam3500GeV-VeloClosed-MagUp/Real Data/Reco14/Stripping21r1/WGBandQSelection11/90000000/PSIX.MDST'       )
qDown_2012 = BKQuery ( '/LHCb/Collision12/Beam4000GeV-VeloClosed-MagDown/Real Data/Reco14/Stripping21/WGBandQSelection11/Merge/90000000/PSIX.MDST' )
qUp_2012   = BKQuery ( '/LHCb/Collision12/Beam4000GeV-VeloClosed-MagUp/Real Data/Reco14/Stripping21/WGBandQSelection11/Merge/90000000/PSIX.MDST'   )

tasks = {
    'Down/11': qDown_2011.getDataset() ,
    'Down/12': qDown_2012.getDataset() ,
    'Up/11'  : qUp_2011  .getDataset() ,
    'Up/12'  : qUp_2012  .getDataset() ,
    }

for  task in tasks :

    ## create the job from template 
    j = Job ( t )

    ## redefine the name for the job
    j.name = task
    
    ## set input data to job 
    j.inputdata = tasks[ task ]
    
    params = {}
    if   0<= task.find ( '11' ) : params = { 'Year' : '2011' }
    elif 0<= task.find ( '12' ) : params = { 'Year' : '2012' }
    
    j.application.params = params 
    
    ## submit the job 
    j.submit()
    
# =============================================================================
# The END 
# =============================================================================

