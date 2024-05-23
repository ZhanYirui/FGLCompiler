#include <cmath>
#include <cctype>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstdarg>
#include <graphics.h>
#include <conio.h>
#define pi acos(-1.0)
#define e exp(1.0)
using namespace std;

enum Token_Type
{
    ORIGIN,SCALE,ROT,IS,TO,STEP,DRAW,FOR,FROM,COLOR,NOTES,//保留字
    T, //参数
    SEMICO,L_BRACKET,R_BRACKET,COMMA,COLON,QUOTES, //; ( ) ,分隔符
    PLUS,MINUS,MUL,DIV,POWER,  //+ - * / **运算符
    FUNC, //函数
    CONST_ID, //常数
    NONTOKEN, //空记号
    ERRTOKEN //出错记号
};

typedef struct Tokens
{
    Token_Type type;
    string lexeme;
    double value;
    double (*FuncPtr)(double);
    int TokenLine;
} Tokens;


static Tokens TokenTab[] = {
        {ORIGIN, "ORIGIN", 0.0, NULL,0},
        {COLOR, "COLOR", 0.0, NULL,0},
        {NOTES, "NOTES", 0.0, NULL,0},
        {SCALE, "SCALE", 0.0, NULL,0},
        {ROT, "ROT", 0.0, NULL,0},
        {IS, "IS", 0.0, NULL,0},
        {TO, "TO", 0.0, NULL,0},
        {STEP, "STEP", 0.0, NULL,0},
        {DRAW, "DRAW", 0.0, NULL,0},
        {FOR, "FOR", 0.0, NULL,0},
        {FROM, "FROM", 0.0, NULL,0},
        {T, "T", 0.0, NULL,0},
        {FUNC, "SIN", 0.0, sin,0},
        {FUNC, "COS", 0.0, cos,0},
        {FUNC, "TAN", 0.0, tan,0},
        {FUNC, "LN", 0.0, log,0},
        {FUNC, "EXP", 0.0, exp,0},
        {FUNC, "SQRT", 0.0, sqrt,0},
        {CONST_ID, "PI", pi, NULL,0},
        {CONST_ID, "E", e, NULL,0}
};

