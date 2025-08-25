a.out:main.o encode.o decode.o
    gcc -o a.out main.o encode.o decode.o
main.o:main.c
    gcc -c main.c
encode.o:encode.c
    gcc -c encode.c
decode.o:decode.c
    gcc -c decode.c
clean:
    rm *.o *.out
