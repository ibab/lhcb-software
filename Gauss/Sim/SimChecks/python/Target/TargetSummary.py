##############################################################################
# Option file to run hadronic cross section checks with multiple targets.
# For moreusage informations: https://twiki.cern.ch/twiki/bin/view/LHCb/TargetStudy
# 
# Last modified: Luca Pescatore, 24/11/2014
##############################################################################



from string import *
from ROOT import *
from decimal import *

class Material :

    _name = ""
    _density = 0
    _A = 0
        
    def __init__ (self, name, density, A) :
        self._name = name
        self._density = density
        self._A = A
    
    
    def GetName(self) :
        return self._name

    def GetDensity(self) :
        return self._density
    
    def GetA(self) :
        return self._A
    
    def SetName(self,name) :
        self._name = name
    
    def SetDensity(self,density) :
        self._density = density
    
    def SetA(self,A) :
        self._A = A
    
    def GetSigmaDxOverPintFactor(self) :
        return self._A/self._density/60.22




class PGuns :

    _name = ""
    _latex = ""
    _charge = 0
    _pdgID = 0
            
    def __init__(self, name, latex, charge, pdgID) :
        self._name = name
        self._latex = latex
        self._charge = charge
        self._pdgID = pdgID
                    
    def GetName(self) :
        return self._name

    def GetLatex(self,opt) :

        ltx = ""
        if(opt == "$") :
            ltx = "$" + self._latex + "$"
        
        elif(opt == "LEG") :
            ltx = self._latex.replace("\\","#")
                    
        else :
            ltx = self._latex
        
        return ltx
        
        
    def GetCharge(self) :
        return self._charge

    def GetPDG(self) :
        return self._pdgID
        
    def SetName(self,name) :
        self._name = name
        
    def SetLatex(self,latex) :
        self._latex = latex
        
    def SetA(self,charge) :
        self._charge = charge



