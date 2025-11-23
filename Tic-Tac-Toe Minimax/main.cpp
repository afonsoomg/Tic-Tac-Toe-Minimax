#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <climits>

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

bool isEqual(char a, char b, char c)
{
    return (a != ' ' && a == b && b == c);
}

int checkWinner(const std::array<std::array<char,3>,3>& b)
{
    bool algumSpot = false;
    for (int r = 0; r < 3 && !algumSpot; ++r)
    {
        for (int c = 0; c < 3; ++c)
        {
            if (b[r][c] == ' ')
            {
                algumSpot = true;
                break;
            }
        }
    }

    // linhas
    for (int r = 0; r < 3; ++r)
    {
        if (isEqual(b[r][0], b[r][1], b[r][2])) return (b[r][0] == 'X') ? 1 : 2;
    }

    // colunas
    for (int c = 0; c < 3; ++c)
    {
        if (isEqual(b[0][c], b[1][c], b[2][c])) return (b[0][c] == 'X') ? 1 : 2;
    }

    // diagonais
    if (isEqual(b[0][0], b[1][1], b[2][2])) return (b[0][0] == 'X') ? 1 : 2;
    if (isEqual(b[2][0], b[1][1], b[0][2])) return (b[2][0] == 'X') ? 1 : 2;

    if (!algumSpot) return 3;
    return 0;
}

int checkWinner()
{
    return checkWinner(board);
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

    int randomSpot = randomInt(0, static_cast<int>(spotsDisponiveis.size()));
    auto pos = spotsDisponiveis[randomSpot];
    board[pos.first][pos.second] = Player;
}

std::array<int, 4> scores = { 0, 1, -1, 0 }; // nenhum vencedor, X venceu, O venceu, empate

int minimax(std::array<std::array<char, 3>, 3> board, int profundidade, bool isMaximizing, char Player)
{
    int result = checkWinner(board);
    if (result != 0)
    {
        if (result == 1) return (Player == 'X') ? 1 : -1;
        if (result == 2) return (Player == 'O') ? 1 : -1;
        return 0; // empate
    }

    if (isMaximizing)
    {
        int bestScore = INT_MIN;
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (board[r][c] == ' ')
                {
                    board[r][c] = Player; // maximizer Player
                    int score = minimax(board, profundidade + 1, false, Player);
                    board[r][c] = ' ';

                    if (score > bestScore)
                    {
                        bestScore = score;
                    }
                }
            }
        }
        return bestScore;
    }
    else
    {
        int bestScore = INT_MAX;
        char opponent = (Player == 'X') ? 'O' : 'X';
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (board[r][c] == ' ')
                {
                    board[r][c] = opponent; // minimizer opponent
                    int score = minimax(board, profundidade + 1, true, Player);
                    board[r][c] = ' ';
                    if (score < bestScore)
                    {
                        bestScore = score;
                    }
                }
            }
        }
        return bestScore;
    }
}

