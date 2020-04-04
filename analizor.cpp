#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <ctype.h>

using namespace std;

const int MATRIX_SIZE = 10;

typedef struct token
{
    string type;
    string value;
} TOKEN;

typedef struct globalState
{
    int transition[MATRIX_SIZE][MATRIX_SIZE];
} GLOBALSTATE;

void init(GLOBALSTATE &gState)
{
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        for(int j=0;j<MATRIX_SIZE;j++)
        {
            gState.transition[i][j]=-1;
        }
    }
}


int main(){

    ifstream inFile;
    char currentChar;

    GLOBALSTATE gState;
    init(gState);
	
    char doNext = 'c';
    
    inFile.open("small_test.txt");
    while((currentChar = inFile.get()) != EOF){
        if(currentChar == '\n')
        {
            cout << "new line here";
        }
        cout << currentChar;
    }

    return 0;
}

void run(){

}


int char_type(char character){
    // a-z A-Z 
    if(isalpha(character)){
        return 0;
    }

    // 0 - 9 
    if(isdigit(character)){
        return 1;
    }

    switch ( character ){ 
        case '/': return 2;
        case '*': return 3;
        case ' ': return 4;
        case '+':
        case '-':
        case '^':
        case '%':
        case '=':
        case '(':
        case ')': 
        case '<': 
        case '>':
        case '{':
        case '}':
        case '[': 
        case ']': return 5;
        case ';': return 6;
        case '\'': return 7;
        case '"': return 8;
        case '\\': return 9;
        default : return -1;
    }
}