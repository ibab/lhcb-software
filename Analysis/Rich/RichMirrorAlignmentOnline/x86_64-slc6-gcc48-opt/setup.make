----------> uses
# use GaudiPolicy v*  (no_version_directory)
#   use LCG_Settings *  (no_version_directory)
#   use Python * LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=2.7.6)
#     use LCG_Configuration v*  (no_version_directory)
#       use LCG_Platforms *  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
#   use tcmalloc * LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=1.7p3)
#     use LCG_Configuration v*  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
#     use libunwind v* LCG_Interfaces (no_version_directory) (native_version=5c2cade)
#       use LCG_Configuration v*  (no_version_directory)
#       use LCG_Settings v*  (no_version_directory)
#   use Reflex v* LCG_Interfaces (no_auto_imports) (no_version_directory)
#     use LCG_Configuration v*  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
#     use ROOT v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=6.02.03)
#       use LCG_Configuration v*  (no_version_directory)
#       use LCG_Settings v*  (no_version_directory)
#       use GCCXML v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=0.9.0_20131026)
#         use LCG_Configuration v*  (no_version_directory)
#         use LCG_Settings v*  (no_version_directory)
#       use Python v* LCG_Interfaces (no_auto_imports) (no_version_directory)
#       use xrootd v* LCG_Interfaces (no_version_directory) (native_version=3.3.6)
#         use LCG_Configuration v*  (no_version_directory)
#         use LCG_Settings v*  (no_version_directory)
#
# Selection :
use CMT v1r20p20090520 (/afs/cern.ch/sw/contrib)
use LCG_Platforms v1  (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6) (no_auto_imports)
use LCG_Configuration v1  (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6) (no_auto_imports)
use LCG_Settings v1  (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6)
use xrootd v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use GCCXML v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use libunwind v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use tcmalloc v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use Python v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use ROOT v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use Reflex v1 LCG_Interfaces (/afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6/) (no_auto_imports)
use GaudiPolicy v15r3  (/afs/cern.ch/lhcb/software/releases/GAUDI/GAUDI_v26r0)
----------> tags
CMTv1 (from CMTVERSION)
CMTr20 (from CMTVERSION)
CMTp20090520 (from CMTVERSION)
Linux (from uname) package LCG_Platforms implies [Unix host-linux]
x86_64-linux (from HOSTTYPE)
x86_64-slc6-gcc48-opt (from CMTCONFIG) package LCG_Platforms implies [target-x86_64 target-slc6 target-gcc48 target-opt]
CERN (from CMTSITE)
PANOPTES_no_config (from PROJECT) excludes [PANOPTES_config]
PANOPTES_root (from PROJECT) excludes [PANOPTES_no_root]
PANOPTES_cleanup (from PROJECT) excludes [PANOPTES_no_cleanup]
PANOPTES_prototypes (from PROJECT) excludes [PANOPTES_no_prototypes]
PANOPTES_with_installarea (from PROJECT) excludes [PANOPTES_without_installarea]
PANOPTES_without_version_directory (from PROJECT) excludes [PANOPTES_with_version_directory]
PANOPTES (from PROJECT)
ONLINE_no_config (from PROJECT) excludes [ONLINE_config]
ONLINE_root (from PROJECT) excludes [ONLINE_no_root]
ONLINE_cleanup (from PROJECT) excludes [ONLINE_no_cleanup]
ONLINE_prototypes (from PROJECT) excludes [ONLINE_no_prototypes]
ONLINE_with_installarea (from PROJECT) excludes [ONLINE_without_installarea]
ONLINE_without_version_directory (from PROJECT) excludes [ONLINE_with_version_directory]
GAUDI_no_config (from PROJECT) excludes [GAUDI_config]
GAUDI_root (from PROJECT) excludes [GAUDI_no_root]
GAUDI_cleanup (from PROJECT) excludes [GAUDI_no_cleanup]
GAUDI_prototypes (from PROJECT) excludes [GAUDI_no_prototypes]
GAUDI_with_installarea (from PROJECT) excludes [GAUDI_without_installarea]
GAUDI_without_version_directory (from PROJECT) excludes [GAUDI_with_version_directory]
LCGCMT_no_config (from PROJECT) excludes [LCGCMT_config]
LCGCMT_no_root (from PROJECT) excludes [LCGCMT_root]
LCGCMT_cleanup (from PROJECT) excludes [LCGCMT_no_cleanup]
LCGCMT_prototypes (from PROJECT) excludes [LCGCMT_no_prototypes]
LCGCMT_without_installarea (from PROJECT) excludes [LCGCMT_with_installarea]
LCGCMT_with_version_directory (from PROJECT) excludes [LCGCMT_without_version_directory]
REC_no_config (from PROJECT) excludes [REC_config]
REC_root (from PROJECT) excludes [REC_no_root]
REC_cleanup (from PROJECT) excludes [REC_no_cleanup]
REC_prototypes (from PROJECT) excludes [REC_no_prototypes]
REC_with_installarea (from PROJECT) excludes [REC_without_installarea]
REC_without_version_directory (from PROJECT) excludes [REC_with_version_directory]
LBCOM_no_config (from PROJECT) excludes [LBCOM_config]
LBCOM_root (from PROJECT) excludes [LBCOM_no_root]
LBCOM_cleanup (from PROJECT) excludes [LBCOM_no_cleanup]
LBCOM_prototypes (from PROJECT) excludes [LBCOM_no_prototypes]
LBCOM_with_installarea (from PROJECT) excludes [LBCOM_without_installarea]
LBCOM_without_version_directory (from PROJECT) excludes [LBCOM_with_version_directory]
LHCB_no_config (from PROJECT) excludes [LHCB_config]
LHCB_root (from PROJECT) excludes [LHCB_no_root]
LHCB_cleanup (from PROJECT) excludes [LHCB_no_cleanup]
LHCB_prototypes (from PROJECT) excludes [LHCB_no_prototypes]
LHCB_with_installarea (from PROJECT) excludes [LHCB_without_installarea]
LHCB_without_version_directory (from PROJECT) excludes [LHCB_with_version_directory]
DBASE_no_config (from PROJECT) excludes [DBASE_config]
DBASE_root (from PROJECT) excludes [DBASE_no_root]
DBASE_cleanup (from PROJECT) excludes [DBASE_no_cleanup]
DBASE_prototypes (from PROJECT) excludes [DBASE_no_prototypes]
DBASE_without_installarea (from PROJECT) excludes [DBASE_with_installarea]
DBASE_with_version_directory (from PROJECT) excludes [DBASE_without_version_directory]
PARAM_no_config (from PROJECT) excludes [PARAM_config]
PARAM_root (from PROJECT) excludes [PARAM_no_root]
PARAM_cleanup (from PROJECT) excludes [PARAM_no_cleanup]
PARAM_prototypes (from PROJECT) excludes [PARAM_no_prototypes]
PARAM_without_installarea (from PROJECT) excludes [PARAM_with_installarea]
PARAM_with_version_directory (from PROJECT) excludes [PARAM_without_version_directory]
x86_64 (from package CMT) package LCG_Platforms implies [host-x86_64]
slc66 (from package CMT) package LCG_Platforms implies [host-slc6]
gcc481 (from package CMT) package LCG_Platforms implies [host-gcc48]
Unix (from package CMT) package LCG_Platforms implies [host-unix] package LCG_Platforms excludes [WIN32 Win32]
experimental (from package LCG_Settings)
target-unix (from package LCG_Settings)
target-x86_64 (from package LCG_Settings)
target-gcc48 (from package LCG_Settings) package LCG_Platforms implies [target-gcc4 target-lcg-compiler lcg-compiler target-c11 experimental]
host-x86_64 (from package LCG_Settings)
target-slc (from package LCG_Settings) package LCG_Platforms implies [target-linux]
target-gcc (from package LCG_Settings)
target-gcc4 (from package LCG_Settings) package LCG_Platforms implies [target-gcc]
target-lcg-compiler (from package LCG_Settings)
host-gcc4 (from package LCG_Platforms) package LCG_Platforms implies [host-gcc]
host-gcc48 (from package LCG_Platforms) package LCG_Platforms implies [host-gcc4]
host-gcc (from package LCG_Platforms)
host-linux (from package LCG_Platforms) package LCG_Platforms implies [host-unix]
host-unix (from package LCG_Platforms)
host-slc6 (from package LCG_Platforms) package LCG_Platforms implies [host-slc]
host-slc (from package LCG_Platforms) package LCG_Platforms implies [host-linux]
target-opt (from package LCG_Platforms)
target-slc6 (from package LCG_Platforms) package LCG_Platforms implies [target-slc]
target-linux (from package LCG_Platforms) package LCG_Platforms implies [target-unix]
lcg-compiler (from package LCG_Platforms)
target-c11 (from package LCG_Platforms)
ROOT_GE_5_15 (from package LCG_Configuration)
ROOT_GE_5_19 (from package LCG_Configuration)
ROOT_GE_6_00 (from package LCG_Configuration)
----------> CMTPATH
# Add path /afs/cern.ch/user/p/pnaik/cmtuser/PANOPTES/PANOPTES_HEAD from initialization
# Add path /afs/cern.ch/lhcb/software/releases/ONLINE/ONLINE_v5r15 from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/REC/REC_v17r4 from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/LBCOM/LBCOM_v16r3 from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/LHCB/LHCB_v38r4 from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/GAUDI/GAUDI_v26r0 from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/DBASE from ProjectPath
# Add path /afs/cern.ch/lhcb/software/releases/PARAM from ProjectPath
# Add path /afs/cern.ch/sw/lcg/releases/LCGCMT/LCGCMT_72root6 from ProjectPath
