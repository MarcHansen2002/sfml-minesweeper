#include "actor.h"
#include <assert.h>

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
}
//When this actor is clicked
void actor::onClick()
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