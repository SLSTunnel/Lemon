const Signals = imports.signals;
const Main = imports.ui.main;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Lang = imports.lang;
const Util = imports.misc.util;
const Meta = imports.gi.Meta;
const Lemon = imports.gi.Lemon;
const AppletManager = imports.ui.appletManager;
const DeskletManager = imports.ui.deskletManager;

const MK = imports.gi.CDesktopEnums.MediaKeyType;
const LemonDesktop = imports.gi.LemonDesktop;

const CUSTOM_KEYS_PARENT_SCHEMA = "org.lemon.desktop.keybindings";
const CUSTOM_KEYS_BASENAME = "/org/lemon/desktop/keybindings/custom-keybindings";
const CUSTOM_KEYS_SCHEMA = "org.lemon.desktop.keybindings.custom-keybinding";

const MEDIA_KEYS_SCHEMA = "org.lemon.desktop.keybindings.media-keys";

const MEDIA_KEYS = [
    // Volume/audio
    { key: MK.MUTE,                       mode: Lemon.ActionMode.ALL },
    { key: MK.MUTE_QUIET,                 mode: Lemon.ActionMode.ALL },
    { key: MK.VOLUME_UP,                  mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.VOLUME_UP_QUIET,            mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.VOLUME_DOWN,                mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.VOLUME_DOWN_QUIET,          mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.MIC_MUTE,                   mode: Lemon.ActionMode.ALL },

    // Disc/media
    { key: MK.EJECT,                      mode: Lemon.ActionMode.ALL },
    { key: MK.MEDIA,                      mode: Lemon.ActionMode.ALL },

    // Playback
    { key: MK.PLAY,                       mode: Lemon.ActionMode.ALL },
    { key: MK.PAUSE,                      mode: Lemon.ActionMode.ALL },
    { key: MK.STOP,                       mode: Lemon.ActionMode.ALL },
    { key: MK.PREVIOUS,                   mode: Lemon.ActionMode.ALL },
    { key: MK.NEXT,                       mode: Lemon.ActionMode.ALL },
    { key: MK.REWIND,                     mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.FORWARD,                    mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.REPEAT,                     mode: Lemon.ActionMode.ALL },
    { key: MK.RANDOM,                     mode: Lemon.ActionMode.ALL },

    // Screenshots
    { key: MK.SCREENSHOT,                 mode: Lemon.ActionMode.ALL },
    { key: MK.SCREENSHOT_CLIP,            mode: Lemon.ActionMode.NORMAL },
    { key: MK.WINDOW_SCREENSHOT,          mode: Lemon.ActionMode.NORMAL },
    { key: MK.WINDOW_SCREENSHOT_CLIP,     mode: Lemon.ActionMode.NORMAL },
    { key: MK.AREA_SCREENSHOT,            mode: Lemon.ActionMode.NORMAL },
    { key: MK.AREA_SCREENSHOT_CLIP,       mode: Lemon.ActionMode.NORMAL },

    // Touchpad
    { key: MK.TOUCHPAD,                   mode: Lemon.ActionMode.NORMAL },
    { key: MK.TOUCHPAD_ON,                mode: Lemon.ActionMode.NORMAL },
    { key: MK.TOUCHPAD_OFF,               mode: Lemon.ActionMode.NORMAL },

    // Session
    { key: MK.LOGOUT,                     mode: Lemon.ActionMode.NORMAL },
    { key: MK.SHUTDOWN,                   mode: Lemon.ActionMode.NORMAL },
    { key: MK.SUSPEND,                    mode: Lemon.ActionMode.NORMAL },
    { key: MK.HIBERNATE,                  mode: Lemon.ActionMode.NORMAL },
    { key: MK.SCREENSAVER,                mode: Lemon.ActionMode.NORMAL },

    // App launchers
    { key: MK.HOME,                       mode: Lemon.ActionMode.NORMAL },
    { key: MK.CALCULATOR,                 mode: Lemon.ActionMode.NORMAL },
    { key: MK.SEARCH,                     mode: Lemon.ActionMode.NORMAL },
    { key: MK.EMAIL,                      mode: Lemon.ActionMode.NORMAL },
    { key: MK.HELP,                       mode: Lemon.ActionMode.NORMAL },
    { key: MK.TERMINAL,                   mode: Lemon.ActionMode.NORMAL },
    { key: MK.WWW,                        mode: Lemon.ActionMode.NORMAL },

    // Display
    { key: MK.ROTATE_VIDEO_LOCK,          mode: Lemon.ActionMode.ALL },

    // Accessibility
    { key: MK.SCREENREADER,               mode: Lemon.ActionMode.NORMAL },
    { key: MK.ON_SCREEN_KEYBOARD,         mode: Lemon.ActionMode.NORMAL |
                                                Lemon.ActionMode.POPUP |
                                                Lemon.ActionMode.UNLOCK_SCREEN |
                                                Lemon.ActionMode.SYSTEM_MODAL },
    { key: MK.INCREASE_TEXT,              mode: Lemon.ActionMode.NORMAL },
    { key: MK.DECREASE_TEXT,              mode: Lemon.ActionMode.NORMAL },
    { key: MK.TOGGLE_CONTRAST,            mode: Lemon.ActionMode.NORMAL },

    // Brightness
    { key: MK.SCREEN_BRIGHTNESS_UP,       mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.SCREEN_BRIGHTNESS_DOWN,     mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.KEYBOARD_BRIGHTNESS_UP,     mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.KEYBOARD_BRIGHTNESS_DOWN,   mode: Lemon.ActionMode.ALL, repeatable: true },
    { key: MK.KEYBOARD_BRIGHTNESS_TOGGLE, mode: Lemon.ActionMode.ALL },

    // Battery
    { key: MK.BATTERY,                    mode: Lemon.ActionMode.NORMAL },
];

