
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

// Funçoes Animator2D

	// Construtor

Animator2D::~Animator2D() {

	for (auto Animation : Animations) {

		delete Animation.second;

	}

}

// Funçoes SpriteCore

	// Construtor

SpriteCore2D::SpriteCore2D(std::string TexturePath, SDL_ScaleMode ScaleMode) {

	Surface = IMG_Load(TexturePath.c_str());
	Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
	
	SDL_SetTextureScaleMode(Texture, ScaleMode);

	if (Texture == nullptr) std::cerr << SDL_GetError() << "\n";

}

	// Destrutor

SpriteCore2D::~SpriteCore2D() {

	SDL_DestroyTexture(Texture);
	SDL_DestroySurface(Surface);
}

// void LoadAnimation(std::string Name, int Size, Vector2 Scale, Uint64 UpdateDelay, Direction4 UpdateDirection = Direction4::LEFT_TO_RIGHT): Carrega uma animaçao no animador

void Animator2D::LoadAnimation(std::string Name, Vector2 Start, int Size, Vector2 Scale, int PixelOffset, float UpdateDelay, Direction4 UpdateDirection) {

	Animations[Name] = new Animation2D(Start, Size, Scale, PixelOffset, UpdateDelay, UpdateDirection);

};

// void Animator2D::UpdateAnimator(): Atualiza as animaçoes

void Animator2D::UpdateAnimator() {

	Uint64 CurrentTimeTick = SDL_GetTicks();

	Animation2D*& Animation = Animations[CurrentAnimation];

	if (CurrentFrame < Animation->Size - 1) CurrentFrame++;
	else CurrentFrame = 0;

	switch (Animation->UpdateDirection) {
	case Direction4::UP_TO_DOWN:

		SpriteCore->TextureRect.y = Padding + (CurrentFrame * (Animation->FrameScale.Y + Animation->PixelOffset));

		break;
	case Direction4::DOWN_TO_UP:

		SpriteCore->TextureRect.y = Padding + (((Animation->Size - 1) - CurrentFrame) * (Animation->FrameScale.Y + Animation->PixelOffset));

		break;
	case Direction4::LEFT_TO_RIGHT:

		SpriteCore->TextureRect.x = Padding + (CurrentFrame * (Animation->FrameScale.X + Animation->PixelOffset));

		break;
	case Direction4::RIGHT_TO_LEFT:

		SpriteCore->TextureRect.x = Padding + (((Animation->Size - 1) - CurrentFrame) * (Animation->FrameScale.X + Animation->PixelOffset));

		break;
	}
}

void Animator2D::PlayAnimation(std::string Name) {

	if (Animations[Name]) {

		CurrentAnimation = Name;

		SpriteCore->TextureRect.w = Animations[CurrentAnimation]->FrameScale.X;
		SpriteCore->TextureRect.h = Animations[CurrentAnimation]->FrameScale.Y;

		UpdateAnimator();
	}
	else std::cerr << "Animation: '" << Name << "' is invalid";

}

void Animator2D::StopAnimation(std::string Name) {

	if (CurrentAnimation == Name) CurrentAnimation = "";

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

Body2D::Body2D(const char* name, Vector2 position, Vector2 scale, SDL_FRect collision_shape) : VoidBody(name), Position(position), Scale(scale), CollisionShape(collision_shape) {

	if (CollisionShape.w == 0 || CollisionShape.h == 0) CanCollide = false;

}

Body2D::~Body2D() {

	delete Animator;
	delete SpriteCore;

}

void Body2D::MoveAndCollide(VoidBody* Workspace, Vector2 NewPosition) {

	std::vector<VoidBody*> Collisors = Workspace->GetAllChildren();

	Position.X = NewPosition.X;

	for (auto VBody : Collisors) {
		if (Body2D* Body = dynamic_cast<Body2D*>(VBody)) {

			

		}
	}
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

	Uint64 CurrentTimeTick = SDL_GetTicks();

	double DeltaTime = (CurrentTimeTick - LastDeltaTimeTick) / 1000.0;

	LastDeltaTimeTick = CurrentTimeTick;

	//

	std::vector<VoidBody*> Workspace = CurrentScene->GetAllChildren();

	for (VoidBody* VBody : Workspace) {

		VBody->_Process(DeltaTime);

		if (auto Body = dynamic_cast<Body2D*>(VBody)) {
			if (Body->SpriteCore == nullptr) continue;

			if (Body->Animator && Body->Animator->CurrentAnimation != "") {

				Animator2D* Animator = Body->Animator;

				if ((CurrentTimeTick - Animator->LastAnimationUpdate) > Animator->Animations[Animator->CurrentAnimation]->AnimationUpdateDelay * 1000) {
					Animator->LastAnimationUpdate = CurrentTimeTick;

					Animator->UpdateAnimator();
				}
			}

			SDL_FRect VisibleRect = CurrentCamera->GetCameraOffset(Body, WindowSize);
			SDL_FRect TextureRect = Body->SpriteCore->TextureRect;

			if (Body->SpriteCore->Texture) {

				SDL_RenderTexture(Renderer, Body->SpriteCore->Texture, &TextureRect, &VisibleRect);

			}
			else {

			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(Renderer, &VisibleRect);

			}
		}
	}
}