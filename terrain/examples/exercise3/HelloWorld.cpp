/*
 * Proland: a procedural landscape rendering library.
 * Website : http://proland.inrialpes.fr/
 * Copyright (c) 2008-2015 INRIA - LJK (CNRS - Grenoble University)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors 
 * may be used to endorse or promote products derived from this software without 
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Proland is distributed under the Berkeley Software Distribution 3 Licence. 
 * For any assistance, feedback and enquiries about training programs, you can check out the 
 * contact page on our website : 
 * http://proland.inrialpes.fr/
 */
/*
 * Main authors: Eric Bruneton, Antoine Begault, Guillaume Piolat.
 */

#include <stdlib.h>

#include "ork/core/FileLogger.h"
#include "ork/render/FrameBuffer.h"
#include "ork/resource/ResourceTemplate.h"
#include "ork/resource/XMLResourceLoader.h"
#include "ork/scenegraph/SceneManager.h"
#include "ork/scenegraph/ShowLogTask.h"
#include "ork/ui/GlfwWindow.h"

#include "proland/ui/BasicViewHandler.h"
#include "proland/ui/twbar/TweakBarManager.h"
#include "proland/util/TerrainViewController.h"

#include "proland/TerrainPlugin.h"
#include "proland/terrain/TerrainNode.h"

using namespace ork;
using namespace proland;

class DrawTerrainBoxTask : public AbstractTask
{
public:
    DrawTerrainBoxTask(const QualifiedName &terrain, const QualifiedName &mesh, bool culling) :
        AbstractTask("DrawTerrainBoxTask")
	{
	    init(terrain, mesh, culling);
	}

    virtual ~DrawTerrainBoxTask()
    {
	}

    virtual ptr<Task> getTask(ptr<Object> context)
    {
        ptr<SceneNode> n = context.cast<Method>()->getOwner();

		ptr<TerrainNode> t = NULL;
		ptr<SceneNode> target = terrain.getTarget(n);
		if (target == NULL) {
			t = n->getOwner()->getResourceManager()->loadResource(terrain.name).cast<TerrainNode>();
		} else {
			t = target->getField(terrain.name).cast<TerrainNode>();
		}
		if (t == NULL) {
			if (Logger::ERROR_LOGGER != NULL) {
				Logger::ERROR_LOGGER->log("TERRAIN", "DrawTerrainBox : cannot find terrain '" + terrain.target + "." + terrain.name + "'");
			}
			throw exception();
		}

		ptr<MeshBuffers> m = NULL;
		target = mesh.getTarget(n);
		if (target == NULL) {
			m = n->getOwner()->getResourceManager()->loadResource(mesh.name + ".mesh").cast<MeshBuffers>();
		} else {
			m = target->getMesh(mesh.name);
		}
		if (m == NULL) {
			if (Logger::ERROR_LOGGER != NULL) {
				Logger::ERROR_LOGGER->log("SCENEGRAPH", "DrawMesh : cannot find mesh '" + mesh.target + "." + mesh.name + "'");
			}
			throw exception();
		}
		return new Impl(n, t, m, culling);
	}

protected:
    DrawTerrainBoxTask() : AbstractTask("DrawTerrainBoxTask")
	{
	}

    void init(const QualifiedName &terrain, const QualifiedName &mesh, bool culling)
	{
		this->terrain = terrain;
		this->mesh = mesh;
		this->culling = culling;
	}

    void swap(ptr<DrawTerrainBoxTask> t)
    {
	    std::swap(*this, *t);
	}

private:
    QualifiedName terrain;

    QualifiedName mesh;

    bool culling;

    class Impl : public Task
    {
    public:
        ptr<SceneNode> n;

        ptr<TerrainNode> t;

        ptr<MeshBuffers> m;

        bool culling;

        Impl(ptr<SceneNode> n, ptr<TerrainNode> t, ptr<MeshBuffers> m, bool culling) :
		    Task("DrawTerrainBox", true, 0), n(n), t(t), m(m), culling(culling)
        {
		}

        virtual ~Impl()
        {
		}

        virtual bool run()
        {
			if (t != NULL) {
				if (Logger::DEBUG_LOGGER != NULL) {
					Logger::DEBUG_LOGGER->log("TERRAIN", "DrawTerrainBox");
				}
				ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
				ptr<Program> p = SceneManager::getCurrentProgram();
				t->deform->setUniforms(n, t, p);
				drawQuad(t->root);
			}
			return true;
		}

        void drawQuad(ptr<TerrainQuad> q)
        {
			if (culling && q->visible == SceneManager::INVISIBLE) {
				return;
			}
			if (q->isLeaf()) {
                ptr<Program> p = SceneManager::getCurrentProgram();
                p->getUniform2f("zminmax")->set(vec2f(q->zmin, q->zmax));
				t->deform->setUniforms(n, q, p);
    			if (m->nindices == 0) {
					SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, 0, m->nvertices);
				} else {
					SceneManager::getCurrentFrameBuffer()->draw(p, *m, m->mode, 0, m->nindices);
				}
			} else {
				for (int i = 0; i < 4; ++i) {
					drawQuad(q->children[i]);
				}
			}
		}
    };
};

