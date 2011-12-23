CC  = /usr/bin/g++
BIN = /usr/local/bin/
LIB = /usr/local/lib/
INC = /usr/local/include/
SRC = tsubomi_indexer.cc tsubomi_basic_searcher.cc tsubomi_compressor.cc
HDR = tsubomi_vertical_code.h tsubomi_indexer.h tsubomi_searcher.h \
      tsubomi_basic_searcher.h tsubomi_compressor.h tsubomi_mmap.h tsubomi_defs.h
UNAME = $(shell uname)

tsubomi: tsubomi_mkary tsubomi_mkcsa tsubomi_search libtsubomi
tools: tsubomi_mkary tsubomi_mkcsa tsubomi_search

tsubomi_mkary: $(HDR) tsubomi_indexer.cc tsubomi_mkary.cc
	$(CC) -O2 -Wall -g tsubomi_indexer.cc tsubomi_mkary.cc -o tsubomi_mkary
tsubomi_mkcsa: $(HDR) tsubomi_compressor.cc tsubomi_mkcsa.cc
	$(CC) -O2 -Wall -g tsubomi_compressor.cc tsubomi_mkcsa.cc -o tsubomi_mkcsa
tsubomi_search: $(HDR) tsubomi_basic_searcher.cc tsubomi_compressor.cc \
  tsubomi_search.cc
	$(CC) -O2 -Wall -g tsubomi_basic_searcher.cc tsubomi_compressor.cc \
      tsubomi_search.cc -o tsubomi_search

libtsubomi: $(SRC) $(HDR)
ifeq ($(UNAME), Darwin)
	$(CC) -fno-common -O2 -Wall -fPIC -g -c tsubomi_indexer.cc
	$(CC) -fno-common -O2 -Wall -fPIC -g -c tsubomi_basic_searcher.cc
	$(CC) -fno-common -O2 -Wall -fPIC -g -c tsubomi_compressor.cc
	$(CC) -O2 -Wall -g -dynamiclib -install_name=$(LIB)libtsubomi.1.0.dylib \
      -o libtsubomi.1.0.dylib \
      tsubomi_indexer.o tsubomi_basic_searcher.o tsubomi_compressor.o
else
	$(CC) -O2 -Wall -fPIC -g -c tsubomi_indexer.cc
	$(CC) -O2 -Wall -fPIC -g -c tsubomi_basic_searcher.cc
	$(CC) -O2 -Wall -fPIC -g -c tsubomi_compressor.cc
	$(CC) -O2 -Wall -g -shared -Wl,-soname,libtsubomi.so.1 \
      -o libtsubomi.so.1.0 \
      tsubomi_indexer.o tsubomi_basic_searcher.o tsubomi_compressor.o
endif
	rm tsubomi_indexer.o
	rm tsubomi_basic_searcher.o
	rm tsubomi_compressor.o

clean:
	- rm -f tsubomi_mkary
	- rm -f tsubomi_mkcsa
	- rm -f tsubomi_search
	- rm -f libtsubomi.so.1.0
	- rm -f libtsubomi.1.0.dylib
	- rm -Rf *.dSYM

install:
ifeq ($(UNAME), Linux)
	cp libtsubomi.so.1.0 $(LIB)
	/sbin/ldconfig -n $(LIB)
	ln -fs $(LIB)libtsubomi.so.1 $(LIB)libtsubomi.so
else
ifeq ($(UNAME), Darwin)
	cp libtsubomi.1.0.dylib $(LIB)
	ln -fs $(LIB)libtsubomi.1.0.dylib $(LIB)libtsubomi.1.dylib
	ln -fs $(LIB)libtsubomi.1.dylib $(LIB)libtsubomi.dylib
else
	cp libtsubomi.so.1.0 $(LIB)
	ln -fs $(LIB)libtsubomi.so.1.0 $(LIB)libtsubomi.so.1
	/sbin/ldconfig -m $(LIB)
	ln -fs $(LIB)libtsubomi.so.1 $(LIB)libtsubomi.so
endif
endif
	cp tsubomi_mkary $(BIN)
	cp tsubomi_mkcsa $(BIN)
	cp tsubomi_search $(BIN)
	cp tsubomi_*.h $(INC)

uninstall:
	- rm -f $(LIB)libtsubomi.so
	- rm -f $(LIB)libtsubomi.so.1
	- rm -f $(LIB)libtsubomi.so.1.0
	- rm -f $(LIB)libtsubomi.dylib
	- rm -f $(LIB)libtsubomi.1.dylib
	- rm -f $(LIB)libtsubomi.1.0.dylib
	- rm -f $(BIN)tsubomi_mkary
	- rm -f $(BIN)tsubomi_mkcsa
	- rm -f $(BIN)tsubomi_search
	- rm -f $(INC)tsubomi_*.h

