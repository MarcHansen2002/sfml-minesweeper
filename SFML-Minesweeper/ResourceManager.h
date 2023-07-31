#pragma once
//SFML
#include <SFML/Graphics.hpp>
#include "SFML/Audio.hpp"
//Other Libs
#include <assert.h>
//My Files
#include "actor.h"
#include "database.h"

class ResourceManager
{
public:
	const std::string assetPath = "../Assets/";

	//Textures
	sf::Texture* GetTexture(std::string filePath);
	void ClearTextures();

	std::map<std::string, sf::Texture*> textures;
	sf::String missingTexture = "MissingTexture.png";

	//Sound
	sf::SoundBuffer* GetSound(std::string filePath);
	void ClearSounds();
	void PlaySound(std::string filePath);

	std::map<std::string, sf::SoundBuffer*> sounds;
	sf::Sound currentSound;
	sf::String missingSound = "MissingSound.wav";

	//Fonts
	sf::Font* GetFont(std::string filePath);
	void ClearFonts();
	std::map<std::string, sf::Font*> fonts;
};