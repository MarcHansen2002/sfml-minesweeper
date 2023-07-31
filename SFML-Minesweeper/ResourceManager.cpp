#include "ResourceManager.h"

sf::Texture* ResourceManager::GetTexture(std::string filePath)
{
	//Check if texture already exists
	if (textures.find(filePath) != textures.end())
	{
		return textures.at(filePath);
	}
	//Load texture if not
	else
	{
		sf::Texture* texture = new sf::Texture();
		//If texture doesn't load properly
		if (!texture->loadFromFile(filePath))
		{
			//Load missing texture
			texture->loadFromFile(missingTexture);
		}
		textures[filePath] = texture;
		return textures[filePath];
	}
}
void ResourceManager::ClearTextures()
{
	//Delete all pointers
	for (auto element : textures)
	{
		delete element.second;
	}
	//Clear texture map
	textures.clear();
}

sf::SoundBuffer* ResourceManager::GetSound(std::string filePath)
{
	if (sounds.find(filePath) != sounds.end())
	{
		return sounds.at(filePath);
	}
	else
	{
		sf::SoundBuffer* sound = new sf::SoundBuffer();
		//If audio doesn't load properly
		if (!sound->loadFromFile(filePath))
		{
			//Load missing sound
			sound->loadFromFile(missingSound);
		}
		sounds[filePath] = sound;
		return sounds[filePath];
	}
}
void ResourceManager::ClearSounds()
{
	//Delete all pointers
	for (auto element : sounds)
	{
		delete element.second;
	}
	//Clear audio map
	sounds.clear();
}

sf::Font* ResourceManager::GetFont(std::string filePath)
{
	//Check if font already exists
	if (fonts.find(filePath) != fonts.end())
	{
		return fonts.at(filePath);
	}
	//Load font if not
	else
	{
		sf::Font* font = new sf::Font();
		//If font doesn't load properly
		if (!font->loadFromFile(filePath))
		{
			//Load missing font
		}
		fonts[filePath] = font;
		return fonts[filePath];
	}
}
void ResourceManager::ClearFonts()
{
	//Delete all pointers
	for (auto element : fonts)
	{
		delete element.second;
	}
	//Clear font map
	fonts.clear();
}

void ResourceManager::PlaySound(std::string filePath)
{
	currentSound.setBuffer(*GetSound(filePath));
	currentSound.play();
}