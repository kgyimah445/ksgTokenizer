#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "toyTokenizer.cpp"
using namespace std;
 
// =====================================================================
// PARSER  — recursive descent following the BNF
//   S -> <A><C><B> | <A>
//   <A> -> a<A> | a
//   <B> -> b<B> | b
//   <C> -> c
// =====================================================================
struct Parser {
    Token*    tokens;
    int       count;
    int       pos;
};
 
struct ParseError {};   // thrown by prError, caught once per sentence
 
const Token* prCurrent(const Parser &p) { return &p.tokens[p.pos]; }
bool prCheck(const Parser &p, TokenType t) { return prCurrent(p)->tokenType == t; }
 
void prAdvance(Parser &p) {
    p.pos++;
}
 
void prError(const Parser &p, string msg) {
    const Token* t = prCurrent(p);
    cout << "  Parser error at line " << t->line << ": column " << t->column << ": " << msg << " (got " << tokenTypeName(t->tokenType) << ")\n";
    throw ParseError();
}
 
// check the current token is t, then move to the next one
void prExpect(Parser &p, TokenType t, string what) {
    if (!prCheck(p, t)) {
        prError(p, "expected " + what + " (" + tokenTypeName(t) + ")");
    }
    prAdvance(p);
}
 
//  <A> -> a<A> | a   ==>  a {a} 
void A(Parser &p) {
    prExpect(p, TOK_a, "a");
    while (1) {
        if (prCheck(p, TOK_a)) prAdvance(p);
        else break;
    }
}
 
//  <B> -> b<B> | b   ==>  b {b} 
void B(Parser &p) {
    prExpect(p, TOK_b, "b");
    while (1) {
        if (prCheck(p, TOK_b)) prAdvance(p);
        else break;
    }
}
 
//  <C> -> c 
void C(Parser &p) {
    prExpect(p, TOK_c, "c");
}
 
//  S -> <A><C><B> | <A>  
void parseProgram(Parser &p) {
    int sentence = 0;
    while (p.pos < p.count) {
        if (prCheck(p, TOK_EOF)) { prAdvance(p); continue; }   
 
        sentence++;
        int start = p.pos;
        string text = "";
        for (int i = start; p.tokens[i].tokenType != TOK_EOF; i++) text += p.tokens[i].lexeme;
        cout << "Sentence " << sentence << " (line " << p.tokens[start].line << "): " << text << "\n";
 
        try {
            A(p);
            if (prCheck(p, TOK_c)) {
                C(p);
                B(p);
            }
            prExpect(p, TOK_EOF, "end of sentence");
            cout << "  No errors found\n";
        } catch (ParseError &) {
            
            while (p.pos < p.count && !prCheck(p, TOK_EOF)) prAdvance(p);
            if (p.pos < p.count) prAdvance(p);
        }
    }
}
 
// =====================================================================
// MAIN
// =====================================================================
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source-file>\n", argv[0]);
        return 1;
    }
 
    FILE* fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file: %s\n", argv[1]);
        return 1;
    }
    // ---- Tokenize ----
    Tokenizer tk;
    tkInit(&tk, fp);
 
    Token tokens[MAX_TOKENS];
    int tokenCount = tokenize(&tk, tokens, MAX_TOKENS);
 
    fclose(fp);
 
    Parser parser;
    parser.tokens = tokens;
    parser.count  = tokenCount;
    parser.pos    = 0;
 
    parseProgram(parser);
 
    return 0;
}
