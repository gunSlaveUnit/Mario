// Поставь inline у методов класса.
// Закомментировать всё, что можно
// Добавить параметры по умолчанию
// Определи уровень карты
// Корректно ли сделано освобождение памяти в деструкторах?
// Подумать про многопоточность
// Возвращать ли значения по ссылкам?

#include "SFML/Graphics.hpp"
#include <sstream>
#include <cstdlib>

float offsetX = 0, offsetY = 0;
const std::string TILESET_NAME = "Images/Tileset.png";
const int HEIGHT_MAP = 17, WIDTH_MAP = 150, TILE_SIZE = 16;
std::string TileMap[HEIGHT_MAP] = {
"000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
"0                                                                                                                                                    0",
"0                                                                                    w                                                               0",
"0                   w                                  w                   w                                                                         0",
"0      w                               w                                                                                                             0",
"0                                                                                                                                                    0",
"0                      c                                                                       w                                                     0",
"0                                                                       r                                                                            0",
"0                    k                                                 rr                                                                            0",
"0                    kk                                               rrr                                                                            0",
"0               c    kckcck                                          rrrr                                                                            0",
"0                                      t0                           rrrrr                                       t0                                   0",
"0G                                     00              t0          rrrrrr            G                          00        G                          0",
"0           d    g       d             00              00         rrrrrrr                                       00                                   0",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
"PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP",
};

// Сообщения исключительно для вузовского задания
enum CODES_MESSAGE {
	CHARACTER_CHANGE_ISALIVE,
	KEY_IS_PRESSED
};

struct Message {
	int code, key, value;
};
Message message = { 0, 0, 0 };

class Character {
public:
	Character(int XNumberTileToSpawn, int YNumberTileToSpawn, float travelSpeed) {
		coordinates = sf::FloatRect(XNumberTileToSpawn * TILE_SIZE,
			YNumberTileToSpawn * TILE_SIZE,
			TILE_SIZE, TILE_SIZE);
		texture = new sf::Texture;
		sprite = new sf::Sprite;
		texture->loadFromFile(TILESET_NAME);
		sprite->setTexture(*texture);
		dx = dy = travelSpeed;
		currentFrame = 0;
		onGround = true;
		isAlive = true;
	}
	virtual void updateSpritePosition(float timeForAnimationSpeed) = 0;
	sf::Sprite& getSprite() { return *sprite; }
	sf::FloatRect& getCoordinates() { return coordinates; }
	float getDX() { return dx; }
	float getDY() { return dy; }
	bool getOnGround() { return onGround; }
	bool getIsAlive() { return isAlive; }
	void setDX(float value) { dx = value; }
	void setDY(float value) { dy = value; }
	void setOnGround(bool value) { onGround = value; }
	virtual void eventHandler(Message& messsage) = 0;
	virtual ~Character() {
		delete texture;
		delete sprite;
	}
protected:
	virtual void collisionWithTiles(int direction) = 0;
	void setIsAlive(bool value) { isAlive = value; }
	sf::FloatRect coordinates;
	sf::Texture* texture;
	sf::Sprite* sprite;
	float currentFrame;
	float dx, dy;
	bool onGround;
	bool isAlive;
};

