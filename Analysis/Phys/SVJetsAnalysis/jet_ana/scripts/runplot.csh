#!/bin/tcsh

set me = `whoami`

set thissel = "./selection_testsb_v22/"
if ( $# > 0 ) then
   set thissel = "${1}"
endif

set webbase = "/home/${me}/public_html/d0_private/diboson/MJControlSample_test/${thissel}/"
#set webbase = "/home/${me}/public_html/d0_private/hznunubb/EPS11/${thissel}/data_preparation/"
#set webbase = "/home/${me}/public_html/d0_private/hznunubb/EPS11/${thissel}/conference_note/"
set webbase = `echo ${webbase} | sed -e 's:selection_::'`
mkdir -p ${webbase}

set samples = "qcd"
#set samples = "signal ew qcd"
set epochs = "RunIIb23"
#set epochs = "RunIIa RunIIb1 RunIIb23 RunII"
#set masses = "115 100 105 110 120 125 130 135 140 145 150 165"
set masses = "115"
set njets = "2jet 3jet"
set seltags = "pretag"# 0tag 1tag 2tag"
set plottags = "pretag"# 0tag 1tag 2tag"# pretag_MJDTcut 0tag_MJDTcut 1tag_MJDTcut 2tag_MJDTcut"
#set plottags = "${plottags} pretag_MJDTcut_PDTcut 1tag_MJDTcut_PDTcut 2tag_MJDTcut_PDTcut"
#set plottags = "${plottags} pretag_MJDTcut_invPDTcut 1tag_MJDTcut_invPDTcut 2tag_MJDTcut_invPDTcut"
#set plottags = "${plottags} onetagg zerobtag onebtag" ## extras here are okay

foreach sample ( `echo ${samples}` )
foreach epoch ( `echo ${epochs}` )
foreach njet ( `echo ${njets}` )
foreach mass ( `echo  ${masses}` )
if ( ( ${epoch} == 'RunIIa' || ${epoch} == 'RunII' ) && ( ${mass} == 155 || ${mass} == 160 ) ) then
   continue;
endif
foreach tag ( `echo ${seltags}` )
./RunPlots.exe ${thissel} ${sample} ${epoch} ${njet} ${tag} none ${mass}
#./RunPlots.exe ${thissel} ${sample} ${epoch} ${njet} ${tag} MJ ${mass}
#./RunPlots.exe ${thissel} ${sample} ${epoch} ${tag} MJPhys ${mass}
#./RunPlots.exe ${thissel} ${sample} ${epoch} ${tag} MJinvPhys ${mass}
end # tag loop
end # mass loop
end # njet loop
end # epoch loop
end # sample loop

# copy plots

foreach sample ( `echo ${samples}` )
foreach epoch ( `echo ${epochs}` )
foreach njet ( `echo ${njets}` )
set webdir = "${webbase}/${epoch}/"
cp ScaleFactors_RunII* ${webbase}/
foreach mass ( `echo  ${masses}` )
if ( ${epoch} == 'RunIIa' && ( ${mass} == 155 || ${mass} == 160 ) ) then
   continue;
endif
foreach tag ( `echo ${plottags}` )
if ( -e ${thissel}/${mass}/nominal/${epoch}/${sample}/${njet}_${tag}/JetN.eps ) then
echo 'Copying '${webdir}'/'${mass}'/nominal/'${epoch}/${sample}'/'${njet}_${tag}'/'
mkdir -p ${webdir}/${mass}/${sample}/${njet}_${tag}/
cp ${thissel}/${mass}/nominal/${epoch}/${sample}/${njet}_${tag}/*.eps ${webdir}/${mass}/${sample}/${njet}_${tag}/
cp ${thissel}/${mass}/nominal/${epoch}/${sample}/${njet}_${tag}/*.gif ${webdir}/${mass}/${sample}/${njet}_${tag}/
cp ${thissel}/${mass}/nominal/${epoch}/${sample}/Events_${epoch}_${njet}_${tag}.txt ${webdir}/${mass}/${sample}/${njet}_${tag}/Events.txt
rm -f ${thissel}/${mass}/nominal/${epoch}/${sample}/${njet}_${tag}/*.eps
rm -f ${thissel}/${mass}/nominal/${epoch}/${sample}/${njet}_${tag}/*.gif
endif
end # tag loop
end # mass loop
end # njet loop
end # epoch loop
end # sample loop

# produce webpages

foreach sample ( `echo ${samples}` )
foreach epoch ( `echo ${epochs}` )
foreach njet ( `echo ${njets}` )
set webdir = "${webbase}/${epoch}/"
foreach mass ( `echo  ${masses}` )
if ( ${epoch} == 'RunIIa' && ( ${mass} == 155 || ${mass} == 160 ) ) then
   continue;
endif
foreach tag ( `echo ${plottags}` )
if ( -e ${webdir}/${mass}/${sample}/${njet}_${tag}/JetN.eps ) then
echo "Creating ${webdir}/${mass}/${sample}/${njet}_${tag}/"
./scripts/createHTMLpageAllPlots.sh ${webdir}/${mass}/${sample}/${njet}_${tag}/ &
endif
end # tag loop
end # mass loop
end # njet loop
end # epoch loop
end # sample loop

exit
