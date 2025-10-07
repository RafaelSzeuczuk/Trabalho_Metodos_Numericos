#define _USE_MATH_DEFINES
#include <cmath>
#include <map>
#include <stack>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <vector>
#include <cstddef>
#include <tuple>
#include <functional>

using namespace std;

const int TIPO_TOKEN_NUMERO = 0;
const int TIPO_TOKEN_VARIAVEL = 1;
const int TIPO_TOKEN_OPERADOR = 2;
const int TIPO_TOKEN_FUNCAO = 3;
const int TIPO_TOKEN_PARENTESE_ABRE = 4;
const int TIPO_TOKEN_PARENTESE_FECHA = 5;

// tipo, valor, precedencia, associativo_direita
using Token = tuple<int, string, int, bool>;

vector<Token> tokenizar(const string& expressao);
vector<Token> shunting_yard(const vector<Token>& tokens_infix);
double avaliar_rpn(const vector<Token>& tokens_rpn, double valor_x);
string pre_processar_string_funcao_para_analise(const string& str_funcao);
function<double(double)> analisar_string_funcao(const string& str_funcao);

int obter_precedencia(const string& operador);
double aplicar_operador(double operando1, double operando2, const string& operador);
double aplicar_funcao_nomeada(const string& nome_funcao, double valor_arg);


int obter_precedencia(const string& operador) {
    if (operador == "+" || operador == "-") return 1;
    if (operador == "*" || operador == "/") return 2;
    if (operador == "^") return 3; 
    return 0;
}

double aplicar_operador(double operando1, double operando2, const string& operador) {
    if (operador == "+") return operando1 + operando2;
    if (operador == "-") return operando1 - operando2;
    if (operador == "*") return operando1 * operando2;
    if (operador == "/") {
        if (abs(operando2) < 1e-12) {
            throw runtime_error("Erro: Divisão por zero ou valor muito próximo de zero.");
        }
        return operando1 / operando2;
    }
    if (operador == "^") return pow(operando1, operando2);
    throw runtime_error("Operador desconhecido: " + operador);
}

double aplicar_funcao_nomeada(const string& nome_funcao, double valor_arg) {
    if (nome_funcao == "sin" || nome_funcao == "sen") return sin(valor_arg);
    if (nome_funcao == "cos") return cos(valor_arg);
    if (nome_funcao == "tan" || nome_funcao == "tg") return tan(valor_arg);
    if (nome_funcao == "cosec") return 1.0 / sin(valor_arg);
    if (nome_funcao == "sec") return 1.0 / cos(valor_arg);
    if (nome_funcao == "cotg" || nome_funcao == "cot") return 1.0 / tan(valor_arg);
    if (nome_funcao == "sinh") return sinh(valor_arg);
    if (nome_funcao == "cosh") return cosh(valor_arg);
    if (nome_funcao == "tanh") return tanh(valor_arg);
    if (nome_funcao == "exp") return exp(valor_arg);
    if (nome_funcao == "log" || nome_funcao == "ln") return log(valor_arg);
    if (nome_funcao == "log10") return log10(valor_arg);
    if (nome_funcao == "sqrt" || nome_funcao == "raiz") return sqrt(valor_arg);
    if (nome_funcao == "abs") return abs(valor_arg);
    throw runtime_error("Função matemática desconhecida: " + nome_funcao);
}

