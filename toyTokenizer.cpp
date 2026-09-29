#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
using namespace std;
 
#define MAX_TOKENS   8192
 
// Tokens names: dictionary of all legal symbols in our toy language
enum TokenType { TOK_a, TOK_b, TOK_c, TOK_EOF, TOK_UNKNOWN };
 
// declare a token structure
struct Token {
	TokenType tokenType;
	string lexeme;
	int line;
	int column;
};
 
/******************************************************/
/*   Helping function to display the token as a string */
const char* tokenTypeName(TokenType t) {
	switch (t) {
		case TOK_a: return "a"; break;
		case TOK_b: return "b"; break;
		case TOK_c: return "c"; break;
		case TOK_EOF: return "ENDFILE"; break;
		default: break;
	}
	return "UNKNOWN";
}
 
// Tokenizer structure 
struct Tokenizer {
	FILE *infp;
	int line;
	int column;
	int cur;
	int hasCur;
};


// --- Basic char reading with position tracking ---
 
void tkInit(Tokenizer *tk, FILE * infp) {
	tk->infp = infp;
	tk->line = 1;
	tk->column = 1;
	tk->cur = 0;
	tk->hasCur = 0;
}
 
/********************************************/
/* errMsg - function to display the error message with the line number of the error detected. */
void errMsg (Tokenizer *tk, string msg) {
	cout << "Tokenizer error at line " << tk->line << " column " << tk->column << ": "<< msg << endl;
	exit(1);
}
 
int getChar(Tokenizer *tk) {
	int c = fgetc(tk->infp);
	return c;
}
 
int peekChar(Tokenizer *tk) {
	if (!tk->hasCur) {
		tk->cur = getChar(tk);
		tk->hasCur = 1; // peeked
	}
	return tk->cur;
}
 
void consumeChar(Tokenizer *tk) {
	if (!tk->hasCur) { // char was peeked but not consumed
		tk->cur = getChar(tk);
		tk->hasCur = 1; // peeked
	}
	if (tk->cur == '\n') {
		tk->line++;
		tk->column = 1;
	} else if (tk->cur != EOF) {
		tk->column++;
	}
	tk->hasCur = 0; // so next peekChar will read fresh new char
}
 
/*******************************************************************
LookupKeyword - every symbol is ONE character: a, b or c.
A newline ends a sentence, so it is returned as TOK_EOF. */
TokenType lookupKeywords (int c) {
	TokenType token = TOK_UNKNOWN;
	switch (c) {
		case 'a': token = TOK_a; break;
		case 'b': token = TOK_b; break;
		case 'c': token = TOK_c; break;
		case '\n': token = TOK_EOF; break;
	}
	return token;
}
 
/*****************************************************/
/* tokenize - only calls lookupKeywords and saves the token type,
   lexeme, line number and column number */
int tokenize(Tokenizer *tk, Token *tokens, int maxTokens) {
	int count = 0;
	while (1) {
		if (count >= maxTokens) errMsg(tk, "too many tokens");
 
		int c = peekChar(tk);
		int ln = tk->line;
		int col = tk->column;
 
		if (c == '\r') { consumeChar(tk); continue; }   // ignore Windows CR
 
		if (c == EOF) {
			tokens[count].tokenType = TOK_EOF;
			tokens[count].lexeme = "";
			tokens[count].line = ln;
			tokens[count].column = col;
			return count+1;
		}
 
		tokens[count].tokenType = lookupKeywords(c);
		tokens[count].lexeme = (c == '\n') ? "\\n" : string(1, char(c));
		tokens[count].line = ln;
		tokens[count].column = col;
		if (tokens[count].tokenType == TOK_UNKNOWN) errMsg(tk, "unknown token");
		count++;
		consumeChar(tk);
	}
}
