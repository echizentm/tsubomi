SRC = tsubomi_indexer.cc tsubomi_searcher.cc tsubomi_mkary.cc tsubomi_sass.cc
HDR = tsubomi_indexer.cc tsubomi_searcher.cc tsubomi_mmap.h tsubomi_defs.h tsubomi.h

tsubomi: tsubomi_mkary tsubomi_sass
	
tsubomi_mkary: $(SRC) $(HDR)
	g++ -O2 tsubomi_indexer.cc tsubomi_mkary.cc -o tsubomi_mkary

tsubomi_sass: $(SRC) $(HDR)
	g++ -O2 tsubomi_searcher.cc tsubomi_sass.cc -o tsubomi_sass

clean:
	rm tsubomi_mkary
	rm tsubomi_sass
