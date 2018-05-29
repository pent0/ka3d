#include <framework/App.h>
#include <io/PathName.h>
#include <lang/Debug.h>
#include <lang/GlobalStorage.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(framework) 


//
// To create this table:
// 1) Copypaste KeyType values to here, and
// 2) Replace ^:b+KEY_{[A-Z1-90]+},:b*$
//	  with         "\1",
// 
static const char* const s_keysz[] =
{
		/// Invalid key code
        "NONE",
		///
        "A",
		///
        "B",
		///
        "C",
		///
        "D",
		///
        "E",
		///
        "F",
		///
        "G",
		///
        "H",
		///
        "I",
		///
        "J",
		///
        "K",
		///
        "L",
		///
        "M",
		///
        "N",
		///
        "O",
		///
        "P",
		///
        "Q",
		///
        "R",
		///
        "S",
		///
        "T",
		///
        "U",
		///
        "V",
		///
        "W",
		///
        "X",
		///
        "Y",
		///
        "Z",
		///
        "PAGEDOWN",
		///
        "PAGEUP",
		///
        "HOME",
		///
        "END",
		///
        "INSERT",
		///
        "DELETE",
		///
        "RETURN",
		///
        "SHIFT",
		///
        "CONTROL",
		///
        "TAB",
		///
        "SPACE",
		///
        "BACKSPACE",
		///
        "PAUSE",
		///
        "ESCAPE",
		///
        "F1",
		///
        "F2",
		///
        "F3",
		///
        "F4",
		///
        "F5",
		///
        "F6",
		///
        "F7",
		///
        "F8",
		///
        "F9",
		///
        "F10",
		///
        "F11",
		///
        "F12",
		/** Arrow left. */
        "LEFT",
		/** Arrow right. */
        "RIGHT",
		/** Arrow up. */
        "UP",
		/** Arrow down. */
        "DOWN",
		/** Left mouse button. */
        "LBUTTON",
		/** Middle mouse button. */
        "MBUTTON",
		/** Right mouse button. */
        "RBUTTON",
		/** Keypad number */
        "0",
		/** Keypad number */
        "1",
		/** Keypad number */
        "2",
		/** Keypad number */
        "3",
		/** Keypad number */
        "4",
		/** Keypad number */
        "5",
		/** Keypad number */
        "6",
		/** Keypad number */
        "7",
		/** Keypad number */
        "8",
		/** Keypad number */
        "9",
		/** Left soft key */
        "LEFTSOFT",
		/** Right soft key */
        "RIGHTSOFT",
		/** Middle soft key */
        "MIDDLE",
		/** Asterisk mark */
        "ASTERISK",
		/** Hash mark */
		"HASH",
		/** Clear key */
		"CLEAR",
		/** Edit key */
		"EDIT",
		/** Send key */
		"SEND",
		/** Volume up key */
		"VOLUME_UP",
		/** Volume down key */
		"VOLUME_DOWN"
};


App::Configuration::Configuration()
{
	name = "";
	width = 0;
	height = 0;
	bits = 0;
	fullscreen = 0;
	stencilbuffer = 0;
	captureMouse = 0;
	iconResourceID = 0;
	usedShaders = 0;
	usedParticles = 0;
	idleTime = 1;
}

App::App( OSInterface* os ) :
	m_os( os ),
	m_quit( false )
{
	assert( !GlobalStorage::get().app );
	GlobalStorage::get().app = this;

	for ( int i = 0 ; i < KEY_COUNT ; ++i )
		m_keyDown[i] = false;

	setDataPath( os->getDefaultDataPath() );
}

App::~App()
{
	GlobalStorage::get().app = 0;
}

App* App::get()
{
	assert( GlobalStorage::get().app );
	return GlobalStorage::get().app;
}

void App::deleteApp()
{
	delete GlobalStorage::get().app;
	GlobalStorage::get().app = 0;
}

bool App::initialized()
{
	return GlobalStorage::get().app != 0;
}

void App::activate( bool /*active*/ )
{
	for ( int i = 0 ; i < KEY_COUNT ; ++i )
		setKeyDown( (KeyType)i, false );
}

void App::keyUp( KeyType key )
{
}

void App::keyDown( KeyType key )
{
}

void App::orientationChanged()
{
}

void App::mouseMove( int dx, int dy )
{
}

void App::quit()
{
	m_quit = true;
}

bool App::quitRequested() const	
{
	return m_quit;
}

void App::setKeyDown( KeyType key, bool down )
{
	m_keyDown[key]= down;
}

bool App::isKeyDown( KeyType key ) const
{
	return m_keyDown[key];
}

const char* App::toString( KeyType key )
{
	assert( sizeof(s_keysz)/sizeof(s_keysz[0]) == KEY_COUNT );
	return s_keysz[key];
}

void App::setDataPath( const String& datapath )
{
	m_dataPath = PathName(datapath);
}

String App::path( const String& filename ) const
{
	return PathName( m_dataPath.toString(), PathName(filename).toString() ).toString();
}


END_NAMESPACE() // framework

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
