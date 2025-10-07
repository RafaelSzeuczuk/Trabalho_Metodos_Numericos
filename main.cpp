#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace std;

using Token = tuple<int, string, int, bool>;
vector<Token> tokenizar(const string& expressao);
vector<Token> shunting_yard(const vector<Token>& tokens_infix);
double avaliar_rpn(const vector<Token>& tokens_rpn, double valor_x);
string pre_processar_string_funcao_para_analise(const string& str_funcao);
function<double(double)> analisar_string_funcao(const string& str_funcao);

void registrarResultado(const std::string& nome_metodo, double raiz, double valor_funcao, double erro, int iteracoes);

double bissecao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double iteracao_ponto_fixo(const function<double(double)>& funcao_f, const function<double(double)>& funcao_g, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double newton(const function<double(double)>& funcao_f, const function<double(double)>& funcao_df, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double secante(const function<double(double)>& funcao, double x0_val, double x1_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double falsa_posicao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);


string remover_espacos(const string& str) {
    size_t inicio = str.find_first_not_of(" \t\n\r\f\v");
    if (inicio == string::npos) return "";
    size_t fim = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(inicio, fim - inicio + 1);
}


void imprimir_resultados(const string& nome_metodo, const string& str_funcao, 
                         double raiz, const function<double(double)>& funcao_f,
                         double diferenca_final, int iteracoes_realizadas) {    
    static bool cabecalho_escrito = false;
    static ofstream arquivo_saida("resultados_finais.txt", ios::out);
    if (!cabecalho_escrito) {
        arquivo_saida << left << setw(15) << "Metodo"
                      << setw(30) << "Funcao"
                      << setw(15) << "Raiz"
                      << setw(15) << "|f(raiz)|"
                      << setw(15) << "|xn - xn-1|"
                      << setw(20) << "Iteracoes" << endl;
        cabecalho_escrito = true;
    }
    arquivo_saida << left << setw(15) << nome_metodo
                  << setw(30) << str_funcao
                  << setw(15) << fixed << setprecision(8) << raiz
                  << setw(15) << fabs(funcao_f(raiz))
                  << setw(15) << diferenca_final
                  << setw(20) << iteracoes_realizadas << endl;
}



int main() {
    
    cout << "Iniciando o resolvedor de funcoes matematicas..." << endl;

    ifstream arquivo_entrada("funcoes.txt");
    if (!arquivo_entrada.is_open()) {
        cerr << "Erro: Nao foi possivel abrir o arquivo functions.txt" << endl;
        return 1;
    }

    cout << "\nProcessando funcoes e parametros do arquivo:" << endl;
    
    string linha;
    int numero_linha = 0;

    while (getline(arquivo_entrada, linha)) {
        numero_linha++;

        if (linha.empty() || linha[0] == '#') {
            continue;
        }

        cout << "\n--------------------------------------------------" << endl;
        cout << "Linha " << numero_linha << ": " << linha << endl;
        cout << "--------------------------------------------------" << endl;

        stringstream stream_linha(linha);
        string tipo_metodo_str;
        getline(stream_linha, tipo_metodo_str, ';');
        tipo_metodo_str = remover_espacos(tipo_metodo_str);

        try {

            if (tipo_metodo_str == "BISSECAO") {
                string str_funcao, str_a, str_b, str_epsilon, str_max_iteracoes;
                getline(stream_linha, str_funcao, ';');
                getline(stream_linha, str_a, ';');
                getline(stream_linha, str_b, ';');
                getline(stream_linha, str_epsilon, ';');
                getline(stream_linha, str_max_iteracoes, ';');

                function<double(double)> funcao_f = analisar_string_funcao(str_funcao);
                double valor_a = stod(str_a);
                double valor_b = stod(str_b);
                double epsilon = stod(str_epsilon);
                int max_iteracoes = stoi(str_max_iteracoes);

                cout << "Metodo: Bissecao" << endl;
                cout << "Funcao: " << str_funcao << endl;
                cout << "Intervalo: [" << valor_a << ", " << valor_b << "]" << endl;
                cout << "Epsilon: " << epsilon << ", Max Iteracoes: " << max_iteracoes << endl;

                int iteracoes_realizadas = 0;
                double diferenca_final = 0.0;
                double raiz = bissecao(funcao_f, valor_a, valor_b, epsilon, max_iteracoes, iteracoes_realizadas, diferenca_final);

                imprimir_resultados("Bissecao", str_funcao, raiz, funcao_f, diferenca_final, iteracoes_realizadas);
            } 

            else if (tipo_metodo_str == "ITERACAO_PONTO_FIXO" || tipo_metodo_str == "MIL") {
                string str_funcao_f, str_funcao_g, str_x0, str_epsilon, str_max_iteracoes;
                getline(stream_linha, str_funcao_f, ';');
                getline(stream_linha, str_funcao_g, ';');
                getline(stream_linha, str_x0, ';');
                getline(stream_linha, str_epsilon, ';');
                getline(stream_linha, str_max_iteracoes, ';');

                function<double(double)> funcao_f = analisar_string_funcao(str_funcao_f);
                function<double(double)> funcao_g = analisar_string_funcao(str_funcao_g);
                double valor_x0 = stod(str_x0);
                double epsilon = stod(str_epsilon);
                int max_iteracoes = stoi(str_max_iteracoes);

                cout << "Metodo: Iteracao de Ponto Fixo (MIL)" << endl;
                cout << "Funcao f(x): " << str_funcao_f << endl;
                cout << "Funcao g(x): " << str_funcao_g << endl;
                cout << "x0: " << valor_x0 << endl;
                cout << "Epsilon: " << epsilon << ", Max Iteracoes: " << max_iteracoes << endl;

                int iteracoes_realizadas = 0;
                double diferenca_final = 0.0;
                double raiz = iteracao_ponto_fixo(funcao_f, funcao_g, valor_x0, epsilon, max_iteracoes, iteracoes_realizadas, diferenca_final);

                imprimir_resultados("Ponto Fixo(MIL)", str_funcao_f, raiz, funcao_f, diferenca_final, iteracoes_realizadas);
            } 

            else if (tipo_metodo_str == "NEWTON") {
                string str_funcao_f, str_funcao_df, str_x0, str_epsilon, str_max_iteracoes;
                getline(stream_linha, str_funcao_f, ';');
                getline(stream_linha, str_funcao_df, ';');
                getline(stream_linha, str_x0, ';');
                getline(stream_linha, str_epsilon, ';');
                getline(stream_linha, str_max_iteracoes, ';');

                function<double(double)> funcao_f = analisar_string_funcao(str_funcao_f);   
                function<double(double)> funcao_df= analisar_string_funcao(str_funcao_df);
                double valor_x0 = stod(str_x0);
                double epsilon = stod(str_epsilon);
                int max_iteracoes = stoi(str_max_iteracoes);

                cout << "Metodo: Newton" << endl;
                cout << "Funcao f(x): " << str_funcao_f << endl;
                cout << "Funcao df(x): " <<str_funcao_df << endl;
                cout << "x0: " << valor_x0 << endl;
                cout << "Epsilon: " << epsilon << ", Max Iteracoes: " << max_iteracoes << endl;

                int iteracoes_realizadas = 0;
                double diferenca_final = 0.0;
                double raiz = newton(funcao_f, funcao_df, valor_x0, epsilon, max_iteracoes, iteracoes_realizadas, diferenca_final);

                imprimir_resultados("Newton", str_funcao_f, raiz, funcao_f, diferenca_final, iteracoes_realizadas);
            } 

            else if (tipo_metodo_str == "SECANTE") {
                string str_funcao, str_x0, str_x1, str_epsilon, str_max_iteracoes;
                getline(stream_linha, str_funcao, ';');
                getline(stream_linha, str_x0, ';');
                getline(stream_linha, str_x1, ';');
                getline(stream_linha, str_epsilon, ';');
                getline(stream_linha, str_max_iteracoes, ';');

                function<double(double)> funcao_f = analisar_string_funcao(str_funcao);
                double valor_x0 = stod(str_x0);
                double valor_x1 = stod(str_x1);
                double epsilon = stod(str_epsilon);
                int max_iteracoes = stoi(str_max_iteracoes);

                cout << "Metodo: Secante" << endl;
                cout << "Funcao: " << str_funcao << endl;
                cout << "x0: " << valor_x0 << ", x1: " << valor_x1 << endl;
                cout << "Epsilon: " << epsilon << ", Max Iteracoes: " << max_iteracoes << endl;

                int iteracoes_realizadas = 0;
                double diferenca_final = 0.0;
                double raiz = secante(funcao_f, valor_x0, valor_x1, epsilon, max_iteracoes, iteracoes_realizadas, diferenca_final);

                imprimir_resultados("Secante", str_funcao, raiz, funcao_f, diferenca_final, iteracoes_realizadas);
            } 

            else if (tipo_metodo_str == "REGULA_FALSI") {
                string str_funcao, str_a, str_b, str_epsilon, str_max_iteracoes;
                getline(stream_linha, str_funcao, ';');
                getline(stream_linha, str_a, ';');
                getline(stream_linha, str_b, ';');
                getline(stream_linha, str_epsilon, ';');
                getline(stream_linha, str_max_iteracoes, ';');

                function<double(double)> funcao_f = analisar_string_funcao(str_funcao);
                double valor_a = stod(str_a);
                double valor_b = stod(str_b);
                double epsilon = stod(str_epsilon);
                int max_iteracoes = stoi(str_max_iteracoes);

                cout << "Metodo: Regula Falsi" << endl;
                cout << "Funcao: " << str_funcao << endl;
                cout << "Intervalo: [" << valor_a << ", " << valor_b << "]" << endl;
                cout << "Epsilon: " << epsilon << ", Max Iteracoes: " << max_iteracoes << endl;

                int iteracoes_realizadas = 0;
                double diferenca_final = 0.0;
                double raiz = falsa_posicao(funcao_f, valor_a, valor_b, epsilon, max_iteracoes, iteracoes_realizadas, diferenca_final);

                imprimir_resultados("Regula Falsi", str_funcao, raiz, funcao_f, diferenca_final, iteracoes_realizadas);
            } 

            else {
                cerr << "Erro: Tipo de metodo desconhecido: " << tipo_metodo_str << endl;
            }
        } catch (const exception& e) {
            cerr << "Erro ao processar a linha " << numero_linha << ": " << e.what() << endl;
        }
    }

    arquivo_entrada.close();
    return 0;
}
