#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Configurables import CreateHistDBPages
app = ApplicationMgr()
border=0.0005

topdir="/Velo/Summary/LumiRegionMon/"

################### TOP PAGE
createTopPageAlg = CreateHistDBPages("createTopPageAlg")
app.TopAlg.append(createTopPageAlg)

createTopPageAlg.PageBase   = topdir
createTopPageAlg.HistoBase  = ""
createTopPageAlg.PageDoc    = ["Velo beam and HLT occupancy monitor for A-side (top) and C-side (bottom)."]
createTopPageAlg.PageNames  = ["..TopSummaryPage.."]

createTopPageAlg.HistoNames = [["VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX","VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_trend",
                                  "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionXZ_trend","VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_vs_BunchId",
                                     "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_cumu","Hlt1VeloOccMon/VeloLiteClusterMonitor/nClusters_ASide_SmallRange",
                                       "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClustersVsZ_ASide",
                                        "VeloBeamMon/ASideLumiRegionMonitor/Ntrks","VeloBeamMon/ASideLumiRegionMonitor/Ntrks_used",
                                "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX","VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_trend",
                                  "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionXZ_trend","VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_vs_BunchId",
                                    "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_cumu","Hlt1VeloOccMon/VeloLiteClusterMonitor/nClusters_CSide_SmallRange",
                                      "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClustersVsZ_CSide",
                                        "VeloBeamMon/CSideLumiRegionMonitor/Ntrks","VeloBeamMon/CSideLumiRegionMonitor/Ntrks_used"
                                ]]

createTopPageAlg.HistoLinks = {"VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX":topdir+"/RefreshedVertexHistos",
                               "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_trend":topdir+"/LuminousRegionPosition",
                               "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionXZ_trend":topdir+"/LuminousRegionSlope",
                               "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_vs_BunchId":topdir+"/VerticesVsBunchId",
                               "VeloBeamMon/ASideLumiRegionMonitor/LumiRegionX_cumu":topdir+"/CumulativeVertexHistos",
                               "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClusters_ASide_SmallRange":topdir+"/NClusterHistos",
                               "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClustersVsZ_ASide":topdir+"/OccupancyHistos",
                               "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX":topdir+"/RefreshedVertexHistos",
                               "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_trend":topdir+"/LuminousRegionPosition",
                               "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionXZ_trend":topdir+"/LuminousRegionSlope",
                               "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_vs_BunchId":topdir+"/VerticesVsBunchId",
                               "VeloBeamMon/CSideLumiRegionMonitor/LumiRegionX_cumu":topdir+"/CumulativeVertexHistos",
                               "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClusters_CSide_SmallRange":topdir+"/NClusterHistos",
                               "Hlt1VeloOccMon/VeloLiteClusterMonitor/nClustersVsZ_CSide":topdir+"/OccupancyHistos"}

createTopPageAlg.PageLayout=[]
createTopPageAlg.PageLayout.append([0.00+border, 0.75+border, 0.25-border, 1.00-border])
createTopPageAlg.PageLayout.append([0.25+border, 0.75+border, 0.50-border, 1.00-border])
createTopPageAlg.PageLayout.append([0.50+border, 0.75+border, 0.75-border, 1.00-border])
createTopPageAlg.PageLayout.append([0.75+border, 0.75+border, 1.00-border, 1.00-border])
createTopPageAlg.PageLayout.append([0.00+border, 0.50+border, 0.25-border, 0.75-border])
createTopPageAlg.PageLayout.append([0.25+border, 0.50+border, 0.50-border, 0.75-border])
createTopPageAlg.PageLayout.append([0.50+border, 0.50+border, 0.75-border, 0.75-border])
createTopPageAlg.PageLayout.append([0.75+border, 0.50+border, 1.00-border, 0.75-border])
createTopPageAlg.PageLayout.append([-1, 0, 0, 0])
createTopPageAlg.PageLayout.append([0.00+border, 0.25+border, 0.25-border, 0.50-border])
createTopPageAlg.PageLayout.append([0.25+border, 0.25+border, 0.50-border, 0.50-border])
createTopPageAlg.PageLayout.append([0.50+border, 0.25+border, 0.75-border, 0.50-border])
createTopPageAlg.PageLayout.append([0.75+border, 0.25+border, 1.00-border, 0.50-border])
createTopPageAlg.PageLayout.append([0.00+border, 0.00+border, 0.25-border, 0.25-border])
createTopPageAlg.PageLayout.append([0.25+border, 0.00+border, 0.50-border, 0.25-border])
createTopPageAlg.PageLayout.append([0.50+border, 0.00+border, 0.75-border, 0.25-border])
createTopPageAlg.PageLayout.append([0.75+border, 0.00+border, 1.00-border, 0.25-border])
createTopPageAlg.PageLayout.append([-1, 0, 0, 0])

