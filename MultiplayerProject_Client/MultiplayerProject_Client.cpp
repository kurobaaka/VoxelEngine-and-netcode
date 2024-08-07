#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include "NetworkClient.h"

using namespace sf;
using namespace std;

class Player
{
    Text t;
    Sprite body;
    bool possesed = false;
public:
    string name;

    Player(bool possesed = false) : possesed(possesed) {};

    void load(Texture& texture, Font& font)
    {
        body.setTexture(texture);
        body.setTextureRect(IntRect(0, 0, texture.getSize().x / 4, texture.getSize().y / 4));
        body.setScale(2, 2);
        if (!possesed) body.setColor(Color::Red);

        t.setFont(font);
        t.setString(name);
        t.setFillColor(sf::Color::Red);
        t.setPosition(body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
    }

    void setPosition(Vector2f newPos)
    {
        body.setPosition(newPos);
        t.setPosition(newPos.x + body.getGlobalBounds().width / 2 - t.getGlobalBounds().width / 2, body.getPosition().y - t.getGlobalBounds().height);
    }

    void move(Vector2f normalizedMovementVec, Time cycleTime)
    {
        body.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });
        t.move({ normalizedMovementVec.x * 50 * cycleTime.asSeconds(), normalizedMovementVec.y * 50 * cycleTime.asSeconds() });
    }

    void draw(RenderWindow& window)
    {
        window.draw(body);
        window.draw(t);
    }

    bool isPossesed() const { return possesed; }
    Vector2f getPos() const { return body.getPosition(); }
};

vector<Player> playersVec;

Clock cycleTimer;
Time cycleTime;

IpAddress S_Ip;
unsigned short S_port;
string clientName;

NetworkClient netC;

Player player(true);

void getUserInputData(string& playerName);
void addPlayer(Texture& t_player, Font& font, string clientName);

void drawPlane()
{
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glEnd();
}

void drawCube(float x, float y, float z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glBegin(GL_QUADS);

    // Front face
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    // Back face
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    // Left face
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Right face
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);

    // Top face
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    // Bottom face
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);

    glEnd();
    glPopMatrix();
}

void drawPlayerName(RenderWindow& window, const Player& p, Font& font)
{
    Text nameText;
    nameText.setFont(font);
    nameText.setString(p.name);
    nameText.setFillColor(Color::White);
    nameText.setCharacterSize(24);
    nameText.setPosition(p.getPos().x, 2.0f); // Установите Y на 2.0f или выше
    window.draw(nameText);
}

int main()
{
    cout << "Starting application..." << endl;

    RenderWindow window(sf::VideoMode(800, 600), "Client works!");

    Texture t_player;
    if (!t_player.loadFromFile("indianajones.png"))
    {
        cerr << "Error loading texture!" << endl;
        return -1;
    }

    Font font;
    if (!font.loadFromFile("8bitOperatorPlus-Regular.ttf"))
    {
        cerr << "Error loading font!" << endl;
        return -1;
    }

    getUserInputData(player.name);
    player.load(t_player, font);

    netC.init();
    netC.registerOnServer(S_Ip, S_port, player.name);

    vector<string> namesVec;
    netC.receiveConnectedClientsNames(namesVec);
    for (const auto& name : namesVec)
    {
        addPlayer(t_player, font, name);
    }

    Packet receivedDataPacket;
    Packet sendDataPacket;

    window.setActive(true);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);

    while (window.isOpen())
    {
        cycleTime = cycleTimer.restart();

        // Обработка событий
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
        }

        // Обработка сетевых событий
        if (netC.receiveData(receivedDataPacket, S_Ip, S_port) == Socket::Status::Done)
        {
            if (receivedDataPacket.getDataSize() > 0)
            {
                string s;
                if (receivedDataPacket >> s)
                {
                    if (s == "DATA")
                    {
                        while (!receivedDataPacket.endOfPacket())
                        {
                            float x, y;
                            receivedDataPacket >> s >> x >> y;

                            // Проверка, существует ли игрок в векторе
                            bool found = false;
                            for (auto& p : playersVec)
                            {
                                if (p.name == s)
                                {
                                    p.setPosition({ x, y });
                                    found = true;
                                    break;
                                }
                            }

                            // Если игрок не найден, добавляем его
                            if (!found)
                            {
                                Player newPlayer;
                                newPlayer.name = s;
                                newPlayer.setPosition({ x, y });
                                newPlayer.load(t_player, font);
                                playersVec.push_back(newPlayer);
                            }
                        }
                    }
                }
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();

        // Установка позиции камеры
        gluLookAt(0.0f, 1.0f, 5.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f);

        drawPlane();

        // Отрисовка игрока
        player.draw(window);
        for (const auto& p : playersVec)
        {
            drawCube(p.getPos().x, 0.5f, p.getPos().y);
            drawPlayerName(window, p, font);
        }

        window.display();
    }

    return 0;
}

void getUserInputData(string& playerName)
{
    S_Ip = "localhost"; // Example IP
    cout << "Enter server registration port: ";
    cin >> S_port;
    cout << "Enter name: ";
    cin >> playerName;
}

void addPlayer(Texture& t_player, Font& font, string clientName)
{
    Player p;
    playersVec.push_back(p);
    playersVec.back().name = clientName;
    playersVec.back().load(t_player, font);
}