# Sistema Controlador de Viagens

Trabalho final de Linguagem de Programacao 1 — sistema de controle de trafego
entre cidades em C++ com Programacao Orientada a Objetos.

Alunos: Danilo Souza Almeida 20250033123 , João Gabriel Da Silva Pinheiro 20250039403

O sistema tem **dois modos de uso**, ambos rodando sobre a MESMA logica C++:
- **Terminal (CLI)** — menu interativo no terminal.
- **Dashboard web** — painel visual no navegador, com a logica C++ por tras
  (um pequeno servidor HTTP embutido). Toda a regra de negocio (rotas, viagens,
  simulacao de tempo) permanece no C++; o navegador so envia comandos e exibe o estado.

---

## Requisitos

- Um compilador `g++` com suporte a C++17.
  - **No Windows**, a forma recomendada e instalar o **MSYS2** (https://www.msys2.org)
    e, no terminal do MSYS2, rodar:
    `pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make`
    Depois adicione `C:\msys64\ucrt64\bin` ao PATH do Windows e reinicie o VS Code.
- Para o dashboard: apenas um navegador. A biblioteca de servidor usada e a
  `cpp-httplib` (um unico arquivo `lib/httplib.h`, licenca MIT) — ja incluida no projeto.

---

## Passo a passo para rodar (Windows + VS Code)

Abra a pasta do projeto no VS Code (File -> Open Folder) e abra o terminal
integrado (menu Terminal -> New Terminal, ou Ctrl+crase).

> IMPORTANTE: o terminal precisa estar DENTRO da pasta que contem o `Makefile`,
> o `servidor.cpp` e as pastas `src`, `web`, `include`, `lib`.
> Se ao rodar `dir` voce nao vir esses arquivos, entre na pasta certa com:
> `cd controlador-viagens`

### 1. Compilar

A forma mais simples (compila os dois modos de uma vez):

```powershell
mingw32-make
```

Isso gera `controlador.exe` (terminal) e `servidor.exe` (dashboard).

> Se `mingw32-make` nao for reconhecido, compile na mao com os dois comandos abaixo.
> (Obs.: no PowerShell, escreva cada arquivo .cpp por extenso — o coringa `src\*.cpp`
> NAO funciona no PowerShell.)
>
> ```powershell
> g++ -std=c++17 -Iinclude src\Cidade.cpp src\Trajeto.cpp src\Transporte.cpp src\Passageiro.cpp src\Viagem.cpp src\ControladorDeTransito.cpp main.cpp -o controlador.exe
> ```
> ```powershell
> g++ -std=c++17 -Iinclude -Ilib -O2 src\Cidade.cpp src\Trajeto.cpp src\Transporte.cpp src\Passageiro.cpp src\Viagem.cpp src\ControladorDeTransito.cpp servidor.cpp -o servidor.exe -lws2_32
> ```

### 2. Executar

**Modo dashboard (web):**

```powershell
.\servidor.exe
```

O terminal vai mostrar uma mensagem de "Dashboard rodando". Deixe esse terminal
ABERTO (o servidor fica rodando nele). Abra o navegador no endereco:

```
http://localhost:8080
```

Para encerrar o servidor, volte ao terminal e pressione Ctrl+C.

**Modo terminal (CLI):**

```powershell
.\controlador.exe
```

---

## Passo a passo para rodar (Linux / Mac)

```bash
make            # compila os dois modos
./controlador   # modo terminal
./servidor      # modo dashboard -> abra http://localhost:8080
```

(No Linux/Mac o servidor linka com -lpthread, ja tratado no Makefile.)

---

## Usando o dashboard

1. Com o `servidor.exe` rodando, abra **http://localhost:8080** no navegador.
2. Use os formularios da coluna esquerda para cadastrar cidades, trajetos,
   transportes e passageiros; inicie viagens; e use o "Relogio do sistema"
   para avancar as horas.
3. A coluna direita mostra o estado em tempo real (atualiza sozinha a cada 2 segundos):
   viagens em andamento, localizacao de transportes e passageiros, cidades mais
   visitadas e o mapa de trajetos.

### Sugestao de teste rapido (viagem com conexao)

1. Cadastre 4 cidades: A, X, Y, B.
2. Cadastre 3 trajetos terrestres: A->X (100 km), X->Y (100 km), Y->B (100 km).
   (Note que NAO existe trajeto direto A->B.)
3. Cadastre um transporte terrestre em A (ex: velocidade 50, capacidade 40,
   distancia entre descansos 0).
4. Cadastre passageiros em A (ex: Joao, Maria).
5. Inicie uma viagem de A para B com esse transporte e esses passageiros.
   O sistema vai rotear automaticamente A -> X -> Y -> B.
6. Avance 6 horas. O transporte e os passageiros chegam em B, e B registra 1 visita.

---

## Estrutura do projeto

```
controlador-viagens/
├── include/      # cabecalhos (.h) — uma classe por arquivo
├── src/          # implementacoes (.cpp)
├── lib/          # httplib.h (servidor HTTP, single-header, MIT)
├── web/          # dashboard (index.html)
├── dados/        # arquivos .txt de persistencia (criados ao salvar)
├── main.cpp      # interface de terminal (CLI)
├── servidor.cpp  # servidor web que expoe o controlador via HTTP
├── compilar.ps1  # (opcional) script de build para Windows
└── Makefile      # build para Linux/Mac e tambem usavel com mingw32-make no Windows
```

## As classes

- **Cidade** — no do mapa; guarda nome e contador de visitas.
- **Trajeto** — aresta entre duas cidades, com tipo (A/T) e distancia. Bidirecional.
- **Transporte** — tipo, capacidade, velocidade e regras de descanso.
- **Passageiro** — pessoa que viaja; `localAtual = nullptr` quando em transito.
- **Viagem** — um trecho direto. Encadeadas por `proxima` para rotas com conexao.
- **ControladorDeTransito** — classe central: cadastros, roteamento (Dijkstra),
  avanco do tempo e relatorios. O metodo `estadoJson()` serializa tudo para o dashboard.

## Decisoes de modelagem

- **Melhor trajeto**: algoritmo de Dijkstra, minimizando a distancia total em km,
  usando apenas trajetos do mesmo tipo do transporte (um barco so anda em trajeto
  aquatico, etc).
- **Viagens com conexao**: sem trajeto direto A->B, o sistema monta a rota completa
  e cria uma `Viagem` por trecho, encadeadas. Passageiros e transporte so sao movidos
  ao destino quando o ULTIMO trecho termina; entre os trechos ficam "em transito".
- **Simulacao de descanso**: a cada `distancia_entre_descansos` km, o transporte para
  por `tempo_de_descanso` horas, somadas ao percurso. O avanco do tempo e feito
  hora a hora, de modo que horas que sobram de um trecho ja sao aplicadas ao proximo.
- **Persistencia**: arquivos `.txt` separados por ponto e virgula na pasta `dados/`.
