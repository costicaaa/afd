#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <ctype.h>

using namespace std;

const int MATRIX_SIZE = 10;

const string INITIAL_STATE = "000";

const char CONTINUE = 'c';
const char BLOCK = 'b';
const char SUCCESS = 's';
const char ERROR = 'e';

const int LETTER        = 0;
const int DIGIT         = 1;
const int FLOAT_POINT   = 2;
const int FLOAT_E       = 3;
const int FLOAT_MINUS   = 4;


typedef struct token
{
    string type;
    string value;
} TOKEN;

typedef struct GSTATE
{
    int transition[MATRIX_SIZE][MATRIX_SIZE];
    int currentState = 0;
} GSTATE;

void init(GSTATE &gstate)
{
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        for(int j=0;j<MATRIX_SIZE;j++)
        {
            gstate.transition[i][j]=-1;
        }
    }
}

void set_transition_states(GSTATE &gstate)
{

    
    gstate.transition[0][LETTER]            = 1;
    gstate.transition[0][DIGIT]             = 2;

    gstate.transition[1][LETTER]            = 1;
    gstate.transition[1][DIGIT]             = 1;

    gstate.transition[2][DIGIT]             = 2;
    gstate.transition[2][FLOAT_POINT]       = 3;
    gstate.transition[2][FLOAT_E]           = 5;


    gstate.transition[3][DIGIT]             = 4;
    gstate.transition[3][FLOAT_E]           = 5;

    gstate.transition[4][DIGIT]             = 4;
    gstate.transition[4][FLOAT_E]           = 5;

    gstate.transition[5][FLOAT_MINUS]       = 6;
    gstate.transition[5][DIGIT]             = 7;

    gstate.transition[6][DIGIT]             = 7;

    gstate.transition[7][DIGIT]             = 7;


}

GSTATE gstate;
char doNext = CONTINUE;
int count = 0;
TOKEN tokens[1000];
string currentToken;
char lastChar;


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

string get_state_type(int state){
    switch(state){
        case 1: return "identificator";
        case 2: return "integer";

        case 4: return "float";
        case 7: return "float";
        // case 3: return "operator";
        // case 9: return "comentariu";
        // case 4: return "spatiu";
        // case 5: return "operator";
        // case 6: return "delimitator";
        default: return "err";
    }
}

void run(char currentChar){
        // cout << " do next = " << doNext;
        // cout << " currentChar = " << currentChar;
        // cout << " current state = " << gstate.currentState;

        // cout << "\n";
        // cout << "-------------";
        // cout << "\n";
    switch(doNext){
        

        case CONTINUE: {
            int typeOfCurrentCharacter = char_type(currentChar);
            int nextState = gstate.transition[gstate.currentState][typeOfCurrentCharacter];
    
            if(nextState == -1){
                lastChar = currentChar;
                doNext = BLOCK;
            } else {
                cout << " -> " << nextState; 
                currentToken += currentChar;
                // currentToken += (char)nextState;

                gstate.currentState = nextState;
            }
            
            break;
        }

        case BLOCK: {
            string stateType = get_state_type(gstate.currentState);
            if(stateType == "err"){
                doNext = ERROR;
            } else {
                cout << " ( " << currentToken << " ) | " << stateType << " \n";
                currentToken = "";
                gstate.currentState = 0;
                doNext = 'c';
                cout << INITIAL_STATE; 
            }

            break;
        }

        case ERROR: {
            cout<<"S-a gasit o EROARE lexicala!\n";
            break;
        }

        case SUCCESS: {
            cout<<"S-a terminat analiza lexicala cu SUCCES!\n";
            break;
        }

        default:{
            cout<<"UNDEFINED INTERNAL ERROR!\n";
        }
    }

}






int main(){

    ifstream inFile;
    char currentChar;

    init(gstate);
    set_transition_states(gstate);
	
    cout << INITIAL_STATE;
    
    inFile.open("small_test.txt");
    while((currentChar = inFile.get()) != EOF){
        if(doNext == ERROR){
            cout << "   !!! do next error in main, stop";
            return 133;
        }

        if(doNext == BLOCK){
            run(lastChar);
            doNext = CONTINUE;
        }

        run(currentChar);
        // if(currentChar == '\n')
        // {
        //     cout << "new line here";
        // }
        // cout << currentChar;
        lastChar = currentChar;
    }

    if(doNext != ERROR){
        cout << "\n";
        cout << "Success final";
        cout << "\n";
    }

    return 0;
}