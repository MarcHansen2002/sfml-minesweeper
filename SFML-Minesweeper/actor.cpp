#include "actor.h"

//Constructor
actor::actor()
{

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