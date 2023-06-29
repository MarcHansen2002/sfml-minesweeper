#include "actor.h"
#include <assert.h>
#include <iostream>

//Constructor
actor::actor()
{

}
//Initialise actor after the constructor
void actor::Init()
{
	LoadTexture(texturePath, texture);
	sprite.setTexture(texture);

	location = { 500, 100 };

	textRect = sf::IntRect(((sheetData.index - 1) % sheetData.columns) * texture.getSize().x / sheetData.columns, ((sheetData.index - 1) / sheetData.columns) * texture.getSize().y / sheetData.rows, texture.getSize().x / sheetData.columns, texture.getSize().y / sheetData.rows);
	sprite.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
	sprite.setTextureRect(textRect);

	posRect.left = location.x - textRect.width / 2.f;
	posRect.width = location.x + textRect.width / 2.f;
	posRect.top = location.y - textRect.height / 2.f;
	posRect.height = location.y + textRect.height / 2.f;
}
//When this actor is clicked
void actor::onClick()
{

}
void actor::CheckCollisions()
{
	if (IsMouseColliding(*this, *windowRef))
	{
		std::cout << "HOVER";
	}
}
void actor::Update(float elapsed)
{
	sprite.setPosition(location);
	sprite.setScale(scale);

}

tile::tile()
{
	texturePath = "../Assets/SweeperSheet.png";
	scale = { 5, 5 };
	sheetData.columns = 3;
	sheetData.rows = 4;
}

void tile::Update(float elapsed)
{
	actor::Update(elapsed);
	if (revealed)
	{
		sheetData.index = id;
	}

}

bool LoadTexture(const sf::String& file, sf::Texture& texture)
{
	if (texture.loadFromFile(file))
	{
		return true;
	}
	else if (texture.loadFromFile("../Assets/MissingTexture.png"))
	{
		return false;
	}
	assert(false);
	return false;
}

sf::FloatRect actor::GetRectCollision()
{
	sf::IntRect TextRect = sprite.getTextureRect();
	sf::FloatRect PositionalRect = { location.x - (TextRect.width / 2.f), //Left
		location.y - (TextRect.height / 2.f), //Top
		location.x + (TextRect.width / 2.f), //Right
		location.y + (TextRect.height / 2.f) }; //Bottom
	return PositionalRect;
}

	
bool IsMouseColliding(actor& actor, sf::RenderWindow& window)
{
	sf::FloatRect actorPos = actor.GetRectCollision();
	sf::Vector2i mousePos = sf::Mouse::getPosition();
	if (mousePos.x < actorPos.left)
	{
		std::cout << "too far left\n";
		return false;
	}
	if (mousePos.x > actorPos.width)
	{
		std::cout << "too far right\n";
		return false;
	}
	if (mousePos.y < actorPos.top)
	{
		std::cout << "too far up\n";
		return false;
	}
	if (mousePos.y > actorPos.top)
	{
		std::cout << "too far down\n";
		return false;
	}
	return true;

}

void actor::DisplayHitbox(sf::RenderWindow& window)
{
	sf::RectangleShape box;
	sf::FloatRect rect = GetRectCollision();
	box.setOrigin(0, 0);
	box.setOutlineColor(sf::Color::Red);
	box.setPosition(rect.left, rect.top);
	box.setSize({ (float)textRect.width, (float)textRect.height });
	box.setOutlineThickness(5);
	window.draw(box);
	
}