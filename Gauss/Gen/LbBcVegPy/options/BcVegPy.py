# $Id$

from Configurables import Generation, Special, BcVegPyProduction

BcVegPyListOfCommands = [
  "mixevnt imix 1" ,
  "mixevnt imixtype 1" ,
  "counter ibcstate 1",
  "vegasinf nitmx 20",
  "vegasinf number 1000000",
  "loggrade igenerate 1",
  "loggrade ivegasopen 0",
  "loggrade igrade 1",
  "usertran idpp 1" 
]

gen = Generation()
gen.addTool( Special , name = "Special" )
gen.Special.ProductionTool = "BcVegPyProduction"
gen.Special.addTool( BcVegPyProduction , name = "BcVegPyProduction" )
gen.Special.BcVegPyProduction.BcVegPyCommands += BcVegPyListOfCommands
gen.PileUpTool = "FixedLuminosityForRareProcess";

gen.Special.BcVegPyProduction.Commands += [
    "pysubs msub 11 0" ,
    "pysubs msub 12 0" ,
    "pysubs msub 13 0" ,
    "pysubs msub 28 0" ,
    "pysubs msub 53 0" ,
    "pysubs msub 68 0" ,
    "pysubs msub 91 0" ,
    "pysubs msub 92 0" ,
    "pysubs msub 93 0" ,
    "pysubs msub 94 0" ,
    "pysubs msub 95 0" ,
    "pysubs msub 421 0" ,
    "pysubs msub 422 0" ,
    "pysubs msub 423 0" ,
    "pysubs msub 424 0" ,
    "pysubs msub 425 0" ,
    "pysubs msub 426 0" ,
    "pysubs msub 427 0" ,
    "pysubs msub 428 0" ,
    "pysubs msub 429 0" ,
    "pysubs msub 430 0" ,
    "pysubs msub 431 0" ,
    "pysubs msub 432 0" ,
    "pysubs msub 433 0" ,
    "pysubs msub 434 0" ,
    "pysubs msub 435 0" ,
    "pysubs msub 436 0" ,
    "pysubs msub 437 0" ,
    "pysubs msub 438 0" ,
    "pysubs msub 439 0" ,
    "pysubs msub 461 0" ,
    "pysubs msub 462 0" ,
    "pysubs msub 463 0" ,
    "pysubs msub 464 0" ,
    "pysubs msub 465 0" ,
    "pysubs msub 466 0" ,
    "pysubs msub 467 0" ,
    "pysubs msub 468 0" ,
    "pysubs msub 469 0" ,
    "pysubs msub 470 0" ,
    "pysubs msub 471 0" ,
    "pysubs msub 472 0" ,
    "pysubs msub 473 0" ,
    "pysubs msub 474 0" ,
    "pysubs msub 475 0" ,
    "pysubs msub 476 0" ,
    "pysubs msub 477 0" ,
    "pysubs msub 478 0" ,
    "pysubs msub 479 0" ,
    "pysubs msub 480 0" ,
    "pysubs msub 481 0" ,
    "pysubs msub 482 0" ,
    "pysubs msub 483 0" ,
    "pysubs msub 484 0" ,
    "pysubs msub 485 0"    
    ]
