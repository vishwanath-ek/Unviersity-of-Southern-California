application: warehouse.o store1.o store2.o store3.o store4.o
	@g++ -o warehouse warehouse.o -g -lnsl -lsocket -lresolv
	@g++ -o store1 store1.o -g -lnsl -lsocket -lresolv
	@g++ -o store2 store2.o -g -lnsl -lsocket -lresolv
	@g++ -o store3 store3.o -g -lnsl -lsocket -lresolv
	@g++ -o store4 store4.o -g -lnsl -lsocket -lresolv

warehouse.o: warehouse.cpp
	@g++ -c warehouse.cpp

store1.o: store1.cpp
	@g++ -c store1.cpp

store2.o: store2.cpp
	@g++ -c store2.cpp

store3.o: store3.cpp
	@g++ -c store3.cpp

store4.o: store4.cpp
	@g++ -c store4.cpp
clean:
	@rm -rf *o warehouse warehouse.o store1 store1.o store2 store2.o store3 store3.o store4 store4.o
