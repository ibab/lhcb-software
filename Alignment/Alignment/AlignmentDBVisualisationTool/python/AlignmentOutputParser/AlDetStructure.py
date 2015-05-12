#####################################
#
# AlDetStructure
#
# An helper class with references to
# the detector structure and the
# alignable objects
#
#####################################

# general imports

# Dicts

# BEGIN ALDETSTRUCTURE
class AlDetStructure:
    """
    An helper class with references to the detector structure and the alignable objects
    """
    def __init__(self):
        """
            Initilisation
        """
        self.Alignables = []
        self.Tracker()
        self.Velo()
        self.TT()
        self.IT()
        self.OT()
        self.Muon()
        return
    
    ## Global ###############################################################################
    def Tracker(self):
        """
            Define Tracker
        """
        self.Alignables += ['Tracker','Velo','TT','IT','OT','Muon']
        return
    
    ##  Velo  ###############################################################################
    def Velo(self):
        """
            Define Velo
        """
        self.Alignables += ['Tracker','Velo','TT','IT','OT','Muon']
        return
    def VeloHalf(self, name):
        """
            Define the structure of a velo half
        """
        if not name in ['Left','Right']: return
        

    ##   TT   ###############################################################################
    def TT(self):
        """
            Define TT
            Layers(X1,U,V,X2) -> Modules
        """
        for station in ['TTa','TTb']: self.TTStation(station)
        return
    # TT Station
    def TTStation(self, name):
        """
            Define TT stations
                TTa, TTb
        """
        if not name in ['TTa','TTb']: return
        for layer in ['U','X','V']: self.TTLayer(layer, name)
        return
    # TT Layer
    def TTLayer(self, layer, station):
        """
            Define TT layer
            TT(a/b)(XUV)Layer
        """
        if  ((station=='TTa' and not layer in ['U','X']) or
             (station=='TTb' and not layer in ['V','X'])): return
        self.Alignables += [station+'/'+layer+'Layer']
        self.TTModule(layer, station)
        return
    # TT Module
    def TTModule(self, layer, station):
        """
            Define TT modules
            TT(a/b)(X/U/V)LayerR(1/2/3)
             - TTa:
                    R1         R2         R3
                1 2 3 4 5 6 | 1 2 3 | 1 2 3 4 5 6
                
             - TTb:
                     R1          R2          R3
                1 2 3 4 5 6 7 | 1 2 3 | 1 2 3 4 5 6 7
        """
        if ( (station=='TTa' and not layer in ['U','X']) or
             (station=='TTb' and not layer in ['V','X'])): return
        # first R1 and R3
        for im in xrange(1,7 if station=='TTa' else 8):
            for ir in [1,3]:
                self.Alignables += [station+layer+'LayerR'+str(ir)+'Module'+str(im)]
        # then R2
        for im in [1,3]:
            self.Alignables += [station+layer+'LayerR2Module'+str(im)]
        # the R2 central module is split in 2
        self.Alignables += [station+layer+'LayerR2Module2'+str(tb) for tb in ['T','B']]
        return
            
    ##   IT   ###############################################################################
    def IT(self):
        """
            Define IT
                Stations(1/2/3)
        """
        for i in xrange(1,4): self.ITStation(i)
        return
    # IT Station
    def ITStation(self, idx):
        """
            Define IT station
             IT/Station(1/2/3)
        """
        if not idx in xrange(1,4): return
        self.Alignables += ['IT/Station%d' % idx]
        for box in ['ASide','CSide','Top','Bottom']: self.ITBox(box, idx)
        return
    # IT Box
    def ITBox(self, box, station):
        """
            Define IT Box
            IT/Station(1/2/3)/(ASide/CSide/Top/Bottom)Box
        """
        if not box in ['ASide','CSide','Top','Bottom']: return
        self.Alignables += ['IT/Station%d/%sBox' % (station,box)]
        for layer in ['X1','U','V','X2']: self.ITLayer(layer, box, idx)
        return
    # IT Double Layer
    def ITDoubleLayer(self, layer, box, station):
        """
            Define IT Double Layer
            IT/Station(1/2/3)/(ASide/CSide/Top/Bottom)Box/Layer(X1U/VX2)
            """
        if (not layer in ['X1U','VX2'] or
            not box in ['ASide','CSide','Top','Bottom'] or
            not station in xrange(1,4)): return
        self.Alignables += ['IT/Station%d/%sBox/Layer%s' % (station,box,layer)]
        return
    # IT Layer
    def ITLayer(self, layer, box, station):
        """
            Define IT Layer
            IT/Station(1/2/3)/(ASide/CSide/Top/Bottom)Box/Layer(X1/U/V/X2)
            """
        if (not layer in ['X1','U','V','X2'] or
            not box in ['ASide','CSide','Top','Bottom'] or
            not station in xrange(1,4)): return
        self.Alignables += ['IT/Station%d/%sBox/Layer%s' % (station,box,layer)]
        self.ITLadders(layer,box, station)
        return
    # IT Ladders
    def ITLadders(self, layer, box, station):
        """
            Define IT Ladders
            IT/Station(1/2/3)/(ASide/CSide/Top/Bottom)Box/Layer(X1/U/V/X2)/Ladder(1/2/3/4/5/6/7)
            """
        if (not layer in ['X1','U','V','X2'] or
            not box in ['ASide','CSide','Top','Bottom'] or
            not station in xrange(1,4)): return
        for idx in xrange(1,8): self.Alignables += ['IT/Station%d/%sBox/Layer%s/Ladder%d' % (station,box,layer,station)]
        return
    
    ##   OT   ###############################################################################
    def OT(self):
        """
            Define TT
            Layers(X1,U,V,X2) -> Modules
            """
        self.Alignables += ['Tracker','Velo','TT','IT','OT','Muon']
        return
    
    ##  Muon  ###############################################################################
    def Muon(self):
        """
            Define TT
            Layers(X1,U,V,X2) -> Modules
            """
        self.Alignables += ['Tracker','Velo','TT','IT','OT','Muon']
        return


# END ALDETSTRUCTURE
