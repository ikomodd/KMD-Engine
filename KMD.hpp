#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

//
// Classes Variaveis
//

struct ColorRGBA {

	unsigned int R, G, B, A = 0;

	ColorRGBA(int r = 0, int g = 0, int b = 0, int a = 255) : R(r), G(g), B(b), A(A) {};
};

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

//
// Classes de corpos
//

	// Hierarquia:
	// > VoidBody
	// > > Body2D
	// > > > Camera2D
	//

	// VoidBody

class VoidBody {
public:

	std::string Name;

	unsigned int ID = 0;

	VoidBody* Parent = nullptr;

	std::vector<VoidBody*> Children;

	//

	VoidBody(const char* name);
	virtual ~VoidBody();

	void AddBody(VoidBody* body);

	void RemoveBody(const char* body_name);
	void ClearAllChildren();


	VoidBody* GetBody(const char* Body_name);
	std::vector<VoidBody*> GetAllChildren();

	//

	virtual void _Ready() {};
	virtual void _Process(double delta) {};

};

// Body2D

class Body2D : public VoidBody {
public:

	Vector2 Position = {};
	Vector2 Scale = {};

	Body2D(const char* name, Vector2 position, Vector2 scale) : VoidBody(name), Position(position), Scale(scale) {};
};

// Camera2D

class Camera2D : public Body2D {
public:

	float Zoom  = 0;

	Camera2D(const char* name, Vector2 position, float start_zoom) : Body2D(name, position, { 1, 1 }), Zoom(start_zoom) {};

	SDL_FRect GetCameraOffset(Body2D* body, iVector2 window_size);
};

//
// Classe do nucleo
//

class KMD_Core {

public:

	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
	
	SDL_Event Event = {};

	iVector2 WindowSize = {};

	//

	Camera2D* CurrentCamera = nullptr;

	VoidBody* CurrentScene = nullptr;

	bool Running = true;
	ColorRGBA BackgroundColor;

	//

	KMD_Core() {}

private:

	void RenderScene();

public:

	void InitCore(const char* title, Vector2 size, SDL_WindowFlags window_flags);

	void StopCore();

};