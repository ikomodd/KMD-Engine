#include "KMD.hpp"

//
// Funçoes privadas
//

void GetAllChildrenDFS(VoidBody* Parent, std::vector<VoidBody*>& List) {


	for (VoidBody* Body : Parent->Children) {

		List.push_back(Body);

		GetAllChildrenDFS(Body, List);
	}
}

//
// Funçoes VoidBody
//

void VoidBody::AddBody(VoidBody* Body) {

	Body->Parent = this;
	Body->ID = Children.size();
	Children.emplace_back(Body);

}

void VoidBody::RemoveBody(const char* BodyName) {



}

void VoidBody::ClearAllChildren() {

	std::vector<VoidBody*> LinearChildren = GetAllChildren();

	for (VoidBody* Child : LinearChildren) {

		delete Child;
	}
}

VoidBody* VoidBody::GetBody(const char* BodyName) {

	for (VoidBody* Body : Children) {

		if (Body->Name == BodyName) return Body;
	}
	return nullptr;
}

std::vector<VoidBody*> VoidBody::GetAllChildren() {

	std::vector<VoidBody*> LinearChildren;

	GetAllChildrenDFS(this, LinearChildren);

	return LinearChildren;
}

VoidBody::VoidBody(const char* Name) {}
VoidBody::~VoidBody() {

	ClearAllChildren();

}

//
// Funçoes da Camera2D
//

SDL_FRect Camera2D::GetCameraOffset(Body2D* Body, iVector2 WindowSize) {
	//Zoom -= 0.0001f;

	Vector2 AnchorPosition = Position - (Vector2{(float)WindowSize.X, (float)WindowSize.Y} / 2.0f * Zoom);

	SDL_FRect OffsetRect = {

		(Body->Position.X - AnchorPosition.X) * Zoom,
		(Body->Position.Y - AnchorPosition.Y) * Zoom,

		Body->Scale.X * Zoom,
		Body->Scale.Y * Zoom

	};

	return OffsetRect;
}

//
// Funçoes do KMD_Core
//

void KMD_Core::InitCore(const char* Title, Vector2 Size, SDL_WindowFlags WindowFlags) {

	// Inicia o SDL

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(Title, Size.X, Size.Y, WindowFlags, &Window, &Renderer);

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
};

void KMD_Core::StopCore() {

	Running = false;

	SDL_DestroyWindow(Window);
	SDL_DestroyRenderer(Renderer);

};

void KMD_Core::RenderScene() {

	std::vector<VoidBody*> Workspace = CurrentScene->GetAllChildren();

	for (VoidBody* VBody : Workspace) {

		if (auto Body = dynamic_cast<Body2D*>(VBody)) {

			SDL_FRect VisibleElement = CurrentCamera->GetCameraOffset(Body, WindowSize);



			SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
			SDL_RenderFillRect(Renderer, &VisibleElement);
		}
	}
}