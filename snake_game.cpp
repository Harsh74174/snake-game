#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

const int WIDTH = 800;
const int HEIGHT = 600;
const int SIZE = 20;

struct Segment {
    int x, y;
};

enum Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<Segment> snake;
    Direction dir;
    sf::Vector2i food;
    int score, speed;
    sf::Font font;
    sf::Text scoreText;
    sf::SoundBuffer eatBuffer, overBuffer;
    sf::Sound eatSound, overSound;

public:
    SnakeGame() : window(sf::VideoMode(WIDTH, HEIGHT), "Snake Game") {
        dir = RIGHT;
        score = 0;
        speed = 150;

        Segment head = {WIDTH / 2 / SIZE * SIZE, HEIGHT / 2 / SIZE * SIZE};
        snake.push_back(head);

        srand(time(0));
        spawnFood();

        font.loadFromFile("assets/font.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);

        eatBuffer.loadFromFile("assets/eat.wav");
        overBuffer.loadFromFile("assets/gameover.wav");
        eatSound.setBuffer(eatBuffer);
        overSound.setBuffer(overBuffer);
    }

    void spawnFood() {
        food.x = rand() % (WIDTH / SIZE) * SIZE;
        food.y = rand() % (HEIGHT / SIZE) * SIZE;
    }

    void run() {
        sf::Clock clock;
        float timer = 0;

        while (window.isOpen()) {
            float time = clock.restart().asMilliseconds();
            timer += time;

            handleInput();

            if (timer > speed) {
                update();
                timer = 0;
            }

            draw();
        }
    }

    void handleInput() {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir != DOWN)
            dir = UP;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir != UP)
            dir = DOWN;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir != RIGHT)
            dir = LEFT;
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir != LEFT)
            dir = RIGHT;
    }

    void update() {
        Segment newHead = snake[0];

        if (dir == UP) newHead.y -= SIZE;
        else if (dir == DOWN) newHead.y += SIZE;
        else if (dir == LEFT) newHead.x -= SIZE;
        else if (dir == RIGHT) newHead.x += SIZE;

        // Collision with wall
        if (newHead.x < 0 || newHead.x >= WIDTH || newHead.y < 0 || newHead.y >= HEIGHT || checkCollision(newHead)) {
            overSound.play();
            window.close(); // Game Over
        }

        snake.insert(snake.begin(), newHead);

        // Eating food
        if (newHead.x == food.x && newHead.y == food.y) {
            score += 10;
            eatSound.play();
            spawnFood();
            if (speed > 50) speed -= 5;
        } else {
            snake.pop_back();
        }
    }

    bool checkCollision(Segment head) {
        for (size_t i = 1; i < snake.size(); i++) {
            if (snake[i].x == head.x && snake[i].y == head.y)
                return true;
        }
        return false;
    }

    void draw() {
        window.clear(sf::Color::Black);

        // Draw food
        sf::RectangleShape foodRect(sf::Vector2f(SIZE, SIZE));
        foodRect.setPosition(food.x, food.y);
        foodRect.setFillColor(sf::Color::Red);
        window.draw(foodRect);

        // Draw snake
        for (auto &s : snake) {
            sf::RectangleShape rect(sf::Vector2f(SIZE - 2, SIZE - 2));
            rect.setPosition(s.x + 1, s.y + 1);
            rect.setFillColor(sf::Color::Green);
            window.draw(rect);
        }

        // Draw score
        std::stringstream ss;
        ss << "Score: " << score;
        scoreText.setString(ss.str());
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        window.display();
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}