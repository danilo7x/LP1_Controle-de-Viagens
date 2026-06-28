#include "Viagem.h"

Viagem::Viagem(Transporte* transporte, std::vector<Passageiro*> passageiros,
               Cidade* origem, Cidade* destino, int distancia)
    : transporte(transporte), passageiros(passageiros), origem(origem),
      destino(destino), distancia(distancia), proxima(nullptr),
      kmPercorridos(0.0), emAndamento(false), concluida(false),
      kmDesdeDescanso(0.0), horasDescansoRestante(0) {}

void Viagem::iniciar() {
    emAndamento = true;
    // o transporte sai da cidade de origem -> fica "em transito" (nullptr)
    transporte->setLocalAtual(nullptr);
    for (Passageiro* p : passageiros) {
        p->setLocalAtual(nullptr);
    }
}

// Simula 'horas' de viagem hora a hora. Em cada hora:
//  - se estiver descansando, gasta a hora descansando;
//  - senao, anda 'velocidade' km. Se ao andar atingir a distancia entre
//    descansos, dispara um descanso (que comeca a consumir as proximas horas).
// Retorna true se ESTE trecho foi concluido durante este avanco.
bool Viagem::avancarHoras(int horas) {
    if (!emAndamento || concluida) return false;

    int v = transporte->getVelocidade();
    int distDescanso = transporte->getDistanciaEntreDescansos();
    int tempoDescanso = transporte->getTempoDescanso();

    for (int h = 0; h < horas; h++) {
        if (concluida) break;

        if (horasDescansoRestante > 0) {
            // transporte parado descansando nesta hora
            horasDescansoRestante--;
            continue;
        }

        // anda uma hora
        kmPercorridos += v;
        kmDesdeDescanso += v;

        // chegou ao destino do trecho?
        if (kmPercorridos >= distancia) {
            kmPercorridos = distancia;
            concluida = true;
            emAndamento = false;
            break;
        }

        // precisa descansar? (so se distDescanso > 0)
        if (distDescanso > 0 && kmDesdeDescanso >= distDescanso) {
            kmDesdeDescanso = 0.0;
            horasDescansoRestante = tempoDescanso;
        }
    }

    return concluida;
}

Cidade* Viagem::getOrigem() const { return origem; }
Cidade* Viagem::getDestino() const { return destino; }
Transporte* Viagem::getTransporte() const { return transporte; }
const std::vector<Passageiro*>& Viagem::getPassageiros() const { return passageiros; }
int Viagem::getDistancia() const { return distancia; }

bool Viagem::isEmAndamento() const { return emAndamento; }
bool Viagem::isConcluida() const { return concluida; }

void Viagem::setProxima(Viagem* p) { proxima = p; }
Viagem* Viagem::getProxima() const { return proxima; }
