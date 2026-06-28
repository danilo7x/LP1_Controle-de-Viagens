#include "ControladorDeTransito.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <queue>

ControladorDeTransito::~ControladorDeTransito() {
    for (Cidade* c : cidades) delete c;
    for (Trajeto* t : trajetos) delete t;
    for (Transporte* t : transportes) delete t;
    for (Passageiro* p : passageiros) delete p;
    for (Viagem* v : viagens) {
        // apaga a cadeia inteira de viagens encadeadas
        Viagem* atual = v;
        while (atual != nullptr) {
            Viagem* prox = atual->getProxima();
            delete atual;
            atual = prox;
        }
    }
}

// ---------------- Helpers ----------------

Cidade* ControladorDeTransito::buscarCidade(const std::string& nome) const {
    for (Cidade* c : cidades)
        if (c->getNome() == nome) return c;
    return nullptr;
}

Transporte* ControladorDeTransito::buscarTransporte(const std::string& nome) const {
    for (Transporte* t : transportes)
        if (t->getNome() == nome) return t;
    return nullptr;
}

Passageiro* ControladorDeTransito::buscarPassageiro(const std::string& nome) const {
    for (Passageiro* p : passageiros)
        if (p->getNome() == nome) return p;
    return nullptr;
}

int ControladorDeTransito::distanciaDireta(Cidade* a, Cidade* b, char tipo) const {
    int melhor = -1;
    for (Trajeto* t : trajetos) {
        if (t->getTipo() != tipo) continue;
        // trajeto tratado como bidirecional
        bool casa = (t->getOrigem() == a && t->getDestino() == b) ||
                    (t->getOrigem() == b && t->getDestino() == a);
        if (casa) {
            if (melhor == -1 || t->getDistancia() < melhor)
                melhor = t->getDistancia();
        }
    }
    return melhor;
}

// ---------------- Cadastros ----------------

bool ControladorDeTransito::cadastrarCidade(const std::string& nome) {
    if (buscarCidade(nome) != nullptr) {
        std::cout << "Erro: ja existe uma cidade com esse nome.\n";
        return false;
    }
    cidades.push_back(new Cidade(nome));
    return true;
}

bool ControladorDeTransito::cadastrarTrajeto(const std::string& nomeOrigem,
                                             const std::string& nomeDestino,
                                             char tipo, int distancia) {
    Cidade* o = buscarCidade(nomeOrigem);
    Cidade* d = buscarCidade(nomeDestino);
    if (!o || !d) {
        std::cout << "Erro: cidade de origem ou destino nao encontrada.\n";
        return false;
    }
    if (tipo != 'A' && tipo != 'T') {
        std::cout << "Erro: tipo de trajeto invalido (use A ou T).\n";
        return false;
    }
    if (distancia <= 0) {
        std::cout << "Erro: distancia deve ser positiva.\n";
        return false;
    }
    trajetos.push_back(new Trajeto(o, d, tipo, distancia));
    return true;
}

bool ControladorDeTransito::cadastrarTransporte(const std::string& nome, char tipo,
                                                int capacidade, int velocidade,
                                                int distanciaEntreDescansos,
                                                int tempoDeDescanso,
                                                const std::string& localAtual) {
    if (buscarTransporte(nome) != nullptr) {
        std::cout << "Erro: ja existe um transporte com esse nome.\n";
        return false;
    }
    if (tipo != 'A' && tipo != 'T') {
        std::cout << "Erro: tipo invalido (use A ou T).\n";
        return false;
    }
    Cidade* local = buscarCidade(localAtual);
    if (!local) {
        std::cout << "Erro: cidade do local atual nao encontrada.\n";
        return false;
    }
    if (velocidade <= 0 || capacidade <= 0) {
        std::cout << "Erro: velocidade e capacidade devem ser positivas.\n";
        return false;
    }
    transportes.push_back(new Transporte(nome, tipo, capacidade, velocidade,
                                         distanciaEntreDescansos, tempoDeDescanso, local));
    return true;
}

bool ControladorDeTransito::cadastrarPassageiro(const std::string& nome,
                                                const std::string& localAtual) {
    if (buscarPassageiro(nome) != nullptr) {
        std::cout << "Erro: ja existe um passageiro com esse nome.\n";
        return false;
    }
    Cidade* local = buscarCidade(localAtual);
    if (!local) {
        std::cout << "Erro: cidade do local atual nao encontrada.\n";
        return false;
    }
    passageiros.push_back(new Passageiro(nome, local));
    return true;
}

