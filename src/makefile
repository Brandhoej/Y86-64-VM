CC: gcc
CC_FLAGS: -wall
APP_FLAGS: test.c

app: main.o chunk.o debug.o memory.o value.o vm.o
	gcc build/objs/main.o build/objs/chunk.o build/objs/debug.o build/objs/memory.o build/objs/value.o build/objs/vm.o -o build/vm

main.o: main.c
	gcc -c $< -o build/objs/$@

chunk.o: chunk.c chunk.h
	gcc -c $< -o build/objs/$@

debug.o: debug.c debug.h
	gcc -c $< -o build/objs/$@

memory.o: memory.c memory.h
	gcc -c $< -o build/objs/$@

value.o: value.c value.h
	gcc -c $< -o build/objs/$@

vm.o: vm.c vm.h
	gcc -c $< -o build/objs/$@

clean:
	rm *.o