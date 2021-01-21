shared_c_files = src/shared/triangle.c \
				 src/shared/vector.c \
				 src/shared/transform.c\
				 src/shared/message.c \
				 src/shared/player.c \
				 src/shared/deltatime.c \
				 src/timer_lib/timer.c \

server_c_files = src/server/server.c \
				 $(shared_c_files) \

client_c_files = src/client/client.c \
				 $(shared_c_files) \


server:
	gcc -g -DSERVER $(server_c_files) -Isrc/shared -Isrc/server -Itimer_lib -o server -lenet -lm -ldl
client:
	gcc -g -Wall -Wextra $(client_c_files) -Isrc/shared -Isrc/client -Itimer_lib -o client -lenet -lSDL2 -lSDL2_ttf -lm -ldl
all:
	make client
	make server
clean:
	-rm client -f
	-rm server -f
	-rm test -f