vector<Token> tokenizar(const string& expressao) {
    vector<Token> tokens;
    for (size_t i = 0; i < expressao.length(); ++i) {
        if (isspace(expressao[i])) continue;
        if (isdigit(expressao[i]) || (expressao[i] == '.' && i + 1 < expressao.length() && isdigit(expressao[i+1]))) {
            string str_num;
            while (i < expressao.length() && (isdigit(expressao[i]) || expressao[i] == '.')) {
                str_num += expressao[i];
                i++;
            }
            --i; 
            tokens.push_back(make_tuple(TIPO_TOKEN_NUMERO, str_num, 0, false));
        } 
        else if (expressao[i] == 'x') {
            tokens.push_back(make_tuple(TIPO_TOKEN_VARIAVEL, "x", 0, false));
        } 
        else if (expressao[i] == 'e' && (i + 1 >= expressao.length() || !isalpha(expressao[i+1]))) {
            tokens.push_back(make_tuple(TIPO_TOKEN_NUMERO, to_string(M_E), 0, false));
        } 
        else if (expressao.substr(i, 2) == "pi") {
            tokens.push_back(make_tuple(TIPO_TOKEN_NUMERO, to_string(M_PI), 0, false));
            i++; 
        } 
        else if (expressao[i] == '(') {
            tokens.push_back(make_tuple(TIPO_TOKEN_PARENTESE_ABRE, "(", 0, false));
        } 
        else if (expressao[i] == ')') {
            tokens.push_back(make_tuple(TIPO_TOKEN_PARENTESE_FECHA, ")", 0, false));
        } 
        else if (expressao[i] == '*' || expressao[i] == '/' || expressao[i] == '^') {
            string str_op(1, expressao[i]);
            tokens.push_back(make_tuple(TIPO_TOKEN_OPERADOR, str_op, obter_precedencia(str_op), (str_op == "^")));
        } 
        else if (expressao[i] == '-' || expressao[i] == '+') {
            bool eh_unario = (tokens.empty() || get<0>(tokens.back()) == TIPO_TOKEN_OPERADOR || get<0>(tokens.back()) == TIPO_TOKEN_PARENTESE_ABRE);
            if (eh_unario) {
                tokens.push_back(make_tuple(TIPO_TOKEN_NUMERO, "0", 0, false));
            }
            string str_op(1, expressao[i]);
            tokens.push_back(make_tuple(TIPO_TOKEN_OPERADOR, str_op, obter_precedencia(str_op), false));
        } 
        else if (isalpha(expressao[i])) {
            string nome;
            while (i < expressao.length() && isalpha(expressao[i])) {
                nome += expressao[i];
                i++;
            }
            --i; 
            tokens.push_back(make_tuple(TIPO_TOKEN_FUNCAO, nome, 0, false));
        } 
        else {
            throw runtime_error("Caractere inesperado na expressao: " + string(1, expressao[i]));
        }
    }
    return tokens;
}

vector<Token> shunting_yard(const vector<Token>& tokens_infix) {
    vector<Token> fila_saida;
    stack<Token> pilha_operadores;
    for (const auto& token : tokens_infix) {
        int tipo_token = get<0>(token);
        if (tipo_token == TIPO_TOKEN_NUMERO || tipo_token == TIPO_TOKEN_VARIAVEL) {
            fila_saida.push_back(token);
        } 
        else if (tipo_token == TIPO_TOKEN_FUNCAO) {
            pilha_operadores.push(token);
        } 
        else if (tipo_token == TIPO_TOKEN_OPERADOR) {
            int precedencia_atual = get<2>(token);
            bool associativo_direita = get<3>(token);
            while (!pilha_operadores.empty() && 
                   get<0>(pilha_operadores.top()) != TIPO_TOKEN_PARENTESE_ABRE &&
                   (get<2>(pilha_operadores.top()) > precedencia_atual || 
                    (get<2>(pilha_operadores.top()) == precedencia_atual && !associativo_direita))) {
                fila_saida.push_back(pilha_operadores.top());
                pilha_operadores.pop();
            }
            pilha_operadores.push(token);
        } 
        else if (tipo_token == TIPO_TOKEN_PARENTESE_ABRE) {
            pilha_operadores.push(token);
        } 
        else if (tipo_token == TIPO_TOKEN_PARENTESE_FECHA) {
            while (!pilha_operadores.empty() && get<0>(pilha_operadores.top()) != TIPO_TOKEN_PARENTESE_ABRE) {
                fila_saida.push_back(pilha_operadores.top());
                pilha_operadores.pop();
            }
            if (pilha_operadores.empty() || get<0>(pilha_operadores.top()) != TIPO_TOKEN_PARENTESE_ABRE) {
                throw runtime_error("Erro: Parênteses desbalanceados.");
            }
            pilha_operadores.pop(); 
            if (!pilha_operadores.empty() && get<0>(pilha_operadores.top()) == TIPO_TOKEN_FUNCAO) {
                fila_saida.push_back(pilha_operadores.top());
                pilha_operadores.pop();
            }
        }
    }
    while (!pilha_operadores.empty()) {
        if (get<0>(pilha_operadores.top()) == TIPO_TOKEN_PARENTESE_ABRE || 
            get<0>(pilha_operadores.top()) == TIPO_TOKEN_PARENTESE_FECHA) {
            throw runtime_error("Erro: Parênteses desbalanceados.");
        }
        fila_saida.push_back(pilha_operadores.top());
        pilha_operadores.pop();
    }
    return fila_saida;
}