class Player : public Character {
public:
	Player(int XNumberTileToSpawn, int YNumberTileToSpawn, float travelSpeed, int amountLivesAfterReload)
		: Character(XNumberTileToSpawn, YNumberTileToSpawn, travelSpeed) {
		livesAmount = amountLivesAfterReload;
		scoresAmount = 0;
		offsetX = offsetY = 0;
	}
	virtual void updateSpritePosition(float timeForAnimationSpeed) override {
		coordinates.left += dx * timeForAnimationSpeed;
		collisionWithTiles(0);
		if (!onGround) dy += 0.0005 * timeForAnimationSpeed;
		coordinates.top += dy * timeForAnimationSpeed;
		onGround = false;
		collisionWithTiles(1);
		currentFrame += timeForAnimationSpeed * 0.005;
		if (currentFrame > 3) currentFrame -= 3;
		if (dx > 0) sprite->setTextureRect(sf::IntRect(112 + 31 * int(currentFrame), 144, TILE_SIZE, TILE_SIZE));
		if (dx < 0) sprite->setTextureRect(sf::IntRect(112 + 31 * int(currentFrame) + TILE_SIZE, 144, -TILE_SIZE, TILE_SIZE));
		sprite->setPosition(coordinates.left - offsetX, coordinates.top - offsetY);
		dx = 0;
	}
	virtual void eventHandler(Message& message) override {
		switch (message.code) {
		case 0:
			this->setIsAlive(false);
			break;
		case 1:
			switch (message.key) {
			case 'L':
				this->setDX(-0.1);
				break;
			case 'R':
				this->setDX(0.1);
				break;
			}
			case 'U':
				if (this->getOnGround()) {
					this->setDY(-0.27); 
					this->setOnGround(false);
				}
				break;
		}
	}
	int& getLivesAmount() { return livesAmount; }
	int& getScoresAmount() { return scoresAmount; }
	void changeLivesAmount(int valueToAddtion) { livesAmount += valueToAddtion; }
	void changeScoresAmount(int valueToAddtion) { scoresAmount += valueToAddtion; }
	~Player() {}
private:
	virtual void collisionWithTiles(int direction) override {
		for (int i = coordinates.top / TILE_SIZE; i < (coordinates.top + coordinates.height) / TILE_SIZE; ++i)
			for (int j = coordinates.left / TILE_SIZE; j < (coordinates.left + coordinates.width) / TILE_SIZE; ++j)
			{
				if ((TileMap[i][j] == 'P') || (TileMap[i][j] == 'k') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r') || (TileMap[i][j] == 't'))
				{
					if (dy > 0 && direction == 1) { coordinates.top = i * TILE_SIZE - coordinates.height;  dy = 0;   onGround = true; }
					if (dy < 0 && direction == 1) { coordinates.top = i * TILE_SIZE + TILE_SIZE;   dy = 0; }
					if (dx > 0 && direction == 0) coordinates.left = j * TILE_SIZE - coordinates.width;
					if (dx < 0 && direction == 0) coordinates.left = j * TILE_SIZE + TILE_SIZE;
				}

				if (TileMap[i][j] == 'c' && dy < 0) { TileMap[i][j] = ' '; dy = 0.1; }
			}
	}
	int livesAmount;
	int scoresAmount;
};

class Enemy : public Character {
public:
	Enemy(int XNumberTileToSpawn, int YNumberTileToSpawn, float travelSpeed)
		: Character(XNumberTileToSpawn, YNumberTileToSpawn, travelSpeed) {}
	virtual void eventHandler(Message& message) override {
		switch (message.code) {
		case 0:
			this->setIsAlive(false);
			break;
		}
	}
	~Enemy() {}
};

class Goomba : public Enemy {
public:
	Goomba(int XNumberTileToSpawn, int YNumberTileToSpawn, float travelSpeed)
		: Enemy(XNumberTileToSpawn, YNumberTileToSpawn, travelSpeed) {}
	virtual void updateSpritePosition(float timeForAnimationSpeed) override {
		coordinates.left += dx * timeForAnimationSpeed;
		collisionWithTiles(0);

		currentFrame += timeForAnimationSpeed * 0.005;
		if (currentFrame > 2) currentFrame -= 2;

		sprite->setTextureRect(sf::IntRect(18 * int(currentFrame), 0, TILE_SIZE, TILE_SIZE));
		if (!isAlive) sprite->setTextureRect(sf::IntRect(58, 0, TILE_SIZE, TILE_SIZE));

		sprite->setPosition(coordinates.left - offsetX, coordinates.top - offsetY);
	}
	~Goomba() {}
private:
	virtual void collisionWithTiles(int direction) override {
		for (int i = coordinates.top / 16; i < (coordinates.top + coordinates.height) / 16; i++)
			for (int j = coordinates.left / 16; j < (coordinates.left + coordinates.width) / 16; j++)
				if ((TileMap[i][j] == 'P') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r')) {
					if (dx > 0) { coordinates.left = j * 16 - coordinates.width; dx *= -1; }
					else if (dx < 0) { coordinates.left = j * 16 + 16;  dx *= -1; }
				}
	}
};

class KoopaTroopa : public Enemy {
public:
	KoopaTroopa(int XNumberTileToSpawn, int YNumberTileToSpawn, float travelSpeed)
		: Enemy(XNumberTileToSpawn, YNumberTileToSpawn, travelSpeed) {}
	virtual void updateSpritePosition(float timeForAnimationSpeed) override {
		coordinates.left += dx * timeForAnimationSpeed;
		collisionWithTiles(0);

		currentFrame += timeForAnimationSpeed * 0.005;
		if (currentFrame > 2) currentFrame -= 2;

		sprite->setTextureRect(sf::IntRect(18 * int(currentFrame), 0, TILE_SIZE, TILE_SIZE));
		if (!isAlive) sprite->setTextureRect(sf::IntRect(58, 0, TILE_SIZE, TILE_SIZE));

		sprite->setPosition(coordinates.left - offsetX, coordinates.top - offsetY);
	}
	~KoopaTroopa() {}
private:
	virtual void collisionWithTiles(int direction) override {
		for (int i = coordinates.top / 16; i < (coordinates.top + coordinates.height) / 16; i++)
			for (int j = coordinates.left / 16; j < (coordinates.left + coordinates.width) / 16; j++)
				if ((TileMap[i][j] == 'P') || (TileMap[i][j] == '0') || (TileMap[i][j] == 'r')) {
					if (dx > 0) { coordinates.left = j * 16 - coordinates.width; dx *= -1; }
					else if (dx < 0) { coordinates.left = j * 16 + 16;  dx *= -1; }
				}
	}
};

class Map {
public:
	Map() {
		restart();
	}
	void update(float time) {
		if (timeTillStart > 0) {
			timeTillStart -= time;
			return;
		}
		mario->updateSpritePosition(time);
		for (int i = 0; i < 3; ++i)
			enemies[i]->updateSpritePosition(time);
	}
	void calculateOffsets(int widthScreen, int heightScreen) {
		if (mario->getCoordinates().left > widthScreen / 2) offsetX = mario->getCoordinates().left - widthScreen / 2;
	}
	void colMario() { // Проверить то, столкнулся ли Марио с противником
		for (int i = 0; i < 3; ++i) // Проверяем столкновение циклом со вееми противниками
			if (mario->getCoordinates().intersects(enemies[i]->getCoordinates())) // Если два спрайта - Марио и врага - пересеклись (intersects - SFML функция)
				if (enemies[i]->getIsAlive()) { // Если враг жив
					// Тут Марио его убивает
					if (mario->getDY() > 0) { // Если Марио в этом момент падал на врага
						enemies[i]->setDX(0); // Враг больше не перемещается
						mario->setDY(-0.2); // Марио чуть подлетает
						message.code = CHARACTER_CHANGE_ISALIVE; // Меняем через сообщение состояние противника с живого на неживого
						enemies[i]->eventHandler(message);       //
						score += 10; // +10 очков Марио
					}
					// А тут умирает Марио
					else {
						if (mario->getLivesAmount() <= 0) { // Если же столкновение произошло, но Марио в этом момент не падал на него
							message.code = CHARACTER_CHANGE_ISALIVE; // То ставим у Марио, что он погиб в бою
							mario->eventHandler(message);            // 
							return;
						}
						reload(); // И запускаем игру "заново", расставляя обратно Марио и противников
					}
				}
	}
	void draw(sf::RenderWindow& window) {
		if (!mario->getIsAlive()) {
			window.clear(sf::Color::Black);

			sf::Text gameOver("Game over! Press <Enter> to restart", *font, 20);
			gameOver.setPosition(window.getSize().x / 2 - gameOver.getGlobalBounds().width / 2, window.getSize().y / 2);

			window.draw(gameOver);
			return;
		}

		if (timeTillStart > 0) {
			window.clear(sf::Color::Black);

			sf::Text lives(std::to_string(mario->getLivesAmount()), *font, 30);
			sf::Text scores(std::to_string(score), *font, 30);

			lives.setPosition(window.getSize().x / 2 - 30, window.getSize().y / 2);
			scores.setPosition(window.getSize().x / 2 + 20, window.getSize().y / 2);

			window.draw(lives);
			window.draw(scores);
			return;
		}

		for (int i = 0; i < HEIGHT_MAP; ++i)
			for (int j = 0; j < WIDTH_MAP; ++j)
			{
				if (TileMap[i][j] == 'P')  sprite->setTextureRect(sf::IntRect(143 - 16 * 3, 112, 16, 16));
				if (TileMap[i][j] == 'k')  sprite->setTextureRect(sf::IntRect(143, 112, 16, 16));
				if (TileMap[i][j] == 'c')  sprite->setTextureRect(sf::IntRect(143 - 16, 112, 16, 16));
				if (TileMap[i][j] == 't')  sprite->setTextureRect(sf::IntRect(0, 47, 32, 95 - 47));
				if (TileMap[i][j] == 'g')  sprite->setTextureRect(sf::IntRect(0, 16 * 9 - 5, 3 * 16, 16 * 2 + 5));
				if (TileMap[i][j] == 'G')  sprite->setTextureRect(sf::IntRect(145, 222, 222 - 145, 255 - 222));
				if (TileMap[i][j] == 'd')  sprite->setTextureRect(sf::IntRect(0, 106, 74, 127 - 106));
				if (TileMap[i][j] == 'w')  sprite->setTextureRect(sf::IntRect(99, 224, 140 - 99, 255 - 224));
				if (TileMap[i][j] == 'r')  sprite->setTextureRect(sf::IntRect(143 - 32, 112, 16, 16));
				if ((TileMap[i][j] == ' ') || (TileMap[i][j] == '0')) continue;

				sprite->setPosition(j * TILE_SIZE - offsetX, i * TILE_SIZE - offsetY);
				window.draw(*sprite);
			}

		std::string textLivesAmount = std::to_string(mario->getLivesAmount());
		std::string textScoresAmount = std::to_string(score);

		livesAmountPlayer->setString(textLivesAmount);
		scoresAmountPlayer->setString(textScoresAmount);
		window.draw(*livesAmountPlayer);
		window.draw(*scoresAmountPlayer);

		window.draw(mario->getSprite());
		for (int i = 0; i < 3; ++i) window.draw(enemies[i]->getSprite());
	}
	void restart() {
		score = 0;
		amountLivesForUserAfterReload = 3;
		reload();
	}
	void reload() {
		timeTillStart = 2000;

		enemies[0] = new Goomba(30, 13, 0.05);
		enemies[1] = new Goomba(50, 13, 0.05);
		enemies[2] = new KoopaTroopa(50, 13, -0.05);

		mario = new Player(1, 13, 0.1, amountLivesForUserAfterReload--);

		texture = new sf::Texture();
		texture->loadFromFile(TILESET_NAME);
		sprite = new sf::Sprite(*texture);

		font = new sf::Font;
		font->loadFromFile(TYPE_FILE_NAME);

		std::string textLivesAmount = std::to_string(mario->getLivesAmount());
		std::string textScoresAmount = std::to_string(mario->getScoresAmount());

		livesAmountPlayer = new sf::Text(textLivesAmount, *font, 30);
		scoresAmountPlayer = new sf::Text(textScoresAmount, *font, 30);

		livesAmountPlayer->setPosition(10, 10);
		scoresAmountPlayer->setPosition(40, 10);
	}
	void eventHandler(Message& message) {
		switch (message.code) {
		case 1:
			switch (message.key) {
			case 'E':
				this->restart();
				break;
			}
			break;
		}
	}
	Player* getPlayerPointer() { return mario; }
private:
	Enemy* enemies[3];
	Player* mario;
	sf::Texture* texture;
	sf::Sprite* sprite;
	std::string TYPE_FILE_NAME = "Types/Sansation.ttf";
	sf::Font* font;
	sf::Text* livesAmountPlayer;
	sf::Text* scoresAmountPlayer;

