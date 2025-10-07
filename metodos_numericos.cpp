#include <cmath>
#include <stdexcept>
#include <iostream>
#include <string>
#include <functional>
#include <iomanip>
#include <fstream>

using namespace std;

void registrarResultado(const std::string& nome_metodo, double raiz, double valor_funcao, double erro, int iteracoes);

double bissecao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double iteracao_ponto_fixo(const function<double(double)>& funcao_f, const function<double(double)>& funcao_g, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double newton(const function<double(double)>& funcao_f, const function<double(double)>& funcao_df, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double secante(const function<double(double)>& funcao, double x0_val, double x1_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);

double falsa_posicao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final);


#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>

void registrarResultado(const string& nome_metodo, double raiz, double valor_funcao, double erro, int iteracoes) {
    static bool cabecalho_escrito = false;
    static ofstream arquivo_saida("iteracoes.txt", ios::out);

    if (!cabecalho_escrito) {
        arquivo_saida << left << setw(20) << "Metodo"
                      << left << setw(20) << "Raiz Aproximada"
                      << left << setw(20) << "|f(raiz)|"
                      << left << setw(20) << "|xn - xn-1|"
                      << left << setw(10) << "Iteracoes" << endl;
        cabecalho_escrito = true;
    }

    arquivo_saida << left << setw(20) << nome_metodo
                  << left << setw(20) << fixed << setprecision(8) << raiz
                  << left << setw(20) << fixed << setprecision(8) << abs(valor_funcao)
                  << left << setw(20) << fixed << setprecision(8) << erro
                  << left << setw(10) << iteracoes + 1 << endl;
}
double bissecao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final) {

    double fa = funcao(a_val);
    double fb = funcao(b_val);
    
    if (fa * fb >= 0) {
        throw runtime_error("Erro (Bisseção): A função deve ter sinais opostos nos limites a e b.");
    }
    
    double c_val = a_val;
    double c_anterior = a_val;
    double fc;
    
    for (int i = 0; i < max_iteracoes; ++i) {

        c_val = (a_val + b_val) / 2;
        fc = funcao(c_val);

        diferenca_final = abs(c_val - c_anterior);

        registrarResultado("Bisecção", c_val, fc, diferenca_final, i);

        if (abs(fc) < tolerancia || diferenca_final < tolerancia) {
            iteracoes_realizadas = i + 1;
            return c_val;
        }

        c_anterior = c_val;

        if (fc * fa < 0) {
            b_val = c_val;
            fb = fc;
        } else {
            a_val = c_val;
            fa = fc;
        }
    }

    iteracoes_realizadas = max_iteracoes;
    diferenca_final = abs(c_val - c_anterior);
    
    cerr << "Aviso (Bisseção): Convergência não alcançada em " << max_iteracoes 
         << " iterações. Erro atual: " << setprecision(10) << abs(fc) << endl;
         
    return c_val;
}

double iteracao_ponto_fixo(const function<double(double)>& funcao_f, const function<double(double)>& funcao_g, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final) {
    double x_anterior = x0_val;
    double x_proximo = x0_val;
    double fx_proximo;
    
    for (int i = 0; i < max_iteracoes; ++i) {

        x_proximo = funcao_g(x_anterior);
        fx_proximo = funcao_f(x_proximo);

        diferenca_final = abs(x_proximo - x_anterior);
        
        registrarResultado("Ponto Fixo", x_proximo, fx_proximo, diferenca_final,i);

        if (abs(fx_proximo) < tolerancia || diferenca_final < tolerancia) {
            iteracoes_realizadas = i + 1;
            return x_proximo;
        }

        x_anterior = x_proximo;

    }

    iteracoes_realizadas = max_iteracoes;
    diferenca_final = abs(x_proximo - x_anterior);
    
    cerr << "Aviso (MIL): Convergência não alcançada em " << max_iteracoes 
         << " iterações. Erro atual: " << setprecision(10) << abs(fx_proximo) << endl;
         
    return x_proximo;
}

