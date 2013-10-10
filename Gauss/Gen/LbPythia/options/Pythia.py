##  Empty Pythia options to be included in production to generate
##  standard events as to havethe same structure for all generators
##
##  author P.Robbe 
##  date 2013-10-09
##

pythiaCommands = [ "pysubs msel 0" ,
                   "pysubs msub 11 1" ,
                   "pysubs msub 12 1" ,
                   "pysubs msub 13 1" ,
                   "pysubs msub 28 1" ,
                   "pysubs msub 53 1" ,
                   "pysubs msub 68 1" ,
                   "pysubs msub 91 1" ,
                   "pysubs msub 92 1" ,
                   "pysubs msub 93 1" ,
                   "pysubs msub 94 1" ,
                   "pysubs msub 95 1" ,
                   "pysubs msub 421 1" ,
                   "pysubs msub 422 1" ,
                   "pysubs msub 423 1" ,
                   "pysubs msub 424 1" ,
                   "pysubs msub 425 1" ,
                   "pysubs msub 426 1" ,
                   "pysubs msub 427 1" ,
                   "pysubs msub 428 1" ,
                   "pysubs msub 429 1" ,
                   "pysubs msub 430 1" ,
                   "pysubs msub 431 1" ,
                   "pysubs msub 432 1" ,
                   "pysubs msub 433 1" ,
                   "pysubs msub 434 1" ,
                   "pysubs msub 435 1" ,
                   "pysubs msub 436 1" ,
                   "pysubs msub 437 1" ,
                   "pysubs msub 438 1" ,
                   "pysubs msub 439 1" ,
                   "pysubs msub 461 1" ,
                   "pysubs msub 462 1" ,
                   "pysubs msub 463 1" ,
                   "pysubs msub 464 1" ,
                   "pysubs msub 465 1" ,
                   "pysubs msub 466 1" ,
                   "pysubs msub 467 1" ,
                   "pysubs msub 468 1" ,
                   "pysubs msub 469 1" ,
                   "pysubs msub 470 1" ,
                   "pysubs msub 471 1" ,
                   "pysubs msub 472 1" ,
                   "pysubs msub 473 1" ,
                   "pysubs msub 474 1" ,
                   "pysubs msub 475 1" ,
                   "pysubs msub 476 1" ,
                   "pysubs msub 477 1" ,
                   "pysubs msub 478 1" ,
                   "pysubs msub 479 1" ,
                   "pysubs msub 480 1" ,
                   "pysubs msub 481 1" ,
                   "pysubs msub 482 1" ,
                   "pysubs msub 483 1" ,
                   "pysubs msub 484 1" ,
                   "pysubs msub 485 1" ,
                   "pysubs ckin 41 3.0" ,
                   "pypars mstp 2 2" ,
                   "pypars mstp 33 3" ,
                   "pypars mstp 128 2" ,
                   "pypars mstp 81 21" ,
                   "pypars mstp 82 3" ,
                   "pypars mstp 52 2" ,
                   "pypars mstp 51 10042" ,
                   "pypars mstp 142 2" ,
                   "pypars parp 67 1.0" ,
                   "pypars parp 82 4.28" ,
                   "pypars parp 89 14000" ,
                   "pypars parp 90 0.238" ,
                   "pypars parp 85 0.33" ,
                   "pypars parp 86 0.66" ,
                   "pypars parp 91 1.0" ,
                   "pypars parp 149 0.02" ,
                   "pypars parp 150 0.085" ,
                   "pydat1 parj 11 0.4" ,
                   "pydat1 parj 12 0.4" ,
                   "pydat1 parj 13 0.769" ,
                   "pydat1 parj 14 0.09" ,
                   "pydat1 parj 15 0.018" ,
                   "pydat1 parj 16 0.0815" ,
                   "pydat1 parj 17 0.0815" ,
                   "pydat1 mstj 26 0" ,
                   "pydat1 parj 33 0.4" ]

from Configurables import Generation, Special, PythiaProduction, MinimumBias, Inclusive, SignalPlain, SignalRepeatedHadronization

gen = Generation()
gen.addTool( Special )
gen.addTool( MinimumBias )
gen.addTool( Inclusive )
gen.addTool( SignalPlain )
gen.addTool( SignalRepeatedHadronization )

gen.Special.ProductionTool = "PythiaProduction"
gen.MinimumBias.ProductionTool = "PythiaProduction"
gen.Inclusive.ProductionTool = "PythiaProduction"
gen.SignalPlain.ProductionTool = "PythiaProduction"
gen.SignalRepeatedHadronization.ProductionTool = "PythiaProduction"

# Do not set the default setting here yet as they already are in the code and
# may be reset by the event type which options are called before in production
## gen.Special.addTool( PythiaProduction )
## gen.MinimumBias.addTool( PythiaProduction )
## gen.Inclusive.addTool( PythiaProduction )
## gen.SignalPlain.addTool( PythiaProduction )
## gen.SignalRepeatedHadronization.addTool( PythiaProduction )

## gen.Special.PythiaProduction.Commands += pythiaCommands
## gen.MinimumBias.PythiaProduction.Commands += pythiaCommands
## gen.Inclusive.PythiaProduction.Commands += pythiaCommands
## gen.SignalPlain.PythiaProduction.Commands += pythiaCommands
## gen.SignalRepeatedHadronization.PythiaProduction.Commands += pythiaCommands
