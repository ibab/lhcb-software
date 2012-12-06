my_opts = '/Bender_v22r0/Phys/DiCharm/python/DiCharm/Bc2.py' 

j = Job (
    #
    name = 'Bc',
    #
    application = Bender  (
    #
    version           = 'v22r0'        ,
    #
    user_release_area = my_area        ,
    masterpackage     = 'Phys/DiCharm' ,
    #
    module            = my_area + my_opts ,
    events            = -1             ,
    ) ,
    #
    outputdata     = [ '*.csv' ] , 
    outputsandbox  = [ '*.root',
                       '*.txt' ,
                       '*.xml' ,
                       '*.db'  ] , 
    #
    backend        = Dirac () ,
    #
    splitter       = SplitByFiles ( filesPerJob   =    12 ,
                                    ignoremissing = False ) 
    )


j.application.params['strippingName'] = 'Stripping20'


mag_down_17b  = BKQuery ( '/LHCb/Collision11/Beam3500GeV-VeloClosed-MagDown/Real Data/Reco12/Stripping17b/90000000/DIMUON.DST' )

tasks = {
    #
    'JC/U17b'   : mag_up_17b    . getDataset () ,
    #
    }

keys = tasks.keys()
keys.sort ()

for key in keys :
    
    if 'new' != j.status  :  j = j.copy()
    
    j.name = key
    
    dset        = tasks[key] 
    j.inputdata = dset
    
    print ' name  %s #files %d ' % ( key , len(dset.files) ) 
    j.submit()


jobs 
print ' The END '


