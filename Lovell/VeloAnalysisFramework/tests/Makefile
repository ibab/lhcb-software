#This used to live in: VeloAnalysisFramework/python/veloview

## Makefile with targets to run tests.
#
# Although sourcing sourceme.sh is required for running interactively
# or using the package from other scripts (w/o installing), it is not
# necessary to run tests with this Makefile.
#

TESTS		:= $(wildcard tests/test_*.py)

DQMTESTS	:= test_avghists test_statcomp
DQMFILES	:= tests/data/dqm_data.root tests/data/dqm_ref.root

.PHONY:	files tests discover $(TESTS:tests/%.py=%)

# all tests
tests:	$(TESTS:tests/%.py=%)

# tests on dqm files (listed by hand in DQMTESTS)
test-dqm:	$(DQMTESTS)

# extra requirement for tests with dqm files
$(DQMTESTS):%:	files

# common rule for all tests
$(TESTS:tests/%.py=%):%:	tests/%.py
	$< $(OPTS)

# same as above, but run under pdb
$(DQMTESTS:%=debug_%):%:	files
$(TESTS:tests/%.py=debug_%):debug_%:	tests/%.py
	python -m pdb $^

# test for dqm files
files:	
	@for f in $(DQMFILES); \
	  do test -f $$f || { echo "Missing ROOT file: $$f" && break; } \
	done

# discover tests with unittest
discover:	files
	python -m unittest discover $(OPTS)

