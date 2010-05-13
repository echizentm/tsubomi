BIN = /usr/local/bin/
LIB = /usr/local/lib/
INC = /usr/local/include/
SRC = tsubomi_indexer.cc tsubomi_searcher.cc
HDR = tsubomi_indexer.h tsubomi_searcher.h tsubomi_mmap.h tsubomi_defs.h tsubomi.h

tsubomi: tsubomi_mkary tsubomi_sass libtsubomi
	
tsubomi_mkary: $(SRC_I) $(HDR) tsubomi_mkary.cc
	g++ -O2 -Wall -fPIC -g tsubomi_indexer.cc tsubomi_mkary.cc -o tsubomi_mkary

tsubomi_sass: $(SRC_S) $(HDR) tsubomi_sass.cc
	g++ -O2 -Wall -g tsubomi_searcher.cc tsubomi_sass.cc -o tsubomi_sass

libtsubomi: $(SRC_S) $(HDR)
	g++ -O2 -Wall -fPIC -g -c tsubomi_indexer.cc
	g++ -O2 -Wall -fPIC -g -c tsubomi_searcher.cc
	g++ -O2 -Wall -g -shared -Wl,-soname,libtsubomi.so.1 -o libtsubomi.so.1.0 tsubomi_indexer.o tsubomi_searcher.o
	rm tsubomi_indexer.o
	rm tsubomi_searcher.o

clean:
	- rm -f tsubomi_mkary
	- rm -f tsubomi_sass
	- rm -f libtsubomi.so.1.0

install: tsubomi
	cp libtsubomi.so.1.0 $(LIB)
	/sbin/ldconfig -n $(LIB)
	ln -fs $(LIB)libtsubomi.so.1 $(LIB)libtsubomi.so
	cp tsubomi_mkary $(BIN)
	cp tsubomi_sass $(BIN)
	cp tsubomi_*.h $(INC)
	cp tsubomi.h $(INC)

uninstall:
	- rm -f $(LIB)libtsubomi.so
	- rm -f $(LIB)libtsubomi.so.1
	- rm -f $(LIB)libtsubomi.so.1.0
	- rm -f $(BIN)tsubomi_mkary
	- rm -f $(BIN)tsubomi_sass
	- rm -f $(INC)tsubomi_*.h
	- rm -f $(INC)tsubomi.h
