
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator:  $
   $Notice: $
   ======================================================================== */
#include "Game.h"
#define PI 3.14159265359f

using namespace std;

bool enableWind = true;
int WindX = 0, WindY = 0, WindZ = 1, WindForce = 50;

Game::Game(int width, int height) : screenWidth(width),
                                    screenHeight(height),
                                    endgame(false)
{
    init();
}

//Check opengl version
void Game::startupGLDiagnostics()
{
    // print diagnostic information
    printf( "    GL VENDOR: %s\n", glGetString( GL_VENDOR ) );
    printf( "      VERSION: %s\n", glGetString( GL_VERSION ) );
    printf( "     RENDERER: %s\n", glGetString( GL_RENDERER ) );
    printf( "GLSL VERSION : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION) );

    // test if we've got GL 3.0
    if( !GLEW_VERSION_3_0 )
    {
        printf( "OpenGL 3.0 not supported.\n" 
                "Please update your drivers and/or buy a better graphics card."
                );
    }
}

//Init opengl window and context
int Game::init()
{
    // SDL Init
    if(!glfwInit())
    {
        std::cout << "Erreur lors de l'initialisation de GLFW"  << std::endl; 
        return(-1);
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int const DPI = 2; // For retina screens only
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    int const DPI = 1;
# endif

    // Create SDL Wikndow
    //window = SDL_CreateWindow("Dungeon Crawler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN| SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);
    window = glfwCreateWindow(screenWidth, screenHeight, "DEMO", NULL, NULL );
    if(window == 0)
    {
        std::cout << "Erreur lors de la creation de la fenetre"  << std::endl;
        glfwTerminate();
        return(-1);
    }

    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return(-1);
    }

    startupGLDiagnostics();

    //glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);    // enable Z-buffering
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetInputMode( window, GLFW_STICKY_KEYS, GL_TRUE );
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);

    // Enable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );
    GLenum glerr = GL_NO_ERROR;
    glerr = glGetError();

    ImGui_ImplGlfwGL3_Init(window, true);
    return(0);
}

//close game
void Game::close()
{
    endgame = true;
}

struct Particle
{
    glm::vec3 position;
    glm::vec3 tmp_position;
    glm::vec3 forces;
    glm::vec3 velocity;
    glm::vec3 d;
    float mass;
    float inv_mass;
    bool enabled;

    Particle(glm::vec3 p, float m) 
            :position(p)
            ,tmp_position(p)
            ,enabled(true)
            ,forces(glm::vec3(0.0))
            ,velocity(glm::vec3(0.0))
            ,d(glm::vec3(0.0))
    {
 
        if(m < 0.001) {
            m = 0.001;
        }
 
        mass = m;
        inv_mass = 1.0 / mass;
    }
 
};

struct Hair
{
    Hair( int num, float d, glm::vec3 root, glm::vec3 d_x, glm::vec3 d_y)
    {
		patch_size = 16;
		dy = d_y;
		dx = d_x;
        setup(num, d, root);		
    }
    
    void setup( int num, float d, glm::vec3 root )
    {        
        length = d;
        glm::vec3 pos = root;
		float mass = 0.01f;
		int size = num + ceil((num - patch_size) / (patch_size - 1));
		///size = num;
		positions = new GLfloat[3 * size];
        for( int i = 0; i < size; ++i)
        {
            Particle* p = new Particle(pos, mass);
            particles.push_back(p);
            positions[3*i] = p->position.x;
            positions[(3*i)+1] = p->position.y;
            positions[(3*i)+2] = p->position.z;
			if ( ( i != 0) && ((i+1) % patch_size) == 0)
			{
				++i;
				particles.push_back(p);
				positions[3 * i] = p->position.x;
				positions[(3 * i) + 1] = p->position.y;
				positions[(3 * i) + 2] = p->position.z;				
			}
            pos.y -= d;            
        }
        particles[0]->enabled = false;

        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 3*size*sizeof(GLfloat), positions, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        currentTime = glfwGetTime();
    }

