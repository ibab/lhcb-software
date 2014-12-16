TESTS := $(wildcard tests/test-*.py)

.PHONY:	tests $(TESTS:tests/%.py=%)

tests:	$(TESTS)
	@for t in $^; \
         do \
            $$t $(OPTS); \
         done

$(TESTS:tests/%.py=%):%:	tests/%.py
	$^ $(OPTS)

$(TESTS:tests/%.py=debug-%):debug-%:	tests/%.py
	python -m pdb $^
