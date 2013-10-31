include ../mk/vars.mk

UNITTESTS += log_unittest

CLEANFILES += $(wildcard *.o) $(UNITTESTS)

log_unittest: log.o log_unittest.o
log_unittest.o: $(wildcard *.h)
log.o: $(wildcard *.h)

include ../mk/rules.mk
