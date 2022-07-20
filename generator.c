#include <stdio.h>
#include <stdbool.h>

#include "lispc.h"

void generate(Node *topNode, FILE *ostream, GeneratorType type)
{
    switch (type)
    {
        case GEN_ARM32:
            generateArm32(topNode, ostream);
            break;
    }
}

