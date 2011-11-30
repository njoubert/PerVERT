#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "backend.h"
#include "server/server.h"

namespace Backend {
	
	
void init() {
	
	Server::Server &server = Backend::Server::Server::Instance();
	server.start();
	
	printf("Started server...\n");
	getchar();
	
}

} /* namespace Backend */


int main(int argc, char *argv[]) {
	
	Backend::init();

	return 0;
}