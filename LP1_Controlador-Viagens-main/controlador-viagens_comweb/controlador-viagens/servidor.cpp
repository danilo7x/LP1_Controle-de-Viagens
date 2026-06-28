// servidor.cpp
// Sobe um servidor HTTP local que serve o dashboard (index.html) e
// expoe a logica do ControladorDeTransito atraves de rotas REST simples.
// Toda a regra de negocio continua no C++; o navegador apenas envia
// comandos e recebe o estado em JSON.

#include "httplib.h"
#include "ControladorDeTransito.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Extrai o valor de uma chave simples de um corpo "x=1&y=2".
// (parser minimalista o suficiente para os nossos formularios)
static std::string param(const httplib::Params& p, const std::string& chave) {
    auto it = p.find(chave);
    return it != p.end() ? it->second : "";
}

int main() {
    ControladorDeTransito controlador;
    const std::string PASTA_DADOS = "dados";

    httplib::Server svr;

    // ---- Estado atual em JSON ----
    svr.Get("/api/estado", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(controlador.estadoJson(), "application/json");
    });

    // ---- Cadastrar cidade ----
    svr.Post("/api/cidade", [&](const httplib::Request& req, httplib::Response& res) {
        std::string nome = param(req.params, "nome");
        bool ok = controlador.cadastrarCidade(nome);
        res.set_content(ok ? "{\"ok\":true}" : "{\"ok\":false}", "application/json");
    });

    // ---- Cadastrar trajeto ----
    svr.Post("/api/trajeto", [&](const httplib::Request& req, httplib::Response& res) {
        std::string o = param(req.params, "origem");
        std::string d = param(req.params, "destino");
        std::string tipo = param(req.params, "tipo");
        std::string dist = param(req.params, "distancia");
        bool ok = false;
        if (!tipo.empty() && !dist.empty())
            ok = controlador.cadastrarTrajeto(o, d, tipo[0], std::stoi(dist));
        res.set_content(ok ? "{\"ok\":true}" : "{\"ok\":false}", "application/json");
    });

    // ---- Cadastrar transporte ----
    svr.Post("/api/transporte", [&](const httplib::Request& req, httplib::Response& res) {
        std::string nome = param(req.params, "nome");
        std::string tipo = param(req.params, "tipo");
        std::string cap = param(req.params, "capacidade");
        std::string vel = param(req.params, "velocidade");
        std::string dd = param(req.params, "descanso_dist");
        std::string td = param(req.params, "descanso_tempo");
        std::string local = param(req.params, "local");
        bool ok = false;
        if (!tipo.empty() && !cap.empty() && !vel.empty() && !dd.empty() && !td.empty())
            ok = controlador.cadastrarTransporte(nome, tipo[0], std::stoi(cap),
                     std::stoi(vel), std::stoi(dd), std::stoi(td), local);
        res.set_content(ok ? "{\"ok\":true}" : "{\"ok\":false}", "application/json");
    });

    // ---- Cadastrar passageiro ----
    svr.Post("/api/passageiro", [&](const httplib::Request& req, httplib::Response& res) {
        std::string nome = param(req.params, "nome");
        std::string local = param(req.params, "local");
        bool ok = controlador.cadastrarPassageiro(nome, local);
        res.set_content(ok ? "{\"ok\":true}" : "{\"ok\":false}", "application/json");
    });

    // ---- Iniciar viagem ----
    svr.Post("/api/viagem", [&](const httplib::Request& req, httplib::Response& res) {
        std::string transp = param(req.params, "transporte");
        std::string o = param(req.params, "origem");
        std::string d = param(req.params, "destino");
        std::string passStr = param(req.params, "passageiros"); // separados por virgula
        std::vector<std::string> nomes;
        std::stringstream ss(passStr);
        std::string nome;
        while (std::getline(ss, nome, ',')) {
            size_t ini = nome.find_first_not_of(" ");
            size_t fim = nome.find_last_not_of(" ");
            if (ini != std::string::npos) nomes.push_back(nome.substr(ini, fim - ini + 1));
        }
        bool ok = controlador.iniciarViagem(transp, nomes, o, d);
        res.set_content(ok ? "{\"ok\":true}" : "{\"ok\":false}", "application/json");
    });

    // ---- Avancar horas ----
    svr.Post("/api/avancar", [&](const httplib::Request& req, httplib::Response& res) {
        std::string h = param(req.params, "horas");
        if (!h.empty()) controlador.avancarHoras(std::stoi(h));
        res.set_content("{\"ok\":true}", "application/json");
    });

    // ---- Salvar / Carregar ----
    svr.Post("/api/salvar", [&](const httplib::Request&, httplib::Response& res) {
        controlador.salvar(PASTA_DADOS);
        res.set_content("{\"ok\":true}", "application/json");
    });
    svr.Post("/api/carregar", [&](const httplib::Request&, httplib::Response& res) {
        controlador.carregar(PASTA_DADOS);
        res.set_content("{\"ok\":true}", "application/json");
    });

    // ---- Servir o dashboard (arquivos estaticos da pasta web/) ----
    svr.set_mount_point("/", "./web");

    std::cout << "==============================================\n";
    std::cout << " Dashboard rodando! Abra no navegador:\n";
    std::cout << "   http://localhost:8080\n";
    std::cout << " (para encerrar, feche este terminal ou Ctrl+C)\n";
    std::cout << "==============================================\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
