all_groups :: all

all :: $(all_constituents)
	@/bin/echo " all ok."

clean :: allclean
	@/bin/echo " clean ok."

allclean :: $(all_constituentsclean)
	@/bin/echo " allclean ok."
all_groups :: RuleChecker

RuleChecker :: $(RuleChecker_constituents)
	@/bin/echo " RuleChecker ok."

clean :: allclean
	@/bin/echo " clean ok."

RuleCheckerclean :: $(RuleChecker_constituentsclean)
	@/bin/echo " RuleCheckerclean ok."

$(bin)PuVeto.make :: dirs
	@$(cmtexe) build constituent_makefile PuVeto -quiet -tag=$(tag); \
	  if test ! "$(bin)" = "./"; then /bin/mv -f PuVeto.make $(bin); fi

PuVeto :: $(PuVeto_dependencies) $(bin)PuVeto.make
	@echo " starting PuVeto"
	@$(MAKE) -f $(bin)PuVeto.make $(MAKEFLAGS) build_strategy=keep_makefiles PuVeto

PuVetoclean :: $(PuVetoclean_dependencies) $(bin)PuVeto.make
	@echo " starting PuVetoclean"
	@$(MAKE) -f $(bin)PuVeto.make $(MAKEFLAGS) build_strategy=keep_makefiles PuVetoclean



$(bin)PuVetoChk.make :: dirs
	@$(cmtexe) build constituent_makefile PuVetoChk -quiet -tag=$(tag); \
	  if test ! "$(bin)" = "./"; then /bin/mv -f PuVetoChk.make $(bin); fi

PuVetoChk :: $(PuVetoChk_dependencies) $(bin)PuVetoChk.make
	@echo " starting PuVetoChk"
	@$(MAKE) -f $(bin)PuVetoChk.make $(MAKEFLAGS) build_strategy=keep_makefiles PuVetoChk

PuVetoChkclean :: $(PuVetoChkclean_dependencies) $(bin)PuVetoChk.make
	@echo " starting PuVetoChkclean"
	@$(MAKE) -f $(bin)PuVetoChk.make $(MAKEFLAGS) build_strategy=keep_makefiles PuVetoChkclean


