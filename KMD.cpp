
// KMD Engine
// Acesse o tutorial disponivel no arquivo 'Docs'
// 
// Atenção! Para funcionar corretamente é necessario o uso de SDL3, SDL3_image e SDL3_ttf. Configure manualmente no seu editor

#include "KMD.hpp"

// SDL Globais

SDL_Window* Window = nullptr;
SDL_Renderer* Renderer = nullptr;

// Funçoes privadas: Usadas para suporte a outras funções

void GetAllChildrenDFS(VoidBody* Parent, std::vector<VoidBody*>& List) {


	for (VoidBody* Body : Parent->Children) {

		List.push_back(Body);

		GetAllChildrenDFS(Body, List);
	}
}

// Funçoes SpriteCore

	// Construtor

SpriteCore2D::SpriteCore2D(std::string TexturePath) {

	Surface = IMG_Load(TexturePath.c_str());
	Texture = SDL_CreateTextureFromSurface(Renderer, Surface);

	if (Texture == nullptr) std::cerr << SDL_GetError() << "\n";

}

	// Destrutor

SpriteCore2D::~SpriteCore2D() {

	delete Animator;

	SDL_DestroyTexture(Texture);
	SDL_DestroySurface(Surface);
}

// Funçoes VoidBody

	// void AddBody(VoidBody* Body): Adiciona um corpo a lista de filhos de outro corpo

void VoidBody::AddBody(VoidBody* Body) {

	Body->Parent = this;
	Body->ID = Children.size();
	Children.emplace_back(Body);

	Body->_Ready();

}

	// void RemoveBody(const char* BodyName): Remove um corpo da lista de filhos de outro corpo

void VoidBody::RemoveBody(const char* BodyName) {



}

	// void ClearAllChildren(): Remove todos os corpos na lista de filhos

void VoidBody::ClearAllChildren() {

	std::vector<VoidBody*> LinearChildren = GetAllChildren();

	for (VoidBody* Child : LinearChildren) {

		delete Child;
	}
}

	// VoidBody* GetBody(const char* BodyName): Retorna o filho referido pelo seu nome da lista de filhos de um corpo

VoidBody* VoidBody::GetBody(const char* BodyName) {

	for (VoidBody* Body : Children) {

		if (Body->Name == BodyName) return Body;
	}
	return nullptr;
}

	// std::vector<VoidBody*> VoidBody::GetAllChildren(): Retorna todos os filhos de um corpo em um Vector linear

std::vector<VoidBody*> VoidBody::GetAllChildren() {

	std::vector<VoidBody*> LinearChildren;

	GetAllChildrenDFS(this, LinearChildren);

	return LinearChildren;
}

	// Construtor e destrutor

VoidBody::VoidBody(const char* name) : Name(name) {}
VoidBody::~VoidBody() {

	_Destroyed();

	ClearAllChildren();

}

// Funcoes do Body2D

	// Destrutor

Body2D::~Body2D() {

	delete SpriteCore;

}

// Funçoes da Camera2D

	// SDL_FRect Camera2D::GetCameraOffset(Body2D* Body, iVector2 WindowSize): Retorna um SDL_FRect referente a posição de um corpo usando como referencia a posição da camera

SDL_FRect Camera2D::GetCameraOffset(Body2D* Body, iVector2 WindowSize) {

	Vector2 AnchorPosition = Position - (Vector2{(float)WindowSize.X, (float)WindowSize.Y} / 2.0f * Zoom);

	SDL_FRect OffsetRect = {

		(Body->Position.X - AnchorPosition.X) * Zoom,
		(Body->Position.Y - AnchorPosition.Y) * Zoom,

		Body->Scale.X * Zoom,
		Body->Scale.Y * Zoom

	};

	return OffsetRect;
}

// Funçoes do KMD_Core

	// void KMD_Core::InitCore(const char* Title, Vector2 Size, SDL_WindowFlags WindowFlags) Inicia o SDL e Nucleo

void KMD_Core::InitCore(const char* Title, Vector2 Size, SDL_WindowFlags WindowFlags) {

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(Title, Size.X, Size.Y, WindowFlags, &Window, &Renderer);

};

	// void KMD_Core::RunCore(): Inicia o loop principal

void KMD_Core::RunCore() {

	// Verificações de segurança

	bool StartError = false;

	if (CurrentScene == nullptr) {

		std::cerr << "Init KMD Error: KMD_Core.CurrentScene = nullptr; \n";
		StartError = true;
	}

	if (CurrentCamera == nullptr) {

		std::cerr << "Init KMD Error: KMD_Core.CurrentCamera = nullptr; \n";
		StartError = true;
	}

	if (StartError) return;

	// Loop Principal

	while (Running) {

		SDL_GetWindowSize(Window, &WindowSize.X, &WindowSize.Y);

		while (SDL_PollEvent(&Event)) {

			if (Event.type == SDL_EVENT_QUIT) StopCore();

		};

		SDL_SetRenderDrawColor(Renderer, BackgroundColor.R, BackgroundColor.G, BackgroundColor.B, BackgroundColor.A);

		SDL_RenderClear(Renderer);

		RenderScene();

		SDL_RenderPresent(Renderer);
	};
}

	// void KMD_Core::StopCore(): Para o loop principal

void KMD_Core::StopCore() {

	Running = false;

	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(Renderer);

};

	// void KMD_Core::PlayScene(VoidBody* Scene): Inicia uma cena

void KMD_Core::PlayScene(VoidBody* Scene) {

	CurrentScene = Scene;

	Scene->_Ready();
}

	// void KMD_Core::InsertCamera(Camera2D* Camera): Define a camera que deve ser usada

void KMD_Core::InsertCamera(Camera2D* Camera) {

	CurrentCamera = Camera;

	Camera->_Ready();
}

	// void KMD_Core::RenderScene(): Renderiza a cena atual. (Não é necessario usa-lo. já é ativado no loop)

Uint64 LastDeltaTimeTick = SDL_GetTicks();

void KMD_Core::RenderScene() {

	Uint64 CurentDeltaTimeTick = SDL_GetTicks();

	double DeltaTime = (CurentDeltaTimeTick - LastDeltaTimeTick) / 1000.0;

	LastDeltaTimeTick = CurentDeltaTimeTick;

	//

	std::vector<VoidBody*> Workspace = CurrentScene->GetAllChildren();

	for (VoidBody* VBody : Workspace) {

		VBody->_Process(DeltaTime);

		if (auto Body = dynamic_cast<Body2D*>(VBody)) {
			if (Body->SpriteCore == nullptr) continue;
			
			SDL_FRect VisibleRect = CurrentCamera->GetCameraOffset(Body, WindowSize);

			if (Body->SpriteCore->Texture != nullptr) {

				SDL_RenderTexture(Renderer, Body->SpriteCore->Texture, NULL, &VisibleRect);

			}
			else {

			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(Renderer, &VisibleRect);

			}
		}
	}
}