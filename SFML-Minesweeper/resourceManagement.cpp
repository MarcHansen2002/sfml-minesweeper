#include "resourceManagement.h"


sf::Texture LoadTexture(const sf::String& file)
{
	sf::Texture texture;
	if (texture.loadFromFile(file))
	{
		return texture;
	}
	else if (texture.loadFromFile("../Assets/MissingTexture.png"))
	{
		return texture;
	}
	assert(false);
	return texture;
}

sf::Texture& resourceManagement::GetTexture(sf::String texture)
{
	bool found = false;
	for (int i = 0; i < textures.size(); i++)
	{
		if (texture == textureLocations[i])
		{
			return textures[i];
		}
	}

	textures.push_back(LoadTexture(texture));
	textureLocations.push_back(texture);
}

