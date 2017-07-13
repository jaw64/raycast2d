#include "rc2d/Game.h"

/**\brief Entry point of the program.
 *
 * \param argc the number of command line arguments
 * \param argv the command line arguments
 * \return the program's exit code
 */
int main(int argc, char **argv)
{
    Game g;
    return g.run();
}
