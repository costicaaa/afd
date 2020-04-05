#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
using namespace std;

const int MATRIX_SIZE = 20;

const string INITIAL_STATE = "0";

const char CONTINUE = 'c';
const char BLOCK = 'b';
const char SUCCESS = 's';
const char ERROR = 'e';


const int CT_LETTER            =  0;
const int CT_DIGIT             =  1;
const int CT_FLOAT_POINT       =  2;
const int CT_FLOAT_E           =  3;
const int CT_FLOAT_MINUS       =  4;
const int CT_SPACE_DELIMITER   =  8;
const int CT_OPERATOR          =  9;
const int CT_DELIM             = 10;
const int CT_AND               = 11;
const int CT_OR                = 12;
const int CT_PLUS              = 13;
const int CT_MINUS             = 14;
const int CT_EQUAL             = 15;


const int ST_0                 = 0;
const int ST_1                 = 1;
const int ST_2                 = 2;
const int ST_3                 = 3;
const int ST_4                 = 4;
const int ST_5                 = 5;
const int ST_6                 = 6;
const int ST_7                 = 7;
const int ST_8                 = 8;
const int ST_9                 = 9;
const int ST_10                = 10;
const int ST_11                = 11; // &
const int ST_12                = 12; // |
const int ST_13                = 13; // +
const int ST_14                = 14; // -
const int ST_15                = 15; // =
const int ST_16                = 16; // double operator final state


 



typedef struct token
{
    string type;
    string value;
} TOKEN;

typedef struct GSTATE
{
    int transition[MATRIX_SIZE][MATRIX_SIZE];
    int currentState    = 0;
} GSTATE;

string file;
GSTATE gstate;
char doNext = CONTINUE;
int count = 0;
TOKEN tokens[1000];
string currentToken;
int fileCursor = 0;
bool singleState = false;


void init(GSTATE &gstate)
{
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        for(int j=0;j<MATRIX_SIZE;j++)
        {
            gstate.transition[i][j] = -1;
        }
    }
}

void set_transition_states(GSTATE &gstate)
{

    
    gstate.transition[ST_0][CT_LETTER]                          = ST_1;
    gstate.transition[ST_0][CT_DIGIT]                           = ST_2;
    gstate.transition[ST_0][CT_OPERATOR]                        = ST_9;
    gstate.transition[ST_0][CT_AND]                             = ST_11;
    gstate.transition[ST_0][CT_OR]                              = ST_12;
    gstate.transition[ST_0][CT_PLUS]                            = ST_13;
    gstate.transition[ST_0][CT_MINUS]                           = ST_14;
    gstate.transition[ST_0][CT_EQUAL]                           = ST_15;

    gstate.transition[ST_1][CT_LETTER]                          = ST_1;
    gstate.transition[ST_1][CT_DIGIT]                           = ST_1;
    gstate.transition[ST_2][CT_DIGIT]                           = ST_2;
    gstate.transition[ST_2][CT_FLOAT_POINT]                     = ST_3;
    gstate.transition[ST_2][CT_FLOAT_E]                         = ST_5;
    gstate.transition[ST_3][CT_DIGIT]                           = ST_4;
    gstate.transition[ST_3][CT_FLOAT_E]                         = ST_5;
    gstate.transition[ST_4][CT_DIGIT]                           = ST_4;
    gstate.transition[ST_4][CT_FLOAT_E]                         = ST_5;
    gstate.transition[ST_5][CT_FLOAT_MINUS]                     = ST_6;
    gstate.transition[ST_5][CT_DIGIT]                           = ST_7;
    gstate.transition[ST_6][CT_DIGIT]                           = ST_7;
    gstate.transition[ST_7][CT_DIGIT]                           = ST_7;


    gstate.transition[ST_11][CT_AND]                            = ST_16;
    gstate.transition[ST_12][CT_OR]                             = ST_16;
    gstate.transition[ST_13][CT_PLUS]                           = ST_16;
    gstate.transition[ST_13][CT_MINUS]                          = ST_16;
    gstate.transition[ST_13][CT_EQUAL]                          = ST_16;
    gstate.transition[ST_14][CT_MINUS]                          = ST_16;
    gstate.transition[ST_14][CT_PLUS]                           = ST_16;
    gstate.transition[ST_14][CT_EQUAL]                          = ST_16;
    gstate.transition[ST_15][CT_EQUAL]                          = ST_16;




}

void set_space_transition_states(GSTATE &gstate){
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        gstate.transition[i][CT_SPACE_DELIMITER] = ST_8;
    }
}

void set_delim_transition_states(GSTATE &gstate){
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        gstate.transition[i][CT_DELIM] = ST_10;
    }
    gstate.transition[CT_DELIM][CT_DELIM] = -1;
}