    void addForce(glm::vec3 f)
    {
        for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
            Particle* p = *it;
            if(p->enabled) {
                p->forces = f;
            }
        }
    }

    void update()
    {
        float dt = (glfwGetTime() - currentTime);  // Time step
		dt = 0.01f;
		float Sdamping = 0.9f;  // Correction scale, must be between [0, 1], 1 = fully compensated
		
		//Collision sphere
		glm::vec3 sphereOrigin = glm::vec3(0.0, 0.0, 0.0);
		float sphereRadius = 0.5;
 
        // update velocities
		int i = 0;
        for(std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it) {
            Particle* p = *it;
            if(!p->enabled) {
                p->tmp_position = p->position;
				i++;
                continue;
            }

			if ((i % patch_size) == 0)
			{
				++i;
				continue;
			}
            p->velocity = p->velocity + dt * (p->forces * p->inv_mass);
            p->tmp_position += (p->velocity * dt);
            p->forces = glm::vec3(0.0, -2.0, 0.0); //Add initial force = gravity
            //p->velocity *= 0.99;
			i++;
        }    
        // solve constraints
        glm::vec3 dir;
        glm::vec3 curr_pos;
        for(size_t i = 1; i < particles.size(); ++i) {
			Particle* pa = particles[i-1];
			Particle* pb = particles[i];
			if ((i % patch_size) == 0)
			{
				continue;
			}
			
			if ((pb->tmp_position.x - sphereOrigin.x) * (pb->tmp_position.x - sphereOrigin.x)
				+ (pb->tmp_position.y - sphereOrigin.y) * (pb->tmp_position.y - sphereOrigin.y)
				+ (pb->tmp_position.z - sphereOrigin.z) * (pb->tmp_position.z - sphereOrigin.z)
				<= sphereRadius*sphereRadius)
			{
				glm::vec3 dir = pb->tmp_position - sphereOrigin;
				dir = glm::normalize(dir);
				pb->tmp_position = sphereOrigin + dir * sphereRadius;
			}
			curr_pos = pb->tmp_position;
            dir = pb->tmp_position - pa->tmp_position;
            dir = glm::normalize(dir);
            pb->tmp_position = pa->tmp_position + dir * length;
            pb->d = curr_pos - pb->tmp_position;
        }    
 
        for(size_t i = 1; i < particles.size(); ++i) {
            Particle* pa = particles[i-1];
			Particle* pb;
			pb = particles[i];
			if (i != (particles.size()-1) && (i % patch_size ==0))
				pb= particles[++i];
            if(!pa->enabled) {
                continue;
            }

            pa->velocity = ((pa->tmp_position - pa->position) / dt) + Sdamping *  (pb->d / dt);
            pa->position = pa->tmp_position;
        }
 
        Particle* last = particles.back();
		last->velocity = (last->tmp_position - last->position) / dt;
        last->position = last->tmp_position;
		

        for(int i = 0; i < particles.size(); ++i)
        {
            positions[3*i] = particles[i]->position.x;
            positions[(3*i)+1] = particles[i]->position.y;
            positions[(3*i)+2] = particles[i]->position.z;          
        }
    }

    void draw( Shader* s)
    {
		glUniform3fv(glGetUniformLocation(s->getProgram(), "d_x"), 1, &dx[0]);
		glUniform3fv(glGetUniformLocation(s->getProgram(), "d_y"), 1, &dy[0]);
        glPatchParameteri( GL_PATCH_VERTICES, patch_size );
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,  3*particles.size()*sizeof(GLfloat) , positions, GL_STATIC_DRAW);
        glDrawArrays(GL_PATCHES, 0, particles.size());  
		Utils::checkGlError("Hair rendering : single tesselation");
    }

    GLfloat* positions;
    GLuint VAO, VBO;
    std::vector<Particle*> particles;
    glm::vec3 color;
    float length;
    float currentTime;
	int patch_size;
	glm::vec3 dy;
	glm::vec3 dx;
};

static void renderMultiStrands(Hair h1, Hair h2, Hair h3)
{
	int size = 3 * h1.particles.size();
	GLfloat* positions = new GLfloat[ 3*size];
	int idx;
	for (int i = 0; i < h1.particles.size(); ++i)
	{
		idx = 9 * i;
		positions[idx] = h1.particles[i]->position.x;
		positions[idx+1] = h1.particles[i]->position.y;
		positions[idx+2] = h1.particles[i]->position.z;
		positions[idx+3] = h2.particles[i]->position.x;
		positions[idx + 4] = h2.particles[i]->position.y;
		positions[idx + 5] = h2.particles[i]->position.z;
		positions[idx+6] = h3.particles[i]->position.x;
		positions[idx + 7] = h3.particles[i]->position.y;
		positions[idx + 8] = h3.particles[i]->position.z;
	}
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 3 *size*sizeof(GLfloat), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glPatchParameteri(GL_PATCH_VERTICES, 16);
	glDrawArrays(GL_PATCHES, 0, size);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	delete[] positions;
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ControllerManager& cm = ControllerManager::getInstance();    
    cm.keyboardCallback( window, key, scancode, action, mods);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    ControllerManager& cm = ControllerManager::getInstance();
    cm.mouseCursorCallback( window, xpos, ypos );
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ControllerManager& cm = ControllerManager::getInstance();
    cm.mouseCallback( window, button, action, mods );
}

