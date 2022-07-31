#include "lispc.h"

static Token *wrapTokenLst(Token *tokenLst)
{
    // TODO: letをサポートしないので書き換える．
    Token headToken;
    Token *curToken = &headToken;

    curToken->next = createToken(TK_LEFT_PAREN);
    curToken = curToken->next;

    curToken->next = createToken(TK_LEFT_PAREN);
    curToken = curToken->next;

    curToken->next = createToken(TK_IDENTIFIER);
    curToken = curToken->next;
    curToken->identifier = "lambda";

    curToken->next = createToken(TK_LEFT_PAREN);
    curToken = curToken->next;

    curToken->next = createToken(TK_RIGHT_PAREN);
    curToken = curToken->next;

    curToken->next = tokenLst;
    while (curToken->next->type != TK_EOF) {
        curToken = curToken->next;
    }

    Token *eof = curToken->next;

    curToken->next = createToken(TK_RIGHT_PAREN);
    curToken = curToken->next;

    curToken->next = createToken(TK_RIGHT_PAREN);
    curToken = curToken->next;

    curToken->next = eof;
    return headToken.next;
}

Token *preprocess(Token *tokenLst)
{
    return wrapTokenLst(tokenLst);
}