bool is_float_state(){
    return gstate.currentState >= 2 && gstate.currentState <=7;
}

int char_type(char character){

    //float rules
    if(character == '-'){
        if(is_float_state())
        {
            return CT_FLOAT_MINUS;
        }
    }

    if(isalpha(character)){
        if(is_float_state())
        {
            return CT_FLOAT_E;
        }
        return CT_LETTER;
    }

    if(character == '_'){
        return CT_LETTER;
    }

    // 0 - 9 
    if(isdigit(character)){
        return CT_DIGIT;
    }

    if(character == '&'){
        return CT_AND;
    }

    if(character == '|'){
        return CT_OR;
    }

    if(character == '+'){
        return CT_PLUS;
    }

    if(character == '-'){
        return CT_MINUS;
    }
    
    if(character == '='){
        return CT_EQUAL;
    }

    switch ( character ){ 
        case '.': return CT_FLOAT_POINT;
        case '/': return 2;
        case ' ': return 8;
        case '\t': return 8;
        case '*': return CT_OPERATOR;
        case '^': return CT_OPERATOR;
        case '%': return CT_OPERATOR;
        case '(': return CT_OPERATOR;
        case ')':  return CT_OPERATOR;
        case '<':  return CT_OPERATOR;
        case '>': return CT_OPERATOR;
        case '{': return CT_OPERATOR;
        case '}': return CT_OPERATOR;
        case '[':  return CT_OPERATOR;
        case ']': return CT_OPERATOR;
        case ';': return CT_DELIM;
        case '\'': return 7;
        case '"': return 99;
        case '\\': return 9;
        case EOF : return EOF;
        default : return -1;
    }
}

string get_state_type(int state){
    switch(state){
        case ST_1: return "identificator";
        case ST_2: return "integer";
        case ST_4: return "float";
        case ST_7: return "float";
        case ST_8: return "spatiu";
        case ST_9: return "operator";
        case ST_10: return "delimitator";
        case ST_11 : return "operator";
        case ST_12 : return "operator";
        case ST_13 : return "operator";
        case ST_14 : return "operator";
        case ST_15: return "operator";
        case ST_16: return "operator";
        default: return "err";
    }
}

void echoCurrentToken(){
    cout << "           " << currentToken << " ( " << get_state_type(gstate.currentState) << " ) \n";

}


void echoInitialState(){
    echoCurrentToken();
    cout << INITIAL_STATE ;
    currentToken = "";

}

void run(){
        // cout << " do next = " << doNext;
        // cout << " current state = " << gstate.currentState;

        // cout << "\n";
        // cout << "-------------";
        // cout << "\n";
    
    switch(doNext){
        case CONTINUE: {
            char currentChar = file[fileCursor];
            fileCursor++;
            int typeOfCurrentCharacter = char_type(currentChar);
            int nextState = gstate.transition[gstate.currentState][typeOfCurrentCharacter];

            // end of file 
            if(typeOfCurrentCharacter == EOF){
                if(get_state_type(gstate.currentState) != "err"){
                    doNext = SUCCESS;
                    currentToken += currentChar;
                    run();
                    return;
                } else {
                    doNext = ERROR;
                }
                return;
            }

            
            if (nextState == -1){
                fileCursor--;
                doNext = BLOCK;
            } else {
                if(nextState == ST_8){
                    if(gstate.currentState != ST_8 && gstate.currentState != 0){
                        echoInitialState();
                    }
                }

                if(nextState == ST_10 ){
                    if(gstate.currentState != ST_10 && gstate.currentState != 0){
                        echoInitialState();
                    }
                }

                
                cout << " -> " << nextState; 
                currentToken += currentChar;
                // currentToken += (char)nextState;

                gstate.currentState = nextState;
            }
            
            run();
            break;
        }

        case BLOCK: {
            if(get_state_type(gstate.currentState) == "err"){
                doNext = ERROR;
            } else {
                echoInitialState();
                gstate.currentState = 0;
                doNext = CONTINUE;
            }
            run();
            break;
        }

        case ERROR: {
            cout << "\nS-a gasit o EROARE lexicala!\n";
            break;
        }
        case SUCCESS: {
            echoCurrentToken();

            cout << "\nS-a terminat analiza lexicala cu SUCCES!\n";
            break;
        }
        default:{
            cout<<"UNDEFINED INTERNAL ERROR!\n";
        }
    }

}



int main(){

    ifstream MyReadFile("small_test.txt");
    string line;
   
    while(getline (MyReadFile,line)){
        file += line;
    }
    MyReadFile.close();


    init(gstate);
    set_transition_states(gstate);
    set_space_transition_states(gstate);
    set_delim_transition_states(gstate);
    cout << INITIAL_STATE;

    run();
    return 0;
}