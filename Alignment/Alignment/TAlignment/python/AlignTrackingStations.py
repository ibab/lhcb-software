from Alignment import run

#C = '''{{}}'''
C = [[]]
# fix tz, rx, and ry of each station
# FixTxRxRyOfEachStationGlobalTxTyTzRxRyRz = ''' { {0,0,1, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0}, 
#                                                  {0,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#                                                  {0,0,0, 0,1,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,1, 0,0,0,  0,0,0, 0,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,0, 1,0,0,  0,0,0, 0,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,0, 0,1,0,  0,0,0, 0,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,1, 0,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 1,0,0,   0},
#                                                  {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,1,0,   0},
#                                                  {1,0,0, 0,0,0,  1,0,0, 0,0,0,  1,0,0, 0,0,0,   0},
#                                                  {0,1,0, 0,0,0,  0,1,0, 0,0,0,  0,1,0, 0,0,0,   0},
#                                                  {0,0,1, 0,0,0,  0,0,1, 0,0,0,  0,0,1, 0,0,0,   0},
#                                                  {0,0,0, 1,0,0,  0,0,0, 1,0,0,  0,0,0, 1,0,0,   0},
#                                                  {0,0,0, 0,1,0,  0,0,0, 0,1,0,  0,0,0, 0,1,0,   0},
#                                                  {0,0,0, 0,0,1,  0,0,0, 0,0,1,  0,0,0, 0,0,1,   0} }'''
#  fix z and rx of each station
#C = ''' { {0,0,1, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0}  ,
#          {0,0,0, 0,0,0,  0,0,1, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,0,0, 0,0,0,  0,0,0, 1,0,0,  0,0,0, 0,0,0,   0}  ,
#          {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,1, 0,0,0,   0},
#          {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 1,0,0,   0}  }  '''

#  fix ty,tz and rx,ry,rz of each station
# C = ''' { {0,1,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,1, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,1,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,1,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,1,0, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,1, 0,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 1,0,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,1,0,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,1,  0,0,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,1,0, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,1, 0,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 1,0,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,1,0,   0},
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,1,   0}  }  '''

##  fix z-position of center of gravity of T
#C = ''' { {0,0,1, 0,0,0,  0,0,1, 0,0,0,  0,0,1, 0,0,0,   0}  }  '''

##  fix z-position of center of gravity of T and rx of each station
# C = ''' { {0,0,1, 0,0,0,  0,0,1, 0,0,0,  0,0,1, 0,0,0,   0}  ,
#           {0,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0}  ,
#           {0,0,0, 0,0,0,  0,0,0, 1,0,0,  0,0,0, 0,0,0,   0}  ,
#           {0,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 1,0,0,   0}  }  '''
##

#  fix center of gravity of T
#C = ''' { {1,0,0, 0,0,0,  1,0,0, 0,0,0,  1,0,0, 0,0,0,   0},
#          {0,1,0, 0,0,0,  0,1,0, 0,0,0,  0,1,0, 0,0,0,   0},
#          {0,0,1, 0,0,0,  0,0,1, 0,0,0,  0,0,1, 0,0,0,   0}  }  '''
#

##  fix position of T1
#C = ''' { {1,0,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,1,0, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},"/dd/Structure/LHCb/AfterMagnetRegion/T/OT"
#          {0,0,1, 0,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0}  }  '''
#

##  fix position  & orientation of T1
#C = ''' { {1,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,1,0, 0,1,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,0,1, 0,0,1,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0}  }  '''

##  fix position  & orientation of T1, and center of gravity of T
#C = ''' { {1,0,0, 1,0,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,1,0, 0,1,0,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {0,0,1, 0,0,1,  0,0,0, 0,0,0,  0,0,0, 0,0,0,   0},
#          {1,0,0, 0,0,0,  1,0,0, 0,0,0,  1,0,0, 0,0,0,   0},
#          {0,1,0, 0,0,0,  0,1,0, 0,0,0,  0,1,0, 0,0,0,   0},
#          {0,0,1, 0,0,0,  0,0,1, 0,0,0,  0,0,1, 0,0,0,   0}  }  '''

# extra= '''
# UpdateManagerSvc.ConditionsOverride +=  {
#            "Conditions/Alignment/OT/T1Station := double_v dPosXYZ =  0.3  1.5 0 ; double_v dRotXYZ = 0  0.0001  0.0005 ;",
#            "Conditions/Alignment/OT/T2Station := double_v dPosXYZ = -0.1  1.0 0 ; double_v dRotXYZ = 0  0.0005  0.0001 ; ",
#            "Conditions/Alignment/OT/T3Station := double_v dPosXYZ =  0.2 -1.0 0 ; double_v dRotXYZ = 0 -0.0001 -0.0005 ; "
# }; '''

# extra = '''UpdateManagerSvc.ConditionsOverride +=  {
#            "Conditions/Alignment/OT/T1Station := double_v dPosXYZ =  1   0  0 ; double_v dRotXYZ = 0  0  0 ;",
#            "Conditions/Alignment/OT/T2Station := double_v dPosXYZ =  0  -1  0 ; double_v dRotXYZ = 0  0  0 ; ",
#            "Conditions/Alignment/OT/T3Station := double_v dPosXYZ =  0   0  1 ; double_v dRotXYZ = 0  0  0 ; "
# }; '''

# extra = '''UpdateManagerSvc.ConditionsOverride +=  {
#            "Conditions/Alignment/OT/T1Station := double_v dPosXYZ =  0   0  0 ; double_v dRotXYZ = 0.001  0  0 ;",
#            "Conditions/Alignment/OT/T2Station := double_v dPosXYZ =  0   0  0 ; double_v dRotXYZ = 0  -0.001  0 ; ",
#            "Conditions/Alignment/OT/T3Station := double_v dPosXYZ =  0   0  0 ; double_v dRotXYZ = 0  0  0.001 ; "

# }; '''

stevesPerfectData = ['PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_0.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_1.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_2.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_3.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_4.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_5.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_6.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_7.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_8.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_9.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_10.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_11.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_12.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_13.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_14.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_15.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_16.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_17.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_18.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_19.dst',
		     'PFN:castor:/castor/cern.ch/user/s/sblusk/Brunel/7TeV_FieldOff_MinBias/dst/Brunel_7TeV_Boff_MinBias_1000_20.dst']

#otStations = '{"OTStations" : {"/dd/Structure/LHCb/AfterMagnetRegion/T/OT/T."}}'
#trackingStations = '{"TrackingStations":{"/dd/Structure/LHCb/BeforeMagnetRegion/Velo", "/dd/Structure/LHCb/AfterMagnetRegion/T/(IT|OT)"}}'
trackingStations = {"TrackingStations":["/dd/Structure/LHCb/BeforeMagnetRegion/Velo", "/dd/Structure/LHCb/AfterMagnetRegion/T/(IT|OT)"]}
run(filename="testTrackingStations", data=stevesPerfectData, nEvents=2, nIter=1, CondTag='DC06', MagOff=True, OTHitsOnly='false', Constraints=C, UseCorrelations=True, UseDrift=False, ApplyMS='true', extraOpts="", what=trackingStations, group=False)
