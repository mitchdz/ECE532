### ECE 532 HW5: Connected Components Labeling
## Requirements
This program was built on Ubuntu 20.04 using GNU Makefile.

## How to Build and Run
To build and run the program simply invoke

```bash
$ make run
```

## Example output
```
$ make run
mkdir -p obj
mkdir -p bin
mkdir -p out
make -C libdip
make[1]: Entering directory '/home/mitch/git/ECE532/hw5/libdip'
mkdir -p obj
make[1]: Leaving directory '/home/mitch/git/ECE532/hw5/libdip'
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/hw5_test.c -o obj/hw5_test.o
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/IO.c -o obj/IO.o
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/thresh.c -o obj/thresh.o
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/mcc.c -o obj/mcc.o
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/mccNode.c -o obj/mccNode.o
gcc -U__STRICT_ANSI__ -fopenmp -I/usr/lib -Isrc -DDEBUG -g -Wall -c src/hw5.c -o obj/hw5.o
gcc -o bin/hw5 obj/hw5_test.o obj/IO.o obj/thresh.o obj/mcc.o obj/mccNode.o obj/hw5.o -Llibdip -ldip -lpng -lz -lm -fopenmp
./bin/hw5 -v -i input/keys.png -o out/keys_hw5.png
Number of Connected Components: 11
./bin/hw5 -v -c 0 -m 0 -i input/book.png -o out/book_hw5.png
Number of Connected Components: 11
```


## Project Details

[hw5 Project](docs/hw5asmt-conn.pdf)

---

![UofA Logo](docs/UofA.jfif)
