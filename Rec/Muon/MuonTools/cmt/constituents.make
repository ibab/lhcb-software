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

$(bin)MuonTools.make :: dirs
	@$(cmtexe) build constituent_makefile MuonTools -quiet -tag=$(tag); \
	  if test ! "$(bin)" = "./"; then /bin/mv -f MuonTools.make $(bin); fi

MuonTools :: $(MuonTools_dependencies) $(bin)MuonTools.make
	@echo " starting MuonTools"
	@$(MAKE) -f $(bin)MuonTools.make $(MAKEFLAGS) build_strategy=keep_makefiles MuonTools

MuonToolsclean :: $(MuonToolsclean_dependencies) $(bin)MuonTools.make
	@echo " starting MuonToolsclean"
	@$(MAKE) -f $(bin)MuonTools.make $(MAKEFLAGS) build_strategy=keep_makefiles MuonToolsclean



$(bin)MuonToolsChk.make :: dirs
	@$(cmtexe) build constituent_makefile MuonToolsChk -quiet -tag=$(tag); \
	  if test ! "$(bin)" = "./"; then /bin/mv -f MuonToolsChk.make $(bin); fi

MuonToolsChk :: $(MuonToolsChk_dependencies) $(bin)MuonToolsChk.make
	@echo " starting MuonToolsChk"
	@$(MAKE) -f $(bin)MuonToolsChk.make $(MAKEFLAGS) build_strategy=keep_makefiles MuonToolsChk

MuonToolsChkclean :: $(MuonToolsChkclean_dependencies) $(bin)MuonToolsChk.make
	@echo " starting MuonToolsChkclean"
	@$(MAKE) -f $(bin)MuonToolsChk.make $(MAKEFLAGS) build_strategy=keep_makefiles MuonToolsChkclean


