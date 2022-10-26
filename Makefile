
usage:
	@echo "Usage:"
	@echo -e "\tmake [build|run|clean]"

build: src/*.cpp
	-mkdir -p obj
	g++ -Wall -Wextra -pedantic -std=c++20 -g src/*.cpp -o obj/main.out

run: build
	./obj/main.out

clean:
	-rm -rf obj

