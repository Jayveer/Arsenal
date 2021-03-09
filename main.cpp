#include "interface/cli/cli.h"

int main(int argc, char** argv) {
	CLI cli = CLI(argc, argv);
	cli.run("Arsenal", "1.0");
}