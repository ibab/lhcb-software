# =============================================================================
# $Id$ 
# =============================================================================
# @file LHCb style file
#
#                   $Revision$
# Last modification $Date$
#                by $Author$
# =============================================================================
import ROOT
__all__ = (
    'lhcbStyle' ,
    'lhcbLabel' ,
    'LHCbStyle' 
    'lhcbLatex' ,
    )
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
logger = getLogger(__name__)
# =============================================================================

## font 
lhcbFont  = 132     ## Times-Roman 
## line thickness
lhcbWidth =   1

##define style for text
lhcbLabel = ROOT.TText   (    )
lhcbLabel . SetTextFont  ( lhcbFont )
lhcbLabel . SetTextColor (  1 )
lhcbLabel . SetTextSize  (  0.04 )
lhcbLabel . SetTextAlign ( 12 )

## define style of latex text
lhcbLatex = ROOT.TLatex   ()
lhcbLatex . SetTextFont   ( lhcbFont )
lhcbLatex . SetTextColor  ( 1    )
lhcbLatex . SetTextSize   ( 0.04 )
lhcbLatex . SetTextAlign  ( 12   )

# =============================================================================
## define LHCb style for plots 
def LHCbStyle ( name      = "LHCbStyle"                 ,
                desc      = "Standard LHCb plots style" ,
                lineWidth = lhcbWidth                   ,
                font      = lhcbFont                    ,
                makeNew   = False                       ,
                force     = True                        ) :
    """
    Define LHCb style for plots
    """
    obj = ROOT.gROOT.FindObject ( name )
    if obj and issubclass ( type( obj  ) , ROOT.TStyle ) and not makeNew : 
        logger.info              ('The style %s is reused' % obj.GetName() )
        if force : 
            logger.info          ('The style %s is forced' % obj.GetName() )
            ROOT.gROOT.SetStyle   ( obj.GetName()  )
            ROOT.gROOT.ForceStyle ( )
        return obj
    
    nam = name
    i   = 1
    while obj :
        nam  = name + '_%d' % i
        obj  = ROOT.gROOT.FindObject ( nam )
        i   += 1
        
    style = ROOT.TStyle ( nam , desc )
    logger.info ('New style %s is created' % style.GetName() )
    
    ## use plain black on white colors
    style . SetFrameBorderMode  ( 0 )
    style . SetCanvasBorderMode ( 0 )
    style . SetPadBorderMode    ( 0 )
    style . SetPadColor         ( 0 )
    style . SetCanvasColor      ( 0 )
    style . SetStatColor        ( 0 )
    style . SetPalette          ( 1 )
    
    ## set the paper & margin sizes
    style . SetPaperSize         ( 20 , 26 )
    style . SetPadTopMargin      ( 0.05    )
    style . SetPadRightMargin    ( 0.05    ) ## increase for colz plots
    style . SetPadBottomMargin   ( 0.16    )
    style . SetPadLeftMargin     ( 0.14    )
        
    ##  use large fonts
    style . SetTextFont          ( font )
    style . SetTextSize          ( 0.08 )
    style . SetLabelFont         ( font , "x" ) 
    style . SetLabelFont         ( font , "y" ) 
    style . SetLabelFont         ( font , "z" ) 
    style . SetLabelSize         ( 0.05 , "x" )
    style . SetLabelSize         ( 0.05 , "y" )
    style . SetLabelSize         ( 0.05 , "z" )
    style . SetTitleFont         ( font )
    style . SetTitleSize         ( 0.06 , "x" )
    style . SetTitleSize         ( 0.06 , "y" )
    style . SetTitleSize         ( 0.06 , "z" ) 
    
    
    ## use bold lines and markers
    style . SetLineWidth         ( lineWidth )
    style . SetFrameLineWidth    ( lineWidth )
    style . SetHistLineWidth     ( lineWidth )
    style . SetFuncWidth         ( lineWidth )
    style . SetGridWidth         ( lineWidth )
    style . SetLineStyleString   ( 2 , "[12 12]" ) ##  postscript dashes
    style . SetMarkerStyle       ( 20  )
    style . SetMarkerSize        ( 1.2 )
    
    ## label offsets
    style . SetLabelOffset(0.015);
    
    ## by default, do not display histogram decorations:
    style . SetOptStat    ( 0 )  
    ## lhcbStyle->SetOptStat("emr");  ##  show only nent -e , mean - m , rms -r
    ## full opts at http://root.cern.ch/root/html/TStyle.html#TStyle:SetOptStat
    style . SetStatFormat ("6.3g") ##  specified as c printf options
    style . SetOptTitle   ( 0 )
    style . SetOptFit     ( 0 )
    ## lhcbStyle . SetOptFit(1011); // order is probability, Chi2, errors, parameters

    ## size of small lines at the end of error bars
    style.  SetEndErrorSize ( 5 ) 

    ## look of the statistics box:
    style . SetStatBorderSize         ( 0 )
    style . SetStatFont               ( font )
    style . SetStatFontSize           ( 0.05 )
    style . SetStatX                  ( 0.9  )
    style . SetStatY                  ( 0.9  )
    style . SetStatW                  ( 0.25 )
    style . SetStatH                  ( 0.15 )
    ## put tick marks on top and RHS of plots
    style . SetPadTickX           ( 1 )
    style . SetPadTickY           ( 1 )
    
