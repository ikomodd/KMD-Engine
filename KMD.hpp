#pragma once

// KMD Engine
// Acesse o tutorial disponivel no arquivo 'Docs'
// 
// Atenção! Para funcionar corretamente é necessario o uso de SDL3, SDL3_image e SDL3_ttf. Configure manualmente no seu editor

#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

// SDL globais

extern SDL_Window* Window;
extern SDL_Renderer* Renderer;

// Enumeradores padroes

	// Direction4: Enum de 4 direções

enum class Direction4 {

	UP_TO_DOWN,
	DOWN_TO_UP,

	LEFT_TO_RIGHT,
	RIGHT_TO_LEFT

};

// Classes Variaveis: Classes para funcionalidades adicionais

	// Color: Sistema de cor no metodo RGBA

struct ColorRGBA {

	unsigned int R, G, B, A = 0;

	ColorRGBA(int r = 0, int g = 0, int b = 0, int a = 255) : R(r), G(g), B(b), A(A) {};
};

	// iVector2: Vetores 2D para valores inteiros

struct iVector2 {

	int X, Y;

	iVector2(int x = 0, int y = 0) : X(x), Y(y) {};

	iVector2 operator + (iVector2 Other) {

		return { X + Other.X, Y + Other.Y };
	}

	iVector2 operator - (iVector2 Other) {

		return { X - Other.X, Y - Other.Y };
	}

	iVector2 operator * (int Number) {

		return { X * Number, Y * Number };
	}

	iVector2 operator / (int Number) {

		return { X / Number, Y / Number};
	}
};

	// Vector2: Vetores 2D para valores flutuantes

struct Vector2 {

	float X, Y;

	Vector2(float x = 0, float y = 0) : X(x), Y(y) {};

	Vector2 operator + (Vector2 Other) {

		return { X + Other.X, Y + Other.Y };
	}

	Vector2 operator - (Vector2 Other) {

		return { X - Other.X, Y - Other.Y };
	}

	Vector2 operator * (float Number) {

		return { X * Number, Y * Number };
	}

	Vector2 operator / (float Number) {

		return { X / Number, Y / Number };
	}
};

	// Animation2D: Unidade de animação

class Animation2D {
public:

	float AnimationUpdateDelay = 0;

	Vector2 StartFramePosition = {};
	Vector2 FrameScale = {};

	int PixelOffset = 0;

	Direction4 UpdateDirection = Direction4::LEFT_TO_RIGHT;

	int Size;

	Animation2D(Vector2 start, int size, Vector2 scale, int pixel_offset, float update_delay, Direction4 update_direction = Direction4::LEFT_TO_RIGHT) : StartFramePosition(start), Size(size), FrameScale(scale), AnimationUpdateDelay(update_delay), PixelOffset(pixel_offset), UpdateDirection(update_direction) {};
	~Animation2D() {};

};

	// SpriteSheet: Representa texturas e a visibilidade de um Body

class SpriteCore2D {
public:

	SDL_Texture* Texture = nullptr;
	SDL_Surface* Surface = nullptr;

	SDL_FRect TextureRect = {};

	SpriteCore2D(std::string TexturePath, SDL_ScaleMode ScaleMode);
	~SpriteCore2D();
};

// Animator2D: Controla as animações e movimentação de imagens

class Animator2D {

	SpriteCore2D* SpriteCore = nullptr;

public:

	Uint64 LastAnimationUpdate = 0;

	std::unordered_map<std::string, Animation2D*> Animations = {};

	int Padding = 0;

	int CurrentFrame = 1;

	std::string CurrentAnimation = "";

	Animator2D(SpriteCore2D* sprite_core, int padding = 0) : SpriteCore(sprite_core), Padding(padding) {};
	~Animator2D();

	void LoadAnimation(std::string Name, Vector2 Start, int Size, Vector2 Scale, int PixelOffset, float UpdateDelay, Direction4 UpdateDirection = Direction4::LEFT_TO_RIGHT);

	void UpdateAnimator();

	void PlayAnimation(std::string Name);
	void StopAnimation(std::string Name);

};

// Classes de corpos: Classes para qualquer coisa que esteja no jogo

	// Hierarquia:
	// 
	// > VoidBody
	// > > Body2D ( SpriteCore2D ( Animator2D ( Animation2D ) ) )
	// > > > Camera2D

	// VoidBody: A base de tudo, segura o sistema de hierarquia e identificação do corpo

class VoidBody {
public:

	std::string Name;

	unsigned int ID = 0;

	VoidBody* Parent = nullptr;

	std::vector<VoidBody*> Children;

	//

	VoidBody(const char* name = "Name Undefined");
	virtual ~VoidBody();

	//

	void AddBody(VoidBody* body);

	void RemoveBody(const char* body_name);
	void ClearAllChildren();


	VoidBody* GetBody(const char* Body_name);
	std::vector<VoidBody*> GetAllChildren();

	//

	virtual void _Ready() {};
	virtual void _Destroyed() {};

	virtual void _Process(double delta) {};

	//

};

	// Body2D: Corpo de duas dimençoes que pode ter aparencia, posição, colisao, etc...

class Body2D : public VoidBody {
public:

	Vector2 Position = {};
	Vector2 Scale = {};

	SpriteCore2D* SpriteCore = nullptr;
	Animator2D* Animator = nullptr;

	
	SDL_FRect CollisionShape;

	bool CanCollide = true;

	Body2D(const char* name = "Body2D Undefined", Vector2 position = {}, Vector2 scale = {}, SDL_FRect collision_shape);
	~Body2D();

	void MoveAndCollide(VoidBody* Workspace, Vector2 NewPosition);

};

	// Camera2D: A camera define o que é visivel no jogo. Necessário para a inicialização do Core


class Camera2D : public Body2D {
public:

	float Zoom;

	Camera2D(const char* name = "Camera2D Undefined Name", Vector2 position = {}, float zoom = 1.0f) : Body2D(name, position, {}), Zoom(zoom) {};

	SDL_FRect GetCameraOffset(Body2D* body, iVector2 window_size);
};

// Classe do Nucleo: O Core (Nucleo) é O cérebro por trás do funcionamento do jogo

class KMD_Core {
public:
	
	SDL_Event Event = {};

	iVector2 WindowSize = {};

	//

	Camera2D* CurrentCamera = nullptr; // Não é recomendado a alocação manual. Use InsertCamera(Camera2D Camera)

	VoidBody* CurrentScene = nullptr;  // Não é recomendado a alocação manual. Use PlayScene(VoidBody Scene)

	bool Running = true;
	ColorRGBA BackgroundColor;

	//

	KMD_Core() {}

	void PlayScene(VoidBody* Scene);
	void InsertCamera(Camera2D* Camera);

private:

	void RenderScene();

public:

	void InitCore(const char* title, Vector2 size, SDL_WindowFlags window_flags);
	void RunCore();

	void StopCore();

};