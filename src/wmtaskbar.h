#ifndef __TASKBAR_H
#define __TASKBAR_H

#include "yaction.h"
#include "ybutton.h"
#include "ymenu.h"
#include "ytimer.h"
#include "wmclient.h"
#include "yxtray.h"

class ObjectBar;
#if CONFIG_APPLET_CPU_STATUS
class CPUStatus;
#endif
#ifdef CONFIG_APPLET_NET_STATUS
class NetStatus;
#endif
class AddressBar;
class MailBoxStatus;
class YClock;
class YApm;
class TaskPane;
class TrayPane;
class WorkspacesPane;
class YXTray;

class IAppletContainer {
public:
    virtual void relayout() = 0;
    virtual void contextMenu(int x_root, int y_root) = 0;
protected:
    virtual ~IAppletContainer() {}
};

#ifdef CONFIG_TASKBAR
class TaskBar;

class YLayout {
public:
    YLayout() {
        visible = true;
        end = false;
        expand = false;
        vertical = false;
        fill = false;
        spacing = 0;
        window = 0;
        w = h = 0;
    }

    bool vertical;
    bool start;
    bool end;
    bool expand;
    bool fill;
    bool visible;
    int spacing;

    YArray<YLayout *> nested;
    YWindow *window;

    int w, h;
};

class EdgeTrigger: public YWindow, public YTimerListener {
public:
    EdgeTrigger(TaskBar *owner);
    virtual ~EdgeTrigger();

    void startHide();
    void stopHide();

    virtual void handleDNDEnter();
    virtual void handleDNDLeave();

    virtual void handleCrossing(const XCrossingEvent &crossing);
    virtual bool handleTimer(YTimer *t);
private:
    TaskBar *fTaskBar;
    YTimer *fAutoHideTimer;
    bool fDoShow;
};

class TaskBar:
    public YFrameClient,
    public YActionListener,
    public YPopDownListener,
    public YXTrayNotifier,
    public IAppletContainer
{
public:
    TaskBar(YWindow *aParent);
    virtual ~TaskBar();

    virtual void paint(Graphics &g, const YRect &r);
    virtual bool handleKey(const XKeyEvent &key);
    virtual void handleButton(const XButtonEvent &button);
    virtual void handleClick(const XButtonEvent &up, int count);
    virtual void handleDrag(const XButtonEvent &down, const XMotionEvent &motion);
    virtual void handleEndDrag(const XButtonEvent &down, const XButtonEvent &up);

    virtual void handleCrossing(const XCrossingEvent &crossing);
#if false
    virtual bool handleTimer(YTimer *t);
#endif

    virtual void actionPerformed(YAction *action, unsigned int modifiers);
    virtual void handlePopDown(YPopupWindow *popup);
    virtual void handleEndPopup(YPopupWindow *popup);

    void updateWMHints();
    void updateLocation();
    void configure(const YRect &r, const bool resized);

    YWindow *initApplet(YLayout *object_layout, ref<class YElement> applet);

    bool windowTrayRequestDock(Window w);
    void setWorkspaceActive(long workspace, int active);

    void removeTasksApp(YFrameWindow *w);
    class TaskBarApp *addTasksApp(YFrameWindow *w);
    void relayoutTasks();

    void popupStartMenu();
    void popupWindowListMenu();

    void popOut();
    void showAddressBar();
    void showBar(bool visible);
    void handleCollapseButton();

    AddressBar *addressBar() const { return fAddressBar; }
    TaskPane *taskPane() const { return fTasks; }
    WorkspacesPane *workspacesPane() const { return fWorkspaces; }
#ifdef CONFIG_TRAY
    TrayPane *windowTrayPane() const { return fWindowTray; }
#endif

#ifdef CONFIG_GRADIENTS
    virtual ref<YImage> getGradient() const { return fGradient; }
#endif    

    void contextMenu(int x_root, int y_root);

    void relayout() { fNeedRelayout = true; }
    void relayoutNow();

    void detachDesktopTray();
    void trayChanged();
    void relayoutTray();
    class TrayApp *addTrayApp(YFrameWindow *w);
    void removeTrayApp(YFrameWindow *w);

    bool autoTimer(bool show);
    void updateFullscreen(bool fullscreen);
    Window edgeTriggerWindow() { return fEdgeTrigger->handle(); }

private:
    TaskPane *fTasks;

#if 0
    YButton *fCollapseButton;
#endif
#ifdef CONFIG_TRAY
    TrayPane *fWindowTray;
#endif

    YButton *fApplications;
    YButton *fWinList;
    AddressBar *fAddressBar;

    YLayout *fLayout;
    WorkspacesPane *fWorkspaces;
    YXTray *fDesktopTray;

    bool fIsHidden;
    bool fFullscreen;
    bool fIsCollapsed;
    bool fIsMapped;
    bool fMenuShown;
#if false
    YTimer *fAutoHideTimer;
#endif

    YMenu *taskBarMenu;

    friend class WindowList;
    friend class WindowListBox;
    
#ifdef CONFIG_GRADIENTS
    ref<YImage> fGradient;
#endif

    bool fNeedRelayout;

    void initMenu();
    void initApplets();
    void loadTaskbar(ref<YElement> element, YLayout *layout);
    void layoutSize(YLayout *layout, int &size_w, int &size_h);
    void layoutPosition(YLayout *layout, int x, int y, int w, int h);
    void updateLayout(int &size_w, int &size_h);

    EdgeTrigger *fEdgeTrigger;
};

extern TaskBar *taskBar; // !!! get rid of this

#if 1
extern ref<YPixmap> startPixmap;
extern ref<YPixmap> windowsPixmap;

extern ref<YPixmap> taskbackPixmap;
extern ref<YPixmap> taskbuttonPixmap;
extern ref<YPixmap> taskbuttonactivePixmap;
extern ref<YPixmap> taskbuttonminimizedPixmap;
#endif

#ifdef CONFIG_GRADIENTS
#if 1
extern ref<YImage> taskbackPixbuf;
extern ref<YImage> taskbuttonPixbuf;
extern ref<YImage> taskbuttonactivePixbuf;
extern ref<YImage> taskbuttonminimizedPixbuf;
#endif
#endif

#endif

#endif
