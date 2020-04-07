#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
using namespace std;

const int MATRIX_SIZE = 50;

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
const int CT_EQUAL             = 15; /* */
const int CT_SLASH             = 16;
const int CT_STAR              = 17;
const int CT_EOL               = 18;
const int CT_R_SLASH           = 19;
const int CT_N                 = 20;
const int CT_QUOTE_X2          = 21;
const int CT_QUOTE             = 22;


const int CT_NOT_SLASH         = 616;
const int CT_NOT_STAR          = 617;
const int CT_NOT_EOL           = 618;
const int CT_NOT_R_SLASH       = 619;
const int CT_NOT_N             = 620;
const int CT_NOT_QUOTE_X2      = 621;
const int CT_ANYTHING          = 622;


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
const int ST_16                = 16; // double operator | FINAL STATE
const int ST_17                = 17; // / -> final operator or intermediate for comments | FINAL STATE or not
const int ST_18                = 18; // start of multiline 
const int ST_19                = 19; // possible state of ending multiline comment
const int ST_20                = 20; // final state multiline comment | FINAL STATE
const int ST_21                = 21; // same line comment style
const int ST_22                = 22; // same line end | FINAL STATE

const int ST_23                = 23; // string states
const int ST_24                = 24; // string states


const int ST_25                = 25; // character literal states
const int ST_26                = 26; // character literal states
const int ST_27                = 27; // character literal states
 



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
    gstate.transition[ST_0][CT_STAR]                            = ST_9;
    gstate.transition[ST_0][CT_AND]                             = ST_11;
    gstate.transition[ST_0][CT_OR]                              = ST_12;
    gstate.transition[ST_0][CT_PLUS]                            = ST_13;
    gstate.transition[ST_0][CT_MINUS]                           = ST_14;
    gstate.transition[ST_0][CT_EQUAL]                           = ST_15;
    gstate.transition[ST_0][CT_SLASH]                           = ST_17;
    gstate.transition[ST_0][CT_QUOTE_X2]                        = ST_23;
    gstate.transition[ST_0][CT_QUOTE]                           = ST_25;

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
    gstate.transition[ST_14][CT_EQUAL]                          = ST_16;/* */ // \n
    gstate.transition[ST_15][CT_EQUAL]                          = ST_16;

    gstate.transition[ST_17][CT_STAR]                           = ST_18;
    gstate.transition[ST_17][CT_SLASH]                          = ST_21;

    gstate.transition[ST_18][CT_STAR]                           = ST_19;
    gstate.transition[ST_18][CT_NOT_STAR]                       = ST_18;


    gstate.transition[ST_19][CT_SLASH]                          = ST_20; 
    gstate.transition[ST_19][CT_STAR]                           = ST_19; 
    gstate.transition[ST_19][CT_NOT_SLASH]                      = ST_18;


    gstate.transition[ST_21][CT_EOL]                            = ST_22;
    gstate.transition[ST_21][CT_NOT_EOL]                        = ST_21;
    gstate.transition[ST_21][CT_SPACE_DELIMITER]                = ST_21;

    gstate.transition[ST_23][CT_NOT_QUOTE_X2]                   = ST_23;
    gstate.transition[ST_23][CT_QUOTE_X2]                       = ST_24;

    gstate.transition[ST_25][CT_ANYTHING]                       = ST_26;
    gstate.transition[ST_26][CT_QUOTE]                          = ST_27;

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
    
    if(gstate.currentState == ST_25){
        return CT_ANYTHING;
    }

    if(gstate.currentState == ST_23){
        if(character != '"'){
            return CT_NOT_QUOTE_X2;
        }
        return CT_QUOTE_X2;
    }


    if(gstate.currentState == ST_18){
        if(character == '*'){
            return CT_STAR;
        }
        return CT_NOT_STAR;
    }

    if(gstate.currentState == ST_19){
        if(character == '/'){
            return CT_SLASH;
        }
        if(character == '*'){
            return CT_STAR;
        }

        return CT_NOT_SLASH;
    }


     if(character == '\n'){
        if(gstate.currentState == ST_21){
            return CT_EOL;
        }
        return CT_SPACE_DELIMITER;
    }

    if(gstate.currentState == ST_21){
        return CT_NOT_EOL;
    }

   
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


    if(character == '/'){
        return CT_SLASH;
    }


    if(character == 'n'){
        return CT_N;
    }

    switch ( character ){ 
        case '.': return CT_FLOAT_POINT;
        case ' ': return 8;
        case '\t': return 8;
        case '*': return CT_STAR;
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
        case ',': return CT_DELIM;
        case '"': return CT_QUOTE_X2;
        case '\'': return CT_QUOTE;
        case EOF : return EOF;
        default : return 49;
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
        case ST_17: return "operator"; // / 
        case ST_20: return "comment"; // / 
        case ST_22: return "comment"; // / 
        case ST_24: return "string"; // / 
        case ST_27: return "character literal"; // / 
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
            if(currentChar == EOF){
                if(get_state_type(gstate.currentState) == "err"){
                    doNext = ERROR;
                } else {
                    doNext = SUCCESS;
                }
                run();
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
                if(file[fileCursor] == EOF){
                    gstate.currentState = 0;
                    doNext = SUCCESS;

                } else {
                    echoInitialState();
                    gstate.currentState = 0;
                    doNext = CONTINUE;
                }
                
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

#define MAXCHAR 1000


int main(){

    ifstream MyReadFile("small_test.txt");
    string line;
   
    while(getline (MyReadFile,line)){
        // cout << line;
        file += line;
        file += "\n";
    }
    file += EOF;
    MyReadFile.close();


    init(gstate);
    set_transition_states(gstate);
    set_space_transition_states(gstate);
    set_delim_transition_states(gstate);
    cout << INITIAL_STATE;

    run();
    return 0;
}