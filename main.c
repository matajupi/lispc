#include <stdio.h>
#include <stdlib.h>

#include "lispc.h"

static void compile(FILE *inputStream, FILE *outputStream,
                    GeneratorType genType)
{
    Token *tokenLst = tokenize(inputStream);
    tokenLst = preprocess(tokenLst);
// for (Token *cur = tokenLst; cur != NULL; cur = cur->next) {
//     dumpToken(stdout, cur);
//     printf("\n");
// }
    Node *topNode = parse(tokenLst);
// dumpNode(stdout, topNode);
    generate(topNode, outputStream, genType);
}

int main(int argc, char **argv)
{
    // Initialization
    FILE *inputStream = stdin;
    FILE *outputStream = stdout;
    setErrorStream(stderr);

    compile(inputStream, outputStream, GEN_ARM32);

    fclose(inputStream);
    fclose(outputStream);
    return EXIT_SUCCESS;
}

