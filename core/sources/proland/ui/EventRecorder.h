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

#ifndef _PROLAND_EVENTRECORDER_H_
#define _PROLAND_EVENTRECORDER_H_

#include <vector>

#include "ork/core/Timer.h"
#include "ork/render/Program.h"
#include "ork/render/Texture2D.h"
#include "ork/ui/EventHandler.h"

using namespace std;

using namespace ork;

namespace proland
{

/**
 * An object that can be recorded by an EventRecorder.
 * See #EventRecorder.
 * @ingroup proland_ui
 * @authors Eric Bruneton, Antoine Begault, Guillaume Piolat
 */
class PROLAND_API Recordable
{
public:
    /**
     * Saves the current scene state. This method is called just before
     * starting recording.
     */
    virtual void saveState() = 0;

    /**
     * Restores the scene state that was saved during the last call to
     * #saveState.
     */
    virtual void restoreState() = 0;
};

/**
 * An EventHandler that can record and replay events, and save a video
 * during replay. Recording is started and stopped with F12. Replay is
 * triggered with F11, or SHIFT+F11 to record a video while replaying.
 * The recorded video does not contain all the original frames, but only
 * 25 frames per second.
 * Since replay cannot be done at the same rate as the original events,
 * especially when saving a video, the events are recorded together with
 * the time at which they originally occured, to be able to override the
 * real-time during replay with the original time during recording.
 * Also if modifications have been performed to the scene during the
 * recording, the initial scene state must be restored before replaying
 * the event. This is managed by a Recordable, that is used by this
 * EventRecorder.
 * @ingroup proland_ui
 */
class PROLAND_API EventRecorder : public EventHandler
{
public:
    /**
     * A recorded event.
     */
    struct PROLAND_API Event
    {
        enum EventType
        {
            DISPLAY, ///< EventHandler#redisplay
            MOUSE, ///< EventHandler#mouseClick
            MOTION, ///< EventHandler#mouseMotion
            PASSIVEMOTION, ///< EventHandler#mousePassiveMotion
            WHEEL, ///< EventHandler#mouseWheel
            KEYBOARD, ///< EventHandler#keyType and EventHandler#keyReleased
            SPECIAL ///< EventHandler#specialKey and EventHandler#specialKeyReleased
        };

        EventType kind; ///< type of this event

        union {
            struct {
                double t;

                double dt;

                float groundHeight;
            } display;

            struct {
                int m; ///< the modifiers state when this event was recorded.

                int arg1; ///< first event argument. Value depends on event type.

                int arg2; ///< second event argument. Value depends on event type.

                int arg3; ///< third event argument. Value depends on event type.

                int arg4; ///< fourth event argument. Value depends on event type.
            } e;
        };

        /**
         * Creates an uninitialized event.
         **/
        Event();

        /**
         * Creates a new DISPLAY event.
         *
         * @param t the time at which this event was recorded.
         * @param
         */
        Event(double t, double dt, float groundHeight);

        /**
         * Creates a new event.
         *
         * @param kind the event type.
         * @param m the current modifiers state.
         * @param arg1 first event argumemt. Value depends on event type.
         * @param arg2 second event argumemt. Value depends on event type.
         * @param arg3 third event argumemt. Value depends on event type.
         * @param arg4 fourth event argumemt. Value depends on event type.
         */
        Event(EventType kind, int m, int arg1 = 0, int arg2 = 0, int arg3 = 0, int arg4 = 0);
    };

    /**
     * Creates a new EventRecorder.
     *
     * @param eventFile the name of a file containing stored events to be
     *      replayed, or NULL.
     * @param autoSave true to save events in a temporary file at each mouse
     *      click and key typed event (useful for saving events if scenario
     *      leads to a crash).
     * @param frames the file name format to be used to save the video frames.
     *      For instance "video/frame-%d.tga" or "video/frame-%.4d.tga".
     * @param cursorProg the GLSL program used to draw the cursor on the screen.
     * @param cursor a cursor texture to display the cursor position during
     *      replay.
     * @param next the EventHandler that must handle the events recorded and
     *      replayed by this EventRecorder.
     */
    EventRecorder(const char *eventFile, bool autoSave, const char *frames, ptr<Program> cursorProg, ptr<Texture2D> cursor, ptr<EventHandler> next);

