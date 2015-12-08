#gcc -finput-charset=UTF-8 -o main main.c
ctags -R
gcc -m32 -L/usr/lib32/ -lstdc++ -o main main.c -lm
./main $1
