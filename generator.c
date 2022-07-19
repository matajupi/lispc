#include "lispc.h"

static FILE *outputStream;

static void generateRecursive(Node *node, Environment *env)
{
    // TODO: 
}

void generate(Node *topNode, FILE *ostream, GeneratorType type)
{
    outputStream = ostream;
    Environment *env = createEnvironment();
    generateRecursive(topNode, env);
}