################### BEAM MON DETAIL PAGES
createBeamMonDetailedPagesAlg = CreateHistDBPages("createBeamMonDetailedPagesAlg")
app.TopAlg.append(createBeamMonDetailedPagesAlg)

createBeamMonDetailedPagesAlg.PageBase   = topdir
createBeamMonDetailedPagesAlg.HistoBase  = "VeloBeamMon/"
createBeamMonDetailedPagesAlg.PageDoc    = ["Velo luminous region monitoring plots as seen by the A-side (top) and C-side (bottom)."]
createBeamMonDetailedPagesAlg.PageNames  = ["LuminousRegionPosition","LuminousRegionSlope","VerticesVsBunchId","CumulativeVertexHistos","RefreshedVertexHistos"]
createBeamMonDetailedPagesAlg.PageLayout = [[0.000+border, 0.500+border, 0.500-border, 1.000-border],[0.500+border, 0.500+border, 1.000-border, 1.000-border],
                                            [0.000+border, 0.000+border, 0.500-border, 0.500-border],[0.500+border, 0.000+border, 1.000-border, 0.500-border]]
createBeamMonDetailedPagesAlg.HistoNames = [["ASideLumiRegionMonitor/LumiRegionX_trend","ASideLumiRegionMonitor/LumiRegionY_trend",
                                             "CSideLumiRegionMonitor/LumiRegionX_trend","CSideLumiRegionMonitor/LumiRegionY_trend"],
                                            ["ASideLumiRegionMonitor/LumiRegionXZ_trend","ASideLumiRegionMonitor/LumiRegionYZ_trend",
                                             "CSideLumiRegionMonitor/LumiRegionXZ_trend","CSideLumiRegionMonitor/LumiRegionYZ_trend"],
                                            ["ASideLumiRegionMonitor/LumiRegionX_vs_BunchId","ASideLumiRegionMonitor/LumiRegionY_vs_BunchId",
                                             "CSideLumiRegionMonitor/LumiRegionX_vs_BunchId","CSideLumiRegionMonitor/LumiRegionY_vs_BunchId"],
                                            ["ASideLumiRegionMonitor/LumiRegionX_cumu","ASideLumiRegionMonitor/LumiRegionY_cumu",
                                             "CSideLumiRegionMonitor/LumiRegionX_cumu","CSideLumiRegionMonitor/LumiRegionY_cumu"],
                                            ["ASideLumiRegionMonitor/LumiRegionX","ASideLumiRegionMonitor/LumiRegionY",
                                             "CSideLumiRegionMonitor/LumiRegionX","CSideLumiRegionMonitor/LumiRegionY"]]


################### VELO OCC.MON PAGES
createOccMonDetailedPagesAlg = CreateHistDBPages("createOccMonDetailedPagesAlg")
app.TopAlg.append(createOccMonDetailedPagesAlg)

createOccMonDetailedPagesAlg.PageBase   = topdir
createOccMonDetailedPagesAlg.HistoBase  = "Hlt1VeloOccMon/"
createOccMonDetailedPagesAlg.PageDoc    = ["Velo Occupancy monitor for A-side (top) and C-side (bottom)."]
createOccMonDetailedPagesAlg.PageNames  = ["HltNLiteClusterHistos","HltOccupancyHistos"]
createOccMonDetailedPagesAlg.PageLayout = [[0.000+border, 0.500+border, 0.500-border, 1.000-border],[0.500+border, 0.500+border, 1.000-border, 1.000-border],
                                           [0.000+border, 0.000+border, 0.500-border, 0.500-border],[0.500+border, 0.000+border, 1.000-border, 0.500-border]]
createOccMonDetailedPagesAlg.HistoNames = [["VeloLiteClusterMonitor/nClusters_ASide_SmallRange","VeloLiteClusterMonitor/nClusters_ASide_LargeRange",
                                            "VeloLiteClusterMonitor/nClusters_CSide_SmallRange","VeloLiteClusterMonitor/nClusters_CSide_LargeRange"],
                                           ["VeloLiteClusterMonitor/nClustersVsZ_ASide", "VeloLiteClusterMonitor/nClustersVsBunchId_ASide",
                                            "VeloLiteClusterMonitor/nClustersVsZ_CSide", "VeloLiteClusterMonitor/nClustersVsBunchId_CSide"]]
