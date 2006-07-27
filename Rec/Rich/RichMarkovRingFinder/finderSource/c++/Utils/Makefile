.PHONEY : default

default:
	make dereferencingOstreamIteratorTest 
	./dereferencingOstreamIteratorTest
	make test
	./testCheck

test: test.cc $(wildcard *.h)
	g++ -o $@ $< `clhepFlags ` -Wall -pedantic -ansi -I..

sum: sum.cc $(wildcard *.h)
	g++ -o $@ $< `clhepFlags ` -Wall -pedantic -ansi -I.. -I$(BOOST_INC_DIR)

dereferencingOstreamIteratorTest: dereferencingOstreamIteratorTest.cc $(wildcard *.h)
	g++ -o $@ $< `clhepFlags ` -Wall -pedantic -ansi -I..