// ---------------- Roteamento (Dijkstra) ----------------

std::vector<Cidade*> ControladorDeTransito::melhorTrajeto(Cidade* origem, Cidade* destino,
                                                          char tipoTransporte) const {
    const int INF = std::numeric_limits<int>::max();
    int n = cidades.size();

    // mapeia cada cidade para um indice
    auto indiceDe = [&](Cidade* c) -> int {
        for (int i = 0; i < n; i++) if (cidades[i] == c) return i;
        return -1;
    };

    int s = indiceDe(origem);
    int t = indiceDe(destino);
    if (s == -1 || t == -1) return {};

    std::vector<int> dist(n, INF);
    std::vector<int> anterior(n, -1);
    dist[s] = 0;

    // fila de prioridade (min-heap) por distancia
    using Par = std::pair<int,int>; // (distancia, indice)
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> pq;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        if (d > dist[u]) continue;

        for (int v = 0; v < n; v++) {
            if (v == u) continue;
            int peso = distanciaDireta(cidades[u], cidades[v], tipoTransporte);
            if (peso < 0) continue; // sem aresta desse tipo
            if (dist[u] + peso < dist[v]) {
                dist[v] = dist[u] + peso;
                anterior[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[t] == INF) return {}; // sem caminho

    // reconstroi o caminho de tras pra frente
    std::vector<Cidade*> caminho;
    for (int at = t; at != -1; at = anterior[at])
        caminho.push_back(cidades[at]);
    std::reverse(caminho.begin(), caminho.end());
    return caminho;
}

// ---------------- Viagens ----------------

bool ControladorDeTransito::iniciarViagem(const std::string& nomeTransporte,
                                          const std::vector<std::string>& nomesPassageiros,
                                          const std::string& nomeOrigem,
                                          const std::string& nomeDestino) {
    Transporte* transp = buscarTransporte(nomeTransporte);
    Cidade* origem = buscarCidade(nomeOrigem);
    Cidade* destino = buscarCidade(nomeDestino);

    if (!transp) { std::cout << "Erro: transporte nao encontrado.\n"; return false; }
    if (!origem || !destino) { std::cout << "Erro: cidade invalida.\n"; return false; }
    if (origem == destino) { std::cout << "Erro: origem e destino iguais.\n"; return false; }

    // o transporte precisa estar na cidade de origem
    if (transp->getLocalAtual() != origem) {
        std::cout << "Erro: o transporte nao esta na cidade de origem.\n";
        return false;
    }

    // valida passageiros
    std::vector<Passageiro*> lista;
    for (const std::string& nome : nomesPassageiros) {
        Passageiro* p = buscarPassageiro(nome);
        if (!p) { std::cout << "Erro: passageiro '" << nome << "' nao encontrado.\n"; return false; }
        if (p->getLocalAtual() != origem) {
            std::cout << "Erro: passageiro '" << nome << "' nao esta na cidade de origem.\n";
            return false;
        }
        lista.push_back(p);
    }

    if ((int)lista.size() > transp->getCapacidade()) {
        std::cout << "Erro: numero de passageiros excede a capacidade do transporte.\n";
        return false;
    }

    // calcula a rota com Dijkstra restrito ao tipo do transporte
    std::vector<Cidade*> rota = melhorTrajeto(origem, destino, transp->getTipo());
    if (rota.empty()) {
        std::cout << "Erro: nao existe rota possivel entre as cidades para esse tipo de transporte.\n";
        return false;
    }

    // cria as viagens encadeadas (uma por trecho da rota)
    Viagem* cabeca = nullptr;
    Viagem* anterior = nullptr;
    for (size_t i = 0; i + 1 < rota.size(); i++) {
        Cidade* a = rota[i];
        Cidade* b = rota[i + 1];
        int dist = distanciaDireta(a, b, transp->getTipo());
        Viagem* v = new Viagem(transp, lista, a, b, dist);
        if (!cabeca) cabeca = v;
        if (anterior) anterior->setProxima(v);
        anterior = v;
    }

    cabeca->iniciar(); // ativa apenas o primeiro trecho
    viagens.push_back(cabeca);

    std::cout << "Viagem iniciada! Rota: ";
    for (size_t i = 0; i < rota.size(); i++) {
        std::cout << rota[i]->getNome();
        if (i + 1 < rota.size()) std::cout << " -> ";
    }
    std::cout << "\n";
    return true;
}

void ControladorDeTransito::avancarHoras(int horas) {
    // Avancamos hora a hora. Assim, se um trecho termina no meio do intervalo,
    // as horas restantes ja sao aplicadas ao proximo trecho da mesma cadeia.
    for (int h = 0; h < horas; h++) {
        for (Viagem* cabeca : viagens) {
            // encontra o trecho ativo da cadeia
            Viagem* atual = cabeca;
            while (atual != nullptr && atual->isConcluida())
                atual = atual->getProxima();
            if (atual == nullptr) continue; // rota inteira ja terminou

            if (!atual->isEmAndamento())
                atual->iniciar();

            bool concluiu = atual->avancarHoras(1); // avanca exatamente 1 hora

            if (concluiu) {
                Viagem* prox = atual->getProxima();
                if (prox != nullptr) {
                    // ainda ha trechos: comeca o proximo. Transporte/passageiros
                    // continuam "em transito" entre os trechos conectados.
                    prox->iniciar();
                } else {
                    // ultimo trecho concluido: agora sim chega no destino final
                    Cidade* destinoFinal = atual->getDestino();
                    Transporte* transp = atual->getTransporte();
                    transp->setLocalAtual(destinoFinal);
                    for (Passageiro* p : atual->getPassageiros())
                        p->setLocalAtual(destinoFinal);
                    destinoFinal->registrarVisita();
                    std::cout << "[CHEGADA] " << transp->getNome()
                              << " chegou em " << destinoFinal->getNome() << ".\n";
                }
            }
        }
    }
}

// ---------------- Relatorios ----------------

void ControladorDeTransito::relatarPassageiros() const {
    std::cout << "\n=== Localizacao dos Passageiros ===\n";
    if (passageiros.empty()) { std::cout << "(nenhum passageiro cadastrado)\n"; return; }
    for (Passageiro* p : passageiros) {
        if (p->getLocalAtual() != nullptr) {
            std::cout << "- " << p->getNome() << ": em " << p->getLocalAtual()->getNome() << "\n";
        } else {
            // procura em qual viagem ele esta
            std::cout << "- " << p->getNome() << ": em transito";
            for (Viagem* cabeca : viagens) {
                Viagem* atual = cabeca;
                while (atual) {
                    if (atual->isEmAndamento()) {
                        const auto& lst = atual->getPassageiros();
                        if (std::find(lst.begin(), lst.end(), p) != lst.end()) {
                            std::cout << " (" << atual->getOrigem()->getNome()
                                      << " -> " << atual->getDestino()->getNome()
                                      << ", transporte " << atual->getTransporte()->getNome() << ")";
                        }
                    }
                    atual = atual->getProxima();
                }
            }
            std::cout << "\n";
        }
    }
}

void ControladorDeTransito::relatarTransportes() const {
    std::cout << "\n=== Localizacao dos Transportes ===\n";
    if (transportes.empty()) { std::cout << "(nenhum transporte cadastrado)\n"; return; }
    for (Transporte* t : transportes) {
        if (t->getLocalAtual() != nullptr) {
            std::cout << "- " << t->getNome() << ": em " << t->getLocalAtual()->getNome() << "\n";
        } else {
            std::cout << "- " << t->getNome() << ": em transito";
            for (Viagem* cabeca : viagens) {
                Viagem* atual = cabeca;
                while (atual) {
                    if (atual->isEmAndamento() && atual->getTransporte() == t) {
                        std::cout << " (" << atual->getOrigem()->getNome()
                                  << " -> " << atual->getDestino()->getNome() << ")";
                    }
                    atual = atual->getProxima();
                }
            }
            std::cout << "\n";
        }
    }
}

void ControladorDeTransito::relatarViagensEmAndamento() const {
    std::cout << "\n=== Viagens em Andamento ===\n";
    bool achou = false;
    for (Viagem* cabeca : viagens) {
        Viagem* atual = cabeca;
        // verifica se a cadeia ainda tem algum trecho nao concluido
        bool cadeiaAtiva = false;
        Viagem* checa = cabeca;
        while (checa) { if (!checa->isConcluida()) { cadeiaAtiva = true; break; } checa = checa->getProxima(); }
        if (!cadeiaAtiva) continue;

        achou = true;
        // trecho ativo
        while (atual && atual->isConcluida()) atual = atual->getProxima();
        if (atual) {
            std::cout << "- Transporte " << atual->getTransporte()->getNome()
                      << " | trecho atual: " << atual->getOrigem()->getNome()
                      << " -> " << atual->getDestino()->getNome()
                      << " | passageiros: " << atual->getPassageiros().size() << "\n";
        }
    }
    if (!achou) std::cout << "(nenhuma viagem em andamento)\n";
}

void ControladorDeTransito::relatarCidadesMaisVisitadas() const {
    std::cout << "\n=== Cidades Mais Visitadas ===\n";
    if (cidades.empty()) { std::cout << "(nenhuma cidade cadastrada)\n"; return; }
    std::vector<Cidade*> ordenadas = cidades;
    std::sort(ordenadas.begin(), ordenadas.end(), [](Cidade* a, Cidade* b) {
        return a->getVezesVisitada() > b->getVezesVisitada();
    });
    for (Cidade* c : ordenadas) {
        std::cout << "- " << c->getNome() << ": " << c->getVezesVisitada() << " visita(s)\n";
    }
}

void ControladorDeTransito::listarCidades() const {
    std::cout << "\n=== Cidades ===\n";
    if (cidades.empty()) { std::cout << "(nenhuma)\n"; return; }
    for (Cidade* c : cidades) std::cout << "- " << c->getNome() << "\n";
}

void ControladorDeTransito::listarTrajetos() const {
    std::cout << "\n=== Trajetos ===\n";
    if (trajetos.empty()) { std::cout << "(nenhum)\n"; return; }
    for (Trajeto* t : trajetos) {
        std::cout << "- " << t->getOrigem()->getNome() << " <-> " << t->getDestino()->getNome()
                  << " | tipo: " << t->getTipo() << " | " << t->getDistancia() << " km\n";
    }
}

// ---------------- Persistencia ----------------

void ControladorDeTransito::salvar(const std::string& pasta) const {
    // Cidades
    std::ofstream fc(pasta + "/cidades.txt");
    for (Cidade* c : cidades)
        fc << c->getNome() << ";" << c->getVezesVisitada() << "\n";
    fc.close();

    // Trajetos
    std::ofstream ft(pasta + "/trajetos.txt");
    for (Trajeto* t : trajetos)
        ft << t->getOrigem()->getNome() << ";" << t->getDestino()->getNome()
           << ";" << t->getTipo() << ";" << t->getDistancia() << "\n";
    ft.close();

    // Transportes
    std::ofstream ftr(pasta + "/transportes.txt");
    for (Transporte* t : transportes) {
        std::string local = t->getLocalAtual() ? t->getLocalAtual()->getNome() : "EM_TRANSITO";
        ftr << t->getNome() << ";" << t->getTipo() << ";" << t->getCapacidade() << ";"
            << t->getVelocidade() << ";" << t->getDistanciaEntreDescansos() << ";"
            << t->getTempoDescanso() << ";" << local << "\n";
    }
    ftr.close();

    // Passageiros
    std::ofstream fp(pasta + "/passageiros.txt");
    for (Passageiro* p : passageiros) {
        std::string local = p->getLocalAtual() ? p->getLocalAtual()->getNome() : "EM_TRANSITO";
        fp << p->getNome() << ";" << local << "\n";
    }
    fp.close();

    std::cout << "Dados salvos na pasta '" << pasta << "'.\n";
}

void ControladorDeTransito::carregar(const std::string& pasta) {
    std::ifstream fc(pasta + "/cidades.txt");
    std::string linha;
    while (std::getline(fc, linha)) {
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        std::string nome, visitas;
        std::getline(ss, nome, ';');
        std::getline(ss, visitas, ';');
        cadastrarCidade(nome);
        Cidade* c = buscarCidade(nome);
        if (c && !visitas.empty()) c->setVezesVisitada(std::stoi(visitas));
    }
    fc.close();

    std::ifstream ft(pasta + "/trajetos.txt");
    while (std::getline(ft, linha)) {
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        std::string o, d, tipo, dist;
        std::getline(ss, o, ';');
        std::getline(ss, d, ';');
        std::getline(ss, tipo, ';');
        std::getline(ss, dist, ';');
        if (!o.empty() && !tipo.empty() && !dist.empty())
            cadastrarTrajeto(o, d, tipo[0], std::stoi(dist));
    }
    ft.close();

    std::ifstream ftr(pasta + "/transportes.txt");
    while (std::getline(ftr, linha)) {
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        std::string nome, tipo, cap, vel, dd, td, local;
        std::getline(ss, nome, ';');
        std::getline(ss, tipo, ';');
        std::getline(ss, cap, ';');
        std::getline(ss, vel, ';');
        std::getline(ss, dd, ';');
        std::getline(ss, td, ';');
        std::getline(ss, local, ';');
        if (local == "EM_TRANSITO") continue; // simplificacao: nao restaura em transito
        if (!nome.empty())
            cadastrarTransporte(nome, tipo[0], std::stoi(cap), std::stoi(vel),
                                std::stoi(dd), std::stoi(td), local);
    }
    ftr.close();

    std::ifstream fp(pasta + "/passageiros.txt");
    while (std::getline(fp, linha)) {
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        std::string nome, local;
        std::getline(ss, nome, ';');
        std::getline(ss, local, ';');
        if (local == "EM_TRANSITO") continue;
        if (!nome.empty()) cadastrarPassageiro(nome, local);
    }
    fp.close();

    std::cout << "Dados carregados da pasta '" << pasta << "'.\n";
}

// ---------------- Suporte ao dashboard web ----------------

// Pequeno helper: escapa aspas e barras para gerar JSON valido.
static std::string esc(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

// Monta uma string JSON com todo o estado do sistema. O front-end le isso
// e desenha os cards. Toda a logica permanece no C++; aqui so "fotografamos".
std::string ControladorDeTransito::estadoJson() const {
    std::stringstream js;
    js << "{";

    // cidades
    js << "\"cidades\":[";
    for (size_t i = 0; i < cidades.size(); i++) {
        js << "{\"nome\":\"" << esc(cidades[i]->getNome()) << "\",\"visitas\":"
           << cidades[i]->getVezesVisitada() << "}";
        if (i + 1 < cidades.size()) js << ",";
    }
    js << "],";

    // trajetos
    js << "\"trajetos\":[";
    for (size_t i = 0; i < trajetos.size(); i++) {
        Trajeto* t = trajetos[i];
        js << "{\"origem\":\"" << esc(t->getOrigem()->getNome())
           << "\",\"destino\":\"" << esc(t->getDestino()->getNome())
           << "\",\"tipo\":\"" << t->getTipo()
           << "\",\"distancia\":" << t->getDistancia() << "}";
        if (i + 1 < trajetos.size()) js << ",";
    }
    js << "],";

    // transportes
    js << "\"transportes\":[";
    for (size_t i = 0; i < transportes.size(); i++) {
        Transporte* t = transportes[i];
        std::string local = t->getLocalAtual() ? t->getLocalAtual()->getNome() : "em transito";
        js << "{\"nome\":\"" << esc(t->getNome())
           << "\",\"tipo\":\"" << t->getTipo()
           << "\",\"capacidade\":" << t->getCapacidade()
           << ",\"velocidade\":" << t->getVelocidade()
           << ",\"local\":\"" << esc(local) << "\"}";
        if (i + 1 < transportes.size()) js << ",";
    }
    js << "],";

    // passageiros
    js << "\"passageiros\":[";
    for (size_t i = 0; i < passageiros.size(); i++) {
        Passageiro* p = passageiros[i];
        std::string local = p->getLocalAtual() ? p->getLocalAtual()->getNome() : "em transito";
        js << "{\"nome\":\"" << esc(p->getNome())
           << "\",\"local\":\"" << esc(local) << "\"}";
        if (i + 1 < passageiros.size()) js << ",";
    }
    js << "],";

    // viagens em andamento (trecho ativo de cada cadeia)
    js << "\"viagens\":[";
    bool primeiro = true;
    for (Viagem* cabeca : viagens) {
        // a cadeia ainda esta ativa?
        bool ativa = false;
        Viagem* checa = cabeca;
        while (checa) { if (!checa->isConcluida()) { ativa = true; break; } checa = checa->getProxima(); }
        if (!ativa) continue;

        Viagem* atual = cabeca;
        while (atual && atual->isConcluida()) atual = atual->getProxima();
        if (!atual) continue;

        if (!primeiro) js << ",";
        primeiro = false;
        js << "{\"transporte\":\"" << esc(atual->getTransporte()->getNome())
           << "\",\"origem\":\"" << esc(atual->getOrigem()->getNome())
           << "\",\"destino\":\"" << esc(atual->getDestino()->getNome())
           << "\",\"passageiros\":" << atual->getPassageiros().size() << "}";
    }
    js << "]";

    js << "}";
    return js.str();
}
