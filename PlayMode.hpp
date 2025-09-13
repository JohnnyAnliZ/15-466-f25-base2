#include "Mode.hpp"

#include "Scene.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>



struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up;

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	//transforms to manipulate
	std::vector<Scene::Transform*>roadSegs;
	Scene::Transform *steeringWheel = nullptr;
	Scene::Transform* windShield = nullptr;
	Scene::Transform *rightExit = nullptr;
	std::vector<Scene::Transform*> wrongExitsTransforms;
	Scene::Transform* gasExit = nullptr;
	std::vector<Scene::Transform*> nasExits;


	//driving parameters
	enum gear{
		R,
		N,
		D
	};
	gear myGear = N;
	float gasTime = 0.0f;
	float turnTime = 0.0f;
	float breakTime = 0.0f;
	bool breaking = false;
	float velocity = 0.0f;

	glm::quat carBaseRot;
	glm::quat steeringWheelBaseRot;
	//rotation of the car in radians about the z axis
	float carAddedRot = 0.0f;
	glm::vec2 move = glm::vec2(0, 0);
	//mouse location to track camera rotation
	glm::vec2 mouseRot = glm::vec2(0, 0);

	//-----infinite road mechanic-----
	//current road segment delta y
	float curY = 0.0f;
	//airwall at the back
	float backWall = -10.0f;
	uint32_t segCount = 1;
	
	//the number of wrong exits
	uint32_t badSignCount = 5;
	uint32_t fakeRandom = 0;

	glm::quat hip_base_rotation;
	glm::quat upper_leg_base_rotation;
	glm::quat lower_leg_base_rotation;
	float wobble = 0.0f;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
