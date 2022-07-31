#include <stdio.h>
#include <stdbool.h>

#include "lispc.h"

void generate(Node *exp, FILE *ostream, GeneratorType type)
{
    switch (type)
    {
        case GEN_C:
            generateC(exp, ostream);
            break;
    }
}