void Game::scene1( Player* p, Skybox* skybox, Times times )
{
    //Make hair
    static std::vector<Hair> vh;
    if ( vh.empty() )
    {
		float head_size = 0.5;
		glm::vec3 head_center = glm::vec3(0.f, 0.f, 0.f);
		float theta = 0.0f;
		float phi = 0.f;
		for (int i = 0; i < 10; ++i)
		{
			theta += (PI/2) / 10;
			for (int j = 0; j < 30; j++)
			{
				phi += (PI) / 30;
				float x = head_size * sin(theta) * cos(phi);
				float y = head_size * sin(theta) * sin(phi);
				float z = head_size * cos(theta);
				glm::vec3 p = glm::vec3(x, z, y);
				glm::vec3 n = glm::normalize(p - head_center);
				glm::vec3 dx = glm::normalize(glm::vec3( -y , 0.f, x));
				glm::vec3 dy = glm::cross( n, dx );
				Hair h(32, 0.03f, glm::vec3(x, z, y), dx, dy);
				h.addForce(glm::vec3(0.01, 0.0, 0.0));
				vh.push_back(h);
			}
			phi = 0.f;
		}
		glLineWidth(0.2f);
    }
    //Get needed assets
    ShaderManager& shaderManager = Manager<ShaderManager>::instance();
	ModelManager& modelManager = Manager<ModelManager>::instance();
    TextureManager& textureManager = Manager<TextureManager>::instance();
    VaoManager& vaoManager = Manager<VaoManager>::instance();
    FboManager& fboManager = Manager<FboManager>::instance();

    glm::mat4 projection = glm::perspective(p->getCamera()->getZoom(),
                                            (float)screenWidth/(float)screenHeight,
                                            0.1f, 100.0f);
    glm::mat4 view = glm::mat4();

    int instanceCount = 1;
    int pointLightCount = 1;
    int directionalLightCount = 1;
    int spotLightCount = 1;
    p->move(times.deltaTime);
    float t = times.globalTime;       
    //Clean FBOs
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport( 0, 0, screenWidth, screenHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"] );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Create matrices
    glm::mat4 worldToView = p->getCamera()->getViewMatrix();
    glm::mat4 objectToWorld = glm::scale(glm::mat4(), glm::vec3(0.48,0.48,0.48));
    glm::mat4 mv = worldToView * objectToWorld;
    glm::mat4 mvp = projection * mv;
    glm::mat4 inverseProjection = glm::inverse( projection );
    //Render in GBUFFER
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["gbuffer"]);
    shaderManager["gbuffer"]->use();
    // Select textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["brick_diff"]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager["brick_spec"]);	
    // Upload uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "InstanceCount"), (int) instanceCount);
    glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "SpecularPower"), 30.f);
    glUniform1f(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Time"), t);
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "Specular"), 1);
	modelManager["sphere"]->Draw(shaderManager["gbuffer"]);
	objectToWorld = glm::translate(glm::mat4(), glm::vec3(0.f, -2.f, 0.f));
	mv = worldToView * objectToWorld;
	mvp = projection * mv;
	inverseProjection = glm::inverse(projection);
	Utils::checkGlError("sphere");
	glBindVertexArray(vaoManager["plane"]);
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["gbuffer"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    //Render scene
    shaderManager["hair"]->use();
	objectToWorld = glm::mat4();
	mv = worldToView * objectToWorld;
	mvp = projection * mv;
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["hair"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["hair"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
    glUniform1i(glGetUniformLocation(shaderManager["hair"]->getProgram(), "InstanceCount"), (int) instanceCount);
    glUniform1f(glGetUniformLocation(shaderManager["hair"]->getProgram(), "SpecularPower"), 30.f);
    glUniform1f(glGetUniformLocation(shaderManager["hair"]->getProgram(), "Time"), t);
    glUniform1i(glGetUniformLocation(shaderManager["hair"]->getProgram(), "Diffuse"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["hair"]->getProgram(), "Specular"), 1);
	shaderManager["hairMulti"]->use();
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(mv));
	glUniform1i(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "InstanceCount"), (int)instanceCount);
	glUniform1f(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "SpecularPower"), 30.f);
	glUniform1f(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "Time"), t);
	glUniform1i(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "Diffuse"), 0);
	glUniform1i(glGetUniformLocation(shaderManager["hairMulti"]->getProgram(), "Specular"), 1);
    Utils::checkGlError("gbuffer0");
	shaderManager["hair"]->use();
	for (int i = 0; i < vh.size(); i++)
	{
		if (enableWind && WindForce != 0 && glm::vec3(WindX, WindZ, WindY) != glm::vec3(0))
		    vh[i].addForce( (float)WindForce / 100.f * glm::normalize(glm::vec3(WindX, WindZ, WindY)));
		vh[i].update();
#if 0
		if ((i + 1) % 3 == 0)
		{
			shaderManager["hair"]->use();
      		renderMultiStrands(vh[i - 2], vh[i - 1], vh[i]);
			shaderManager["hair"]->use();
		}
#endif
		vh[i].draw(shaderManager["hair"]);
	}
    shaderManager["hair"]->unuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);
    Utils::checkGlError("gbuffer");
    

    struct DirectionalLight
    {
        glm::vec3 direction;
        int padding;
        glm::vec3 color;
        float intensity;
        glm::mat4 worldToLightScreen;
    };

