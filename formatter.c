#include "lispc.h"



Node *format(Node *node)
{
    // TODO:
    // procedure-definition -> lambda-definition
    // let -> lambda-call
    // Leafまで再帰下降
    // 登りながら変換
    if (isLeaf(node)) {
        return node;
    }

    Node *carNode = format(car(node));
    Node *cdrNode = format(cdr(node));
    Node *newNode = cons(carNode, cdrNode);

    if (isProcedureDefinition(newNode)) {
        Node *lambda = createLambda(definitionValue(newNode));
    }
}
