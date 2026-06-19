CC = gcc
TARGET = rls_analyze.exe
OBJS = main.o tracks.o args.o file.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) -c $< -o $@

main.o: tracks.h args.h file.h
tracks.o: tracks.h
args.o: args.h
file.o: file.h

clean:
	rm *.o $(TARGET)
