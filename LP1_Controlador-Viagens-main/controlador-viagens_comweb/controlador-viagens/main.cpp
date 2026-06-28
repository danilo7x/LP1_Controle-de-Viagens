#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include "ControladorDeTransito.h"

// Le uma linha inteira do usuario (permite nomes com espaco)
static std::string lerLinha(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

static int lerInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string s;
        std::getline(std::cin, s);
        try {
            return std::stoi(s);
        } catch (...) {
            std::cout << "Valor invalido, tente novamente.\n";
        }
    }
}

static void menu() {
    std::cout << "\n========= CONTROLADOR DE VIAGENS =========\n";
    std::cout << " 1. Cadastrar cidade\n";
    std::cout << " 2. Cadastrar trajeto\n";
    std::cout << " 3. Cadastrar transporte\n";
    std::cout << " 4. Cadastrar passageiro\n";
    std::cout << " 5. Iniciar viagem\n";
    std::cout << " 6. Avancar horas\n";
    std::cout << " 7. Relatorio: localizacao dos passageiros\n";
    std::cout << " 8. Relatorio: localizacao dos transportes\n";
    std::cout << " 9. Relatorio: viagens em andamento\n";
    std::cout << "10. Relatorio: cidades mais visitadas\n";
    std::cout << "11. Listar cidades\n";
    std::cout << "12. Listar trajetos\n";
    std::cout << "13. Salvar dados\n";
    std::cout << "14. Carregar dados\n";
    std::cout << " 0. Sair\n";
    std::cout << "==========================================\n";
}

int main() {
    ControladorDeTransito controlador;
    const std::string PASTA_DADOS = "dados";

    std::cout << "Bem-vindo ao Sistema Controlador de Viagens!\n";

    while (true) {
        menu();
        int opcao = lerInt("Escolha uma opcao: ");

        if (opcao == 0) {
            std::cout << "Encerrando. Ate mais!\n";
            break;
        }

        switch (opcao) {
            case 1: {
                std::string nome = lerLinha("Nome da cidade: ");
                if (controlador.cadastrarCidade(nome))
                    std::cout << "Cidade cadastrada com sucesso.\n";
                break;
            }
            case 2: {
                std::string o = lerLinha("Cidade de origem: ");
                std::string d = lerLinha("Cidade de destino: ");
                std::string tipo = lerLinha("Tipo (A=Aquatico, T=Terrestre): ");
                int dist = lerInt("Distancia (km): ");
                if (tipo.empty()) { std::cout << "Tipo invalido.\n"; break; }
                if (controlador.cadastrarTrajeto(o, d, tipo[0], dist))
                    std::cout << "Trajeto cadastrado com sucesso.\n";
                break;
            }
            case 3: {
                std::string nome = lerLinha("Nome do transporte: ");
                std::string tipo = lerLinha("Tipo (A=Aquatico, T=Terrestre): ");
                int cap = lerInt("Capacidade de passageiros: ");
                int vel = lerInt("Velocidade (km/h): ");
                int dd = lerInt("Distancia entre descansos (km, 0 = nunca): ");
                int td = lerInt("Tempo de descanso (horas): ");
                std::string local = lerLinha("Cidade onde esta atualmente: ");
                if (tipo.empty()) { std::cout << "Tipo invalido.\n"; break; }
                if (controlador.cadastrarTransporte(nome, tipo[0], cap, vel, dd, td, local))
                    std::cout << "Transporte cadastrado com sucesso.\n";
                break;
            }
            case 4: {
                std::string nome = lerLinha("Nome do passageiro: ");
                std::string local = lerLinha("Cidade onde esta atualmente: ");
                if (controlador.cadastrarPassageiro(nome, local))
                    std::cout << "Passageiro cadastrado com sucesso.\n";
                break;
            }
            case 5: {
                std::string transp = lerLinha("Nome do transporte: ");
                std::string o = lerLinha("Cidade de origem: ");
                std::string d = lerLinha("Cidade de destino: ");
                std::string linhaPass = lerLinha("Passageiros (nomes separados por virgula): ");
                std::vector<std::string> nomes;
                std::stringstream ss(linhaPass);
                std::string nome;
                while (std::getline(ss, nome, ',')) {
                    // tira espacos das pontas
                    size_t ini = nome.find_first_not_of(" ");
                    size_t fim = nome.find_last_not_of(" ");
                    if (ini != std::string::npos)
                        nomes.push_back(nome.substr(ini, fim - ini + 1));
                }
                controlador.iniciarViagem(transp, nomes, o, d);
                break;
            }
            case 6: {
                int horas = lerInt("Quantas horas avancar? ");
                controlador.avancarHoras(horas);
                std::cout << "Tempo avancado em " << horas << " hora(s).\n";
                break;
            }
            case 7: controlador.relatarPassageiros(); break;
            case 8: controlador.relatarTransportes(); break;
            case 9: controlador.relatarViagensEmAndamento(); break;
            case 10: controlador.relatarCidadesMaisVisitadas(); break;
            case 11: controlador.listarCidades(); break;
            case 12: controlador.listarTrajetos(); break;
            case 13: controlador.salvar(PASTA_DADOS); break;
            case 14: controlador.carregar(PASTA_DADOS); break;
            default:
                std::cout << "Opcao invalida.\n";
        }
    }

    return 0;
}
