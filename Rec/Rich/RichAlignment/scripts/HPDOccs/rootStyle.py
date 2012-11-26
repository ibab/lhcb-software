
def applyRootStyle():
    
    from ROOT import gROOT, gStyle, kWhite, kBlack, TColor

    # Start from a plain default
    gROOT.SetStyle("Plain")

    lhcbMarkerType    = 8
    lhcbMarkerSize    = 0.8
    lhcbFont          = 62
    lhcbStatFontSize  = 0.02
    lhcbStatBoxWidth  = 0.12
    lhcbStatBoxHeight = 0.12
    lhcbWidth         = 1
    lhcbTextSize      = 0.05
    lhcbLabelSize     = 0.035
    lhcbAxisLabelSize = 0.035
    lhcbForeColour = kBlack

    gStyle.SetFrameBorderMode(0)
    gStyle.SetPadBorderMode(0)

    # canvas options
    gStyle.SetCanvasBorderSize(0)
    gStyle.SetCanvasBorderMode(0)

    # fonts
    gStyle.SetTextFont(lhcbFont)
    gStyle.SetTextSize(lhcbTextSize)
    gStyle.SetLabelFont(lhcbFont,"x")
    gStyle.SetLabelFont(lhcbFont,"y")
    gStyle.SetLabelFont(lhcbFont,"z")
    gStyle.SetLabelSize(lhcbLabelSize,"x")
    gStyle.SetLabelSize(lhcbLabelSize,"y")
    gStyle.SetLabelSize(lhcbLabelSize,"z")
    gStyle.SetTitleFont(lhcbFont)
    gStyle.SetTitleSize(lhcbAxisLabelSize,"x")
    gStyle.SetTitleSize(lhcbAxisLabelSize,"y")
    gStyle.SetTitleSize(lhcbAxisLabelSize,"z")
    gStyle.SetTitleColor(kWhite)
    gStyle.SetTitleFillColor(kWhite)
    gStyle.SetTitleColor(kBlack)
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleTextColor(kBlack)

    # set title position
    gStyle.SetTitleX(0.15)
    gStyle.SetTitleY(0.97)
    # turn off Title box
    gStyle.SetTitleBorderSize(0)
    gStyle.SetTitleTextColor(lhcbForeColour)
    gStyle.SetTitleColor(lhcbForeColour)

    # use bold lines and markers
    gStyle.SetLineWidth(lhcbWidth)
    gStyle.SetFrameLineWidth(lhcbWidth)
    gStyle.SetHistLineWidth(lhcbWidth)
    gStyle.SetFuncWidth(lhcbWidth)
    gStyle.SetGridWidth(lhcbWidth)
    gStyle.SetLineStyleString(2,"[12 12]")
    gStyle.SetMarkerStyle(lhcbMarkerType)
    gStyle.SetMarkerSize(lhcbMarkerSize)

    # Grid
    # gStyle.SetGridStyle(3)

    # label offsets
    gStyle.SetLabelOffset(0.015)

    # by default, do not display histogram decorations:
    gStyle.SetOptStat(1111)
    # show probability, parameters and errors
    gStyle.SetOptFit(1011)

    # look of the statistics box:
    gStyle.SetStatBorderSize(1)
    gStyle.SetStatFont(lhcbFont)
    gStyle.SetStatFontSize(lhcbStatFontSize)
    gStyle.SetStatX(0.9)
    gStyle.SetStatY(0.9)
    gStyle.SetStatW(lhcbStatBoxWidth)
    gStyle.SetStatH(lhcbStatBoxHeight)

    # put tick marks on top and RHS of plots
    gStyle.SetPadTickX(1)
    gStyle.SetPadTickY(1)

    # histogram divisions
    gStyle.SetNdivisions(505,"x")
    gStyle.SetNdivisions(510,"y")

    # Style for 2D zcol plots
    NRGBs = 5
    NCont = 255
    from array import array
    stops = array('d',[ 0.00, 0.34, 0.61, 0.84, 1.00 ])
    red   = array('d',[ 0.00, 0.00, 0.87, 1.00, 0.51 ])
    green = array('d',[ 0.00, 0.81, 1.00, 0.20, 0.00 ])
    blue  = array('d',[ 0.51, 1.00, 0.12, 0.00, 0.00 ])
    TColor().CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont)
    gStyle.SetNumberContours(NCont)

    # Force the style
    gROOT.ForceStyle()