	int amountLivesForUserAfterReload;
	int score;
	float timeTillStart;
};

int main() {
	const int WINDOW_WIDTH = 400, WINDOW_HEIGHT = 250;
	const std::string WINDOW_TITLE = "MARIO_0.6";

	sf::RenderWindow mainWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);

	Map map;

	sf::Clock clock;

	while (mainWindow.isOpen()) {
		float time = clock.getElapsedTime().asMicroseconds();
		clock.restart();

		time /= 500;
		if (time > 20) time = 20;

		sf::Event event;
		while (mainWindow.pollEvent(event)) if (event.type == sf::Event::Closed) mainWindow.close();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) map.getPlayerPointer()->setDX(-0.1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) map.getPlayerPointer()->setDX(0.1);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && map.getPlayerPointer()->getOnGround()) {
			map.getPlayerPointer()->setDY(-0.27); map.getPlayerPointer()->setOnGround(false);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !map.getPlayerPointer()->getIsAlive())
		{
			message.code = KEY_IS_PRESSED;
			message.key = 'E';
			map.eventHandler(message);
		}

		map.update(time);

		map.colMario();

		map.calculateOffsets(WINDOW_WIDTH, WINDOW_HEIGHT);

		mainWindow.clear(sf::Color(107, 140, 255));

		map.draw(mainWindow);

		mainWindow.display();
	}
}
