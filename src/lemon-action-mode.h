/**
 * LemonActionMode:
 * @LEMON_ACTION_MODE_NONE: block action
 * @LEMON_ACTION_MODE_NORMAL: allow action when in window mode,
 *     e.g. when the focus is in an application window
 * @LEMON_ACTION_MODE_OVERVIEW: allow action while the overview
 *     is active
 * @LEMON_ACTION_MODE_LOCK_SCREEN: allow action when the screen
 *     is locked, e.g. when the screen shield is shown
 * @LEMON_ACTION_MODE_UNLOCK_SCREEN: allow action in the unlock
 *     dialog
 * @LEMON_ACTION_MODE_LOGIN_SCREEN: allow action in the login screen
 * @LEMON_ACTION_MODE_SYSTEM_MODAL: allow action when a system modal
 *     dialog (e.g. authentication or session dialogs) is open
 * @LEMON_ACTION_MODE_LOOKING_GLASS: allow action in looking glass
 * @LEMON_ACTION_MODE_POPUP: allow action while a shell menu is open
 * @LEMON_ACTION_MODE_EXPO: allow action while the expo is active
 * @LEMON_ACTION_MODE_ALL: always allow action
 *
 * Controls in which Lemon states an action (like keybindings and gestures)
 * should be handled.
*/
typedef enum {
  LEMON_ACTION_MODE_NONE          = 0,
  LEMON_ACTION_MODE_NORMAL        = 1 << 0,
  LEMON_ACTION_MODE_OVERVIEW      = 1 << 1,
  LEMON_ACTION_MODE_EXPO          = 1 << 2,
  LEMON_ACTION_MODE_LOCK_SCREEN   = 1 << 3,
  LEMON_ACTION_MODE_UNLOCK_SCREEN = 1 << 4,
  LEMON_ACTION_MODE_SYSTEM_MODAL  = 1 << 5,
  LEMON_ACTION_MODE_LOOKING_GLASS = 1 << 6,
  LEMON_ACTION_MODE_POPUP         = 1 << 7,

  LEMON_ACTION_MODE_ALL = ~0,
} LemonActionMode;

