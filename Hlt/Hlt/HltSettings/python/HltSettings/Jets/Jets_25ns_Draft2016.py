from GaudiKernel.SystemOfUnits import MeV, GeV, picosecond, mm

class Jets_25ns_Draft2016:
   """
   Threshold settings for Hlt2 jets lines for 2016 25 ns data-taking.
   
   WARNING :: DO NOT EDIT WITHOUT PERMISSION OF THE AUTHORS
   
   @author P. Ilten and M. Williams
   @date 2016-02-13
   """
   
   __all__ = ( 'ActiveHlt2Lines' )
   
   def ActiveHlt2Lines(self) :
      """Returns a list of active lines."""
      
      lines = [
         'Hlt2JetsDiJet',
         'Hlt2JetsDiJetSV',
         'Hlt2JetsDiJetSVSV',
         'Hlt2JetsDiJetSVMu',
         'Hlt2JetsDiJetMuMu'
         ]
      
      return lines
   
   def Thresholds(self) :
      """Return the trigger thresholds."""
      
      d = {}
      
      from Hlt2Lines.Jets.Lines import JetsLines
      d.update({
            JetsLines : {
               'Common' : {
                  'GHOSTPROB' : 0.2,
                  'DPHI'      : 1.5,
                  'SV_VCHI2'  : 10,
                  'SV_TRK_PT' : 500*MeV,
                  'SV_TRK_IPCHI2' : 16,
                  'SV_FDCHI2' : 25,
                  'MU_PT'     : 1000*MeV,
                  'MU_PROBNNMU'  : 0.5,
                  'JET_PT' : 17*GeV},
               
               'JetBuilder' : {
                  'JetPtMin' : 5 * GeV,
                  'JetInfo' : False,
                  'JetEcPath' : ""}}})
      
      return d
