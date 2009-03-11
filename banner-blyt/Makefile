CPPFLAGS=-g

dump_brlyt: dump_brlyt.o chunk.o chunk_lyt1.o chunk_pane.o chunk_group.o
	g++ dump_brlyt.o chunk.o chunk_lyt1.o chunk_pane.o chunk_group.o -o dump_brlyt

chunk.o: chunk.cpp
	g++ -c chunk.cpp $(CPPFLAGS)

chunk_lyt1.o: chunk_lyt1.cpp
	g++ -c chunk_lyt1.cpp $(CPPFLAGS)


chunk_pane.o: chunk_pane.cpp
	g++ -c chunk_pane.cpp $(CPPFLAGS)

chunk_group.o: chunk_group.cpp
	g++ -c chunk_group.cpp $(CPPFLAGS)

dump_brlyt.o: dump_brlyt.cpp
	g++ -c dump_brlyt.cpp $(CPPFLAGS)

.PHONY: clean
clean:
	-rm dump_brlyt.o chunk.o chunk_lyt1.o chunk_pane.o chunk_group.o dump_brlyt
