//-----------------------------------------------------------------------------
// File: jazz.cpp
//
// Desc: This is the first tutorial for using Direct3D. In this tutorial, all
//       we are doing is creating a Direct3D device and using it to clear the
//       window.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define PI 3.14592


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device
LPD3DXSPRITE g_pSprite;

struct Image {
	RECT rect;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 center;
	LPDIRECT3DTEXTURE9 texture;
	BOOL visible;
};

struct Property {
	INT hp;
	INT bulletDamage;
	INT gp;
	FLOAT speed;
	D3DXVECTOR3 bdir;
};

struct Mode {
	BOOL jump;
	BOOL idle;
	BOOL walk;

	BOOL OnGround;

	BOOL left;
	BOOL right;
};

struct Basic {
	Image image;
	Property proprety;
	Mode mode;

};

Basic bg; // 배경
Basic land;

//-----------------------------------------------------------------------------

Basic player;
Basic playerbottom;
Basic playerleft;
Basic playerright;

Basic playerhp;

Basic bullet[30];

Basic box[4];

//-----------------------------------------------------------------------------

Basic enemy[10];
Basic ebullet[30];

//-----------------------------------------------------------------------------

INT bulletcnt = 0;
INT bulletdelay = 0;

INT ebulletcnt = 0;
INT ebulletdelay = 0;

INT enemycnt = 0;
INT enemydelay = 0;

INT pl = 0;
INT pr = 0;


VOID Init();
VOID Update();
VOID enemyctrl();

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And 
    // we request a back buffer format that matches the current desktop display 
    // format.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is 
    // specified since we know it will work on all cards. On cards that support 
    // hardware vertex processing, though, we would see a big performance gain 
    // by specifying hardware vertex processing.
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

BOOL coll(Basic a, Basic b) {

	if (a.image.pos.x <= b.image.pos.x + b.image.rect.right &&
		a.image.pos.x + a.image.rect.right >= b.image.pos.x &&
		a.image.pos.y <= b.image.rect.bottom + b.image.pos.y &&
		a.image.pos.y + a.image.rect.bottom >= b.image.pos.y) {
		return TRUE;
	}

	return FALSE;
}

VOID Init() {
	D3DXCreateSprite(g_pd3dDevice, &g_pSprite);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bg.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &bg.image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"char.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &player.image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"land.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &land.image.texture);

	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"H.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &playerbottom.image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"H.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &playerleft.image.texture);
	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"H.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &playerright.image.texture);

	D3DXCreateTextureFromFileEx(g_pd3dDevice, L"hp.png",
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
		D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &playerhp.image.texture);

	for (int i = 0; i < 30; i++) {
		D3DXCreateTextureFromFileEx(g_pd3dDevice, L"bullet.png",
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &bullet[i].image.texture);
	}
	for (int i = 0; i < 10; i++) {
		D3DXCreateTextureFromFileEx(g_pd3dDevice, L"enemy.png",
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &enemy[i].image.texture);
	}
	for (int i = 0; i < 5; i++) {
		D3DXCreateTextureFromFileEx(g_pd3dDevice, L"container-S.png",
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &box[i].image.texture);
	}

	bg.image.rect = { 0,0,2048,768 };
	bg.image.center = { 0,0,0 };
	bg.image.pos = { 0,0,0 };

	land.image.rect = { 0,0,2048,100 };
	land.image.center = { 0,0,0 };
	land.image.pos = { 0,600,0 };

	box[0].image.rect = { 0,0,300,100 };
	box[0].image.center = { 0,0,0 };

	box[0].image.pos = { 300,500,0 };
	box[1].image.pos = { 900,500,0 };
	box[2].image.pos = { 1500,500,0 };
	box[3].image.pos = { 2000,500,0 };

	for (int i = 1; i < 4; i++) {
		box[i].image.rect = box[0].image.rect;
		box[i].image.center = box[0].image.center;
	}

	player.image.rect = { 0,0,81,124 };
	player.image.center = { 0,0,0 };
	player.image.pos = { 100,300,0 };
	player.proprety.gp = 0;

	player.mode.jump = FALSE;
	player.mode.idle = TRUE;
	player.mode.walk = FALSE;

	player.mode.left = FALSE;
	player.mode.right = FALSE;
	player.mode.OnGround = FALSE;

	playerhp.image.rect = { 0,0,40,3 };
	playerhp.image.center = { 0,0,0 };

	playerbottom.image.rect = { 0,0,30,2 };
	playerbottom.image.center = { 0,0,0 };

	playerleft.image.rect = { 0,0,2,30 };
	playerleft.image.center = { 0,0,0 };

	playerright.image.rect = { 0,0,2,30 };
	playerright.image.center = { 0,0,0 };

	bullet[0].image.rect = { 0,0,4,4 };
	bullet[0].image.center = { 0,0,0 };
	bullet[0].image.pos = { 10,0,0 };
	bullet[0].image.visible = FALSE;
	for (int i = 1; i < 30; i++) {
		bullet[i].image = bullet[0].image;
	}

	enemy[0].image.rect = { 0,0,81,124 };
	enemy[0].image.center = { 0,0,0 };
	enemy[0].image.pos = { 630,535,0 };
	enemy[0].image.visible = TRUE;
	enemy[0].proprety.hp = 50;
}

