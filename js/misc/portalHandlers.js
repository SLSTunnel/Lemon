// -*- mode: js; js-indent-level: 4; indent-tabs-mode: nil -*-

const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Lemon = imports.gi.Lemon;

const XdgAppState = {
    BACKGROUND: 0, // window.is_hidden
    RUNNING: 1, // window visible
    ACTIVE: 2  // window focused
}

const LemonPortalIface =
    '<node> \
        <interface name="org.lemon.PortalHandlers"> \
            <method name="GetAppStates"> \
                <arg type="a{sv}" direction="out" name="apps" /> \
            </method> \
            <signal name="RunningAppsChanged"/> \
        </interface> \
    </node>';


var LemonPortalHandler = class LemonPortalHandler {
    constructor() {
        this._dbusImpl = Gio.DBusExportedObject.wrapJSObject(LemonPortalIface, this);
        this._dbusImpl.export(Gio.DBus.session, '/org/Lemon');

        this.running_apps = {};
        Lemon.AppSystem.get_default().connect("app-state-changed", () => this.EmitRunningAppsChanged());
        Lemon.WindowTracker.get_default().connect("notify::focus-app", () => this.EmitRunningAppsChanged());
    }

    EmitRunningAppsChanged() {
        this._dbusImpl.emit_signal('RunningAppsChanged', null);
    }

    has_focus(app) {
        const fwin = global.display.get_focus_window();
        if (!fwin) {
            return false;
        }
        return app.get_windows().includes(fwin);
    }

    /* org.freedesktop.impl.portal.Background.GetAppState:
     * A big issue right now is that in X11, LemonAppSystem stops caring
     * about an app if its windows are closed or *hidden* to tray, so our list
     * here won't contain any background apps until this behavior is addressed. */
    GetAppStates() {
        const appsys = Lemon.AppSystem.get_default();
        const running = appsys.get_running();
        const apps = {}

        for (let app of running) {
            var id = null;

            if (app.get_is_flatpak()) {
                id = app.get_flatpak_app_id();
            }
            else {
                id = app.get_id();
            }
            let state;
            if (app.get_n_windows() === 0) {
                state = XdgAppState.BACKGROUND;
            } else if (this.has_focus(app)) {
                state = XdgAppState.ACTIVE;
            } else {
                state = XdgAppState.RUNNING;
            }
            apps[id] = GLib.Variant.new_uint32(state);
        }

        return new GLib.Variant("(a{sv})", [apps]);
    }
}
