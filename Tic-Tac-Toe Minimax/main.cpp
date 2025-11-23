#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <random>
#include <chrono>
#include <vector>

using namespace sf;

std::array<std::array<char,3>,3> board = 
{{
    {{' ', ' ', ' '}},
    {{' ', ' ', ' '}},
    {{' ', ' ', ' '}}
}};

const char* player1 = "X";
const char* player2 = "O";

int randomInt(int min, int max)
{
    static std::mt19937 mt(static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(min, max - 1);
    return dist(mt);
}

void resetBoard()
{
    for (auto& row : board)
    {
        for (auto& cell : row)
        {
            cell = ' ';
        }
    }
}

int checkWinner()
{
    // verifica se existe qualquer célula vazia
    bool algumSpot = false;
    for (int linha = 0; linha < 3 && !algumSpot; ++linha)
        for (int coluna = 0; coluna < 3; ++coluna)
            if (board[linha][coluna] == ' ') { algumSpot = true; break; }

    // verifica linhas
    for (int linha = 0; linha < 3; ++linha) {
        if (board[linha][0] != ' ' &&
            board[linha][0] == board[linha][1] &&
            board[linha][1] == board[linha][2]) {
            return (board[linha][0] == 'X') ? 1 : 2;
        }
    }

    // verifica colunas
    for (int coluna = 0; coluna < 3; ++coluna) {
        if (board[0][coluna] != ' ' &&
            board[0][coluna] == board[1][coluna] &&
            board[1][coluna] == board[2][coluna]) {
            return (board[0][coluna] == 'X') ? 1 : 2;
        }
    }

    // verifica diagonais
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return (board[0][0] == 'X') ? 1 : 2;
    if (board[2][0] != ' ' && board[2][0] == board[1][1] && board[1][1] == board[0][2])
        return (board[2][0] == 'X') ? 1 : 2;

    if (!algumSpot) return 3; // empate
    return 0; // sem resultado
}

void nextMove(char Player)
{
    if (Player != 'X' && Player != 'O') return;

    std::vector<std::pair<int,int>> spotsDisponiveis;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board[r][c] == ' ')
                spotsDisponiveis.emplace_back(r,c);

    if (spotsDisponiveis.empty()) return;

    int randomSpot = randomInt(0, static_cast<int>(spotsDisponiveis.size())); // 0 .. size-1
    auto pos = spotsDisponiveis[randomSpot];
    board[pos.first][pos.second] = Player;
}


int main()
{
    const int windowWidth = 400;
    const int windowHeight = 400;

    sf::Font font;
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
    {
        std::cerr << "Warning: failed to load font\n";
    }

    sf::Text text;
    text.setFont(font);

    RenderWindow window(VideoMode(windowWidth, windowHeight), "Jogo da velha");
    window.setVerticalSyncEnabled(true);


    float larguraDaLinha = 4.f;
    float w = static_cast<float>(windowWidth) / 3.f;
    float h = static_cast<float>(windowHeight) / 3.f;

    char currentPlayer = 'X';

    bool jogoAcabou = false;
    sf::Clock aiClock;
    const sf::Time aiDelay = sf::milliseconds(300); // evita jogadas instantâneas

    while (window.isOpen()) {
        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) window.close();
			// Fazer um reset do jogo ao pressionar R para evitar reiniciar o programa
        }

        // Processa jogadas automáticas enquanto o jogo roda
        if (!jogoAcabou && aiClock.getElapsedTime() >= aiDelay) {
            // alterna automaticamente entre X/O
            nextMove(currentPlayer);
            aiClock.restart();

            int result = checkWinner();
            if (result == 1) { std::cout << "X venceu\n"; jogoAcabou = true; }
            else if (result == 2) { std::cout << "O venceu\n"; jogoAcabou = true; }
            else if (result == 3) { std::cout << "Empate\n"; jogoAcabou = true; }

            // alterna jogador
            if (!jogoAcabou) currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }

        window.clear(Color::Black);

        RectangleShape line;
        // Desenho das linhas Verticais
        line.setSize(Vector2f(larguraDaLinha, static_cast<float>(windowHeight)));
        line.setFillColor(Color::White);
        line.setPosition(Vector2f(w - larguraDaLinha * 0.5f, 0.f));
        window.draw(line);
        line.setPosition(Vector2f(2.f * w - larguraDaLinha * 0.5f, 0.f));
        window.draw(line);

        // Desenhos das linhas Horizontais
        line.setSize(Vector2f(static_cast<float>(windowWidth), larguraDaLinha));
        line.setPosition(Vector2f(0.f, h - larguraDaLinha * 0.5f));
        window.draw(line);
        line.setPosition(Vector2f(0.f, 2.f * h - larguraDaLinha * 0.5f));
        window.draw(line);



        for (int linha = 0; linha < 3; ++linha)
        {
            for (int coluna = 0; coluna < 3; ++coluna)
            {
                auto spot = board[linha][coluna];
                if (spot == ' ') continue;
          
                std::string s(1, spot);
			text.setString(s);
                text.setCharacterSize(96);
			text.setStyle(Text::Bold);
                text.setFillColor(spot == 'X' ? Color::Red : Color::Green);

			FloatRect bounds = text.getLocalBounds();
                text.setOrigin(bounds.left + bounds.width * 0.5f,
				       bounds.top + bounds.height * 0.5f);

                float x = coluna * w + w * 0.5f;
                float y = linha * h + h * 0.5f;
			text.setPosition(Vector2f(x, y));

                window.draw(text); 
            }
		}

        window.display();
    }
    return 0;
}