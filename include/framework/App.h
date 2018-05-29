#ifndef _FRAMEWORK_APP_H
#define _FRAMEWORK_APP_H


#include <io/PathName.h>
#include <lang/Array.h>
#include <lang/Object.h>
#include <lang/String.h>
#include <framework/OSInterface.h>


BEGIN_NAMESPACE(io) 
	class DataInputStream;
	class DataOutputStream;END_NAMESPACE()

BEGIN_NAMESPACE(gr) 
	class Context;END_NAMESPACE()


BEGIN_NAMESPACE(framework) 


/** 
 * Base class for application instances.
 * There must be only one application instance.
 * Application instance must always be allocated to heap (with operator new).
 * @ingroup framework
 */
class App
{
public:
	/** Initialization parameters. */
	class Configuration
	{
	public:
		/** Name of the application (main window). */
		NS(lang,String)		name;
		/** Width of rendering target. */
		int					width;
		/** Height of rendering target. */
		int					height;
		/** Bits per pixel. */
		int 				bits;
		/** Fullscreen window. */
		bool 				fullscreen;
		/** Stencil buffer required. */
		bool 				stencilbuffer;
		/** Capture all mouse input. */
		bool				captureMouse;
		/** Icon resource id. 0 if default. */
		int					iconResourceID;
		/** Used shaders to optimize PC driver startup time. Optional. 0 Terminated list. */
		const char* const*	usedShaders;
		/** Used shaders to optimize PC driver startup time. Optional. 0 Terminated list. */
		const char* const*	usedParticles;
		/** Maximum idle time in milliseconds allowed for background tasks per game update. Used with non-pre-emptive scheduling (e.g. Symbian). */
		int					idleTime;
		/** Debug/trace memory allocations. Default is true for _DEBUG build. */
		bool				debugMemoryAllocs;

		Configuration();
	};

	/** Key type. */
	enum KeyType
	{
		/// Invalid key code
		KEY_NONE,
		///
		KEY_A,
		///
		KEY_B,
		///
		KEY_C,
		///
		KEY_D,
		///
		KEY_E,
		///
		KEY_F,
		///
		KEY_G,
		///
		KEY_H,
		///
		KEY_I,
		///
		KEY_J,
		///
		KEY_K,
		///
		KEY_L,
		///
		KEY_M,
		///
		KEY_N,
		///
		KEY_O,
		///
		KEY_P,
		///
		KEY_Q,
		///
		KEY_R,
		///
		KEY_S,
		///
		KEY_T,
		///
		KEY_U,
		///
		KEY_V,
		///
		KEY_W,
		///
		KEY_X,
		///
		KEY_Y,
		///
		KEY_Z,
		///
		KEY_PAGEDOWN,
		///
		KEY_PAGEUP,
		///
		KEY_HOME,
		///
		KEY_END,
		///
		KEY_INSERT,
		///
		KEY_DELETE,
		///
		KEY_RETURN,
		///
		KEY_SHIFT,
		///
		KEY_CONTROL,
		///
		KEY_TAB,
		///
		KEY_SPACE,
		///
		KEY_BACKSPACE,
		///
		KEY_PAUSE,
		///
		KEY_ESCAPE,
		///
		KEY_F1,
		///
		KEY_F2,
		///
		KEY_F3,
		///
		KEY_F4,
		///
		KEY_F5,
		///
		KEY_F6,
		///
		KEY_F7,
		///
		KEY_F8,
		///
		KEY_F9,
		///
		KEY_F10,
		///
		KEY_F11,
		///
		KEY_F12,
		/** Arrow left. */
		KEY_LEFT,
		/** Arrow right. */
		KEY_RIGHT,
		/** Arrow up. */
		KEY_UP,
		/** Arrow down. */
		KEY_DOWN,
		/** Left mouse button. */
		KEY_LBUTTON,
		/** Middle mouse button. */
		KEY_MBUTTON,
		/** Right mouse button. */
		KEY_RBUTTON,
		/** Keypad number */
		KEY_0,
		/** Keypad number */
		KEY_1,
		/** Keypad number */
		KEY_2,
		/** Keypad number */
		KEY_3,
		/** Keypad number */
		KEY_4,
		/** Keypad number */
		KEY_5,
		/** Keypad number */
		KEY_6,
		/** Keypad number */
		KEY_7,
		/** Keypad number */
		KEY_8,
		/** Keypad number */
		KEY_9,
		/** Left soft key */
		KEY_LEFTSOFT,
		/** Right soft key */
		KEY_RIGHTSOFT,
		/** Middle soft key */
		KEY_MIDDLE,
		/** Asterisk mark */
		KEY_ASTERISK,
		/** Hash mark */
		KEY_HASH,
		/** Clear key */
		KEY_CLEAR,
		/** Edit key */
		KEY_EDIT,
		/** Send key */
		KEY_SEND,
		/** Volume up key */
		KEY_VOLUME_UP,
		/** Volume down key */
		KEY_VOLUME_DOWN,
		/** Number of key codes. */
		KEY_COUNT
	};