class Scanner
{
public:
    void OpenFile() //打开文件
    {
        cin >> FileName;
        F.open(FileName,ios::in|ios::out);
    }
    void CloseFile() //关闭文件
    {
        F.close();
    }
    void EmptyBuffer() //清空缓冲区
    {
        TokenBuffer = "";
    }
    void AddCharToBuffer(char TempC) //将字符添加到缓冲区
    {
        TokenBuffer += TempC;
    }
    Tokens SearchCharInDict(string TempS) //查字典
    {
        Tokens T = {ERRTOKEN, TempS, 0.0, NULL,0};
        transform(TempS.begin(),TempS.end(),TempS.begin(),::toupper);
        for(int i = 0; i < 20; i ++) {
            if (TempS == TokenTab[i].lexeme)
            {
                T.type = TokenTab[i].type;
                T.value = TokenTab[i].value;
                T.FuncPtr = TokenTab[i].FuncPtr;
            }
        }
        return T;
    }
    Tokens CreateToken(Token_Type type, string lexeme, double value, double (*FuncPtr)(double), int Line){
        Tokens TempToken;
        TempToken.type = type;
        TempToken.lexeme = lexeme;
        TempToken.value = value;
        TempToken.FuncPtr = FuncPtr;
        TempToken.TokenLine = Line;
        return TempToken;
    }
    Tokens GetToken()
    {
        char c;
        static int line = 1; //行数
        Tokens TempT;
        EmptyBuffer();   //清空缓冲区
        while(1)
        {
            c = F.get();
            if(c == EOF)
            {
                TempT = CreateToken(NONTOKEN,"EOF",0.0,NULL,line);
                return TempT;
            }
            else if(c == ' ' || c == '\t') continue;
            else if(c == '\n'){ line++; continue;}
            else if(isalpha(c))
            {
                AddCharToBuffer(c);
                while(1)
                {
                    c = F.get();
                    if(isalnum(c)) AddCharToBuffer(c);
                    else break;
                }
                F.putback(c);
                TempT = SearchCharInDict(TokenBuffer);
                TempT.TokenLine = line;
                return TempT;
            }
            else if(isdigit(c))
            {
                AddCharToBuffer(c);
                while(1)
                {
                    c = F.get();
                    if(isdigit(c)) AddCharToBuffer(c);
                    else break;
                }
                if(c == '.') {
                    AddCharToBuffer(c);
                    while (1)
                    {
                        c = F.get();
                        if(isdigit(c)) AddCharToBuffer(c);
                        else break;
                    }
                }
                F.putback(c);
                TempT = CreateToken(CONST_ID,TokenBuffer, stod(TokenBuffer),NULL,line);
                return TempT;
            }
            else if(c == ';')
            {
                TempT = CreateToken(SEMICO,";",0.0,NULL,line);
                return TempT;
            }
            else if(c == '\"'){
                TempT = CreateToken(QUOTES,"\"",0.0,NULL,line);
                return TempT;
            }
            else if(c == ':'){
                TempT = CreateToken(COLON,":",0.0,NULL,line);
                return TempT;
            }
            else if(c == '(')
            {
                TempT = CreateToken(L_BRACKET,"(",0.0,NULL,line);
                return TempT;
            }
            else if(c == ')')
            {
                TempT = CreateToken(R_BRACKET,")",0.0,NULL,line);
                return TempT;
            }
            else if(c == ',')
            {
                TempT = CreateToken(COMMA,",",0.0,NULL,line);
                return TempT;
            }
            else if(c == '+')
            {
                TempT = CreateToken(PLUS,"+",0.0,NULL,line);
                return TempT;
            }
            else if(c == '-') {
                c = F.get();
                if(c == '-')
                {
                    while(c!='\n' && c!=EOF) c = F.get();
                    if(c == EOF)
                    {
                        TempT = CreateToken(NONTOKEN,"EOF",0.0,NULL,line);
                        return TempT;
                    }
                    else {
                        line++;
                        continue;
                    }
                }
                else {
                    F.putback(c);
                    TempT = CreateToken(MINUS,"-",0.0,NULL,line);
                    return TempT;
                }
            }
            else if(c == '/'){
                c = F.get();
                if(c == '/')
                {
                    while(c!='\n'&&c!=EOF) c = F.get();
                    if(c == EOF)
                    {
                        TempT = CreateToken(NONTOKEN,"EOF",0.0,NULL,line);
                        return TempT;
                    }
                    else {
                        line++;
                        continue;
                    }
                }
                else {
                    F.putback(c);
                    TempT = CreateToken(DIV,"/",0.0,NULL,line);
                    return TempT;
                }
            }
            else if(c == '*')
            {
                c = F.get();
                if(c == '*')
                {
                    TempT = CreateToken(POWER,"**",0.0,NULL,line);
                    return TempT;
                }
                else
                {
                    F.putback(c);
                    TempT = CreateToken(MUL,"*",0.0,NULL,line);
                    return TempT;
                }
            }
            else
            {
                TempT = CreateToken(ERRTOKEN,"EOF",0.0,NULL,line);
                return TempT;
            }
        }
    }

private:
    string FileName, TokenBuffer = ""; //文件名和缓冲区
    fstream F;
};

vector<Tokens> TokenStream;
void LoadFileTokens()
{
    Scanner s;
    Tokens t;
    cout << "Please Enter A File With Path:" << endl;
    s.OpenFile();
    t = s.GetToken();
    TokenStream.push_back(t);
    while (t.type != NONTOKEN)
    {
        t = s.GetToken();
        TokenStream.push_back(t);
    }
    s.CloseFile();
}

typedef double (*FuncPtr)(double);
struct ExprNode{
    Token_Type OpCode;
    union{
        struct{
            ExprNode *Left, *Right;
        } CaseOperator;
        struct{
            ExprNode *Child;
            FuncPtr MathFuncPtr;
        }CaseFunc;
        double CaseConst;
        double *CaseParmPtr;
    }Content;
};

vector<double> R;
vector<double> G;
vector<double> B;
vector<double> Origin_X;
vector<double> Origin_Y;
vector<double> Scale_X;
vector<double> Scale_Y;
vector<double> Rot_ang;
vector<double> Start;
vector<double> End;
vector<double> Step;
vector<struct ExprNode*> For_X;
vector<struct ExprNode*> For_Y;
vector<double> Notes_X;
vector<double> Notes_Y;
vector<string> Notes_String;
vector<double> Notes_R;
vector<double> Notes_G;
vector<double> Notes_B;