class Plotter :

    _dataTree = TTree("dataTree","dataTree")
    _all_materials = { "Be" : Material("Be",1.85,9.012), "Si" : Material("Si",2.329,28.085),
            "Cu" : Material("Cu",8.92,63.546), "Pb" : Material("Pb",11.34,207.2), "Al" : Material("Al",2.7,26.98) }
    _all_pguns = { "Piplus" : PGuns("Piplus","\\pi^{+}",1,211), "Piminus" : PGuns("Piminus","\\pi^{-}",-1,-211),
        "Kplus" : PGuns("Kplus","K^{+}",1,321), "Kminus" : PGuns("Kminus","K^{-}",-1,-321),
        "p" : PGuns("p","p",1,2212), "pbar" : PGuns("pbar","\\bar{p}",-1,-2212) }
    _materials = []
    _pguns = []
    _thicks = []
    _models = []
    _energies = []

    def __init__(self, path = "", outputPath = "", models = [], pguns = [], energies = [], materials = [], thicks = [], doprint = False) :

        if(models == [] and  pguns == [] and energies ==[] and materials == [] and thicks == []) :
            return;

        self._thicks = thicks
        self._models = models
        self._energies = energies
    
        gROOT.ProcessLine(
            "struct tvars_t {\
            int pGun, mat, model;\
            double energy, thickness;\
            double multi, multi_err;\
            double xsec, xsec_err;\
            double inel_xsec, inel_xsec_err;\
            double el_xsec, el_xsec_err;\
            double percPlus, percMinus, percNCh;\
            double multiNCh_nogamma, multi_gamma, multiNCh;\
            } ")

        tvars = tvars_t()

        dataTree.Branch("pGun",AddressOf(tvars,"pGun"),"pGun/I")
        dataTree.Branch("energy",AddressOf(tvars,"energy"),"energy/D")
        dataTree.Branch("thickness",AddressOf(tvars,"thickness"),"thickness/D")
        dataTree.Branch("material",AddressOf(tvars,"mat"),"material/I")
        dataTree.Branch("model",AddressOf(tvars,"model"),"model/I")
        dataTree.Branch("multi",AddressOf(tvars,"multi"),"multi/D")
        dataTree.Branch("multi_err",AddressOf(tvars,"multi_err"),"multi_err/D")
        dataTree.Branch("xsec",AddressOf(tvars,"xsec"),"xsec/D")
        dataTree.Branch("xsec_err",AddressOf(tvars,"xsec_err"),"xsec_err/D")
        dataTree.Branch("inel_xsec",AddressOf(tvars,"inel_xsec"),"inel_xsec/D")
        dataTree.Branch("inel_xsec_err",AddressOf(tvars,"inel_xsec_err"),"inel_xsec_err/D")
        dataTree.Branch("el_xsec",AddressOf(tvars,"el_xsec"),"el_xsec/D")
        dataTree.Branch("el_xsec_err",AddressOf(tvars,"el_xsec_err"),"el_xsec_err/D")
        dataTree.Branch("percPlus",AddressOf(tvars,"percPlus"),"percPlus/D")
        dataTree.Branch("percMinus",AddressOf(tvars,"percMinus"),"percMinus/D")
        dataTree.Branch("multi_gamma",AddressOf(tvars,"multi_gamma"),"multi_gamma/D")
        dataTree.Branch("percNCh",AddressOf(tvars,"percNCh"),"percNCh/D")
        dataTree.Branch("multiNCh",AddressOf(tvars,"multiNCh"),"multiNCh/D")
        dataTree.Branch("multiNCh_nogamma",AddressOf(tvars,"multiNCh_nogamma"),"multiNCh_nogamma/D")

        rootFileName = outputPath+"/TargetsPlots.root"
        rootin = TFile(path+"TargetOutput.root")
        nt = rootin.Get("GiGa.TrackSeq.GaussTargetMultiplicity/tree")
        rootout = TFile(rootFileName,"RECREATE")
        getcontext().prec = 2

        for m in range(0,len(self._models)) :
        
            print "\n##############################\n  Analysing ", self._models[m], " data\n##############################\n "

            dir0 = rootout.mkdir(self._models[m]);
            dir0.cd()
            
            xsec_txtout = open(outputPath+"/XsecSummary_"+self._models[m]+".txt","w")
            multi_txtout = open(outputPath+"/MultiSummary_"+self._models[m]+".txt","w")
    
            xsec_txtout.write( "\\begin{tabular}{ c | c | c | c | c  } \n" )
            xsec_txtout.write( "PGun in Material & $N^{inel}/N^{gen}$ ($10^{-3}$) & $\\sigma_{inel}$ (mb) & $N^{el}/N^{gen}$ ($10^{-3}$) & $\\sigma_{el}$ (mb) \\\\ \\hline \n")
    
            multi_txtout.write( "\\begin{tabular}{ c | c | c } \n" )
            multi_txtout.write( "PGun in Material & $< Mult >$ & RMS \\\\ \\hline \n" )
    
    
            for g in self._energies :
    
                rootout.cd()
                dir1 = dir0.mkdir("E_"+str(g)+"GeV");
                dir1.cd();

                xsec_txtout.write( "\\multicolumn{5}{c}{Energy = %i GeV }} \\\\ \n" % g )
                multi_txtout.write( "\\multicolumn{3}{c}{Energy = %i GeV }} \\\\ \n" % g ) 
    
                for j in self._thicks:

                    dir2 = dir1.mkdir("Thick_"+str(j)+"mm");
                    dir2.cd();

                    for i in materials :
                        for k in pguns :
                
                            matname = self._all_materials[i].GetName()
                            pname = self._all_pguns[k].GetName()

                            print "\nAnalysing... ", g, "GeV", pname, " in ", j, "mm of ", matname
                            
                            dir3 = dir2.mkdir(pname+"_in"+matname);

                            res = doMultiHistos(nt,dir3,ord(self._models[m][0]),ord(matname[0]),j,self._all_pguns[k].GetPDG(),g)

                            Dx = j
                            SigmaOverPint = 1000*self._all_materials[i].GetSigmaDxOverPintFactor() / Dx
        
                            head = '{pgun} in {thick:.0f} mm {material}'.format(
                                    pgun = self._all_pguns[k].GetLatex("$"),
                                    thick = j,
                                    material = matname )

                            xsec_txtout.write( '{0} & $ {1:.2f} \pm {2:.2f}$ & $ {3:.2f} \pm {4:.2f}$ & $ {5:.2f} \pm {6:.2f}$ & $ {7:.2f} \pm {8:.2f}$ \\\\ \n'
                                    .format(head, res[1] * 1e3, res[2] * 1e3, res[1] * SigmaOverPint * 1e3, res[2] * SigmaOverPint * 1e3,
                                        res[3] * 1e3, res[4] * 1e3, res[3] * SigmaOverPint * 1e3, res[4] * SigmaOverPint * 1e3) )

                            multi_txtout.write( '{0} & {1:.1f} & {2:.1f}  \\\\ \n'.format(head,res[5],res[6]) )
    
                            tvars.pGun = self._all_pguns[k].GetPDG()
                            tvars.energy = g
                            tvars.thickness = Dx
                            tvars.mat = ord(matname[0])
                            tvars.model = ord(self._models[m][0])

                            tvars.multi = res["multi"][0]
                            tvars.multi_err = res["multi"][1]
                            
                            tvars.xsec = res["xsec_el"][0]+res["xsec_inel"][0]
                            tvars.xsec_err = res["xsec_el"][1]+res["xsec_inel"][1]

                            tvars.inel_xsec  = res["xsec_inel"][0]
                            tvars.inel_xsec_err = res["xsec_inel"][1]

                            tvars.el_xsec = res["xsec_el"][0]
                            tvars.el_xsec_err = res["xsec_el"][1]

                            tvars.percNCh = res["NChPerc"]*100.
                            tvars.percPlus = res["PlusPerc"]*100.
                            tvars.percMinus = res["MinusPerc"]*100.
                            tvars.multiNCh = res["NChMulti"]
                            tvars.multiNCh_nogamma = res["NChMulti_nogamma"]
                            tvars.multi_gamma = res["Ngamma"]
        
                            dataTree.Fill()


                    xsec_txtout.write( "\\hline \n" )
                    multi_txtout.write( "\\hline \n" )


        xsec_txtout.write( "\\end{tabular} \n" )
        multi_txtout.write( "\\end{tabular} \n" )
        
        
        rootout.cd()
        dataTree.Write("summaryTree")
        rootout.Close();

