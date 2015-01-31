#include <SDL.h>

static const int KEYBUFFERSIZE = 256;

static int keybuffer[KEYBUFFERSIZE];
static int keybuf_start;
static int keybuf_end;
static int keymap[KEYBUFFERSIZE];


void KeyBuf_Init(void)
{
	keybuf_start = 0;
	keybuf_end = 0;
}


void KeyBuf_PutKey(int key)
{
	int t = keybuf_end;
	if (++t > KEYBUFFERSIZE)
		t = 0;
	if (t == keybuf_start)		// keybuffer is full
		return;
	keybuffer[t] = key;
	keybuf_end = t;
}


bool KeyBuf_IsEmpty(void)
{
	return keybuf_start == keybuf_end;
}


// returns -1 if there is no key to be read.
int KeyBuf_GetKey(void)
{
	int key;
	if (KeyBuf_IsEmpty())
		return -1;
	key = keybuffer[keybuf_start++];
	if (keybuf_start > KEYBUFFERSIZE)
		keybuf_start = 0;
}


static int KeyFilter(const SDL_Event *event)
{
	static int Esc_KeyPress = 0;
	static int Space_KeyPress = 0;
	static int Return_KeyPress = 0;

	int r = 1;
	switch (event->type) {
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_ESCAPE:
			if (1 == Esc_KeyPress) {
				r = 0;
			} else {
				Esc_KeyPress = 1;
			}
			break;
		case SDLK_RETURN:
			if (1 == Return_KeyPress) {
				r = 0;
			} else {
				Return_KeyPress = 1;
			}
			break;
		case SDLK_SPACE:
			if (1 == Space_KeyPress) {
				r = 0;
			} else {
				Space_KeyPress = 1;
			}
			break;
		default:
			break;
		}
		break;
	case SDL_KEYUP:
		switch (event->key.keysym.sym) {
		case SDLK_ESCAPE:
			Esc_KeyPress = 0;
			break;
		case SDLK_SPACE:
			Space_KeyPress = 0;
			break;
		case SDLK_RETURN:
			Return_KeyPress = 0;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return r;
}


//����ESC��RETURN��SPACE����ʹ���ǰ��º����ظ���
//static int KeyFilter(const SDL_Event *event)

int Input_Init(void)
{
    SDL_SetEventFilter(KeyFilter);
	return 0;
}


void Input_Quit(void)
{
}


/*
====================
JY_GetKey
@return:
    -1: no keypress
    -3: window's close button is pressed
====================
*/

int JY_GetKey(void)
{
    static bool virgin = true;
    if (virgin) {
        Input_Init();
        virgin = false;
    }
	SDL_Event event;
    SDLKey keycode = SDLK_UNKNOWN;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			keycode = event.key.keysym.sym;
			if (keycode == SDLK_F4)
				return -3;
			return keycode;
			break;

			// When Close Window Button was pressed
		case SDL_QUIT:
			return -3;
			break;

		default:
			break;
		}
	}
    if (keycode == SDLK_UNKNOWN)
        return -1;
    else
        return keycode;
}


