CC  = /usr/bin/g++
BIN = /usr/local/bin/
LIB = /usr/local/lib/
INC = /usr/local/include/
UNAME = $(shell uname)

futaba: erika_match
erika_match: futaba_match.cc
	$(CC) -O2 -Wall -g futaba_match.cc -ltsubomi -o futaba_match

clean:
	- rm -f futaba_match

install:
	cp futaba_make_tfidf.pl $(BIN)
	cp futaba_make_svm.pl $(BIN)
	cp futaba_match $(BIN)

uninstall:
	- rm -f $(BIN)futaba_make_tfidf.pl
	- rm -f $(BIN)futaba_make_svm.pl
	- rm -f $(BIN)futaba_match