#if 1
    const int uboSize = 512;
    const int SPOT_LIGHT_COUNT = 1;
    // Map the spot light data UBO
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    char * directionalLightBuffer = (char *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, uboSize, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    // Bind the shadow FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["shadow"]);
    // Use shadow program
    shaderManager["shadow"]->use();
    glViewport(0, 0, 1024, 1024);

    for (int i = 0; i < directionalLightCount; ++i)
    {
        glm::vec3 lp = glm::vec3( 0.0, 4.0, -4.0);
        // Light space matrices
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
        glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.f));
        glm::mat4 objectToWorld;
        glm::mat4 objectToLight = worldToLight * objectToWorld;
        glm::mat4 objectToLightScreen = lightProjection * objectToLight;

        // Attach shadow texture for current light

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureManager["shadow"+to_string(i)], 0);
        // Clear only the depth buffer
        glClear(GL_DEPTH_BUFFER_BIT);

        // Update scene uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
        glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));

        // Render the scene
        //Render scene
		for (int i = 0; i < vh.size(); i++)
		{
#if 0
			if ((i + 1) % 3 == 0)
			{
				shaderManager["hair"]->use();
				renderMultiStrands(vh[i - 2], vh[i - 1], vh[i]);
				shaderManager["hair"]->use();
			}
#endif
			vh[i].draw(shaderManager["shadow"]);
		}
	objectToWorld = glm::scale(glm::mat4(), glm::vec3(0.48, 0.48, 0.48));
	objectToLight = worldToLight * objectToWorld;
	objectToLightScreen = lightProjection * objectToLight;
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MVP"), 1, GL_FALSE, glm::value_ptr(objectToLightScreen));
	glUniformMatrix4fv(glGetUniformLocation(shaderManager["shadow"]->getProgram(), "MV"), 1, GL_FALSE, glm::value_ptr(objectToLight));
	modelManager["sphere"]->Draw(shaderManager["shadow"]);
  }        
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Utils::checkGlError("Shadows");
#endif
    
