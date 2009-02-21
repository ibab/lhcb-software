def setPlainStyle( gStyle ) :
    import ROOT
    ROOT.gROOT.SetStyle( "Plain" )
    gStyle.SetLineWidth( 1 )
    gStyle.SetFrameLineWidth( 1 )
    gStyle.SetFillStyle( 0 )
    gStyle.SetFillColor( 0 )
    # Histo stuff
    gStyle.SetHistLineWidth( 1 )
    gStyle.SetTitleFont( 41, "t" ) # t = anything byt x, y and z
    gStyle.SetTitleX( 0.10 )
    gStyle.SetTitleY( 0.9 )
    gStyle.SetTitleW( 0.3 )
    gStyle.SetTitleH( 0.15 )
    # Axis stuff
    gStyle.SetTitleFont( 41, "xyz" )
    gStyle.SetLabelFont( 41 )
    gStyle.SetLabelFont( 41, "y" )
    gStyle.SetLabelFont( 41, "z" )
    gStyle.SetLabelSize( 0.04 )
    gStyle.SetLabelSize( 0.04, "y" )
    gStyle.SetLabelSize( 0.04, "z" )
    gStyle.SetLabelOffset(0.016 )
    gStyle.SetLabelOffset( 0.016, "y" )
    gStyle.SetLabelOffset( 0.016, "z" )
    # Stat box stuff
    gStyle.SetStatFont( 41 )
    gStyle.SetOptStat()#"emrou" ) #MRou" )
    gStyle.SetOptTitle( False )
    gStyle.SetStatBorderSize( 1 )
    gStyle.SetStatX( 0.9 )
    gStyle.SetStatY( 0.9 )
    gStyle.SetStatW( 0.3 )
    gStyle.SetStatH( 0.5 )
    # Use the current style
    gStyle.UseCurrentStyle()