const iface = "\
    <node> \
      <interface name='org.lemon.SettingsDaemon.KeybindingHandler'> \
        <annotation name='org.freedesktop.DBus.GLib.CSymbol' value='csd_media_keys_manager'/> \
        <method name='HandleKeybinding'> \
          <arg name='type' direction='in' type='u'/> \
        </method> \
      </interface> \
    </node>";

const proxy = Gio.DBusProxy.makeProxyWrapper(iface);

function KeybindingManager() {
    this._init();
}

KeybindingManager.prototype = {
    _init: function() {
        this._proxy = new proxy(Gio.DBus.session,
                                'org.lemon.SettingsDaemon.KeybindingHandler',
                                '/org/lemon/SettingsDaemon/KeybindingHandler');

        /* Keep track of bindings so we can a) check if they've change (and avoid the work
         * if the haven't), b) handle the callbacks when the keystrokes are captured by
         * Main._stageEventHandler.
         *
         * This dict will contain [name, bindings, callback] and keyed on the id returned by
         * add_custom_keybinding. */
        this.bindings = new Map();
        this.applet_bindings = new Map();
        this.kb_schema = Gio.Settings.new(CUSTOM_KEYS_PARENT_SCHEMA);
        this.setup_custom_keybindings();
        this.kb_schema.connect("changed::custom-list", Lang.bind(this, this.on_customs_changed));

        this.media_key_settings = new Gio.Settings({ schema_id: MEDIA_KEYS_SCHEMA });
        this.media_key_settings.connect("changed", Lang.bind(this, this.setup_media_keys));

        this.screensaver_settings = new Gio.Settings({ schema_id: "org.lemon.desktop.screensaver" });

        this.setup_media_keys();
    },

    on_customs_changed: function(settings, key) {
        this.remove_custom_keybindings();
        this.setup_custom_keybindings();
    },

    addHotKey: function(name, bindings_string, callback, flags, allowedModes) {
        if (!bindings_string)
            return false;
        return this.addHotKeyArray(name, bindings_string.split("::"), callback, flags, allowedModes);
    },

    _makeXletKey: function(xlet, name, binding) {
        return `${xlet._uuid}::${name}::${binding}`;
    },

    _uuidFromXletKey: function(xlet_key) {
        return xlet_key.split("::")[0];
    },

    /*  Menu applet example
     *
     *  uuid: menu@lemon.org
     *  binding name: overlay-key
     *    instances:
     *      49: super-l, super-r
     *      52: super-l, ctrl-shift-f7
     *
     *  is in applet_bindings as:
     *
     *  {
     *      "menu@lemon.org::overlay-key:super-l" : {
     *          "49": callback49,
     *          "52": callback52
     *      },
     *      "menu@lemon.org::overlay-key:super-r" : {
     *          "49": callback49
     *      },
     *      "menu@lemon.org::overlay-key:ctrl-shift-f7" : {
     *          "52": callback52
     *      }
     *  }
     */

    addXletHotKey: function(xlet, name, bindings_string, callback, flags, allowedModes) {
        this._removeMatchingXletBindings(xlet, name);

        if (!bindings_string)
            return false;

        let xlet_set = null;
        const instanceId = xlet.instance_id || 0; // extensions == undefined
        const binding_array = bindings_string.split("::");

        for (const binding of binding_array) {
            const xlet_key = this._makeXletKey(xlet, name, binding);
            xlet_set = this.applet_bindings.get(xlet_key);

            if (xlet_set === undefined) {
                xlet_set = new Map([
                    ["commitTimeoutId", 0]
                ]);
                this.applet_bindings.set(xlet_key, xlet_set);
            }

            xlet_set.set(instanceId, callback);

            this._queueCommitXletHotKey(xlet_key, binding, xlet_set, flags, allowedModes);
        }
    },

    _removeMatchingXletBindings: function(xlet, name) {
        // This sucks, but since the individual binding string is part of the name
        // name we send to muffin, we can't just call display.remove_keybinding(name),
        // and need to iterate thru the list finding our matching uuid and instance ids.
        const key_prefix = `${xlet._uuid}::${name}::`;
        const instanceId = xlet.instance_id || 0;
        const iter = this.applet_bindings.keys();

        for (const xlet_key of iter) {
            if (xlet_key.startsWith(key_prefix)) {
                const xlet_set = this.applet_bindings.get(xlet_key);
                if (xlet_set.has(instanceId)) {
                    xlet_set.delete(instanceId);
                    if (xlet_set.size === 1) { // only commitTimeoutId left
                        this.applet_bindings.delete(xlet_key);
                        this.removeHotKey(xlet_key);
                    }
                }
            }
        }
    },

    _xletCallback: function(lookup_key, display, window, kb, action) {
        const xlet_set = this.applet_bindings.get(lookup_key);
        if (!xlet_set) {
            return;
        }

        /* This should catch extensions also. The minimum size is 2 - 1 will be a
         * binding, plus the commitTimeoutId. */
        if (xlet_set.size === 2) {
            const iter = xlet_set.keys();
            for (const instanceId of iter) {
                if (instanceId === "commitTimeoutId") {
                    continue;
                }
                const callback = xlet_set.get(instanceId);
                callback(display, window, kb, action);
                break;
            }
            return;
        }

        const iter = xlet_set.keys();
        const uuid = this._uuidFromXletKey(lookup_key);
        const currentMonitor = Main.layoutManager.currentMonitor.index;

        let xlet = null
        let xletMonitor = 0;
        let primary_callback = null;
        let current_callback = null;

        for (instanceId of iter) {
            current_callback = xlet_set.get(instanceId);

            xlet = AppletManager.get_object_for_uuid(uuid, instanceId);

            if (!xlet) {
                xlet = DeskletManager.get_object_for_uuid(uuid, instanceId);
            }

            if (xlet) {
                const actor = xlet.actor;
                if (actor) {
                    xletMonitor = Main.layoutManager.findMonitorIndexForActor(actor);

                    if (xletMonitor === Main.layoutManager.primaryMonitor.index) {
                        primary_callback = current_callback;
                    }
                    if (xletMonitor == currentMonitor) {
                        current_callback(display, window, kb, action);
                        return;
                    }
                }
            }
        }

        // No match... more monitors than instances? Prefer the primary monitor's if we encountered it.
        if (primary_callback) {
            primary_callback(display, window, kb, action);
        } else {
            // Fallback to the last one we looked at otherwise.
            current_callback(display, window, kb, action);
        }
    },

    removeXletHotKey: function(xlet, name) {
        this._removeMatchingXletBindings(xlet, name);
    },

    _queueCommitXletHotKey: function(xlet_key, binding, xlet_set, flags, allowedModes) {
        let id = xlet_set.get("commitTimeoutId") ?? 0;

        if (id > 0) {
            GLib.source_remove(id);
        }

        id = GLib.idle_add(GLib.PRIORITY_DEFAULT, () => {
            this.addHotKeyArray(xlet_key, [binding], this._xletCallback.bind(this, xlet_key), flags, allowedModes);
            xlet_set.set("commitTimeoutId", 0);
            return GLib.SOURCE_REMOVE;
        });

        xlet_set.set("commitTimeoutId", id);
    },

    _lookupEntry: function(name) {
        let found = 0;
        for (const action_id of this.bindings.keys()) {
            let entry = this.bindings.get(action_id);
            if (entry !== undefined && entry.name === name) {
                return [action_id, entry];
            }
        }

        return [Meta.KeyBindingAction.NONE, undefined];
    },

    getBindingById: function(action_id) {
        return this.bindings.get(action_id);
    },

    addHotKeyArray: function(name, bindings, callback,
                             flags=Meta.KeyBindingFlags.IGNORE_AUTOREPEAT,
                             allowedModes=Lemon.ActionMode.NORMAL |
                                          Lemon.ActionMode.POPUP) {
        let [existing_action_id, entry] = this._lookupEntry(name);

        if (entry !== undefined) {
            if (entry.bindings.toString() === bindings.toString()) {
                return true;
            }
            global.display.remove_keybinding(name);
            this.bindings.delete(existing_action_id);
        }

        if (!bindings) {
            global.logError("Missing bindings array for keybinding: " + name);
            return false;
        }

        let empty = true;
        for (let i = 0; empty && (i < bindings.length); i++) {
            empty = bindings[i].toString().trim() == "";
        }

        if (empty) {
            return true;
        }

        action_id = global.display.add_custom_keybinding_full(name, bindings, flags, callback);
        // log(`set keybinding: ${name}, bindings: ${bindings}, flags: ${flags}, allowedModes: ${allowedModes} - action id: ${action_id}`);

        if (action_id === Meta.KeyBindingAction.NONE) {
            global.logError("Warning, unable to bind hotkey with name '" + name + "'.  The selected keybinding could already be in use.");
            return false;
        }
        this.bindings.set(action_id, {
            "name"        : name,
            "bindings"    : bindings,
            "callback"    : callback,
            "allowedModes": allowedModes
        });

        return true;
    },

    removeHotKey: function(name) {
        let [action_id, entry] = this._lookupEntry(name);

        if (entry === undefined) {
            return;
        }

        global.display.remove_keybinding(name);
        this.bindings.delete(action_id);
    },

    setup_custom_keybindings: function() {
        let list = this.kb_schema.get_strv("custom-list");

        for (let i = 0; i < list.length; i++) {
            if (list[i] === "__dummy__") {
                continue;
            }

            let custom_path = CUSTOM_KEYS_BASENAME + "/" + list[i] + "/";
            let schema = Gio.Settings.new_with_path(CUSTOM_KEYS_SCHEMA, custom_path);
            let command = schema.get_string("command");
            let binding = schema.get_strv("binding");
            let name = list[i];
            this.addHotKeyArray(name, binding, Lang.bind(this, function() {
                Util.spawnCommandLine(command);
            }))
        }
    },

    remove_custom_keybindings: function() {
        for (const action_id of this.bindings.keys()) {
            name = this.bindings.get(action_id).name;
            if (name && name.indexOf("custom") > -1) {
                global.display.remove_keybinding(name);
                this.bindings.delete(action_id);
            }
        }
    },

    setup_media_keys: function() {
        for (const mk of MEDIA_KEYS) {
            let flags = mk.repeatable
                ? Meta.KeyBindingFlags.NONE
                : Meta.KeyBindingFlags.IGNORE_AUTOREPEAT;

            let bindings = this.media_key_settings.get_strv(
                LemonDesktop.desktop_get_media_key_string(mk.key)
            );

            this.addHotKeyArray(
                "media-keys-" + mk.key.toString(),
                bindings,
                Lang.bind(this, this.on_media_key_pressed, mk.key),
                flags,
                mk.mode
            );
        }
        return true;
    },

    on_media_key_pressed: function(display, window, kb, action) {
        let [, entry] = this._lookupEntry("media-keys-" + action.toString());
        if (Main._shouldFilterKeybinding(entry))
            return;

        // Check if this is the screensaver key and internal screensaver is enabled
        if (action === MK.SCREENSAVER && global.settings.get_boolean('internal-screensaver-enabled')) {
            // If a custom screensaver is configured, skip internal handling and
            // let csd-media-keys run lemon-screensaver-command instead.
            if (!this.screensaver_settings.get_string('custom-screensaver-command').trim()) {
                GLib.idle_add(GLib.PRIORITY_DEFAULT, () => {
                    Main.lockScreen(false);
                    return GLib.SOURCE_REMOVE;
                });
                return;
            }
        }

        // Otherwise, forward to csd-media-keys (or other handler)
        this._proxy.HandleKeybindingRemote(action);
    },

    _onBuiltinKeyPressed: function(display, window, binding, actionId) {
        let entry = this.bindings.get(actionId);

        if (entry === undefined || Main._shouldFilterKeybinding(entry))
            return;

        entry.callback(display, window, binding);
    },

    setBuiltinHandler: function(name, actionId, callback, allowedModes=Lemon.ActionMode.NORMAL) {
        Meta.keybindings_set_custom_handler(name,
            (display, window, binding) => this._onBuiltinKeyPressed(display, window, binding, actionId)
        );

        this.bindings.set(actionId, {
            name: name,
            bindings: [],
            callback: callback,
            allowedModes: allowedModes
        });
    },

    invoke_keybinding_action_by_id: function(id) {
        const entry = this.bindings.get(id);
        if (entry !== undefined) {
            entry.callback(null, null, { get_name: () => entry.name });
        }
    }
};
Signals.addSignalMethods(KeybindingManager.prototype);

