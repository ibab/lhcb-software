#!/bin/bash

# Script designed to modify the necessary POWHEG-BOX files.

###############################################################################
# Define all variables.
###############################################################################

# Set the debug level to use.
DEBUG_LEVEL=3

# Set the revision number to use.
REVISION=""

# Set the POWHEG-BOX source directory to use.
SRCDIR="POWHEG-BOX"

# Set the target directory where the packages should be copied.
TRGDIR="src"

# Main top level code.
FORTRAN="pwhg_init.f bbinit.f lhefwrite.f LesHouches.f gen_Born_phsp.f find_regions.f test_Sudakov.f sigborn.f gen_real_phsp.f maxrat.f gen_index.f gen_radiation.f Bornzerodamp.f sigremnants.f random.f boostrot.f bra_ket_subroutines.f cernroutines.f init_phys.f powheginput.f pdfcalls.f sigreal.f sigcollremn.f pwhg_bookhist.f pwhg_analysis_driver.f checkmomzero.f setstrongcoupl.f integrator.f newunit.f mwarn.f sigsoftvirt.f sigcollsoft.f sigvirtual.f init_couplings.f init_processes.f Born_phsp.f Born.f virtual.f real.f btilde.f pt2maxreg.f LesHouchesreg.f pwhg_analysis-dummy.f mlmpdfif.f hvqpdfpho.f"

# Header top level code.
HEADERS="pwhg_br.h pwhg_flg.h pwhg_kn.h pwhg_par.h pwhg_physpar.h pwhg_rnd.h hepevt.h LesHouches.h pwhg_dbg.h pwhg_flst.h pwhg_math.h pwhg_pdf.h pwhg_rad.h pwhg_st.h nlegborn.h pwhg_book.h"

# Script top level code.
SCRIPTS=""

