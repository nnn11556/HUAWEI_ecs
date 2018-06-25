
LINlib = ../lib

CXXFLAGS = -Wall -O3 -std -I$(LINlib) -I.. -g
all : result
result :date_time.o ecs.o handle_data.o io.o linear_regression.o\
		predict.o put_flavors_to_servers.o server.o
		
date_time.o : ../date_time.cpp
	g++ -c $(CXXFLAGS) ../date_time.cpp
ecs.o :../ecs.cpp
	g++ -c $(CXXFLAGS) ../ecs.cpp
handle_data.o :../handle_data.cpp
	g++ -c $(CXXFLAGS) ../handle_data.cpp
io.o :../io.cpp
	g++ -c $(CXXFLAGS) ../io.cpp
linear_regression.o :../linear_regression.cpp
	g++ -c $(CXXFLAGS) ../linear_regression.cpp
predict.o :../predict.cpp
	g++ -c $(CXXFLAGS) ../predict.cpp
put_flavors_to_servers.o :../put_flavors_to_servers.cpp
	g++ -c $(CXXFLAGS) ../put_flavors_to_servers.cpp
server.o :../server.cpp
	g++ -c $(CXXFLAGS) ../server.cpp

date_time.o : ../predict.h
ecs.o : ../predict.h $(LINlib)/lib_io.h $(LINlib)/lib_time.h 
handle_data.o : ../predict.h
io.o : ../unistd.h
linear_regression.o : ../predict.h
predict.o : ../predict.h
put_flavors_to_servers.o : ../predict.h
server.o : predict.h