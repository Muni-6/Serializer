PROBLEM_SUBDIRS = problem-reader-writer problem-monkey-crossing problem-child-care
TESTER_SUBDIRS = tester-1 tester-2
CLEAN_SUBDIRS = clean-serializer $(addprefix clean-,$(PROBLEM_SUBDIRS)) $(addprefix clean-,$(TESTER_SUBDIRS)) 

.PHONY: clean serializer $(PROBLEM_SUBDIRS) $(TESTER_SUBDIRS) $(CLEAN_SUBDIRS)

default: serializer $(PROBLEM_SUBDIRS) $(TESTER_SUBDIRS)

serializer:
	+$(MAKE) -C $@

$(PROBLEM_SUBDIRS): serializer
	+$(MAKE) -C $@

$(TESTER_SUBDIRS): serializer $(PROBLEM_SUBDIRS)
	+$(MAKE) -C $@

clean: $(CLEAN_SUBDIRS)
	rm -f *.txt *.db

$(CLEAN_SUBDIRS): clean-%:
	+$(MAKE) -C $* clean
