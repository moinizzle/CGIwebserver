# Instructions
```shell
## build executables from source code
$ make
gcc -g -Wall  -c wserver.c
gcc -g -Wall  -c wrapsock.c
gcc -g -Wall  -c progtable.c
gcc -g -Wall  -c ws_helpers.c
gcc -g -Wall  -c process_request.c
gcc -g -Wall -o wserver wserver.o wrapsock.o progtable.o ws_helpers.o process_request.o  
gcc -g -Wall  -c simple.c
gcc -g -Wall  -c cgi.c
gcc -g -Wall -o simple simple.o cgi.o  
gcc -g -Wall  -c term.c
gcc -g -Wall -o term term.o  
gcc -g -Wall  -c slowcgi.c
gcc -g -Wall -o slowcgi slowcgi.o  
gcc -g -Wall  -c testprogtable.c
gcc -g -Wall -o testprogtable testprogtable.o progtable.o  
gcc -g -Wall  -c large.c
gcc -g -Wall -o large large.o cgi.o
```

# Clean up resources
```shell
$ make clean
rm *.o wserver simple term slowcgi large testprogtable
```