    /**
     * Deletes this EventRecorder.
     */
    virtual ~EventRecorder();

    void setEventFile(const char *eventFile);

    virtual void redisplay(double t, double dt);

    virtual void reshape(int x, int y);

    virtual void idle(bool damaged);

    virtual bool mouseClick(button b, state s, modifier m, int x, int y);

    virtual bool mouseMotion(int x, int y);

    virtual bool mousePassiveMotion(int x, int y);

    virtual bool mouseWheel(wheel b, modifier m, int x, int y);

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y);

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y);

    virtual bool specialKey(key k, modifier m, int x, int y);

    virtual bool specialKeyReleased(key k, modifier m, int x, int y);

protected:
    /**
     * The object used to save/restore the framerate and the scene state.
     */
    Recordable *r;

    /**
     * Creates an uninitialized EventRecorder.
     */
    EventRecorder();

    /**
     * Initializes this EventRecorder.
     * See #EventRecorder.
     */
    void init(const char *eventFile, bool autoSave, const char *frames, ptr<Program> cursorProg, ptr<Texture2D> cursor, ptr<EventHandler> next);

    /**
     * Returns the used to save/restore the framerate and the scene state.
     */
    virtual Recordable *getRecorded() = 0;

    void swap(ptr<EventRecorder> eventRecorder);

private:
    /**
     * The name of a file containing stored events to be replayed, or NULL.
     */
    const char *eventFile;

    /**
     * True to save events in a temporary file at each mouse click and key
     * typed event (useful for saving events if scenario leads to a crash).
     */
    bool autoSave;

    /**
     * The file name format to be used to save the video frames.
     */
    const char *frames;

    /**
     * The width of the cursor texture #cursor.
     */
    int cursW;

    /**
     * The height of the cursor texture #cursor.
     */
    int cursH;

    /**
     * The GLSL program used to draw the cursor on replaying videos.
     */
    ptr<Program> cursorProg;

    /**
     * The cursor RGBA texture pixels.
     */
    ptr<Texture2D> cursor;

    /**
     * The cursor texture sampler.
     */
    ptr<UniformSampler> cursorSamplerU;

    /**
     * Rescaling factor used to draw the cursor on the screen.
     */
    ptr<Uniform4f> rescaleU;

    /**
     * Rescaling factor used to draw the cursor on the screen.
     */
    ptr<Uniform3f> correctU;

    /**
     * True if this EventHandler is currently recording events.
     */
    bool isRecording;

    /**
     * True if this EventHandler is currently replaying events.
     */
    bool isPlaying;

    /**
     * True if a video must be saved during replaying.
     */
    bool saveVideo;

    /**
     * The index of the recorded event corresponding to the last saved video
     * frame.
     */
    int lastSavedEvent;

    /**
     * The index of the last saved video frame.
     */
    int lastSavedFrame;

    /**
     * The x cursor position of the last replayed mouse event.
     */
    int savedX;

    /**
     * The x cursor position of the last replayed mouse event.
     */
    int savedY;

    /**
     * The index of the last replayed event.
     */
    unsigned int lastPlayed;

    /**
     * The recorded events.
     */
    vector<Event> recordedEvents;

    /**
     * The EventHandler that must handle the events recorded and replayed by
     * this EventRecorder.
     */
    ptr<EventHandler> next;

    /**
     * Saves the framebuffer content to the given TGA file.
     *
     * @param tga a TGA file name.
     */
    void saveFrame(char *tga);

    /**
     * Saves the recorded events to a temporary file if #autoSave is true.
     */
    void saveEvents();
};

}

#endif