###############################################################################
# Determine library specific FORTRAN, headers, scripts, and fixes needed.
###############################################################################
function library_specific {
    FORTRAN_LIB=""; HEADERS_LIB=""; SCRIPTS_LIB=""
    # Dijet.
    if [ "$LIB" == "Dijet" ]; then
	FORTRAN_LIB="reshufflemoms.f"
    # Gluon fusion to Higgs.
    elif [ "$LIB" == "gg_H" ]; then
	HEADERS_LIB="PhysPars.h"
	SCRIPTS_LIB="make_btilde.sh make_sigremnants.sh"
    # Heavy quark.
    elif [ "$LIB" == "hvq" ]; then
	FORTRAN_LIB="ttdec.f"
	HEADERS_LIB="MEcoupl.inc"
    # Single top through s or t-channel.
    elif [[ "$LIB" == "ST_sch" || "$LIB" == "ST_tch" ]]; then
	FORTRAN_LIB="boost.f my_setpara.f switchmom.f printout.f bdx_tux.f bu_td.f dxb_tux.f dxu_tbx.f ub_td.f udx_tbx.f udx_tbxg.f ub_tdg.f gb_tuxd_T.f dxu_tbxg.f dxb_tuxg.f bu_tdg.f bg_tuxd_T.f bdx_tuxg.f gdx_tuxbx_S.f gdx_tuxbx_T.f gu_tdbx_S.f gu_tdbx_T.f dxg_tuxbx_S.f dxg_tuxbx_T.f ug_tdbx_S.f ug_tdbx_T.f bdx_epvebux.f dxb_epvebux.f ub_epvebd.f bu_epvebd.f dxu_epvebbx.f udx_epvebbx.f bdx_epvebuxg.f dxb_epvebuxg.f gb_epvebuxd_T.f udx_epvebbxg.f bg_epvebuxd_T.f dxu_epvebbxg.f bu_epvebdg.f ub_epvebdg.f gdx_epvebuxbx_S.f gu_epvebdbx_S.f dxg_epvebuxbx_S.f ug_epvebdbx_S.f gdx_epvebuxbx_T.f gu_epvebdbx_T.f dxg_epvebuxbx_T.f ug_epvebdbx_T.f boostx.F fsocxx.f fvodmx.F hioxxx.F hvvsxx.F iovgld.f jiocxx.f jvsxxx.F sssxxx.F uvvvlx.F vvvkxx.F fsogld.f fvogld.f hsssxx.F hvvtxx.F iovkxx.F jiodmx.F jvtxxx.F sstlxx.F uvvxxx.F vvvsxx.F fsoxxx.F fvoxxx.F hsstxx.F hvvvxx.F iovtkk.F jiogld.f jvvkxx.F mom2cx.F sxxxxx.F vssxxx.F vvvtlx.F eaixxx.F ftixkk.F hssxxx.F hvvxxx.F iovxxx.F jiokxx.F jvvsxx.F momntx.F ttssxx.F vvshxx.F vvvxxx.F eaoxxx.F ftoxkk.F hstlxx.F ioscxx.f ixxxxx.F jioxxx.F jvvtlx.F oxxxxx.F ttsxxx.F vvsshx.F vxxxxx.F ficxxx.f fvicxx.f ggggxx.F httsxx.F iosgld.f ixxxxx.F.orig jssxxx.F jvvxxx.F txxxx2.f vvssxx.F w3w3xx.F focxxx.f fvidmx.F gggxxx.F httxxx.F iosxxx.F j3xxxx.F jvshxx.F jw3wxx.F txxxxx.f vvstxx.F wwwwxx.F fsicxx.f fvigld.f hvsxxx.F iotxkk.F jeexxx.F jvsshx.F jwwwxx.F rotxxx.F usslxx.F vvsxxx.F fsigld.f fvixxx.F hiocxx.f hvvhxx.F iovcxx.f jgggxx.F jvssxx.F ssssxx.F utssxx.F vvtxkk.F fsixxx.F fvocxx.f hiogld.f hvvshx.F iovdmx.F jggxxx.F jvstxx.F ssstxx.F utsxxx.F vvtxxx.F"
	HEADERS_LIB="PhysPars.h bgenps_d.inc bexternal_d.inc coupl.inc bgenps.inc bexternal.inc genps.inc nexternal.inc genps_d.inc nexternal_d.inc dimension.inc"
    # Single top through w-channel (diagram removal).
    elif [ "$LIB" == "ST_wtch_DR" ]; then
	report "ERROR" "The POWHEG library $LIB is not supported."; exit
    # Single top through w-channel (diagram subtraction).
    elif [ "$LIB" == "ST_wtch_DS" ]; then
	report "ERROR" "The POWHEG library $LIB is not supported."; exit
    # Higgs from vector boson fusion.
    elif [ "$LIB" == "VBF_H" ]; then
	HEADERS_LIB="PhysPars.h"
	# Fix a bug in the POWHEG code when using the dummy analysis.
	# Currently the PARTICLE_IDENTIF subroutine is only in pwhg_analysis.f.
	EXIST=`grep -i "^[[:blank:]]\+subroutine[[:blank:]]\+particle_identif" $SRCDIR/$LIB/pwhg_analysis-dummy.f`
	if [ -z "$EXIST" ]; then
	    report "WARN" "Applying bug fix for POWHEG library $LIB."
	    echo -e "\n" >> $SRCDIR/$LIB/pwhg_analysis-dummy.f
	    sed '/^[[:blank:]]\+subroutine[[:blank:]]\+particle_identif/,/end\$/!d' $SRCDIR/$LIB/pwhg_analysis.f >> $SRCDIR/$LIB/pwhg_analysis-dummy.f
	fi
    # VBF_Wp_Wp.
    elif [ "$LIB" == "VBF_Wp_Wp" ]; then
	FORTRAN_LIB="reshufflemoms.f calc_invariants.f compute_tensors.f compute_tensors_real.f convert_coup.f boost.f phi1_2.f phi3m0.f breit.f amp_aux.f braket_vbf.f help.f init_tensors.f jvirt-wpp.f qqwppqqj.f qqwppqq-lo.f qqwppqq-vonly.f towpm.f toww.f towz.f boostx.F eaixxx.F eaoxxx.F ficxxx.f focxxx.f fsicxx.f fsigld.f fsixxx.F fsocxx.f fsogld.f fsoxxx.F ftixkk.F ftoxkk.F fvicxx.f fvidmx.F fvigld.f fvixxx.F fvocxx.f fvodmx.F fvogld.f fvoxxx.F ggggxx.F gggxxx.F hiocxx.f hiogld.f hioxxx.F hsssxx.F hssxxx.F hvsxxx.F hvvsxx.F hvvxxx.F ioscxx.f iosgld.f iosxxx.F iotxkk.F iovcxx.f iovdmx.F iovgld.f iovtkk.F iovxxx.F ixxxxx.F j3xxxx.F jeexxx.F jgggxx.F jggxxx.F jiocxx.f jiodmx.F jiogld.f jioxxx.F jssxxx.F jvshxx.F jvssxx.F jvsxxx.F jvvxxx.F jw3wxx.F jwwwxx.F mom2cx.F momntx.F oxxxxx.F rotxxx.F ssssxx.F sssxxx.F sxxxxx.F txxxx2.f txxxxx.f vssxxx.F vvshxx.F vvssxx.F vvsxxx.F vvtxkk.F vvvsxx.F vvvxxx.F vxxxxx.F w3w3xx.F wwwwxx.F B0C0D0fin.f boost.f bra_ket_subroutines_vbf.f dilog.f tensor_redux_Cij.f tensor_redux_Dij.f tensor_redux_Eij.f tri_box_penta_tens_red.f virtual1.f virtual2.f"
	HEADERS_LIB="cvecbos.h PhysPars.h PhysPars_Higgs.h ww_widths.h virtuals.h coupl.inc global.inc koppln_ew.inc scales.inc tensor.inc tensorw.inc tensorww.inc"
	# Fix a bug in the POWHEG code where subroutines are defined twice.
	# The subroutines CCDOTP, EXCHANGE_MOM, RAND_NUM, are defined
	# both in the file bra_ket_subroutines.f and bra_ket_subroutines_vbf.f.
	EXIST=`grep -i "^[[:blank:]]\+function[[:blank:]]\+ccdotp" $SRCDIR/$LIB/vbfnlo-files/tensor_redux/bra_ket_subroutines_vbf.f`
	if [ -n "$EXIST" ]; then
	    report "WARN" "Applying bug fix for POWHEG library $LIB."
	    sed -i "/^[[:blank:]]\+function[[:blank:]]\+ccdotp/,/end\$/d" $SRCDIR/$LIB/vbfnlo-files/tensor_redux/bra_ket_subroutines_vbf.f
	    sed -i "/^[[:blank:]]\+subroutine[[:blank:]]\+exchange_mom/,/end\$/d" $SRCDIR/$LIB/vbfnlo-files/tensor_redux/bra_ket_subroutines_vbf.f
	    sed -i "/^[[:blank:]]\+function[[:blank:]]\+rand_num/,/end\$/d" $SRCDIR/$LIB/vbfnlo-files/tensor_redux/bra_ket_subroutines_vbf.f
	fi
    # W.
    elif [ "$LIB" == "W" ]; then
	HEADERS_LIB="PhysPars.h"
	# Change specific instance so SED does not fail.
	EXIST=`grep -i "8powheginput" $SRCDIR/$LIB/init_couplings.f`
	if [ -n "$EXIST" ]; then
	    report "WARN" "Replacing 8powheginput in init_couplings.f."
	    sed -i "s/8powheginput/8 powheginput/gi" \
		$SRCDIR/$LIB/init_couplings.f
	fi
    # WWb.
    elif [ "$LIB" == "Wbb" ]; then
	FORTRAN_LIB="helas3.f Wm_decay.f Wp_decay.f initialize_MG4.f"
	HEADERS_LIB="coupl.inc nexternal_2to3.inc nexternal_2to4.inc nexternal_2to5.inc PhysPars.h"
    # This library doesn't exist (yet) in SVN.
    elif [ "$LIB" == "W_ew-BW" ]; then
	report "ERROR" "The POWHEG library $LIB is not supported."; exit
    # W + jet.
    elif [ "$LIB" == "Wj" ]; then
	FORTRAN_LIB="reshufflemoms.f"
	HEADERS_LIB="PhysPars.h"
	mv_headers "$SRCDIR/$LIB/MCFM_include" "ckm constants epinv2 epinv ewcharge ewcouple masses nflav qcdcouple scale scheme sprods_com zprods_com zprods_decl"
    # Wp_Wp_J_J.
    elif [ "$LIB" == "Wp_Wp_J_J" ]; then
	report "ERROR" "The POWHEG library $LIB is not supported."; exit
    # WW.
    elif [ "$LIB" == "WW" ]; then
	FORTRAN_LIB="setanomcoup.f phi1_2.f reshufflemoms.f qqb_ww_v.f breit.f smcouplings.f couplzw.f qqb_ww_g.f qqb_ww.f phi3m0.f spinoru.f cpropfac.f a6loop.f boost.f vpole.f wwamps.f a6tree.f amps_anom.f a7tree.f b7tree.f fa.f t.f lnrat.f b7tree.f lfunctions.f i3m.f BigT.f L34_12.f dclaus.f"
	HEADERS_LIB="PhysPars.h cvecbos.h"
	mv_headers "$SRCDIR/$LIB" "constants zcouple wcouple ewcharge qcdcouple anomcoup vvsettings sprods_com zprods_decl"
    # WZ.
    elif [ "$LIB" == "WZ" ]; then
	FORTRAN_LIB="reshufflemoms.f smcouplings.f suppfact2e.f cpropfac.f phi1_2.f phi3m0.f boost.f breit.f couplz.f qqb_wz.f amps_anom.f spinoru.f a6tree.f t.f qqb_wz_v.f lnrat.f fa.f vpole.f a6loop.f lfunctions.f L34_12.f i3m.f dclaus.f BigT.f qqb_wz_g.f wzamps.f a7tree.f b7tree.f setckmmatrix.f setanomcoup.f"
	HEADERS_LIB="PhysPars.h cvecbos.h"
	mv_headers "$SRCDIR/$LIB" "anomcoup cabibbo ckm constants ewcharge nwz qcdcouple sprods_com vvsettings zprods_decl zwcouple"
    # Z.
    elif [ "$LIB" == "Z" ]; then
	HEADERS_LIB="PhysPars.h"
	# Change specific instance so SED does not fail.
	EXIST=`grep -i "8powheginput" $SRCDIR/$LIB/init_couplings.f`
	if [ -n "$EXIST" ]; then
	    report "WARN" "Replacing 8powheginput in init_couplings.f."
	    sed -i "s/8powheginput/8 powheginput/gi" \
		$SRCDIR/$LIB/init_couplings.f
	fi
    # Z + jet.
    elif [ "$LIB" == "Zj" ]; then
	FORTRAN_LIB="reshufflemoms.f"
	HEADERS_LIB="PhysPars.h"
	mv_headers "$SRCDIR/$LIB/MCFM_include" "constants epinv2 epinv ewcharge ewcouple masses nflav qcdcouple scale scheme sprods_com zcouple zprods_com zprods_decl"
    # ZZ.
    elif [ "$LIB" == "ZZ" ]; then
	FORTRAN_LIB="reshufflemoms.f a6loop.f a6tree.f a7tree.f a7treesingres.f BigT.f boost.f breit.f couplz.f cpropfac.f dclaus.f fa.f i3m.f L34_12.f lfunctions.f lnrat.f phi1_2.f phi3m0.f qqb_zz.f qqb_zz_g.f qqb_zz_v.f smcouplings.f spinoru.f suppfact4e.f t.f vpole.f zzamps.f zzsingres.f"
	HEADERS_LIB="PhysPars.h cvecbos.h"
	mv_headers "$SRCDIR/$LIB" "ewcharge qcdcouple sprods_com vvsettings zcouple zprods_decl"
    else report "ERROR" "Unknown POWHEG library $LIB."; exit
    fi
}

