Trabalho de Métodos Numéricos com reconhecedor de funções a partir de um txt, o programa então gera 2 txt's de saída, um com todas as iterações e outro com as raízes finais.
No TXT de entrada utilizei "#" como linha de comentário
O padrão para a entrada de funções é: METODO; FUNCAO_F; [FUNCAO_G/FUNCAO_DF]; PARAMETRO1; PARAMETRO2; EPSILON; MAX_ITERACOES
Para compliar o programa deve-se copilar os 3 arquivos ao mesmo tempo, segue comandos:
  g++ -std=c++17 main.cpp analisador_funcao.cpp metodos_numericos.cpp -o trabalho_computacional 
  ./trabalho_computacional
