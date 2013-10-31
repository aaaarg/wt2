include ../mk/vars.mk

DEPS += ../utils/log.o

UNITTESTS += pdu_unittest
UNITTESTS += options_unittest
UNITTESTS += optstore_unittest

CLEANFILES += $(wildcard *.o) $(UNITTESTS)

all: $(UNITTESTS)

proto.o: proto.h

pdu_unittest: pdu.o proto.o pdu_unittest.o $(DEPS)
pdu_unittest.o: $(wildcard *.h)
pdu.o: $(wildcard *.h)

options_unittest: proto.o options.o options_unittest.o $(DEPS)
options_unittest.o: $(wildcard *.h)
options.o: $(wildcard *.h)

optstore_unittest: optstore_unittest.o $(DEPS)
optstore_unittest.o: $(wildcard *.h)

include ../mk/rules.mk