###############################################################################
# Helper functions for the main script.
###############################################################################

# Print report statement.
function report {
    
    # Set numeric mappings.
    if [ "${1}" == "ALWAYS" ]; then LEVEL=0
    elif [ "${1}" == "INFO" ]; then LEVEL=1
    elif [ "${1}" == "WARN" ]; then LEVEL=2
    elif [ "${1}" == "ERROR" ]; then LEVEL=-1
    fi

    # Print the message.
    if [ "${DEBUG_LEVEL}" -ge "${LEVEL}" ]; then
	echo "${1}(${LIB}): ${2}"
    fi
}

# Move headers to match header naming convention.
function mv_headers {
    BASEDIR=$1
    for FILE in $2; do
	if [ -f "$BASEDIR/${FILE}.f" ]; then
	    report "WARN" "Moving ${FILE}.f to ${FILE}.h."
	    mv $BASEDIR/${FILE}.f $BASEDIR/${FILE}.h
	    sed -i "s/${FILE}.f/${FILE}.h/g" $SRCDIR/$LIB/*.f
	fi
	HEADERS_LIB="${HEADERS_LIB} ${FILE}.h"
    done
}

# Safely copy all requested files.
function safe_cp {

    # Sort files and remove duplicates.
    FILES=`echo "$@" | sed 's/[[:space:]]\+/\\n/gi' | sort | uniq -i`
    
    # Loop over all the files.
    for FILE in $FILES; do

	# Check the file does not already exist in the directory.
	if [ ! -f "$TRGDIR/$LIB_LC/$FILE" ]; then
	    
	    # Check library main directory.
	    if [ -f "$SRCDIR/$LIB/$FILE" ]; then
		cp $SRCDIR/$LIB/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library include directory.
	    elif [ -f "$SRCDIR/$LIB/include/$FILE" ]; then
		cp $SRCDIR/$LIB/include/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library Madgraph directory.
	    elif [ -f "$SRCDIR/$LIB/madgraph/$FILE" ]; then
		cp $SRCDIR/$LIB/madgraph/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library Madgraph decayed matrix elements directory.
	    elif [ -f "$SRCDIR/$LIB/madgraph/ME_decayed/$FILE" ]; then
		cp $SRCDIR/$LIB/madgraph/ME_decayed/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library Madgraph undecayed matrix elements directory.
	    elif [ -f "$SRCDIR/$LIB/madgraph/ME_undecayed/$FILE" ]; then
		cp $SRCDIR/$LIB/madgraph/ME_undecayed/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library Madgraph HELAS directory.
	    elif [ -f "$SRCDIR/$LIB/madgraph/dhelas3.2/$FILE" ]; then
		cp $SRCDIR/$LIB/madgraph/dhelas3.2/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library vbfnlo-files directory.
	    elif [ -f "$SRCDIR/$LIB/vbfnlo-files/$FILE" ]; then
		cp $SRCDIR/$LIB/vbfnlo-files/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library vbfnlo-files dhelas subdirectory.
	    elif [ -f "$SRCDIR/$LIB/vbfnlo-files/dhelas/$FILE" ]; then
		cp $SRCDIR/$LIB/vbfnlo-files/dhelas/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library vbfnlo-files tensor_redux subdirectory.
	    elif [ -f "$SRCDIR/$LIB/vbfnlo-files/tensor_redux/$FILE" ]; then
		cp $SRCDIR/$LIB/vbfnlo-files/tensor_redux/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check library MCFM_include directory.
	    elif [ -f "$SRCDIR/$LIB/MCFM_include/$FILE" ]; then
		cp $SRCDIR/$LIB/MCFM_include/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check top main directory.
	    elif [ -f "$SRCDIR/$FILE" ]; then
		cp $SRCDIR/$FILE $TRGDIR/$LIB_LC/$FILE
	    # Check top include directory.
	    elif [ -f "$SRCDIR/include/$FILE" ]; then
		cp $SRCDIR/include/$FILE $TRGDIR/$LIB_LC/$FILE
	    else
		report "WARN" "Can not find $FILE."
	    fi
	else
	    report "WARN" "Refusing to overwrite $FILE."
	fi
    done
}

# Run the POWHEG library specific scripts and change file names.
function run_script {

    # Loop over all the files.
    for SCRIPT in $@; do
	
	# Parse the file name to use.
	CODE=${SCRIPT/*_/}
	CODE=${CODE%.sh}

	# Run the script.
	if [ -f "$SRCDIR/$LIB/$SCRIPT" ]; then
	    if [ ! -x "$SRCDIR/$LIB/$SCRIPT" ]; then
		report "WARN" "Making $SCRIPT executable."
		chmod a+x "$SRCDIR/$LIB/$SCRIPT"
	    fi
	    report "INFO" "Running $SCRIPT."
	    cd $SRCDIR/$LIB/
	    ./$SCRIPT
	    if [ ! -f "$CODE.f" ]; then
		mv *$CODE*.f $CODE.f
	    fi
	    cd $OLDPWD
	else
	    report "WARN" "$SCRIPT not found."
	fi
    done
}

# SED command used to mangle names.
function mangle_sed {
    # First command handles standard usage (excluding ending with a .).
    # Second command handles non-header instances ending with a period.
    # Third command handles instances with an end line.
    sed -i \
	-e "s/\([^[:alnum:]_]\+\)${1}\([^[:alnum:]_.]\+\)/\1${2}\2/gi" \
	-e "s/\([^[:alnum:]_]\+\)${1}\([.]\+[^hi]\)/\1${2}\2/gi" \
	-e "s/\([^[:alnum:]_]\+\)${1}\$/\1${2}/gi" \
	$TRGDIR/$LIB_LC/*
}

# Mangle a list of names in all source files.
function mangle_names {

    # Remove non-unique names and sort.
    NAMES=`echo "$@" | sed 's/[[:space:]]\+/\\n/gi' | sort | uniq -i`

    # Loop over the names.
    for NAME in $NAMES; do

	# Skip the name if it has special meaning in FORTRAN.
	NAME_LC=`echo "${NAME}" | tr "[:upper:]" "[:lower:]"`
	if [ "$NAME_LC" == "d0" ]; then
	    report "WARN" "Skipping mangling the name $NAME."
	    continue
	elif [ "$NAME_LC" == "e0" ]; then
	    report "WARN" "Skipping mangling the name $NAME."
	    continue
	elif [ "$NAME_LC" == "i2" ]; then
	    report "WARN" "Skipping mangling the name $NAME."
	    continue
	fi

        # Report the mangling.
	report "INFO" "Mangling name $NAME."
	
        # Ensure that mangled name is unique.
	MANGLE="${LIB_LC}_${NAME}"
	EXIST=`grep "${MANGLE}" $TRGDIR/$LIB_LC/*`
	if [ -n "$EXIST" ]; then
	    report "WARN" "Base mangled name is not unique."
	    while [ -n "$EXIST" ]; do
		MANGLE="${MANGLE}_X"
		EXIST=`grep "${MANGLE}" $TRGDIR/$LIB_LC/*`
	    done
	    report "WARN" "Mangled name set to ${MANGLE}."
	fi
    
    	# Perform the first SED pass.
    	mangle_sed $NAME $MANGLE

	# Replace instances not caught in first pass (regexp overlap).
	EXIST=`grep -i "[^[:alnum:]_]\+${NAME}[^[:alnum:]_.]\+" $TRGDIR/$LIB_LC/*`
	PASS=1
	while [ -n "$EXIST" ]; do

	    # Make the SED pass.
	    mangle_sed $NAME $MANGLE

	    # Determine if instances still exist.
	    EXIST=`grep -i "[^[:alnum:]_]\+${NAME}[^[:alnum:]_.]\+" $TRGDIR/$LIB_LC/*`

	    # Increment the pass and print warning.
	    let PASS=$PASS+1
	    report "WARN" "Pass number ${PASS} by SED for name ${NAME}."
	done
	
    done
}

###############################################################################
# Main body of the code.
###############################################################################

# Check out the POWHEG-BOX code from SVN.
if [ ! -d "$SRCDIR" ]; then
    svn checkout $REVISION --username anonymous --password anonymous svn://powhegbox.mib.infn.it/trunk/$SRCDIR
    if [ ! -d "$SRCDIR" ]; then
	report "ERROR" "SVN checkout failed."
	exit 1
    fi
fi

# Check that arguments are given.
if [ "$#" -eq 0 ]; then
    LIBS="Dijet gg_H hvq ST_sch ST_tch VBF_H VBF_Wp_Wp W Wbb Wj WW WZ Z Zj ZZ"
else LIBS=$@
fi

# Loop over the libraries.
for LIB in $LIBS; do

    # Convert library name to all lower case.
    # This needs to be done because GCC assigns the external symbol names in 
    # all lower case, so all lower case is used for consistancy.
    LIB_LC=`echo "${LIB}" | tr "[:upper:]" "[:lower:]"`

    # Determine library specific FORTRAN, headers, scripts, and fixes needed.
    report "ALWAYS" "Packaging the $LIB library."
    library_specific

    # Check the target diretory exists.
    if [ ! -d "$TRGDIR" ]; then
	report "ERROR" "Target directory $TRGDIR does not exist."
	exit
    fi
    
    # Create the library directory.
    if [ ! -d "$TRGDIR/$LIB_LC" ]; then
	mkdir $TRGDIR/$LIB_LC
    fi

    # Run any scripts required by the library..
    run_script $SCRIPTS $SCRIPTS_LIB

    # Copy all the necessary files.
    safe_cp $FORTRAN $FORTRAN_LIB $HEADERS $HEADERS_LIB

    # Fix the include file paths.
    sed -i "s/\([[:space:]]\+include[[:space:]]\+['\"]\)[^'\"]*\//\1/gi" $TRGDIR/$LIB_LC/*

    # Find all subroutines.
    SUBROUTINES=`grep --no-filename --ignore-case --only-matching "^[[:space:]]*subroutine[[:space:]]*[[:alnum:]_]*" $TRGDIR/$LIB_LC/* | sed 's/subroutine//gi' `

    # Find all entries.
    ENTRIES=`grep --no-filename --ignore-case --only-matching "^[[:space:]]*entry[[:space:]]*[[:alnum:]_]*" $TRGDIR/$LIB_LC/* | sed 's/entry//gi' `
    
    # Find all functions.
    FUNCTIONS=`grep --no-filename --ignore-case "[[:space:]]\+function[[:space:]]\+[[:alnum:]_]*" $TRGDIR/$LIB_LC/* | grep --ignore-case --invert-match "^[c*\!]" | grep --ignore-case --invert-match "['\"].*function.*['\"]" | grep --ignore-case --only-matching "function[[:space:]]\+[[:alnum:]_]*" | sed 's/function//gi'`

    # Find all common blocks.
    COMMONS=`grep --no-filename --ignore-case --only-matching "[[:space:]]*common[[:space:]]*/[[:space:]]*[[:alnum:]_]\+[[:space:]]*/" $TRGDIR/$LIB_LC/* | sed 's/\// /gi' | sed 's/common/ /gi' | sed 's/[[:space:]]\+/\\n/gi'`

    # Find all data blocks.
    DATAS=`grep --no-filename --ignore-case "[[:space:]]\+block[[:space:]]\+data[[:space:]]\+[[:alnum:]_]*" $TRGDIR/$LIB_LC/* | grep --ignore-case --invert-match "^[c*\!]" | grep --ignore-case --invert-match "['\"].*function.*['\"]" | grep --ignore-case --only-matching "block[[:space:]]\+data[[:space:]]\+[[:alnum:]_]*" | sed 's/block[[:space:]]\+data//gi'`

    # Mangle the names for all subroutines, functions, and common blocks.
    mangle_names $ENTRIES $SUBROUTINES $FUNCTIONS $COMMONS $DATAS
done