double avaliar_rpn(const vector<Token>& tokens_rpn, double valor_x) {
    stack<double> pilha_operandos;
    for (const auto& token : tokens_rpn) {
        int tipo_token = get<0>(token);
        string valor_token = get<1>(token);
        if (tipo_token == TIPO_TOKEN_NUMERO) {
            pilha_operandos.push(stod(valor_token));
        } 
        else if (tipo_token == TIPO_TOKEN_VARIAVEL) {
            pilha_operandos.push(valor_x);
        } 
        else if (tipo_token == TIPO_TOKEN_OPERADOR) {
            if (pilha_operandos.size() < 2) {
                throw runtime_error("Erro de sintaxe: operador '" + valor_token + "' sem operandos suficientes.");
            }
            double operando2 = pilha_operandos.top(); pilha_operandos.pop();
            double operando1 = pilha_operandos.top(); pilha_operandos.pop();
            pilha_operandos.push(aplicar_operador(operando1, operando2, valor_token));
        } 
        else if (tipo_token == TIPO_TOKEN_FUNCAO) {
            if (pilha_operandos.empty()) {
                throw runtime_error("Erro de sintaxe: função '" + valor_token + "' sem argumento.");
            }
            double arg = pilha_operandos.top(); pilha_operandos.pop();
            pilha_operandos.push(aplicar_funcao_nomeada(valor_token, arg));
        }
    }
    if (pilha_operandos.size() != 1) {
        throw runtime_error("Erro de sintaxe: expressão inválida ou incompleta.");
    }
    return pilha_operandos.top();
}

string pre_processar_string_funcao_para_analise(const string& str_funcao) {
    string str_processada = str_funcao;
    size_t pos_igual = str_processada.find("=");
    if (pos_igual != string::npos) {
        str_processada = str_processada.substr(pos_igual + 1);
    }
    size_t pos_e_exp = str_processada.find("e^");
    while (pos_e_exp != string::npos) {
        if (pos_e_exp == 0 || !isalpha(str_processada[pos_e_exp-1])) {
            str_processada.replace(pos_e_exp, 2, "exp");
        }
        pos_e_exp = str_processada.find("e^", pos_e_exp + 1);
    }
    str_processada.erase(remove(str_processada.begin(), str_processada.end(), ' '), str_processada.end());
    return str_processada;
}

function<double(double)> analisar_string_funcao(const string& str_funcao) {
    string str_pre_processada = pre_processar_string_funcao_para_analise(str_funcao);
    return [str_pre_processada](double valor_x) {
        try {
            vector<Token> tokens = tokenizar(str_pre_processada);
            vector<Token> tokens_rpn = shunting_yard(tokens);
            return avaliar_rpn(tokens_rpn, valor_x);
        } catch (const exception& e) {
            throw runtime_error("Erro ao avaliar função '" + str_pre_processada + "': " + e.what());
        }
    };
}
