objects = main.o http_server.o

http: $(objects)
	gcc -o http $(objects) -lmicrohttpd

main.o: main.c
http_server.o: http_server.c -lmicrohttpd

.PHONY : clean 

clean: 
	rm http $(objects) 