double Parameter = 0;
string TempNotes = "";

class Parsers{
public:
    struct ExprNode  *origin_xptr, *origin_yptr, *scale_xptr, *scale_yptr, *rot_ptr, *start_ptr, *end_ptr, *step_ptr, *for_xptr, *for_yptr, *color_r, *color_g, *color_b, *notes_x, *notes_y;
    static int cnt;
    double origin_x = 0, origin_y = 0, scale_x = 1, scale_y = 1, rot_ang = 0, r = 255, g = 255, b = 255, xnotes, ynotes;
    Tokens TempToken;
    Parsers(){LoadFileTokens();}
    struct ExprNode* MakeExprNode(Token_Type opcode,...){
        struct ExprNode *ExprPtr = new (struct ExprNode);
        ExprPtr -> OpCode = opcode;
        va_list ArgPtr;
        va_start(ArgPtr,opcode);
        switch (opcode) {
            case CONST_ID:ExprPtr->Content.CaseConst =(double) va_arg(ArgPtr,double );break;
            case T:ExprPtr->Content.CaseParmPtr = &Parameter;break;
            case FUNC:
                ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr) va_arg(ArgPtr,FuncPtr);
                ExprPtr->Content.CaseFunc.Child = (struct ExprNode*) va_arg(ArgPtr,struct ExprNode*);break;
            default:
                ExprPtr->Content.CaseOperator.Left = (struct ExprNode*) va_arg(ArgPtr,struct ExprNode*);
                ExprPtr->Content.CaseOperator.Right = (struct ExprNode*) va_arg(ArgPtr,struct ExprNode*);break;
        }
        va_end(ArgPtr);
        return ExprPtr;
    }
    void FetchToken()
    {
        TempToken = TokenStream[cnt];
        cnt ++;
        if(TempToken.type == ERRTOKEN) SyntaxError(1);
    }
    void MatchToken(Token_Type t)
    {
        if(TempToken.type != t) SyntaxError(2);
        FetchToken();
    }
    void SyntaxError(int x){
        if(x == 1){
            cout << "line No:" << TempToken.TokenLine << " Has An Error Token: " << TempToken.lexeme << endl;
            exit(0);
        }
        else if(x == 2) {
            cout << "line No:" << TempToken.TokenLine << " Has An Unexpected Token: " << TempToken.lexeme  << endl;
            exit(0);
        }
    }

    double GetExpValue(struct ExprNode* Tree) {
        if (Tree == NULL) return 0.0;
        switch (Tree->OpCode) {
            case PLUS:
                return GetExpValue(Tree->Content.CaseOperator.Left) + GetExpValue(Tree->Content.CaseOperator.Right);
            case MINUS:
                return GetExpValue(Tree->Content.CaseOperator.Left) - GetExpValue(Tree->Content.CaseOperator.Right);
            case MUL:
                return GetExpValue(Tree->Content.CaseOperator.Left) * GetExpValue(Tree->Content.CaseOperator.Right);
            case DIV:
                return GetExpValue(Tree->Content.CaseOperator.Left) / GetExpValue(Tree->Content.CaseOperator.Right);
            case POWER:
                return pow(GetExpValue(Tree->Content.CaseOperator.Left), GetExpValue(Tree->Content.CaseOperator.Right));
            case FUNC:
                return Tree->Content.CaseFunc.MathFuncPtr(GetExpValue(Tree->Content.CaseFunc.Child));
            case CONST_ID:
                return Tree->Content.CaseConst;
            case T:
                return *(Tree->Content.CaseParmPtr);
            default:
                return 0.0;
        }
    }
    void Parser(){
        FetchToken();
        Program();
    }
    void Program(){
        while (TempToken.type != NONTOKEN){
            Statement();
            MatchToken(SEMICO);
        }
    }
    void Statement(){
        if(TempToken.type == ORIGIN) OriginStatement();
        else if(TempToken.type == SCALE) ScaleStatement();
        else if(TempToken.type == ROT) RotStatement();
        else if(TempToken.type == FOR) ForStatement();
        else if(TempToken.type == COLOR) ColorStatement();
        else if(TempToken.type == NOTES) NotesStatement();
        else SyntaxError(2);
    }
    void NotesStatement(){
        TempNotes = "";
        MatchToken(NOTES);
        MatchToken(COLON);
        MatchToken(L_BRACKET);
        notes_x = Expression();
        xnotes = GetExpValue(notes_x);
        Notes_X.push_back(xnotes);
        MatchToken(COMMA);
        notes_y = Expression();
        ynotes = GetExpValue(notes_y);
        Notes_Y.push_back(ynotes);
        MatchToken(COMMA);
        if(TempToken.type != QUOTES) SyntaxError(2);
        else {
            TempToken = TokenStream[cnt];
            cnt++;
        }
        while(TempToken.type != QUOTES){
            TempNotes = TempNotes + TempToken.lexeme;
            TempNotes = TempNotes + " ";
            TempToken = TokenStream[cnt];
            cnt ++;
        }
        Notes_String.push_back(TempNotes);
        Notes_R.push_back(r);
        Notes_G.push_back(g);
        Notes_B.push_back(b);
        MatchToken(QUOTES);
        MatchToken(R_BRACKET);
    }
    void ColorStatement(){
        MatchToken(COLOR);
        MatchToken(IS);
        MatchToken(L_BRACKET);
        color_r = Expression();
        r = GetExpValue(color_r);
        MatchToken(COMMA);
        color_g = Expression();
        g = GetExpValue(color_g);
        MatchToken(COMMA);
        color_b = Expression();
        b = GetExpValue(color_b);
        if(r<0||r>255||g<0||g>255||b<0||b>255)
        {
            cout << "RGB Color Value Out Of Range(0~255)" << endl;
            exit(0);
        }
        MatchToken(R_BRACKET);
    }
    void OriginStatement(){
        MatchToken(ORIGIN);
        MatchToken(IS);
        MatchToken(L_BRACKET);
        origin_xptr = Expression();
        origin_x = GetExpValue(origin_xptr);
        MatchToken(COMMA);
        origin_yptr = Expression();
        origin_y = GetExpValue(origin_yptr);
        MatchToken(R_BRACKET);
    }
    void RotStatement(){
        MatchToken(ROT);
        MatchToken(IS);
        rot_ptr = Expression();
        rot_ang = GetExpValue(rot_ptr);
    }
    void ScaleStatement(){
        MatchToken(SCALE);
        MatchToken(IS);
        MatchToken(L_BRACKET);
        scale_xptr = Expression();
        scale_x = GetExpValue(scale_xptr);
        MatchToken(COMMA);
        scale_yptr = Expression();
        scale_y = GetExpValue(scale_yptr);
        MatchToken(R_BRACKET);
    }
    void ForStatement(){
        MatchToken(FOR);
        MatchToken(T);
        MatchToken(FROM);
        start_ptr = Expression();
        Start.push_back(GetExpValue(start_ptr));
        MatchToken(TO);
        end_ptr = Expression();
        End.push_back(GetExpValue(end_ptr));
        MatchToken(STEP);
        step_ptr = Expression();
        Step.push_back(GetExpValue(step_ptr));
        MatchToken(DRAW);
        MatchToken(L_BRACKET);
        for_xptr = Expression();
        For_X.push_back(for_xptr);
        MatchToken(COMMA);
        for_yptr = Expression();
        For_Y.push_back(for_yptr);
        Origin_X.push_back(origin_x);
        Origin_Y.push_back(origin_y);
        Scale_X.push_back(scale_x);
        Scale_Y.push_back(scale_y);
        Rot_ang.push_back(rot_ang);
        R.push_back(r);
        G.push_back(g);
        B.push_back(b);
        MatchToken(R_BRACKET);
    }

    struct ExprNode* Expression(){
        struct ExprNode *left, *right;
        Token_Type token_tmp;
        left = Term();
        while (TempToken.type == PLUS || TempToken.type == MINUS)
        {
            token_tmp = TempToken.type;
            MatchToken(token_tmp);
            right = Term();
            left = MakeExprNode(token_tmp,left,right);
        }
        return left;
    }

    struct ExprNode* Term(){
        struct ExprNode *left, *right;
        Token_Type token_tmp;
        left = Factor();
        while (TempToken.type == MUL || TempToken.type == DIV)
        {
            token_tmp = TempToken.type;
            MatchToken(token_tmp);
            right = Factor();
            left = MakeExprNode(token_tmp,left,right);
        }
        return left;
    }

    struct ExprNode* Factor(){
        struct ExprNode *left, *right;
        if(TempToken.type == PLUS){
            MatchToken(PLUS);
            right = Factor();
            left = NULL;
            right = MakeExprNode(PLUS,left,right);
        }
        else if(TempToken.type == MINUS){
            MatchToken(MINUS);
            right = Factor();
            left = MakeExprNode(CONST_ID,0.0);
            right = MakeExprNode(MINUS,left,right);
        }
        else right = Component();
        return right;
    }

    struct ExprNode* Component(){
        struct ExprNode *left, *right;
        left = Atom();
        if(TempToken.type == POWER){
            MatchToken(POWER);
            right = Component();
            left = MakeExprNode(POWER,left,right);
        }
        return left;
    }

    struct ExprNode* Atom(){
        struct ExprNode *address, *tmp;
        double const_value;
        FuncPtr funcPtr_value;
        if(TempToken.type == CONST_ID){
            const_value = TempToken.value;
            MatchToken(CONST_ID);
            address = MakeExprNode(CONST_ID,const_value);
        }
        else if(TempToken.type == T){
            MatchToken(T);
            address = MakeExprNode(T);
        }
        else if(TempToken.type == FUNC){
            funcPtr_value = TempToken.FuncPtr;
            MatchToken(FUNC);
            MatchToken(L_BRACKET);
            tmp = Expression();
            address = MakeExprNode(FUNC,funcPtr_value,tmp);
            MatchToken(R_BRACKET);
        }
        else if(TempToken.type == L_BRACKET){
            MatchToken(L_BRACKET);
            address = Expression();
            MatchToken(R_BRACKET);
        }
        else SyntaxError(2);
        return address;
    }
};
int Parsers::cnt = 0;