	explicit App( OSInterface* os );
	virtual ~App();

	/** 
	 * Main loop for your application.
	 * @param dt Time (seconds) since last update.
	 * @param context Rendering device context.
	 */
	virtual void		update( float dt, NS(gr,Context)* context ) = 0;

	/**
	 * Called when user pressed key down.
	 * Called only once per key press, i.e. keeping key down doesnt cause more key presses.
	 * Default implementation does nothing.
	 */
	virtual void		keyDown( KeyType key );

	/**
	 * Called when user releases key.
	 * Default implementation does nothing.
	 */
	virtual void		keyUp( KeyType key );

	/**
	 * Called when the orientation of the screen changes.
	 */
	virtual void		orientationChanged();

	/**
	 * Called when user moves the mouse.
	 * If mouse has been captured (config.captureMouse=true in App init() function),
	 * then relative coordinates are received. Otherwise the coordinates
	 * are absolute, in window client area.
	 */
	virtual void		mouseMove( int dx, int dy );

	/**
	 * Called when main window is activated or deactivated.
	 */
	virtual void		activate( bool active );

	/**
	 * Request application exit.
	 */
	void				quit();

	/**
	 * Sets default data path.
	 */
	void				setDataPath( const NS(lang,String)& datapath );

	/**
	 * Returns expanded default path to data files.
	 */
	NS(lang,String)		path( const NS(lang,String)& filename="" ) const;

	/**
	 * Returns true if specified key is currently pressed down.
	 * State returned by this function is updated in default keyEvent implementation.
	 */
	bool				isKeyDown( KeyType key ) const;

	/** 
	 * Returns the application instance. 
	 */
	static App*			get();

	/**
	 * Converts key type to string representation.
	 */
	static const char*	toString( KeyType key );

	/*
	 * USED BY PLATFORM SPECIFIC IMPLEMENTATION.
	 * Note that platform specific implementation needs to use this
	 * since otherwise the state returned by isKeyDown state is not right.
	 * Note also that for example focus losing/gaining might cause
	 * key state buffer to be reset, so thats why better that
	 * platform specific impl handles everything instead of user application.
	 * Note that assumed behavior for focus lost/gain is that
	 * all key states are set to false, i.e. to application losing/gaining
	 * focus always looks like all the keys were released.
	 * Sets key down state.
	 * @see isKeyDown
	 */
	void				setKeyDown( KeyType key, bool down );

	/*
	 * USED BY PLATFORM SPECIFIC IMPLEMENTATION.
	 * Returns true if application has requested exit.
	 */
	bool				quitRequested() const;

	/*
	 * USED BY PLATFORM SPECIFIC IMPLEMENTATION.
	 * Deletes application instance. 
	 */
	static void			deleteApp();

	/*
	 * USED BY PLATFORM SPECIFIC IMPLEMENTATION.
	 * Returns true if App has been initialized.
	 */
	static bool			initialized();

private:
	OSInterface*		m_os;
	NS(io,PathName)		m_dataPath;
	bool				m_keyDown[KEY_COUNT];
	bool				m_quit;

	App( const App& );
	App& operator=( const App& );
};


/**
 * Allows application to adjust rendering device initialization parameters.
 * Called before init().
 * Implemented by the application.
 */
void configure( App::Configuration& config );

/**
 * Initializes the application instance.
 * Implemented by the application.
 */
NS(framework,App)* init( OSInterface* os, NS(gr,Context)* context );


END_NAMESPACE() // framework


#endif // _FRAMEWORK_APP_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