VOID Update() {


	playerhp.image.pos.x = player.image.pos.x + 5;
	playerhp.image.pos.y = player.image.pos.y - 5;

	playerbottom.image.pos.x = player.image.pos.x + 10;
	playerbottom.image.pos.y = player.image.pos.y + 120;

	playerleft.image.pos.x = player.image.pos.x;
	playerleft.image.pos.y = player.image.pos.y + 80;
	 
	playerright.image.pos.x = player.image.pos.x + 81;
	playerright.image.pos.y = player.image.pos.y + 80;

	//이동과 맵스크롤-------------------------------------------------------------
	if (GetKeyState(VK_LEFT) & 0x80000000) {

		D3DXCreateTextureFromFileEx(g_pd3dDevice, L"charR.png",
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &player.image.texture);
		player.proprety.speed += 1.0f;
		player.mode.left = TRUE;
		if (player.image.pos.x + player.image.rect.right / 2 <= 1024 / 2
			&& bg.image.pos.x < 0)
		{
			bg.image.pos.x += player.proprety.speed;
			land.image.pos.x += player.proprety.speed;
			box[0].image.pos.x += player.proprety.speed;
			box[1].image.pos.x += player.proprety.speed;
			box[2].image.pos.x += player.proprety.speed;
			box[3].image.pos.x += player.proprety.speed;
			enemy[0].image.pos.x += 1;
		}
		else
		{
			player.image.pos.x -= player.proprety.speed;
		}
		if (player.proprety.speed >= 1.0f)
			player.proprety.speed = 1.0f;
	}
	else {
		player.mode.left = FALSE;
	}

	if (GetKeyState(VK_RIGHT) & 0x80000000) {


		pl = 0;
		pr = 1;


		D3DXCreateTextureFromFileEx(g_pd3dDevice, L"char.png",
			D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, NULL, D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, D3DX_FILTER_NONE, NULL, NULL, NULL, &player.image.texture);
		player.proprety.speed += 1.0f;
		player.mode.right = TRUE;
		if (player.image.pos.x + player.image.rect.right / 2 >= 1024 / 2
			&& bg.image.pos.x > -1024)
		{
			bg.image.pos.x -= player.proprety.speed;
			land.image.pos.x -= player.proprety.speed;
			box[0].image.pos.x -= player.proprety.speed;
			box[1].image.pos.x -= player.proprety.speed;
			box[2].image.pos.x -= player.proprety.speed;
			box[3].image.pos.x -= player.proprety.speed;
			enemy[0].image.pos.x -= 1;
		}
		else
		{
			player.image.pos.x += player.proprety.speed;
		}
		if (player.proprety.speed >= 1.0f)
			player.proprety.speed = 1.0f;
	}
	else {
		player.mode.right = FALSE;
	}
	/*if (GetKeyState(VK_DOWN) & 0x80000000 && coll(player, box)) {
		player.image.pos.y = land.image.pos.y - 64;
		player.proprety.gp = 0;
	}*/

	//-----------------------------------------------------------------------------

	//점프-------------------------------------------------------------------------
	player.image.pos.y -= player.proprety.gp;

	if (GetKeyState(VK_UP) & 0x80000000 && player.mode.jump == FALSE) {
		player.proprety.gp = 18;
		player.mode.OnGround = FALSE;
	}

	if (player.mode.OnGround == TRUE) {
		player.proprety.gp = 0;
		player.mode.jump = FALSE;
	}
	if (player.mode.OnGround == FALSE) {
		player.proprety.gp--;
		player.mode.jump = TRUE;
	}	

	if (coll(player, land)) {
		player.mode.OnGround = TRUE;
		player.image.pos.y = land.image.pos.y - 124;
	}


	for (int i = 0; i < 4; i++) {
		if (coll(playerbottom, box[i])) {
			player.mode.OnGround = TRUE;
			player.image.pos.y = box[i].image.pos.y - 124;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (coll(playerleft, box[i])) {
			player.image.pos.x++;
			player.proprety.speed = 0;
		}
	}

	for (int i = 0; i < 4; i++) {
		if (coll(playerright, box[i])) {
			player.mode.OnGround = TRUE;
			player.image.pos.x--;
			player.proprety.speed = 0;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 30; j++) {
			if (coll(bullet[j], box[i])) {
				bullet[j].image.visible = FALSE;
			}
		}
	}

		for (int j = 0; j < 30; j++) {
			if (coll(bullet[j], enemy[0])) {
				enemy[0].proprety.hp--;
				bullet[j].image.visible = FALSE;
				if (enemy[0].proprety.hp < 0) {
					enemy[0].image.visible = FALSE;
				}
			}
		}


	//if (coll(playerbottom, box) == FALSE || coll(player, land) == FALSE) {
	//	player.proprety.gp--;
	//}

	//-----------------------------------------------------------------------------

	//총알발사---------------------------------------------------------------------
	bulletdelay++;
	if (GetKeyState(VK_SPACE) & 0x80000000)
	{
		if (bulletdelay >= 4)
		{
			bulletdelay = 0;
			bulletcnt++;
			if (bullet[bulletcnt].image.visible == FALSE) {
				bullet[bulletcnt].image.visible = TRUE;
			}
			
			if (bulletcnt >= 30)
				bulletcnt = 0;
			
			
		}
	}

	//for (int i = 0; i < 30; i++) {
	//	if (bullet[i].image.pos.x > 1024) {
	//		bullet[i].image.visible = FALSE;
	//	}
	//}
	for (int i = 0; i < 30; i++) {
		bullet[i].proprety.bdir.x = cos(PI / 180 * 180) * 20;
		bullet[i].proprety.bdir.y = sin(PI / 180 * 180) * 20;
	}


	for (int i = 0; i < 30; i++) {
		if (bullet[i].image.visible == TRUE) {
			bullet[bulletcnt].image.pos.y = player.image.pos.y + 40;
			bullet[bulletcnt].image.pos.x = player.image.pos.x;

			//bullet[i].image.pos += bullet[i].proprety.bdir;
			bullet[i].image.pos.x += 20;

		}
	}
	//-----------------------------------------------------------------------------
	enemyctrl();
}

VOID enemyctrl() {


	if(player.image.pos.x - enemy[0].image.pos.x < 0){
		enemy[0].image.pos.x -= 1.0f;
	}
	else {
		enemy[0].image.pos.x += 1.0f;
	}

		if (coll(enemy[0], box[0])) {
			enemy[0].image.pos.y = box[0].image.pos.y - 124;
		}
		else {
			enemy[0].image.pos.y = 480;
		}
		//



}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;

    // Clear the backbuffer to a blue color
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        // Rendering of scene objects can happen here
		g_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		g_pSprite->Draw(bg.image.texture, &bg.image.rect, &bg.image.center, &bg.image.pos, 0xffffffff);
		
		g_pSprite->Draw(land.image.texture, &land.image.rect, &land.image.center, &land.image.pos, 0xffffffff);

		for (int i = 0; i < 4; i++) {
			g_pSprite->Draw(box[i].image.texture, &box[i].image.rect, &box[i].image.center, &box[i].image.pos, 0xffffffff);
		}
		

		g_pSprite->Draw(player.image.texture, &player.image.rect, &player.image.center, &player.image.pos, 0xffffffff);

		g_pSprite->Draw(playerhp.image.texture, &playerhp.image.rect, &playerhp.image.center, &playerhp.image.pos, 0xffffffff);

		g_pSprite->Draw(playerbottom.image.texture, &playerbottom.image.rect, &playerbottom.image.center, &playerbottom.image.pos, 0xffffffff);
		g_pSprite->Draw(playerleft.image.texture, &playerleft.image.rect, &playerleft.image.center, &playerleft.image.pos, 0xffffffff);
		g_pSprite->Draw(playerright.image.texture, &playerright.image.rect, &playerright.image.center, &playerright.image.pos, 0xffffffff);

		for (int i = 0; i < 30; i++) {
			if (bullet[i].image.visible == TRUE) {
				g_pSprite->Draw(bullet[i].image.texture, &bullet[i].image.rect, &bullet[i].image.center, &bullet[i].image.pos, 0xffffffff);
			}
		}

		for (int i = 0; i < 10; i++) {
			if (enemy[i].image.visible == TRUE) {
				g_pSprite->Draw(enemy[i].image.texture, &enemy[i].image.rect, &enemy[i].image.center, &enemy[i].image.pos, 0xffffffff);
			}
		}

		g_pSprite->End();
        // End the scene
        g_pd3dDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
    UNREFERENCED_PARAMETER( hInst );

    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 01: jazz",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
		Init();
        // Show the window
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );

        // Enter the message loop
        MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				Render();
				Update();
			}
		}

    }

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



