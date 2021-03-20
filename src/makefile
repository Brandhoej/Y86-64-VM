CC: gcc
CC_FLAGS: -wall
APP_FLAGS: test.c

app: main.o writer.o memory.o vm.o printer.o
	gcc build/objs/main.o build/objs/writer.o build/objs/memory.o build/objs/vm.o build/objs/printer.o -o build/vm

main.o: main.c
	gcc -c $< -o build/objs/$@

memory.o: memory.c memory.h
	gcc -c $< -o build/objs/$@

vm.o: vm.c vm.h
	gcc -c $< -o build/objs/$@

writer.o: writer.c writer.h
	gcc -c $< -o build/objs/$@

printer.o: printer.c printer.h
	gcc -c $< -o build/objs/$@

clean:
	rm *.o