#if 1
    //Render lighting in postfx fbo
    glBindFramebuffer(GL_FRAMEBUFFER, fboManager["fx"]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, textureManager["fx0"], 0);

    glViewport(0,0,screenWidth, screenHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    Utils::checkGlError("before lights");
    // Select textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    // Bind the same VAO for all lights
    glBindVertexArray(vaoManager["quad"]);

// Render directional lights
    shaderManager["dirLight"]->use();
    glUniformMatrix4fv(glGetUniformLocation(shaderManager["dirLight"]->getProgram(),"InverseProjection") , 1, 0, glm::value_ptr(inverseProjection));
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "ColorBuffer"), 0);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "NormalBuffer"), 1);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "DepthBuffer"), 2);
    glUniform1i(glGetUniformLocation(shaderManager["dirLight"]->getProgram(), "Shadow"), 3);

    glActiveTexture(GL_TEXTURE3);
    for (int i = 0; i < directionalLightCount; ++i)
    {
    glBindTexture(GL_TEXTURE_2D, textureManager["shadow" + to_string(i)]);
    glBindBuffer(GL_UNIFORM_BUFFER, fboManager["ubo"]);
    glm::vec3 lp = glm::vec3(0.f, 4.0f, -4.0f);
    // Light space matrices
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
    glm::mat4 worldToLight = glm::lookAt(lp, glm::vec3(0.0f), glm::vec3(1.0f));
    glm::mat4 objectToWorld;
    glm::mat4 objectToLight = worldToLight * objectToWorld;
    glm::mat4 objectToLightScreen = lightProjection * objectToLight;
        
    DirectionalLight d = { 
    glm::vec3( worldToView * glm::vec4(0.0, -1.0, -1.0, 0.0)), 0,
        glm::vec3(1.0, 1.0, 1.0),
        0.8f,
        objectToLightScreen * glm::inverse(worldToView)
        };
    DirectionalLight * directionalLightBuffer = (DirectionalLight *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, 512, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    *directionalLightBuffer = d;
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBufferBase(GL_UNIFORM_BUFFER, glGetUniformBlockIndex(shaderManager["dirLight"]->getProgram(), "light"), fboManager["ubo"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
}
    shaderManager["dirLight"]->unuse();
#endif
 
    Utils::checkGlError("Lighting");

    // Draw skybox as last
    glm::vec4 sunNDC = mvp * ((-10000.f)*glm::vec4(-1.f, -1.f, 0.f, 0.f));
    sunNDC = glm::vec4( glm::vec3(sunNDC)/sunNDC.w, 1.0);
    sunNDC += 1.0;
    sunNDC *= 0.5;
    view = glm::mat4(glm::mat3(p->getCamera()->getViewMatrix()));    // Remove any translation component of the view matrix
    skybox->display(view, projection, textureManager["gBufferDepth"]);

    Utils::checkGlError("Skybox");
    glDisable(GL_BLEND);
      
    //Draw final frame on screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(vaoManager["quad"]);
    glViewport(0,0, screenWidth, screenHeight);
    shaderManager["blitHDR"]->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, textureManager["fx0"]);
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["blitHDR"]->unuse();
    Utils::checkGlError("blit"); 

#if 1     
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shaderManager["blit"]->use();
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vaoManager["quad"]);
    // Viewport 
    glViewport( 0, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferColor"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    // Viewport 
    glViewport( screenWidth/4, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferNormals"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["depth"]->use();
    // Viewport 
    glViewport( screenWidth/4 * 2, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["gBufferDepth"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["depth"]->unuse();
    shaderManager["blit"]->use();
    // Viewport 
    glViewport( screenWidth/4 * 3, 0, screenWidth/4, screenHeight/4  );
    // Bind texture
    glBindTexture(GL_TEXTURE_2D, textureManager["fx3"]);
    // Draw quad
    glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, (void*)0);
    shaderManager["blit"]->unuse();
    #endif
}

void Game::loadShaders()
{
    ShaderManager& shaderManager = Manager<ShaderManager>::instance();
    //DEBUG SHADERS
    Shader* blitShader = new Shader("Blit shader");
    blitShader->attach(GL_VERTEX_SHADER, "assets/shaders/blit.vert");
    blitShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blit.frag");
    blitShader->link();
    GLuint blitVertShader = (*blitShader)[0];
    Shader* blitHDRShader = new Shader("HDR blit shader");
    blitHDRShader->attach(blitVertShader);
    blitHDRShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blitHDR.frag");
    blitHDRShader->link();
    Shader* blitDepth = new Shader("Depth blit");
    blitDepth->attach(blitVertShader);
    blitDepth->attach(GL_FRAGMENT_SHADER, "assets/shaders/blitDepth.frag");
    blitDepth->link();
    //RENDERING SHADERS
    Shader* gbuffer = new Shader("G-buffer");
    gbuffer->attach(GL_VERTEX_SHADER, "assets/shaders/gbuffer.vert");
    gbuffer->attach(GL_FRAGMENT_SHADER, "assets/shaders/gbuffer.frag");
    gbuffer->link();
    Shader* hair = new Shader( "Hair" );
    hair->attach(GL_VERTEX_SHADER, "assets/shaders/hair.vert");
    hair->attach(GL_TESS_CONTROL_SHADER, "assets/shaders/hair.tcs");
    hair->attach(GL_TESS_EVALUATION_SHADER, "assets/shaders/hair.tes");
    hair->attach(GL_FRAGMENT_SHADER, "assets/shaders/hair.frag");
    hair->link();
	Shader* hairMulti = new Shader("Hair");
	hairMulti->attach(GL_VERTEX_SHADER, "assets/shaders/hair.vert");
	hairMulti->attach(GL_TESS_CONTROL_SHADER, "assets/shaders/hairMulti.tcs");
	hairMulti->attach(GL_TESS_EVALUATION_SHADER, "assets/shaders/hairMulti.tes");
	hairMulti->attach(GL_FRAGMENT_SHADER, "assets/shaders/hair.frag");
	hairMulti->link();
    
    Shader* shadowShader = new Shader( "Shadows" );
    shadowShader->attach( (*gbuffer)[0] );
    shadowShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/shadow.frag");
    shadowShader->link();
    Shader* plShader = new Shader("Point light");
    plShader->attach(blitVertShader);
    plShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/pointlight.frag");
    plShader->link();
    Shader* slShader = new Shader("Spot light");
    slShader->attach(blitVertShader);
    slShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/spotlight.frag");
    slShader->link();
    Shader* dlShader = new Shader("Directional light");
    dlShader->attach(blitVertShader);
    dlShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/directionallight.frag");
    dlShader->link();
    Shader* plShadowShader = new Shader("piint light shadow");
    plShadowShader->attach(GL_VERTEX_SHADER, "assets/shaders/cubemap.vert");
    plShadowShader->attach(GL_GEOMETRY_SHADER, "assets/shaders/cubemap.geom");
    plShadowShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/cubemap.frag");
    plShadowShader->link();
    //POSTFX SHADERS
    Shader* blurShader = new Shader("Blur shader");
    blurShader->attach(GL_VERTEX_SHADER, "assets/shaders/blur.vert");
    blurShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/blur.frag");
    blurShader->link();
    Shader* brightShader = new Shader("Bright shader");
    brightShader->attach(blitVertShader);
    brightShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/extractBright.frag");
    brightShader->link();
    Shader* bloomShader = new Shader("Bloom shader");
    bloomShader->attach(blitVertShader);
    bloomShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/bloom.frag");
    bloomShader->link();
    Shader* lightShaftShader = new Shader("Light shaft");
    lightShaftShader->attach(blitVertShader);
    lightShaftShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/lightShaft.frag");
    lightShaftShader->link();
    Shader* sunShader = new Shader("Sun shader");
    sunShader->attach(blitVertShader);
    sunShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/sun.frag");
    sunShader->link();
    Shader* ssrShader = new Shader("SSR shader");
    ssrShader->attach(blitVertShader);
    ssrShader->attach(GL_FRAGMENT_SHADER, "assets/shaders/SSRR.frag");
    ssrShader->link();
    
    shaderManager.getManaged().insert( pair<string, Shader*>( "dirLight", dlShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "ssr", ssrShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "sun", sunShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "lightShaft", lightShaftShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "plShadow", plShadowShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "depth", blitDepth));
    shaderManager.getManaged().insert( pair<string, Shader*>( "shadow", shadowShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "pointLight", plShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "spotLight", slShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blur", blurShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "bloom", bloomShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "bright", brightShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "gbuffer", gbuffer));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blit", blitShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "blitHDR", blitHDRShader));
    shaderManager.getManaged().insert( pair<string, Shader*>( "hair", hair));
	shaderManager.getManaged().insert(pair<string, Shader*>("hairMulti", hairMulti));

    bloomShader->unuse();
}


void Game::loadGeometry()
{
    VaoManager& vaoManager = Manager<VaoManager>::instance();
	ModelManager& modelManager = Manager<ModelManager>::instance();
    // Load geometry
    int cube_triangleCount = 12;
    int cube_triangleList[] = {0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 19, 17, 20, 21, 22, 23, 24, 25, 26, };
    float cube_uvs[] = {0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f, 1.f, 1.f, 0.f, 0.f, 0.f, 1.f, 1.f, 0.f,  1.f, 0.f,  1.f, 1.f,  0.f, 1.f,  1.f, 1.f,  0.f, 0.f, 0.f, 0.f, 1.f, 1.f,  1.f, 0.f,  };
    float cube_vertices[] = {-0.5, -0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, -0.5, -0.5, -0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, -0.5, -0.5, 0.5, -0.5, -0.5, -0.5, 0.5, -0.5, 0.5, 0.5 };
    float cube_normals[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, };
    int plane_triangleCount = 2;
    int plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float plane_uvs[] = {0.f, 0.f, 0.f, 50.f, 50.f, 0.f, 50.f, 50.f};
    float plane_vertices[] = {-50.0, -1.0, 50.0, 50.0, -1.0, 50.0, -50.0, -1.0, -50.0, 50.0, -1.0, -50.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};
    // Vertex Array Object
    GLuint vao[3];
    glGenVertexArrays(3, vao);
    // Vertex Buffer Objects
    GLuint vbo[10];
    glGenBuffers(10, vbo);
    // Cube
    glBindVertexArray(vao[0]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_triangleList), cube_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_uvs), cube_uvs, GL_STATIC_DRAW);
    // Plane
    glBindVertexArray(vao[1]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    // Bind normals and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
    // Bind uv coords and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);
    // Quad
    glBindVertexArray(vao[2]);
    // Bind indices and upload data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // Bind vertices and upload data
    glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);

    // Unbind everything. Potentially illegal on some implementations
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	Model* sphere = new Model("assets/sphere.nff");
	modelManager.getManaged().insert(pair<string, Model*>("sphere", sphere));
	Utils::checkGlError("loadsphere");
    vaoManager.getManaged().insert( pair<string, VAO>("cube", {vao[0]}));
    vaoManager.getManaged().insert( pair<string, VAO>("plane", {vao[1]}));
    vaoManager.getManaged().insert( pair<string, VAO>("quad", {vao[2]}));    
}

void Game::initGbuffer()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();
    // Init frame buffers
    GLuint gbufferFbo;
    GLuint gbufferTextures[4];
    GLuint gbufferDrawBuffers[3];
    glGenTextures(4, gbufferTextures);

    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Create color texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Create normal texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Create depth texture
    glBindTexture(GL_TEXTURE_2D, gbufferTextures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Create Framebuffer Object
    glGenFramebuffers(1, &gbufferFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, gbufferFbo);
    gbufferDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    gbufferDrawBuffers[1] = GL_COLOR_ATTACHMENT1;
    gbufferDrawBuffers[2] = GL_COLOR_ATTACHMENT2;
    glDrawBuffers(3, gbufferDrawBuffers);
    // Attach textures to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, gbufferTextures[0], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1 , GL_TEXTURE_2D, gbufferTextures[1], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2 , GL_TEXTURE_2D, gbufferTextures[3], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gbufferTextures[2], 0);

    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error Gbuffer fbo" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0 );

    textureManager.getManaged().insert( pair<string, Tex>( "gBufferColor", {gbufferTextures[0]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferNormals", {gbufferTextures[1]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferDepth", {gbufferTextures[2]}));
    textureManager.getManaged().insert( pair<string, Tex>( "gBufferOccludee", {gbufferTextures[3]}));
    fboManager.getManaged().insert( pair<string, FBO>( "gbuffer", {gbufferFbo}));
}

void Game::initFX()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();

    GLuint fxFbo;
    GLuint fxDrawBuffers[1];
    glGenFramebuffers(1, &fxFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fxFbo);
    fxDrawBuffers[0] = GL_COLOR_ATTACHMENT0;
    glDrawBuffers(1, fxDrawBuffers);
    // Create Fx textures
    const int FX_TEXTURE_COUNT = 5;
    GLuint fxTextures[FX_TEXTURE_COUNT];
    glGenTextures(FX_TEXTURE_COUNT, fxTextures);
    for (int i = 0; i < FX_TEXTURE_COUNT; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, fxTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    // Attach first fx texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 , GL_TEXTURE_2D, fxTextures[0], 0);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error FX fbo" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    fboManager.getManaged().insert( pair<string, FBO>( "fx", {fxFbo}));
    textureManager.getManaged().insert( pair<string, Tex>( "fx0", {fxTextures[0]}));
    textureManager.getManaged().insert( pair<string, Tex>( "fx1", {fxTextures[1]}));
    textureManager.getManaged().insert( pair<string, Tex>( "fx2", {fxTextures[2]}));
    textureManager.getManaged().insert( pair<string, Tex>( "fx3", {fxTextures[3]}));
    textureManager.getManaged().insert( pair<string, Tex>( "fx4", {fxTextures[4]}));
}

void Game::initShadows()
{
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();

    const int TEX_SIZE = 1024;
    const int LIGHT_COUNT = 2;
    // Create shadow FBO
    GLuint shadowFbo;
    glGenFramebuffers(1, &shadowFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
    // Create shadow textures
    GLuint shadowTextures[LIGHT_COUNT];
    glGenTextures(LIGHT_COUNT, shadowTextures);
    for (int i = 0; i < LIGHT_COUNT; ++i) 
    {
        glBindTexture(GL_TEXTURE_2D, shadowTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,  GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,  GL_LEQUAL);
        textureManager.getManaged().insert( pair<string, Tex>( "shadow"+to_string(i), {shadowTextures[i]}));
    }
    fboManager.getManaged().insert(pair<string, FBO>( "shadow", {shadowFbo}));
    // Create a render buffer since we don't need to read shadow color 
    // in a texture
    GLuint shadowRenderBuffer;
    glGenRenderbuffers(1, &shadowRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, shadowRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, TEX_SIZE, TEX_SIZE);
    // Attach the first texture to the depth attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTextures[0], 0);
    // Attach the renderbuffer
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, shadowRenderBuffer);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error shadow fbo" << std::endl;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Omnidirectional Shadow
    GLuint plShadowFBO;
    glGenFramebuffers(1, &plShadowFBO);
    GLuint plShadowTex;
    glGenTextures(1, &plShadowTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, plShadowTex);
    for (GLuint i = 0; i < 6; ++i )
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, TEX_SIZE, TEX_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0); 
    textureManager.getManaged().insert(pair<string, Tex>( "plShadow", {plShadowTex}));
    glBindFramebuffer(GL_FRAMEBUFFER, plShadowFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, plShadowTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if ( glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
        std::cout << "Error shadow fbo" << std::endl;
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    fboManager.getManaged().insert(pair<string, FBO>("plShadow", {plShadowFBO}));


}

//Load models into the singleton modelmanager
void Game::loadAssets()
{

    std::cout << std::endl << "---- LOAD ASSETS AND CREATE FBOs ---" << std::endl << std::endl;
    //Manager<Model*>& modelManager = Manager<Model*>::getInstance();
    TextureManager& textureManager = Manager<TextureManager>::instance();    
    FboManager& fboManager = Manager<FboManager>::instance();
	ModelManager& modelManager = Manager<ModelManager>::instance();

    loadShaders();
    loadGeometry();
    initGbuffer();
    initFX();
    initShadows();

    // Load images and upload textures
    GLuint textures[2];
    glGenTextures(2, textures);
    int x;
    int y;
    int comp;

    unsigned char * diffuse = stbi_load("assets/textures/spnza_bricks_a_diff.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned char * spec = stbi_load("assets/textures/spnza_bricks_a_spec.tga", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

    textureManager.getManaged().insert( pair<string, Tex>( "brick_diff", {textures[0]}));
    textureManager.getManaged().insert( pair<string, Tex>( "brick_spec", {textures[1]}));

    //create UBO
    // Update and bind uniform buffer object
    GLuint ubo[1];
    glGenBuffers(1, ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo[0]);
    // Ignore ubo size, allocate it sufficiently big for all light data structures
    GLint uboSize = 512;
    glBufferData(GL_UNIFORM_BUFFER, uboSize, 0, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    fboManager.getManaged().insert( pair<string, FBO>("ubo", {ubo[0]}));

    Utils::checkGlError("LOAD ASSETS");
    //manager.getModels().insert( pair<string, Model*>( "scalp", new Model( "../Assets/Models/Hair/scalp_mesh.obj" )));
}

//Main rendering loop
int Game::mainLoop()
{
    /*
      Create all the vars that we may need for rendering such as shader, VBO, FBO, etc
      .     */
    Player* p = new Player();
    p->setPosition(glm::vec3(-1.f, 0.f, 0.f));
    
    glfwSetKeyCallback( window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    loadAssets();
    //create skybox
    vector<const GLchar*> faces;
    faces.push_back("assets/skyboxes/Test/xpos.png");
    faces.push_back("assets/skyboxes/Test/xneg.png");
    faces.push_back("assets/skyboxes/Test/ypos.png");
    faces.push_back("assets/skyboxes/Test/yneg.png");
    faces.push_back("assets/skyboxes/Test/zpos.png");
    faces.push_back("assets/skyboxes/Test/zneg.png");
    Skybox* skybox = new Skybox(faces); 
    Utils::checkGlError("skybox error");

#if 1




    Times t;
    while(glfwGetKey( window, GLFW_KEY_ESCAPE ) != GLFW_PRESS )
    {
        ImGui_ImplGlfwGL3_NewFrame();
        t.globalTime = glfwGetTime();
        t.elapsedTime = t.globalTime - t.startTime;
        t.deltaTime = t.globalTime - t.previousTime;
        t.previousTime = t.globalTime;
        scene1(p, skybox, t);
#if 1
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("aogl");
        ImGui::DragInt("WinDir X", &WindX, .1f, -5, 5);
        ImGui::DragInt("WinDir Y", &WindY, .1f, -5, 5);
        ImGui::DragInt("WinDir Z", &WindZ, .1f,-5, 5);
		ImGui::DragInt("WinForce", &WindForce, .1f, 0, 100);
		ImGui::Checkbox("Enable wind", &enableWind);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Render();
#endif
        Utils::checkGlError("end frame");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //NOTE(marc) : We should properly clean the app, but since this will be the last
    //thing the program will do, it will clean them for us
#endif
    // Close OpenGL window and terminate GLFW
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
    return(0);
}