## histogram divisions: only 5 in x to avoid label overlaps
    style . SetNdivisions    ( 505 , "x" )
    style . SetNdivisions    ( 510 , "y" )
    
    if force : 
        logger.info ('The style %s is forced' % style.GetName() )
        ROOT.gROOT.SetStyle   ( style.GetName()  )
        ROOT.gROOT.ForceStyle ()
    
    return style 

lhcbStyle = LHCbStyle() 
  
## ============================================================================
# //////////////////////////////////////////////////////////////////////////
# // routine to print 'LHCb', 'LHCb Preliminary' on plots 
# // options: optLR=L (top left) / R (top right) of plots
# //          optPrelim= Final (LHCb), Prelim (LHCb Preliminary), Other
# //          optText= text printed if 'Other' specified
# ////////////////////////////////////////////////////////////////////
def printLHCb ( LR     = "L"     ,
                prelim = "Final" ,
                text   = ""      ) :
    """
    ////////////////////////////////////////////////////////////////////
    // routine to print 'LHCb', 'LHCb Preliminary' on plots 
    // options: optLR=L (top left) / R (top right) of plots
    //          optPrelim= Final (LHCb), Prelim (LHCb Preliminary), Other
    //          optText= text printed if 'Other' specified
    ////////////////////////////////////////////////////////////////////
    """
    
    if not LR in ( 'L' , 'R' ) :
        raise TypeError( "Unknown LR-option: %s" % LR )
    
    global lhcbName
    if 'R' == LR :     
        lhcbName = ROOT.TPaveText ( 0.70 - lhcbStyle . GetPadRightMargin (),
                                    0.75 - lhcbStyle . GetPadTopMargin   (),
                                    0.95 - lhcbStyle . GetPadRightMargin (),
                                    0.85 - lhcbStyle . GetPadTopMargin   (),
                                    "BRNDC" )
        
    else : ## LR="L"
        lhcbName = ROOT.TPaveText ( lhcbStyle.GetPadLeftMargin() + 0.05 ,
                                    0.87 - lhcbStyle.GetPadTopMargin()  ,
                                    lhcbStyle.GetPadLeftMargin() + 0.30 ,
                                    0.95 - lhcbStyle.GetPadTopMargin()  ,
                                    "BRNDC"  )

    if   "Final"   == prelim : lhcbName.AddText ( "LHCb" )
    elif "Prelim"  == prelim : lhcbName.AddText ( "#splitline{LHCb}{#scale[1.0]{Preliminary}}") 
    else                     : lhcbName.AddText ( text )
    
    lhcbName . SetFillColor(0);
    lhcbName . SetTextAlign(12);
    lhcbName . SetBorderSize(0);
    lhcbName . Draw() 

    return lhcbName 
    
# =============================================================================
# The END 
# =============================================================================
