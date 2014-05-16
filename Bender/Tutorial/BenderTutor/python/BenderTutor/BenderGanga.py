#!/usr/bin/env ganga
# =============================================================================
# $Id$ 
# =============================================================================
# @file BenderGanga.py
#
# Ganga script to submit Bender job 
#
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date 2014-05-16
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
"""
Ganga script to submit Bender Job  
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2014-02-03"
__version__ = "$Revision$" 
# =============================================================================
#
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