class DrawTerrainBoxTaskResource : public ResourceTemplate<40, DrawTerrainBoxTask>
{
public:
    DrawTerrainBoxTaskResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc, const TiXmlElement *e = NULL) :
        ResourceTemplate<40, DrawTerrainBoxTask>(manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,mesh,culling,");
        string n = getParameter(desc, e, "name");
        string m = getParameter(desc, e, "mesh");
        bool culling = false;
        if (e->Attribute("culling") != NULL && strcmp(e->Attribute("culling"), "true") == 0) {
            culling = true;
        }
        init(QualifiedName(n), QualifiedName(m), culling);
    }
};

extern const char drawTerrainBox[] = "drawTerrainBox";

static ResourceFactory::Type<drawTerrainBox, DrawTerrainBoxTaskResource> DrawTerrainBoxTaskType;

class HelloWorld : public GlfwWindow, public ViewManager
{
public:
    ptr<SceneManager> manager;
    ptr<TerrainViewController> controller;
    ptr<BasicViewHandler> view;
    ptr<EventHandler> ui;

    HelloWorld() : GlfwWindow(Window::Parameters().size(1024, 768))
    {
        FileLogger::File *out = new FileLogger::File("log.html");
        Logger::INFO_LOGGER = new FileLogger("INFO", out, Logger::INFO_LOGGER);
        Logger::WARNING_LOGGER = new FileLogger("WARNING", out, Logger::WARNING_LOGGER);
        Logger::ERROR_LOGGER = new FileLogger("ERROR", out, Logger::ERROR_LOGGER);

        ptr<XMLResourceLoader> resLoader = new XMLResourceLoader();
        resLoader->addPath(".");
        resLoader->addArchive("helloworld.xml");

        ptr<ResourceManager> resManager = new ResourceManager(resLoader, 8);

        manager = new SceneManager();
        manager->setResourceManager(resManager);

        manager->setScheduler(resManager->loadResource("defaultScheduler").cast<Scheduler>());
        manager->setRoot(resManager->loadResource("scene").cast<SceneNode>());
        manager->setCameraNode("camera");
        manager->setCameraMethod("draw");

        controller = new TerrainViewController(manager->getCameraNode(), 50000.0);
        view = new BasicViewHandler(true, this, NULL);

        ptr<TweakBarManager> tb = resManager->loadResource("ui").cast<TweakBarManager>();
        tb->setNext(view);
        ui = tb;
    }

    virtual ~HelloWorld()
    {
    }

    virtual void redisplay(double t, double dt)
    {
        ui->redisplay(t, dt);
        GlfwWindow::redisplay(t, dt);

        if (Logger::ERROR_LOGGER != NULL) {
            Logger::ERROR_LOGGER->flush();
        }
    }

    virtual void reshape(int x, int y)
    {
        ptr<FrameBuffer> fb = FrameBuffer::getDefault();
        fb->setDepthTest(true, LESS);
		fb->setViewport(vec4<GLint>(0, 0, x, y));
        ui->reshape(x, y);
        GlfwWindow::reshape(x, y);
        idle(false);
    }

    virtual void idle(bool damaged)
    {
        GlfwWindow::idle(damaged);
        if (damaged) {
            updateResources();
        }
        ui->idle(damaged);
    }

    virtual bool mouseClick(button b, state s, modifier m, int x, int y)
    {
        return ui->mouseClick(b, s, m, x, y);
    }

    virtual bool mouseMotion(int x, int y)
    {
        return ui->mouseMotion(x, y);
    }

    virtual bool mousePassiveMotion(int x, int y)
    {
        return ui->mousePassiveMotion(x, y);
    }

    virtual bool mouseWheel(wheel b, modifier m, int x, int y)
    {
        return ui->mouseWheel(b, m, x, y);
    }

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        if (ui->keyTyped(c, m, x, y)) {
            return true;
        }
        if (c == 27) {
            ::exit(0);
        }
        return false;
    }

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y)
    {
        return ui->keyReleased(c, m, x, y);
    }

    virtual bool specialKey(key k, modifier m, int x, int y)
    {
        if (ui->specialKey(k, m, x, y)) {
            return true;
        }

        switch (k) {
        case KEY_F1:
            ShowLogTask::enabled = !ShowLogTask::enabled;
            return true;
        case KEY_F5:
            updateResources();
            return true;
        default:
            break;
        }
        return false;
    }

    virtual bool specialKeyReleased(key k, modifier m, int x, int y)
    {
        return ui->specialKeyReleased(k, m, x, y);
    }

    virtual ptr<SceneManager> getScene()
    {
        return manager;
    }

    virtual ptr<TerrainViewController> getViewController()
    {
        return controller;
    }

    virtual vec3d getWorldCoordinates(int x, int y)
    {
        vec3d p = manager->getWorldCoordinates(x, y);
        if (abs(p.x) > 100000.0 || abs(p.y) > 100000.0 || abs(p.z) > 100000.0) {
            p = vec3d(NAN, NAN, NAN);
        }
        return p;
    }

    void updateResources()
    {
        BasicViewHandler::Position p;
        view->getPosition(p);
        manager->getResourceManager()->updateResources();
        controller->setNode(manager->getCameraNode());
        view->setPosition(p);
    }

    static void exit() {
        app.cast<HelloWorld>()->manager->getResourceManager()->close();
        Object::exit();
    }

    static static_ptr<Window> app;
};

static_ptr<Window> HelloWorld::app;

int main(int argc, char* argv[])
{
    initTerrainPlugin();
    atexit(HelloWorld::exit);
    HelloWorld::app = new HelloWorld();
    HelloWorld::app->start();
    return 0;
}
