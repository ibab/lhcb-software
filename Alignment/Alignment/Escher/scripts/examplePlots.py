#!/usr/bin/env python
__author__  = "Albert Bursche"
__version__ = ""

    
    plots = [ArrowPlot("TT-ZX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.Z+10*do.TZ,do.X,do.X+10*do.TX]),
             ArrowPlot("TT-XY"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.X,do.X+10*do.TX,do.Y,do.Y+10*do.TY]),
             ArrowPlot("TT-ZY"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.Z+10*do.TZ,do.Y,do.Y+10*do.TY]),
             ArrowPlot("ST-ZX"+opts.plot,".*TT(?!.*Sensor.*)",lambda do:[do.Z,do.Z+100*do.TZ,do.X,do.X+100*do.TX]),
             ArrowPlot("all-ZXarr"+opts.plot,".*(?!.*Sensor.*)",lambda do:[do.Z,do.Z+100*do.TZ,do.X,do.X+100*do.TX]),
             ArrowPlot("VeLo-ZXarr"+opts.plot,".*",lambda do:[do.Z,do.Z+10*do.TZ,do.X,do.X+10*do.TX],lambda do:do.Z<500),
             
             Plot("TT-ZtX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.Z,do.TX]),
             Plot("TT-XtX"+opts.plot,"/TT(?!T)(?!.*Sensor.*)",lambda do:[do.X,do.TX]),

             Plot("all-ZX"+opts.plot,".*",lambda do:[do.Z,do.X]),
             
             Plot("TT-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 3000),
             Plot("TT-YTX"+opts.plot,"/TT.*",lambda do:[do.Y,do.TX],lambda do:do.Z < 3000),
             Plot("TT-ZTX"+opts.plot,"/TT.*",lambda do:[do.Z,do.TX],lambda do:do.Z < 3000),
             
             Plot("TT-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 3000),
             Plot("TT-YTY"+opts.plot,"/TT.*",lambda do:[do.Y,do.TY],lambda do:do.Z < 3000),
             Plot("TT-ZTY"+opts.plot,"/TT.*",lambda do:[do.Z,do.TY],lambda do:do.Z < 3000),
             
             Plot("TT-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 3000),
             Plot("TT-YTZ"+opts.plot,"/TT.*",lambda do:[do.Y,do.TZ],lambda do:do.Z < 3000),
             Plot("TT-ZTZ"+opts.plot,"/TT.*",lambda do:[do.Z,do.TZ],lambda do:do.Z < 3000),
             
             Plot("TTaX-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2340 and do.Z>2300),
             Plot("TTaU-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2400 and do.Z>2360),
             Plot("TTbV-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2625 and do.Z>2550),
             Plot("TTbX-XTX"+opts.plot,"/TT.*",lambda do:[do.X,do.TX],lambda do:do.Z < 2670 and do.Z>2625),
             Plot("TTaX-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2340 and do.Z>2300),
             Plot("TTaU-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2400 and do.Z>2360),
             Plot("TTbV-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2625 and do.Z>2550),
             Plot("TTbX-XTY"+opts.plot,"/TT.*",lambda do:[do.X,do.TY],lambda do:do.Z < 2670 and do.Z>2625),
             Plot("TTaX-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2340 and do.Z>2300),
             Plot("TTaU-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2400 and do.Z>2360),
             Plot("TTbV-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2625 and do.Z>2550),
             Plot("TTbX-XTZ"+opts.plot,"/TT.*",lambda do:[do.X,do.TZ],lambda do:do.Z < 2670 and do.Z>2625),
             Plot("IT-ZY"+opts.plot,".*",lambda do:[do.Z,do.Y],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),
             Plot("IT-ZX"+opts.plot,".*",lambda do:[do.Z,do.X],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),

             Plot("IT-XTX"+opts.plot,".*",lambda do:[do.X,do.TX],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),

             Plot("IT-RhoPhi"+opts.plot,".*",lambda do:[do.rho(),do.phi()],lambda do:do.Z>3000 and math.fabs(do.rho()<500)),
             Plot("all-ZY"+opts.plot,".*",lambda do:[do.Z,do.Y]),
             Plot("all-RhoPhi"+opts.plot,".*",lambda do:[do.rho(),do.phi()]) ]
   
    for s in ["TTbV","TTbX","TTaX","TTaU",
              "ITT1CSide","ITT1ASide","ITT1BottomLayer","ITT1TopLayer",
              "ITT2CSide","ITT2ASide","ITT2BottomLayer","ITT2TopLayer",
              "ITT3CSide","ITT3ASide","ITT3BottomLayer","ITT3TopLayer",
              "ITT"]:
        for (i,function) in enumerate([lambda do:[do.X,do.TX],
                                       lambda do:[do.X,do.TY],
                                       lambda do:[do.X,do.TZ],
                                       
                                       lambda do:[do.Y,do.TX],
                                       lambda do:[do.Y,do.TY],
                                       lambda do:[do.Y,do.TZ],
                                       
                                       lambda do:[do.Z,do.TX],
                                       lambda do:[do.Z,do.TY],
                                       lambda do:[do.Z,do.TZ]]):

            plots.append(Plot(s+"XYZ"[i//3]+"T"+"XYZ"[i%3]+opts.plot,"/"+s+"(?!.*Sensor.*)",function))

