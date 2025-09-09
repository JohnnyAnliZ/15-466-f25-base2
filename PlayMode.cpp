#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint road_meshes_for_lit_color_texture_program = 0;
Load< MeshBuffer > road_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("road.pnct"));
	road_meshes_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > road_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("road.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = road_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = road_meshes_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*road_scene) {
	//get pointers to leg for convenience:
	//for (auto &transform : scene.transforms) {
	//	if (transform.name == "Hip.FL") hip = &transform;
	//	else if (transform.name == "UpperLeg.FL") upper_leg = &transform;
	//	else if (transform.name == "LowerLeg.FL") lower_leg = &transform;
	//}
	//if (hip == nullptr) throw std::runtime_error("Hip not found.");
	//if (upper_leg == nullptr) throw std::runtime_error("Upper leg not found.");
	//if (lower_leg == nullptr) throw std::runtime_error("Lower leg not found.");

	//hip_base_rotation = hip->rotation;
	//upper_leg_base_rotation = upper_leg->rotation;
	//lower_leg_base_rotation = lower_leg->rotation;

	//get pointer to camera for convenience:
	if (scene.cameras.size() != 1) throw std::runtime_error("Expecting scene to have exactly one camera, but it has " + std::to_string(scene.cameras.size()));
	camera = &scene.cameras.front();

	//get pointer to the road surface
	for (auto &transform : scene.transforms) {
		if (transform.name == "RoadSegment1") roadSegs.emplace_back(&transform);
		else if (transform.name == "RoadSegment2") roadSegs.emplace_back(&transform);
		else if (transform.name == "RoadSegment3") roadSegs.emplace_back(&transform);
		else if (transform.name == "SteeringWheel") steeringWheel = &transform;
		else if (transform.name == "WindShield") windShield = &transform;
		else if (transform.name == "GasExit") gasExit = &transform;
		else if (transform.name == "ExitWithNoGas1") nasExits.emplace_back(&transform);
		else if (transform.name == "ExitWithNoGas2") nasExits.emplace_back(&transform);
		else if (transform.name == "ExitWithNoGas3") nasExits.emplace_back(&transform);
		else if (transform.name == "ExitWithNoGas4") nasExits.emplace_back(&transform);
		else if (transform.name == "RoadSign.Gas") rightExit = &transform;
		else if (transform.name == "RoadSign.Hospital1") wrongExitsTransforms.emplace_back(&transform);
		else if (transform.name == "RoadSign.Hospital2") wrongExitsTransforms.emplace_back(&transform);
		//this is the rather normal one
		else if (transform.name == "RoadSign.Hospital3") wrongExitsTransforms.emplace_back(&transform);
		else if (transform.name == "RoadSign.Hospital4") wrongExitsTransforms.emplace_back(&transform);
		else if (transform.name == "RoadSign.Hospital5") wrongExitsTransforms.emplace_back(&transform);

	}
	//if (roadSeg == nullptr) throw std::runtime_error("Hip not found.");
	if (steeringWheel == nullptr) throw std::runtime_error("Upper leg not found.");
	if (windShield == nullptr) throw std::runtime_error("Lower leg not found.");


	//increase the size of roadSegs by one 
	roadSegs.emplace_back(roadSegs[0]);

	
	std::cout << windShield->position.x << std::endl;

	steeringWheel->position = glm::vec3(-0.440105f, -0.277918f, -0.129515f);
	steeringWheel->scale = glm::vec3(0.246365f, 0.24396f, 0.333387f);
	steeringWheel->rotation = glm::quat(0.902628f, 0.430421f, 0.0f, 0.0f);
	steeringWheel->parent = windShield;

	carBaseRot = windShield->rotation;


	camera->transform->rotation = glm::quat(0.7069f,0.7072f,0.0f,0.0f);
	

	//try to parent camera to the windShield
	camera->transform->position = glm::vec3(-0.08587f, -2.16435f, 1.77435f);
	camera->transform->rotation = glm::quat(0.905369f, 0.424626f, 0.0f, 0.0f);
	camera->transform->scale = glm::vec3(0.85884f, 0.85884f, 0.85884f);
	camera->transform->parent = windShield;

	/*hip_base_rotation = hip->rotation;
	upper_leg_base_rotation = upper_leg->rotation;
	lower_leg_base_rotation = lower_leg->rotation;*/


}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const& evt, glm::uvec2 const& window_size) {

	if (evt.type == SDL_EVENT_KEY_DOWN) {
		if (evt.key.key == SDLK_ESCAPE) {
			SDL_SetWindowRelativeMouseMode(Mode::window, false);
			return true;
		} else if (evt.key.key == SDLK_A) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.downs += 1;
			down.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_EVENT_KEY_UP) {
		if (evt.key.key == SDLK_A) {
			left.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_D) {
			right.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_W) {
			up.pressed = false;
			return true;
		} else if (evt.key.key == SDLK_S) {
			down.pressed = false;
			return true;
		}
	} else if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (SDL_GetWindowRelativeMouseMode(Mode::window) == false) {
			SDL_SetWindowRelativeMouseMode(Mode::window, true);
			return true;
		}

	} else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
		if (SDL_GetWindowRelativeMouseMode(Mode::window) == true) {
			glm::vec2 motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				-evt.motion.yrel / float(window_size.y)
			);
			mouseRot += motion;
			camera->transform->rotation = glm::normalize(
				glm::quat(0.7069f, 0.7072f, 0.0f, 0.0f)
				* glm::angleAxis(-mouseRot.x * camera->fovy, glm::vec3(0.0f, 0.8f, -0.79f))
				* glm::angleAxis(mouseRot.y * camera->fovy, glm::vec3(1.0f, 0.0f, 0.0f))
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {

	//slowly rotates through [0,1):
	wobble += elapsed / 10.0f;
	wobble -= std::floor(wobble);

	turnTime += elapsed;
	//std::cout << camera->transform->rotation.w << camera->transform->rotation.x << camera->transform->rotation.y << camera->transform->rotation.z << std::endl;

	//hip->rotation = hip_base_rotation * glm::angleAxis(
	//	glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
	//	glm::vec3(0.0f, 1.0f, 0.0f)
	//);
	//upper_leg->rotation = upper_leg_base_rotation * glm::angleAxis(
	//	glm::radians(7.0f * std::sin(wobble * 2.0f * 2.0f * float(M_PI))),
	//	glm::vec3(0.0f, 0.0f, 1.0f)
	//);
	//lower_leg->rotation = lower_leg_base_rotation * glm::angleAxis(
	//	glm::radians(10.0f * std::sin(wobble * 3.0f * 2.0f * float(M_PI))),
	//	glm::vec3(0.0f, 0.0f, 1.0f)
	//);

	//deltaY for car
	float deltaY = 0.0;
	
	

	//combine inputs into a move:
	//constexpr float PlayerSpeed = 30.0f;

	constexpr float maxD = 5.0f;
	constexpr float maxR = -2.0f;

	constexpr float gas = 0.04f;
	//constexpr float halt= 0.2f;


	//gear logic 
	if ((myGear == N) && up.pressed && !breaking) myGear = D;
	if ((myGear == N) && down.pressed && !breaking) myGear = R;
		

	if (myGear != N) {
		//std::cout << "gastime" << std::endl;
		gasTime += elapsed;

	}

	//move camera: Instead move the car
	//if (left.pressed && !right.pressed) move.x =-1.0f;
	//if (!left.pressed && right.pressed) move.x = 1.0f;
	//if (down.pressed && !up.pressed) move.y = -1.0f;
	//if (!down.pressed && up.pressed) move.y = 1.0f;
	//make it so that moving diagonally doesn't go faster:
	//if (move != glm::vec2(0.0f)) move = glm::normalize(move) * PlayerSpeed * elapsed;

		
	//gear system
	if(myGear == R){		
		//In R, gas
		if (down.pressed && !up.pressed) {
			breaking = false;
			if (gasTime > 0.25f) {
				velocity = std::max(velocity - gas*(velocity+2.0f)/10, maxR); 
				gasTime = 0.0f;
			}

		}
		//In R, break
		if (!down.pressed && up.pressed) {
			breaking = true;
		}
	}
	if (myGear == D) {	
		//In D, gas
		if (!down.pressed && up.pressed) {
			breaking = false;
			if (gasTime > 0.25f) {	
				velocity = std::min(velocity + gas * (velocity + 2.0f) / 10, maxD);
				gasTime = 0.0f;
			}
		}
		//In D, break
		if (down.pressed && !up.pressed) {
			breaking = true;	
		}
	}

	//car turning
	if (turnTime > 0.03 && (velocity <-0.02 || velocity >0.02)) {
		if (left.pressed && !right.pressed) {	
			steeringWheel->rotation = glm::normalize(carBaseRot * glm::angleAxis(carAddedRot*5, glm::vec3(0.0f, 0.0f, 1.0f)));
			if(myGear == D)carAddedRot += glm::radians(1 - velocity/ 7);
			else carAddedRot -= glm::radians(1 - velocity / 7);
		}
		if (right.pressed && !left.pressed) {
			steeringWheel->rotation = glm::normalize(carBaseRot * glm::angleAxis(carAddedRot*5, glm::vec3(0.0f, 0.0f, 1.0f)));
			if (myGear == D)carAddedRot -= glm::radians(1 - velocity/ 7);
			else carAddedRot += glm::radians(1 - velocity / 7);
		}
		turnTime = 0;
	}
	windShield->rotation = glm::normalize(carBaseRot * glm::angleAxis(carAddedRot, glm::vec3(0.0f, 0.75f, 1.0f)));

	//when there's no gas or break
	if (!(down.pressed ^ up.pressed)) {
		breaking = false;
		velocity -= velocity / 100;
	}
	//halt when velocity is low enough
	if (breaking) {
		if (velocity < 0.01f && velocity > -0.01f) {
			std::cout << "neutral" << std::endl;
			myGear = N;
			velocity = 0.0f;
		}
		velocity -= velocity * 5 / 100;
	}

	//add velocity to the windshield facing direction, -4.0 is the left boundary for the road
	if (myGear != N) {
		/*std::cout << windShield->position.x<<std::endl;*/

		//-4.0 is the left boundary for the road
		windShield->position.x = std::max(-4.0f, windShield->position.x - glm::sin(carAddedRot) * velocity);
		//no going back
		deltaY = glm::cos(carAddedRot) * velocity;
		curY += deltaY;
		windShield->position.y = std::max(backWall, windShield->position.y + glm::cos(carAddedRot) * velocity);
	}

	glm::mat4x3 frame = camera->transform->make_parent_from_local();
	glm::vec3 frame_right = frame[0];
	//glm::vec3 up = frame[1];
	glm::vec3 frame_forward = -frame[2];
	
	//at every 460m, there's a new segment of road, move the one behind to the top
	
	if (curY > 470.0f) {
		std::cout << "newSeg" << std::endl;
		segCount += 1;
		fakeRandom += 13 * segCount;
		curY -= 470.0f;
		//update a new road segment
		roadSegs[0]->position.y += 1410.0;
		roadSegs[3] = roadSegs[0];
		roadSegs[0] = roadSegs[1];
		roadSegs[1] = roadSegs[2];
		roadSegs[2] = roadSegs[3];
		
	

		//make a new exit for the new road Segment start having gas stations after 10
		if (segCount % 10== 0) {
			rightExit->position.y = roadSegs[2]->position.y;
			gasExit->position.y = roadSegs[2]->position.y + 470.0f;
		}
		else {
			if ( segCount % 2 == 0) {
				wrongExitsTransforms[fakeRandom % 5]->position.y = roadSegs[2]->position.y;
				nasExits[(segCount/2)%4]->position.y = roadSegs[2]->position.y + 470.0f;
			}
		}
	}



	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	GL_ERRORS(); //print any errors produced by this setup code

	scene.draw(*camera);

	//use DrawLines to overlay some text:
	{
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.09f;
		lines.draw_text("DiD i jUst misS thE eXit fOr tHe gaS sTATion????",
			glm::vec3(-aspect + 0.1f * H, -1.0 + 0.1f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
		lines.draw_text("DiD i jUst misS thE eXit fOr tHe gaS sTATion????",
			glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + 0.1f * H + ofs, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}
}
