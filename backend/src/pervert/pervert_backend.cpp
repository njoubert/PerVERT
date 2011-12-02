#include <cstring>

#include "pervert/backend.h"
#include "pervert/server/server.h"
#include "pervert/server/config.h"
#include "pervert/app/loggerlayer.h"
#include "pervert/app/pervertlayer.h"
#include "pervert/app/staticlayer.h"

namespace PerVERT {
	
using namespace Utils;

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int getWord(char* buffer) {
	int count = 0;
	int c;
	while ((c = getchar()) != ' ' && c != '\n') { buffer[count] = c; count++; }
	return count;
}

void interactiveloop() {
	char buffer[1024];
	int c;
	while (1) {
		printf("> ");
		c = getWord(buffer);
		if (buffer[0] == 'q') {
			break;			
		} else  if (buffer[0] == 's') {
			printf("No status available.\n");
		} else  if (buffer[0] == 'l') {
			c = getWord(buffer);
			if (c == 1 && buffer[0] > 47 && buffer[0] < 56) {
				Singleton<LogFactory>::Instance().setAllLogLevel((LOGLEVEL)(buffer[0]-48));
			} else {
				printf("Specify a logging level 0-7 (0 is off).\n");
			}
		} else if (buffer[0] == 'h') {
			printf("Commands:\n");
			printf("  \'q\' - quit\n");
			printf("  \'s\' - status\n");
			printf("  \'l [0-6]\' - change logging level\n");
		} else {
			printf("Press \'q'\' to quit, 'h' for help.\n");			
		}			
	}	
}

void init(Server::Config *config) {
	Log& log = GETLOG("MAIN");
	log.log(LOG_STATUS, "Welcome to the PerVERT PerVERT.\n");
	
	log.log(LOG_DEBUG, "DEBUG IS ON");
	
	Server::Server &server = PerVERT::Server::Server::Instance();
	server.registerLayer(new App::LoggerLayer(App::TINY,"server.log"));
	server.registerLayer(new App::PervertLayer());
	server.registerLayer(new App::StaticLayer("../frontend/"));
	server.start();
	


	if (config->makeDaemon) {
		log.log(LOG_MESSAGE, "Daemonizing...\n");
		Singleton<LogFactory>::Instance().setAllLogTo(fopen("server.err", "w"));
		Utils::daemonize(EXIT_SUCCESS,EXIT_FAILURE);
		while(1) {
			getchar();
		}
	} else {
		interactiveloop();
	}

	
}

} /* namespace PerVERT */


void usage() {
	printf("usage: server [--daemon]\n");
	printf("  --daemon        Forks the service as a daemon.\n");
}

int main(int argc, char *argv[]) {
	
	PerVERT::Server::Config config;
	
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i],"--daemon") == 0) {
			config.makeDaemon = true;
		} else {
			fprintf(stderr,"Option not recognized, quitting %s\n", argv[i]);
			usage();
			exit(1);
		}
	}

	
	PerVERT::init(&config);

	return 0;
}
