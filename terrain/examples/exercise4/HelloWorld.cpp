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
#include "proland/producer/TileLayer.h"
#include "proland/producer/GPUTileStorage.h"

using namespace ork;
using namespace proland;

class DebugOrthoLayer : public TileLayer
{
public:
    DebugOrthoLayer(ptr<Font> f, ptr<Program> p, float fontHeight) : TileLayer("DebugOrthoLayer")
    {
        TileLayer::init(false);
        init(f, p, fontHeight);
    }

    virtual ~DebugOrthoLayer()
    {
    }

    virtual bool doCreateTile(int level, int tx, int ty, TileStorage::Slot *data)
    {
        if (Logger::DEBUG_LOGGER != NULL) {
            ostringstream oss;
            oss << "Debug tile " << getProducerId() << " " << level << " " << tx << " " << ty;
            Logger::DEBUG_LOGGER->log("ORTHO", oss.str());
        }

        ostringstream os;
        os << dynamic_cast<GPUTileStorage::GPUSlot*>(data)->l;

        ptr<FrameBuffer> fb = SceneManager::getCurrentFrameBuffer();
        vec4f vp = fb->getViewport().cast<float>();
        fb->setBlend(true, ADD, SRC_ALPHA, ONE_MINUS_SRC_ALPHA, ADD, ZERO, ONE);
        fontMesh->clear();
        font->addLine(vp, 2.0f, 2.0f, os.str(), fontHeight, 0xFF0000FF, fontMesh);
        fontU->set(font->getImage());
        fb->draw(fontProgram, *fontMesh);
        fb->setBlend(false);
        return true;
    }

protected:
    DebugOrthoLayer() : TileLayer("DebugOrthoLayer")
    {
    }

    void init(ptr<Font> f, ptr<Program> p, float fontHeight)
    {
        this->font = f;
        this->fontProgram = p;
        this->fontHeight = fontHeight;
        this->fontU = p->getUniformSampler("font");
        if (fontMesh == NULL) {
            fontMesh = new Mesh<Font::Vertex, unsigned int>(TRIANGLES, GPU_DYNAMIC);
            fontMesh->addAttributeType(0, 4, A16F, false);
            fontMesh->addAttributeType(1, 4, A8UI, true);
        }
    }

    virtual void swap(ptr<DebugOrthoLayer> p)
    {
        TileLayer::swap(p);
        std::swap(font, p->font);
        std::swap(fontProgram, p->fontProgram);
        std::swap(fontHeight, p->fontHeight);
    }

private:
    ptr<Font> font;

    ptr<Program> fontProgram;

    float fontHeight;

    ptr<UniformSampler> fontU;

    static static_ptr< Mesh<Font::Vertex, unsigned int> > fontMesh;
};

static_ptr< Mesh<Font::Vertex, unsigned int> > DebugOrthoLayer::fontMesh;

class DebugOrthoLayerResource : public ResourceTemplate<40, DebugOrthoLayer>
{
public:
    DebugOrthoLayerResource(ptr<ResourceManager> manager, const string &name, ptr<ResourceDescriptor> desc,
            const TiXmlElement *e = NULL) :
        ResourceTemplate<40, DebugOrthoLayer> (manager, name, desc)
    {
        e = e == NULL ? desc->descriptor : e;
        checkParameters(desc, e, "name,font,fontSize,fontProgram,");
        string fontName = "defaultFont";
        if (e->Attribute("font") != NULL) {
            fontName = Resource::getParameter(desc, e, "font");
        }
        ptr<Font> f = manager->loadResource(fontName).cast<Font>();

        float size = f->getTileHeight();
        if (e->Attribute("fontSize") != NULL) {
            getFloatParameter(desc, e, "fontSize", &size);
        }

        string fontProgram = "text;";
        if (e->Attribute("fontProgram") != NULL) {
            fontProgram = string(e->Attribute("fontProgram"));
        }
        ptr<Program> p = manager->loadResource(fontProgram).cast<Program>();

        init(f, p, size);
    }

    virtual bool prepareUpdate()
    {
        oldValue = NULL;
        newDesc = NULL;
        return true;
    }
};

extern const char debugOrthoLayer[] = "debugOrthoLayer";

static ResourceFactory::Type<debugOrthoLayer, DebugOrthoLayerResource> DebugOrthoLayerType;

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