void bestMove(char Player)
{
    auto move = std::make_pair(-1, -1);

    if (Player == 'X')
    {
        int bestScore = INT_MIN;
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (board[r][c] == ' ')
                {
                    board[r][c] = Player;
                    int score = minimax(board, 0, false, Player);
                    board[r][c] = ' ';

                    if(score > bestScore)
                    {
                        bestScore = score;
                        move = std::make_pair(r, c);
                    }
                }
            }
        }
    }
    else // Player == 'O' -> minimizer
    {
        int bestScore = INT_MAX;
        for (int r = 0; r < 3; r++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (board[r][c] == ' ')
                {
                    board[r][c] = Player;
                    int score = minimax(board, 0, false, Player);
                    board[r][c] = ' ';

                    if(score < bestScore)
                    {
                        bestScore = score;
                        move = std::make_pair(r, c);
                    }
                }
            }
        }
    }

    if (move.first != -1 && move.second != -1)
    {
        board[move.first][move.second] = Player;
    }
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

    sf::Text resultDisplay;
    resultDisplay.setFont(font);
    resultDisplay.setCharacterSize(28);


    RenderWindow window(VideoMode(windowWidth, windowHeight), "Jogo da velha");
    window.setVerticalSyncEnabled(true);


    float larguraDaLinha = 4.f;
    float w = static_cast<float>(windowWidth) / 3.f;
    float h = static_cast<float>(windowHeight) / 3.f;

    char currentPlayer = 'X';

    bool jogoAcabou = false;
    bool useMinimax = true; // toggle for using minimax algorithm

    sf::Clock Clock;
    const sf::Time Delay = sf::milliseconds(300);

    while (window.isOpen()) 
    {
        Event e;
        while (window.pollEvent(e)) 
        {
            if (e.type == Event::Closed) window.close();

            if (e.type == Event::KeyPressed && e.key.code == Keyboard::R)
            {
                resetBoard();
                jogoAcabou = false;
                resultDisplay.setString("");
                currentPlayer = 'X';
                Clock.restart();
            }

            if (e.type == Event::KeyPressed && e.key.code == Keyboard::T)
            {
                useMinimax = !useMinimax;
                std::cout << "Minimax toggled: " << (useMinimax ? "ON" : "OFF") << "\n";
            }
            
            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
            {
                if (!jogoAcabou && currentPlayer == 'O')
                {
                    int mx = e.mouseButton.x;
                    int my = e.mouseButton.y;
                    int c = static_cast<int>(mx / w);
                    int r = static_cast<int>(my / h);
                    if (r >= 0 && r < 3 && c >= 0 && c < 3 && board[r][c] == ' ')
                    {
                        board[r][c] = 'O';
                        int result = checkWinner();
                        if (result == 1) { std::cout << "X venceu\n"; jogoAcabou = true; resultDisplay.setString("X venceu\nR para reiniciar"); resultDisplay.setFillColor(Color::Red); }
                        else if (result == 2) { std::cout << "O venceu\n"; jogoAcabou = true; resultDisplay.setString("O venceu\nR para reiniciar"); resultDisplay.setFillColor(Color::Green); }
                        else if (result == 3) { std::cout << "Empate\n"; jogoAcabou = true; resultDisplay.setString("Empate\nR para reiniciar"); resultDisplay.setFillColor(Color::Yellow); }

                        if (jogoAcabou)
                        {
                            FloatRect bounds = resultDisplay.getLocalBounds();
                            resultDisplay.setOrigin(bounds.left + bounds.width * 0.5f,
                                                    bounds.top + bounds.height * 0.5f);
                            resultDisplay.setPosition(static_cast<float>(windowWidth) * 0.5f,
                                                      static_cast<float>(windowHeight) * 0.5f);
                        }

                        if (!jogoAcabou) currentPlayer = 'X';
                    }
                }
            }

        }

        // Processa jogadas automáticas enquanto o jogo roda (AI = X)
        if (!jogoAcabou && Clock.getElapsedTime() >= Delay) 
        {
            if (currentPlayer == 'X')
            {
                if (useMinimax) bestMove(currentPlayer);
                else nextMove(currentPlayer);
                 Clock.restart();

                 int result = checkWinner();
                 if (result == 1) { std::cout << "X venceu\n"; jogoAcabou = true; resultDisplay.setString("X venceu\nR para reiniciar"); resultDisplay.setFillColor(Color::Red);} 
                 else if (result == 2) { std::cout << "O venceu\n"; jogoAcabou = true; resultDisplay.setString("O venceu\nR para reiniciar"); resultDisplay.setFillColor(Color::Green);} 
                 else if (result == 3) { std::cout << "Empate\n"; jogoAcabou = true; resultDisplay.setString("Empate\nR para reiniciar"); resultDisplay.setFillColor(Color::Yellow);} 

                 if (jogoAcabou)
                 {
                     FloatRect bounds = resultDisplay.getLocalBounds();
                     resultDisplay.setOrigin(bounds.left + bounds.width * 0.5f,
                                             bounds.top + bounds.height * 0.5f);
                     resultDisplay.setPosition(static_cast<float>(windowWidth) * 0.5f,
                                               static_cast<float>(windowHeight) * 0.5f);
                 }

                 if (!jogoAcabou) currentPlayer = 'O';
             }
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



         for (int r = 0; r < 3; ++r)
         {
             for (int c = 0; c < 3; ++c)
             {
                 auto spot = board[r][c];
                 if (spot == ' ') continue;
           
                 std::string s(1, spot);
                 text.setString(s);
                 text.setCharacterSize(96);
                 text.setStyle(Text::Bold);
                 text.setFillColor(spot == 'X' ? Color::Red : Color::Green);

                 FloatRect bounds = text.getLocalBounds();
                 text.setOrigin(bounds.left + bounds.width * 0.5f,
                                bounds.top + bounds.height * 0.5f);

                 float x = c * w + w * 0.5f;
                 float y = r * h + h * 0.5f;
                 text.setPosition(Vector2f(x, y));

                 window.draw(text); 
             }
         }

         if (jogoAcabou)
         {
             window.draw(resultDisplay);
         }

         window.display();
     }
     return 0;
 }