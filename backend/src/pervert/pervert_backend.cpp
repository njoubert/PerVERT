#include <cstring>

#include "utils/server/server.h"
#include "utils/server/config.h"
#include "utils/server/layers/loggerlayer.h"
#include "utils/server/layers/staticlayer.h"
#include "utils/server/layers/querylayer.h"

#include "pervert/backend.h"
#include "pervert/app/pervertlayer.h"

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

	//If you want to use relative paths to the current running directory,
	//you HAVE to initialize this code BEFORE you daemonize, since 
	//daemonizing changes the current directory  to avoid locking a dir.
	Server::Server &server = Server::Server::Instance();
	server.registerLayer(new Server::Layers::LoggerLayer(Server::Layers::TINY,"server.log"));
	server.registerLayer(new Server::Layers::QueryLayer());
	server.registerLayer(new App::PervertLayer());
	server.registerLayer(new Server::Layers::StaticLayer("/../frontend",10485760));


	if (config->makeDaemon) {
		log.log(LOG_MESSAGE, "Daemonizing...\n");
		Singleton<LogFactory>::Instance().setAllLogTo(fopen("server.err", "a"));
		
		#define MAXPATHLEN 1024
		char path[MAXPATHLEN];
        getcwd(path, MAXPATHLEN);
		strcat(path, "/server.pid");
		#undef MAXPATHLEN
		
		Utils::daemonize(EXIT_SUCCESS,EXIT_FAILURE);
		
		int pid = getpid();
		
		log.log(LOG_MESSAGE, "PerVERT daemon running, pid is %d\n", pid);
		FILE* fpid = fopen(path, "w");
		fprintf(fpid, "%d\n",pid);
		fclose(fpid);
	}
	
	
	server.start();
	

	if (config->makeDaemon) {
		Utils::infinitesleep();
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
	
	Utils::Server::Config config;
	
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