void DrawXY(double x, double y, double Origin_X,double Origin_Y,double Scale_X,double Scale_Y,double Rot_ang, double R, double G, double B)
{
    double tempx, tempy;
    x *= Scale_X;
    y *= Scale_Y;
    tempx = x;
    tempy = y;
    x = tempx * cos(Rot_ang) + tempy * sin(Rot_ang);
    y = tempy * cos(Rot_ang) - tempx * sin(Rot_ang);
    x +=  Origin_X;
    y +=  Origin_Y;
    putpixel(x,y,RGB(R,G,B));
}


void DrawTotalXY(Parsers P, double Origin_X,double Origin_Y,double Scale_X,double Scale_Y,double Rot_ang, double R, double G, double B, double Start, double End, double Step, struct ExprNode* For_X, struct ExprNode* For_Y) {
    double x,y;
    Parameter = Start;
    if (Step > 0) {
        while (Parameter <= End) {
            x = P.GetExpValue(For_X) ;
            y = P.GetExpValue(For_Y) ;
            DrawXY(x,y,Origin_X,Origin_Y,Scale_X,Scale_Y,Rot_ang,R,G,B);
            Parameter += Step;
        }
    }
    else if (Step < 0) {
        while (Parameter >= End) {
            x = P.GetExpValue(For_X) ;
            y = P.GetExpValue(For_Y) ;
            DrawXY(x,y,Origin_X,Origin_Y,Scale_X,Scale_Y,Rot_ang,R,G,B);
            Parameter += Step;
        }
    }

}

void OutTextXY(double Notes_X, double Notes_Y, string Notes_String, double Notes_R, double Notes_G, double Notes_B)
{
    settextcolor(RGB(round(Notes_R), round(Notes_G),round(Notes_B)));
    outtextxy(Notes_X,Notes_Y,Notes_String.c_str());
}

int main() {
    Parsers P;
    P.Parser();
    initgraph(1000, 700);
    for(int i = 0; i < Start.size(); i ++)
        DrawTotalXY(P,Origin_X[i],Origin_Y[i],Scale_X[i],Scale_Y[i],Rot_ang[i],R[i],G[i],B[i],Start[i],End[i],Step[i],For_X[i],For_Y[i]);
    for(int i = 0; i < Notes_String.size(); i ++) {
        OutTextXY(Notes_X[i], Notes_Y[i], Notes_String[i], Notes_R[i], Notes_G[i], Notes_B[i]);
    }
    _getch();
    closegraph();
    return 0;
}