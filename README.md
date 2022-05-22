# Instructions
## build executables from source code
```shell
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
## run server
```shell
$ ./wserver 12345
Starting...
```
## use netcat to connect to server
```shell
$ nc -C localhost 12345
```
## check if server accepted connection
```shell
Starting...
Accepted connection 0
```
## type GET request
```shell
$ nc -C localhost 12345
$ GET /simple?arg1=name&arg2=value HTTP/1.1
```
## send newline to execute GET request
```shell
$ nc -C localhost 12345
$ GET /simple?arg1=name&arg2=value HTTP/1.1

HTTP/1.1 200 OK
Content-type: text/html

<html><head>
<title>Hello World</title>
<link rel="icon" href="data:,"></head>
<body>
<h2>Hello, world!</h2>
<p>QUERY_STRING = arg1=name&arg2=value</p>
<ul>
<li>arg1 = name</li>
<li>arg2 = value</li>
</ul>
</body></html>
```
# Clean up resources
```shell
$ make clean
rm *.o wserver simple term slowcgi large testprogtable
```
