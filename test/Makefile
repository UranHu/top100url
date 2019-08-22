all: turl_test

FDIR=$(shell pwd)/..
SRC=$(FDIR)/src

INCLUDE=-I $(FDIR)

turl_test : turl_test.cc $(SRC)/turl_map.cc $(SRC)/turl_define.cc
	g++ turl_test.cc $(SRC)/turl_map.cc $(SRC)/turl_define.cc -o turl_test -pthread -lgtest -lgflags -std=c++11 $(INCLUDE) -g