double newton(const function<double(double)>& funcao_f, const function<double(double)>& funcao_df, double x0_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final) {
    double x_anterior = x0_val;
    double x_proximo = x0_val;
    double fx, dfx;
    
    for (int i = 0; i < max_iteracoes; ++i) {

        fx = funcao_f(x_anterior);
        dfx = funcao_df(x_anterior);

        if (abs(dfx) < 1e-12) 
            throw runtime_error("Erro (Newton): Derivada próxima de zero em x = " + to_string(x_anterior));

        x_proximo = x_anterior - fx / dfx;

        diferenca_final = abs(x_proximo - x_anterior);
        double fx_proximo = funcao_f(x_proximo);
        
        registrarResultado("Newton", x_proximo, funcao_f(x_proximo), diferenca_final, i);

        if (abs(fx_proximo) < tolerancia || diferenca_final < tolerancia) {
            iteracoes_realizadas = i + 1;
            return x_proximo;
        }

        x_anterior = x_proximo;
    }

    iteracoes_realizadas = max_iteracoes;
    diferenca_final = abs(x_proximo - x_anterior);
    
    cerr << "Aviso (Newton): Convergência não alcançada em " << max_iteracoes 
         << " iterações. Erro atual: " << setprecision(10) << abs(funcao_f(x_proximo)) << endl;
         
    return x_proximo;
}

double secante(const function<double(double)>& funcao, double x0_val, double x1_val, double tolerancia, int max_iteracoes, int& iteracoes_realizadas, double& diferenca_final) {
    double x0 = x0_val;
    double x1 = x1_val;
    double fx0 = funcao(x0);
    double fx1 = funcao(x1);
    double x_proximo, fx_proximo;

    if (abs(fx0 - fx1) < 1e-12) {
        throw runtime_error("Erro (Secante): f(x0) e f(x1) são muito próximos, divisão por zero iminente.");
    }

    for (int i = 0; i < max_iteracoes; ++i) {

        if (abs(fx1 - fx0) < 1e-12) {
            throw runtime_error("Erro (Secante): f(x1) e f(x0) são muito próximos, divisão por zero iminente.");
        }

        x_proximo = x1 - fx1 * (x1 - x0) / (fx1 - fx0);
        fx_proximo = funcao(x_proximo);

        diferenca_final = abs(x_proximo - x1);

        registrarResultado("Secante", x_proximo, fx_proximo, diferenca_final,i);

        if (abs(fx_proximo) < tolerancia || diferenca_final < tolerancia) {
            iteracoes_realizadas = i + 1;
            return x_proximo;
        }

        x0 = x1;
        x1 = x_proximo;
        fx0 = fx1;
        fx1 = fx_proximo;
    }

    iteracoes_realizadas = max_iteracoes;
    diferenca_final = abs(x1 - x0);
    
    cerr << "Aviso (Secante): Convergência não alcançada em " << max_iteracoes 
         << " iterações. Erro atual: " << setprecision(10) << abs(fx1) << endl;
         
    return x1;
}

double falsa_posicao(const function<double(double)>& funcao, double a_val, double b_val, double tolerancia, int max_iteracoes,  int& iteracoes_realizadas, double& diferenca_final) {

    double a = a_val;
    double b = b_val;
    double fa = funcao(a);
    double fb = funcao(b);
    
    if (fa * fb >= 0) {
        throw runtime_error("Erro (Regula Falsi): A função deve ter sinais opostos nos limites a e b.");
    }
    
    double c = a;
    double c_anterior = a;
    double fc;
    
    for (int i = 0; i < max_iteracoes; ++i) {

        if (abs(fb - fa) < 1e-12) {
            throw runtime_error("Erro (Regula Falsi): f(b) e f(a) são muito próximos, divisão por zero iminente.");
        }

        c = (a * fb - b * fa) / (fb - fa);
        fc = funcao(c);

        diferenca_final = abs(c - c_anterior);

        registrarResultado("Falsa Posição", c, fc, diferenca_final, i);

        if (abs(fc) < tolerancia || diferenca_final < tolerancia) {
            iteracoes_realizadas = i + 1;
            return c;
        }
        
        c_anterior = c;

        if (fc * fa < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
    }

    iteracoes_realizadas = max_iteracoes;
    diferenca_final = abs(c - c_anterior);
    
    cerr << "Aviso (Regula Falsi): Convergência não alcançada em " << max_iteracoes 
         << " iterações. Erro atual: " << setprecision(10) << abs(fc) << endl;
         
    return c;
}
