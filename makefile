all:Map.o PostingList.o Trie.o JobExecutor.o ListOfWorkersDir.o  MaxMinCountRes.o NetworkFunctions.o ResultParser.o SearchRes.o WC.o Worker.o main.o main
CC = g++
OUT = main
OBJS = Map.o PostingList.o Trie.o JobExecutor.o ListOfWorkersDir.o  MaxMinCountRes.o NetworkFunctions.o ResultParser.o SearchRes.o WC.o Worker.o main.o
RM = rm -f

Map.o: Map.cpp Map.h
	$(CC) -c -o Map.o Map.cpp
PostingList.o: PostingList.cpp PostingList.h
	$(CC) -c -o PostingList.o PostingList.cpp
Trie.o: Trie.cpp Trie.h
	$(CC) -c -o Trie.o Trie.cpp
JobExecutor.o: JobExecutor.cpp JobExecutor.h
	$(CC) -c -o JobExecutor.o JobExecutor.cpp
ListOfWorkersDir.o: ListOfWorkersDir.cpp ListOfWorkersDir.h
	$(CC) -c -o ListOfWorkersDir.o ListOfWorkersDir.cpp
MaxMinCountRes.o: MaxMinCountRes.cpp MaxMinCountRes.h
	$(CC) -c -o MaxMinCountRes.o MaxMinCountRes.cpp
NetworkFunctions.o: NetworkFunctions.cpp NetworkFunctions.h
	$(CC) -c -o NetworkFunctions.o NetworkFunctions.cpp
ResultParser.o: ResultParser.cpp ResultParser.h
	$(CC) -c -o ResultParser.o ResultParser.cpp
SearchRes.o: SearchRes.cpp SearchRes.h
	$(CC) -c -o SearchRes.o SearchRes.cpp
WC.o: WC.cpp WC.h
	$(CC) -c -o WC.o WC.cpp
main.o: main.cpp
	$(CC) -c -o main.o main.cpp
main: Map.o PostingList.o Trie.o JobExecutor.o ListOfWorkersDir.o  MaxMinCountRes.o NetworkFunctions.o ResultParser.o SearchRes.o WC.o Worker.o main.o
	$(CC) -o main Map.o PostingList.o Trie.o JobExecutor.o ListOfWorkersDir.o  MaxMinCountRes.o NetworkFunctions.o ResultParser.o SearchRes.o WC.o Worker.o main.o
clean:
	$(RM) $(OBJS) main

