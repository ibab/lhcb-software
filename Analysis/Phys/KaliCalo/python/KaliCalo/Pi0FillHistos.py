#!/usr/bin/env python
# =============================================================================
"""
A module for creating and filling the histograms
"""
# =============================================================================
from ROOT import TPySelector, TH1F
from math import sqrt

# ========================== some global variables =========================
#== histograms and coefficients
FilledHistos={}                        # array of the pi0mass histograms
alam = {}                              # array of the calibration coefficients

# ========================== starting of the selector =========================
class MyPySelector( TPySelector ):
    """
    ROOT selector for filling the histograms
    """
    def Begin( self ):
        print 'py: beginning'

    def SlaveBegin( self, tree ):
        print 'py: slave beginning'

    #== a function for getting the values of calibration coefficients
    def CoefGet(self, coef, FilledHistos):
        """
        Gets the current coefficient values and deletes the histograms
        created during the previous iteration
        """
        for i in FilledHistos.keys():
            del FilledHistos[i]
        alam=coef

# ============================== Processing tree ==============================
    def Process( self, entry ):
        """
        Fills the histograms from a tree
        """
        #== for more convenience
        bamboo=self.fChain

        #== getting the next entry from the tree
        bamboo.GetEntry( entry )

        #== printout
        if((entry%1000)==0):
            print 'py: processing', entry

        #== cell (category) number
        ic1=bamboo.ind1
        ic2=bamboo.ind2

        #== creating the histograms and correction coefficients for the cell
        alam[ic1],FilledHistos[ic1] = HiCreate(alam,FilledHistos,ic1)
        alam[ic2],FilledHistos[ic2] = HiCreate(alam,FilledHistos,ic2)

        #== caculating the corrected mass
        corrMass = bamboo.m12*sqrt(alam[ic1]*alam[ic2])

        #== cuts
        MassCut=(corrMass>0.05)and(corrMass<0.2) # pi0 mass limits
        CellCut = (ic1 != ic2)                   # the two photons should not
                                                 # hit the same cell

        #== loop over all the hit cells
        for k in alam.keys():
            #== checking if at least one of the photons hits the current
            #== cell category and applying pt and prs cuts to it
            cut1=(ic1==k)
            cut2=(ic2==k)

            #== filling the histograms
            if (CellCut and MassCut and (cut1 or cut2)):
                FilledHistos[k].Fill(corrMass)   # for the corresponding cell

        #== return true
        return 1


    def SlaveTerminate( self ):
        print 'py: slave terminating'

    def Terminate( self ):
        print 'py: Terminate'

# =========== Return the histograms and coefficients (called from Pi0.py) =====
    # return the filled histograms
    def HiReturn( self ):
        """
        Returnes the filled histograms and calibration coefficients to Pi0.py
        """
        return FilledHistos,alam

# ==== Function for adding a histogram and a coefficient to the dictionary ====
def HiCreate(alam, FilledHistos, index):
    """
    Fills the arrays of calibration coefficients and histograms
    in case of the first iteration
    """
    #== histogram parameters
    nbins = 100                            # number of bins
    left  = 0.05                           # left mass limit
    right = 0.2                            # right mass limit

    #== filling the arrays of calibration coefficients and histograms
    #== in case of the first iteration
    if not alam.has_key(index):
        alam[index]=1.

    if not FilledHistos.has_key(index):
        FilledHistos[index] = TH1F("FH%i" % index,"pi0mass, cell%i" % index,nbins,left,right)
    
    return alam[index],FilledHistos[index]
### EOF