gROOT.ProcessLine(
        "struct vars_t {\
        Double_t  endx,endy,endz,prox,proy,proz;\
        int  cptype,cpstype;\
        int  trid,trpdg,ndau;\
        int  dirparent[100],isinside[100],parent[100];\
        int eptype[100],epstype[100],daupdg[100];\
        Double_t daukine[100];\
        Double_t daucharge[100];\
    }")

part_dict = { 2212 : 0, -2212 : 1, 2112 : 2, 321 : 3, -321 : 4, 211 : 5, -211 : 6, 111 : 7, 221 : 8, 3122 : 9, 310 : 10, 22 : 11 }



def doMultiHistos(nt, curdir, mod, mat, Dx, pgun, eng) :

    avg_nChPerc = 0
    avg_ChPlusPerc = 0 
    avg_ChMinusPerc = 0 

    vars = vars_t()

    nt.SetBranchAddress("TrackID",AddressOf(vars,"trid"))
    nt.SetBranchAddress("TrackPDG",AddressOf(vars,"trpdg"))
    nt.SetBranchAddress("CreatorProcessSubType",AddressOf(vars,"cpstype"))
    nt.SetBranchAddress("CreatorProcessType",AddressOf(vars,"cptype"))
    nt.SetBranchAddress("Produced_at_x",AddressOf(vars,"prox"))
    nt.SetBranchAddress("Produced_at_y",AddressOf(vars,"proy"))
    nt.SetBranchAddress("Produced_at_z",AddressOf(vars,"proz"))
    nt.SetBranchAddress("Ended_at_x",AddressOf(vars,"endx"))
    nt.SetBranchAddress("Ended_at_y",AddressOf(vars,"endy"))
    nt.SetBranchAddress("Ended_at_z",AddressOf(vars,"endz"))
    nt.SetBranchAddress("NDaugthers",AddressOf(vars,"ndau"))
    nt.SetBranchAddress("ParentID",AddressOf(vars,"parent"))
    nt.SetBranchAddress("directParent",AddressOf(vars,"dirparent"))
    nt.SetBranchAddress("IsInside",AddressOf(vars,"isinside"))
    nt.SetBranchAddress("EndingProcessSubType",AddressOf(vars,"epstype"))
    nt.SetBranchAddress("EndingProcessType",AddressOf(vars,"eptype"))
    nt.SetBranchAddress("DaughterPDG",AddressOf(vars,"daupdg"))
    nt.SetBranchAddress("DaughterKinE",AddressOf(vars,"daukine"))
    nt.SetBranchAddress("DaughterCharge",AddressOf(vars,"daucharge"))

    curdir.cd()

    p_endsubproc = TH1D("endsubproc","p_endsubproc", 100, 0, 200)
    in_dau_mult = TH1D("in_dau_mult","Multi of inelastic daughters", 50, 0, 100)
    in_dau_mult.GetXaxis().SetTitle("Multi")
    in_dau_mult.GetYaxis().SetTitle("N evts")
    el_dau_mult = TH1D("el_dau_mult","Multi of elastic daughters", 5, 0, 5)
    in_nchdau_mult = TH1D("in_chdau_mult","Multi of neutral inelastic daughters", 50, 0, 100)
    in_nchdau_mult.GetXaxis().SetTitle("Multi")
    in_nchdau_mult.GetYaxis().SetTitle("N evts")
    in_dau_gamma = TH1D("in_dau_gamma","Multi gamma inelastic daughters", 50, 0, 100)
    in_dau_gamma.GetXaxis().SetTitle("Multi")
    in_dau_gamma.GetYaxis().SetTitle("N evts")
    in_nchdau_nogamma = TH1D("in_chdau_nogamma","Multi of neutral inelastic daughters but no gamma", 50, 0, 100)
    in_nchdau_nogamma.GetXaxis().SetTitle("Multi")
    in_nchdau_nogamma.GetYaxis().SetTitle("N evts")
    in_chplusdau_mult = TH1D("in_chplusdau_mult","Multi of positively charged inelastic daughters", 50, 0, 100)
    in_chplusdau_mult.GetXaxis().SetTitle("Pos Multi")
    in_chplusdau_mult.GetYaxis().SetTitle("N evts")
    in_chminusdau_mult = TH1D("in_chminusdau_mult","Multi of negatively charged inelastic daughters", 50, 0, 100)
    in_chminusdau_mult.GetXaxis().SetTitle("Neg Multi")
    in_chminusdau_mult.GetYaxis().SetTitle("N evts")
    el_chdau_mult = TH1D("el_chdau_mult","Multi of charged elastic daughters", 5, 0, 5)
    pdg_dau = TH1D("pdg_dau","PDG ID of daughters (only inelastic, no heavy nuclei)", 8000, -4000, 4000)
    pdg_dau_high = TH1D("pdg_dau_high","pdg_dau_high", 150000, -150000, 150000)
    dau_gammas_kine = TH1D("Ekine_gammas","E of gammas", 100, 0., 15)
    dau_gammas_kine.GetXaxis().SetTitle("E_{kine} (MeV)")
    dau_gammas_kine.GetYaxis().SetTitle("N evts")
    dau_high_kine = TH1D("Ekine_high","Ekine_high", 100, 0., 1000)
    dau_kine = TH1D("Ekine_daughters","E of daughters", 200, 0., 20000)
    dau_kine.GetXaxis().SetTitle("E_{kine} (MeV)")
    dau_kine.GetYaxis().SetTitle("N evts")
    end_z = TH1D("end_z","End z coord", 2000, -1000, 1000)
    end_z_inside = TH1D("end_z_inside","End z coord", 2000, -1000, 1000)
    end_proc = TH1I("end_process","End Process", 4, 0, 4)
    end_proc.GetXaxis().SetBinLabel(1,"Others")
    end_proc.GetXaxis().SetBinLabel(2,"Elastic")
    end_proc.GetXaxis().SetBinLabel(3,"Inelastic")
    end_proc.GetXaxis().SetBinLabel(4,"Decay")

    pdg_dau_named = TH1D("pdg_dau_named","ID of daughters (only inelastic, no heavy nuclei)", 13, 0, 13)
    pdg_dau_named.GetXaxis().SetBinLabel(1,"p")
    pdg_dau_named.GetXaxis().SetBinLabel(2,"#bar{p}")
    pdg_dau_named.GetXaxis().SetBinLabel(3,"n")
    pdg_dau_named.GetXaxis().SetBinLabel(4,"K^{+}")
    pdg_dau_named.GetXaxis().SetBinLabel(5,"K^{-}")
    pdg_dau_named.GetXaxis().SetBinLabel(6,"#pi^{+}")
    pdg_dau_named.GetXaxis().SetBinLabel(7,"#pi^{-}")
    pdg_dau_named.GetXaxis().SetBinLabel(8,"#pi^{0}")
    pdg_dau_named.GetXaxis().SetBinLabel(9,"#eta")
    pdg_dau_named.GetXaxis().SetBinLabel(10,"#Lambda")
    pdg_dau_named.GetXaxis().SetBinLabel(11,"K^{0}_{s}")
    pdg_dau_named.GetXaxis().SetBinLabel(12,"#gamma")
    pdg_dau_named.GetXaxis().SetBinLabel(13,"Others")

    in_nch_perc = TH1D("in_nch_perc","Percent of neutral inelastic daughters", 50, 0, 1)
    in_chplus_perc = TH1D("in_chplus_perc","Percent of positively charged inelastic daughters", 50, 0, 1)
    in_chminus_perc = TH1D("in_chminus_perc","Multi of negatively charged inelastic daughters", 50, 0, 1)

    select_template = "CreatorProcessType == -2 && PhysicsList == {mod} && TargetMaterial == {mat} && ProjectilePdgID == {pgun} && TargetThickness == {Dx} && ProjectileEnergy == {eng}"
    select = select_template.format(pgun=pgun,mod=mod,mat=mat,Dx=Dx,eng=eng)
    print select

    varexp = "elist"
    nt.SetEntryList(0)
    nt.Draw(">>"+varexp,select,"entrylist")
    list = gDirectory.Get(varexp)
    ntracks = list.GetN()
    nt.SetEntryList(list)
    
    countel = 0
    countinel = 0
    countdecay = 0
    totP = ntracks
    totInside = 0
    
    for i in range(0,ntracks) :
        
        nt.GetEntry(list.GetEntry(i))

        in_daughters = 0
        el_daughters = 0
        in_nchDaughters = 0
        in_dauGamma = 0
        in_nchDaughters_nogamma = 0
        in_chPlusDaughters = 0
        in_chMinusDaughters = 0
        el_chDaughters = 0
        
        
        for nd in range(0,vars.ndau) :    #Loop on daughters
        
            end_z.Fill(vars.endz)
            if (vars.isinside[nd]==0) :
                continue

            totInside += 1
            end_z_inside.Fill(vars.endz)
                
            if (vars.epstype[nd] == 111) :  #Elastic interaction
            
                el_daughters+=1 
                end_proc.Fill(1)
                if(vars.daucharge[nd] == 0) :
                    el_chDaughters+=1
                
            elif (vars.epstype[nd] == 121) : #Inelastic interaction
            
                in_daughters+=1
                end_proc.Fill(2)
                if(vars.daucharge[nd] == 0) :
                    in_nchDaughters+=1 
                    if(vars.daupdg[nd]!=22) :
                        in_nchDaughters_nogamma+=1
                elif(vars.daucharge[nd] > 0) :
                    in_chPlusDaughters+=1
                elif(vars.daucharge[nd] < 0) :
                    in_chMinusDaughters+=1


                if(vars.daupdg[nd]>1000000000) : #Is heavy nucleus?
                
                    pdg_dau_high.Fill(vars.daupdg[nd]-1000000000)
                    dau_high_kine.Fill(vars.daukine[nd])
                
                else :
                
                    pdg_dau.Fill(vars.daupdg[nd])
                    dau_kine.Fill(vars.daukine[nd])
                    if( vars.daupdg[nd]==22 ) :
                        dau_gammas_kine.Fill(vars.daukine[nd])
                        pdg_dau_named.Fill(11)
                        in_dauGamma+=1
                    elif vars.daupdg[nd] in part_dict :
                        pdg_dau_named.Fill(part_dict[vars.daupdg[nd]])
                    else :
                        pdg_dau_named.Fill(12)
                
            elif(vars.epstype[nd]==201) :   #decays
                end_proc.Fill(3)
                countdecay+=1
    
            else :
                end_proc.Fill(0)
                p_endsubproc.Fill(vars.epstype[nd])
        
        if(el_daughters > 0) :
            el_dau_mult.Fill(el_daughters)
            countel+=1
            if(el_chDaughters > 0) :
                el_chdau_mult.Fill(el_chDaughters)
        
        if(in_daughters > 0) :
        
            if(in_nchDaughters > 0) :
                in_nchdau_mult.Fill(in_nchDaughters)
            if(in_dauGamma>0) :
                in_dau_gamma.Fill(in_dauGamma)
            if(in_nchDaughters_nogamma > 0) :
                in_nchdau_nogamma.Fill(in_nchDaughters_nogamma)
            if(in_chPlusDaughters > 0) :
                in_chplusdau_mult.Fill(in_chPlusDaughters)
            if(in_chMinusDaughters > 0) :
                in_chminusdau_mult.Fill(in_chMinusDaughters)
        
            countinel+=1
            in_dau_mult.Fill(in_daughters)

            avg_nChPerc += float(in_nchDaughters) / in_daughters
            avg_ChPlusPerc += float(in_chPlusDaughters) / in_daughters
            avg_ChMinusPerc += float(in_chMinusDaughters) / in_daughters

            in_nch_perc.Fill(in_nchDaughters / in_daughters)
            in_chplus_perc.Fill(in_chPlusDaughters / in_daughters)
            in_chminus_perc.Fill(in_chMinusDaughters / in_daughters)
                
    Xsecinel = countinel/float(totP)
    Xsecel = countel/float(totP)
    XsecinelErr = TMath.Sqrt(Xsecinel*(1-Xsecinel)/float(totP))
    XsecelErr = TMath.Sqrt(Xsecel*(1-Xsecel)/float(totP))
    multi = in_dau_mult.GetMean()
    multi_err = in_dau_mult.GetMeanError()
    print "Tot projectiles = ", totP
    print "Pint inel = ", '{0:4.5f} +/- {1:4.5f}'.format(Xsecinel,XsecinelErr)
    print "Pint el = ", '{0:4.5f} +/- {1:4.5f}'.format(Xsecel,XsecelErr)
    print "< Multi > = ", '{0:3.1f} +/- {1:3.1f}'.format(multi,multi_err)

    result = { "total"     :  totP, 
               "xsec_inel" :  [ Xsecinel, XsecinelErr ] , 
               "xsec_el"   :  [ Xsecel, XsecelErr ] , 
               "multi"     :  [ multi, multi_err ] ,
               "NChPerc"   :  0,
               "PlusPerc" :  0,
               "MinusPerc"   :  0,
               "NChMulti"  :  in_nchdau_mult.GetMean(),
               "NChMulti_nogamma"  :  in_nchdau_nogamma.GetMean(),
               "Ngamma"    :   in_dau_gamma.GetMean()
                   }
    
    if(countinel>0)
        reult["NChPerc"]   = avg_nChPerc/countinel
        reult["PlusPerc"]  = avg_ChPlusPerc/countinel
        reult["MinusPerc"] = avg_ChMinusPerc/countinel
        

    curdir.Write()
    p_endsubproc.Write()
    in_dau_mult.Write()
    el_dau_mult.Write()
    in_nchdau_mult.Write()
    in_chplusdau_mult.Write()
    in_chminusdau_mult.Write()
    el_chdau_mult.Write()
    pdg_dau.Write()
    pdg_dau_high.Write()
    dau_gammas_kine.Write()
    dau_high_kine.Write()
    dau_kine.Write()
    end_proc.Write()
    end_z.Write()
    end_z_inside.Write()

    return result



if __name__ == "__main__" :
    models = ['QGSP_BERT','FTFP_BERT']
    path = "TargetOutput/"
    outputPath = path
    energies = [1,3,10]
    materials = ["Al"]
    pguns = ["p","pbar"]
    thicks = [1]
    p = Plotter(path, outputPath, models, pguns, energies, materials, thicks)



