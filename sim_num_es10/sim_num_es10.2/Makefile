CC = mpicxx
CFLAGS = -Wall -O3 --std=c++11

main.exe : main.o random.o city.o individual.o population.o
	$(CC) main.o random.o city.o individual.o population.o -o main.exe

main.o : main.cpp
	$(CC) -c main.cpp -o main.o $(CFLAGS)

random.o : random.cpp random.h
	$(CC) -c random.cpp -o random.o $(CFLAGS)

city.o : city.cpp city.h
	$(CC) -c city.cpp -o city.o $(CFLAGS)

individual.o : individual.cpp individual.h city.h
	$(CC) -c individual.cpp -o individual.o $(CFLAGS)

population.o : population.cpp population.h individual.h city.h
	$(CC) -c population.cpp -o population.o $(CFLAGS)

clean :
	rm -f *.o main.exe seed.out