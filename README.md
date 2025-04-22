# High Ground Engine

This is a 2D game engine written in C++, with scripting available in Lua.

Making a game requires creating a resources folder at the base of the engine.

	.resources
	├── actor_templates
	│   └── Enemy.template
	├── audio
	│   └── GameMusic.mp3
	├── component_types
	│   └── GameScript.lua
	├── fonts
	│   └── BasicFont.ttf
	├── images
	│   └── GameImage.png
	├── scenes
	│    └── basic.scene
	├── lua (optional!)
	│    └── dkjson.lua
	├── rendering.config
	└── game.config

All resources (think images, audio, fonts, components, actor templates, and scenes) must be in their respective folders to work with the engine.

Actor templates, scenes, and config files are in JSON.

## Configs

A game.config might look something like this:
{
  "game_title": "my super cool game",
  "initial_scene": "scene1"
}

A rendering.config might look something like this:
{
  "x_resolution": 480,
  "y_resolution": 480
}
These determine how many pixels the game window will be on startup.

## Scenes
Here's an example of a scene.

	"actors": [
		{
			"name": "Player",
			"components": {
				"1": {
					"type": "Rigidbody",
					"body_type": "static"
				},
				"2": {
					"type": "SpriteRenderer",
					"sprite": "grass_platform_small"
				},
				"3": {
					"type": "PlayerControllerExample"
				}
			}
		},
		{
			"name": "Enemy",
			"components": {
					"1": {
						"type": "Rigidbody"
					},
					"2": {
						"type": "EnemyController",
						"speed": 2
					}
			}
		}
	]

 A scene is simply a list of the actors that should be spawned at the beginning of a scene, and the list of components they should consist of.

 Components must have an actor-specific unique key (which can be anything!), and a type which tells the engine what kind of component to look for in your component_types folder.

 Component variables can be overwritten here as well. For example, if your EnemyController has a speed variable, you can overwrite it as shown.

 ## Actor Templates
 Actor templates function similar to "prefabs" in Unity, and work as an easy way to store the information of an actor so it can be instantiated at runtime.

 A template might look like this:

 	"name": "Dog",
	"components": {
		"1": {
			"type": "Rigidbody"
		},
		"2": {
			"type": "AnimalController",
			"speed": 1,
      "sound": "woof"
		}
	}

 Be sure to name it [name].template and put it in the actor_templates folder.

 ## Components

 This engine has a couple of native components (ParticleSystem, RigidBody) that you can use so you don't have to create them yourself.

 However, the majority of functionality will come from what components you create!

 To help you out, there are a few lifecycle functions that are available, such as "OnStart", "OnUpdate", "OnLateUpdate", and "OnDestroy".

 OnStart immediately after a component is created, OnUpdate runs every frame the component is enabled, OnLateUpdate runs after the update of each frame, and OnDestroy runs when the component is deleted.

 Here is an example lua component:

PlayerControllerExample = {

	movement_speed = 1.0,

	OnStart = function(self)
		self.rb = Actors.GetComponentByType(self.actor.ID, "Rigidbody")
	end,

	OnUpdate = function(self)
		local v = vec2:new(0.0, 0.0)
		v.y = self.rb:GetVelocity().y

		if Input.GetKey("right") then
			print("right")
            		v.x = 1 * self.movement_speed
		elseif Input.GetKey("left") then
			print("left")
			v.x = -1 * self.movement_speed
		end
		
		self.rb:SetVelocity(v)
	end
}

The name of the file must be the same as the name inside the file, so this file must be named PlayerControllerExample.lua

## Building Your Game

This engine was originally built for compatability with Windows, Linux, and OSX. However, in adding networking (for Windows), I have not yet updated OSX and Linux.

For Windows, the easiest solution is to open the .sln file in the repository and run it from there. It should additionally create a build folder and executable, which can be used in lieu of running it from inside of Visual Studio.

I am hoping to add CMake in the future, and link the ENet files as well.

## Documentation

I have not yet made official documentation, but I am hoping to create documentation and examples primarily for the Lua exposed functions that are available.
