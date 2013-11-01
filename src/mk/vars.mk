CXX = clang++
LINK.o = $(LINK.cc)

CXXFLAGS += -ggdb -W -Wall -Wextra
CXXFLAGS += -std=c++11 -stdlib=libc++
CXXFLAGS += -I..

CPPLINT_FLAGS = --root=$$HOME/github/wt2/src/ --filter=-readability/streams --filter=-legal/copyright
