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
	std::cout << "init";
	LoadTexture(texturePath, texture);
	sprite.setTexture(texture);

	UpdateSprite();
	
}
void actor::Render(sf::RenderWindow& window)
{
	window.draw(sprite);
}
void actor::UpdateSprite()
{
	textRect = sf::IntRect(((sheetData.index - 1) % sheetData.columns) * texture.getSize().x / sheetData.columns, ((sheetData.index - 1) / sheetData.columns) * texture.getSize().y / sheetData.rows, texture.getSize().x / sheetData.columns, texture.getSize().y / sheetData.rows);
	sprite.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
	sprite.setTextureRect(textRect);

	posRect.left = location.x - textRect.width / 2.f;
	posRect.width = location.x + textRect.width / 2.f;
	posRect.top = location.y - textRect.height / 2.f;
	posRect.height = location.y + textRect.height / 2.f;
}
//When this actor is clicked
void actor::onLeftClick()
{

}
void actor::onRightClick()
{
	std::cout << "test";
}
void actor::CheckCollisions(sf::RenderWindow& window)
{
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

	revealed = false;
	flagged = false;
}

void tile::Update(float elapsed)
{
	actor::Update(elapsed);
	if (revealed)
	{
		sheetData.index = id;
	}
	else if (flagged)
	{
		sheetData.index = 10;
	}
	else
	{
		sheetData.index = 1;
	}
	UpdateSprite();

}
void tile::onLeftClick()
{
	if (!flagged && !revealed)
	{
		revealed = true;
		if (id == 11)
		{
			//End Game, mine clicked
		}
	}
}
void tile::onRightClick()
{
	std::cout << "flagged";
	flagged = !flagged;
}



sf::FloatRect actor::GetRectCollision()
{
	sf::IntRect TextRect = sprite.getTextureRect();
	sf::FloatRect PositionalRect = { location.x - ((TextRect.width / 2.f) * scale.x), //Left
		location.y - ((TextRect.height / 2.f) * scale.y), //Top
		((float)TextRect.width) * scale.x, //Right
		((float)TextRect.height) * scale.y }; //Bottom
	return PositionalRect;
}

	
bool IsMouseColliding(actor& actor, sf::RenderWindow& window)
{
	sf::FloatRect actorPos = actor.GetRectCollision();
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (mousePos.x < actorPos.left)
	{
		return false;
	}
	if (mousePos.x > actorPos.width + actorPos.left)
	{
		return false;
	}
	if (mousePos.y < actorPos.top)
	{
		return false;
	}
	if (mousePos.y > actorPos.top + actorPos.height)
	{
		return false;
	}
	return true;

}

void actor::DisplayHitbox(sf::RenderWindow& window)
{
	sf::RectangleShape box;
	sf::FloatRect rect = GetRectCollision();
	box.setOrigin(0, 0);
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Red);
	box.setPosition(rect.left, rect.top);
	box.setSize({ (float)rect.width, (float)rect.height });
	box.setOutlineThickness(5);
	window.draw(box);
	
}
void actor::DisplayTextureRect(sf::RenderWindow& window)
{
	sf::RectangleShape box;
	sf::IntRect rect = this->sprite.getTextureRect();
	box.setOrigin(0, 0);
	box.setFillColor(sf::Color::Transparent);
	box.setOutlineColor(sf::Color::Blue);
	box.setPosition(location.x, location.y);
	box.setSize({ (float)rect.width, (float)rect.height });
	box.setOutlineThickness(5);
	window.draw(box);
}


bool LoadTexture(const sf::String& file, sf::Texture& texture)
{
	if (texture.loadFromFile(file))
	{
		return true;
	}
	assert(false);
	return false;
}