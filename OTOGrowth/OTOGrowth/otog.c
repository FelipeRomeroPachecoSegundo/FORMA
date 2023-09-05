#include <windows.h>
#include <string.h>
#include <commdlg.h>
#include <memory.h>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#include "resource.h"
#include "random.h"
#include "struct.h"

#define UNI	   ((s1new<<16)+(s2new&0xffff))

#define PI	3.1415926535897932385

#define TRL		0
#define OFL4	1
#define SQL		2
#define OFL8	3



//unsigned long s1=1, s2=2,unicalls=0;

//function declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgLatticeProceed(HWND hDlgLat, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgParamProceed(HWND hDlgLat, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL DibSaveImage (PTSTR pstrFileName, BITMAPFILEHEADER * pbmfh, int run);
BOOL SaveBitmapFile(HDC, HBITMAP, PSTR);


BOOL IsButtonChecked(int nID);

int Width(RECT rc); //comprimento do cir
int Height(RECT rc); //altura do cir
void StartThingsGoing(HWND hDlgParam);
void NewPoint(int* xpos, int* ypos); //novo ponto
//void CheckRadius(int i,int j);
int OutofRange(int i, int j); //localização
int CheckRadius(int i, int j); //ver distancia maxima
void vonmise(double random); //valor aleatorio vonmise


/**************************ESTRUTURAS*************************************/


//Global Variables

double ox, oy, oox, ooy;	// oflattica walker position
double step_size = 1.0;	// step size
double disobs = 110.0;
double pospart1 = 5.0;
double mi, Parte1, Parte2, radius; //aqui modifiquei
int ciratu = 5; //aqui modifiquei colocar valor maximo 5
int Quadrante = 1;  //define o quatrate
int corre = 1; //variavel correcao do fluxo

HWND ghwnd, hDlgLat, hDlgParam;
HMENU hMenu;
HINSTANCE hDLinstance;
MSG          msg;
HDC hdc, hdcMemory;
HBITMAP hLattice;
BITMAP bm;
DWORD size;
HCURSOR hcold;
BITMAPINFOHEADER bmih;
BITMAPINFO bmi;


int LATTICE_SIZE = MAXSIZE;
int WINDOW_SIZE = 512, XWINDOW_SIZE = 512, YWINDOW_SIZE = 512;


double alphap, tscale;
int maxsize, rmin, deltr, step = 1000;
FILE* filepoint;
int display, show, closed, running, bAbort, equilib = 0, new_damage = 0;
unsigned long prob, prob2, prob3, prob4, prob5, prob6, massmax, mass, r = 3735839L;
long xpos, ypos, total;
BYTE* buffer;
int im[MAXSIZE], ip[MAXSIZE], circle = 0, elipse = 0;
int LATT = 0; //DEFINE LATT PARA O CAMINHAR
int radius2, cx, cy, cxt, cyt, count = 0, xradius, yradius, focus;
unsigned long RMAX16, RMAX26, RMAX36, RMAX46, RMAX56;
unsigned long RMAX14, RMAX24, RMAX34;
char filename[100];

time_t start_time, end_time;

PIXEL** pixel;
PARTICLE part[MAXPARTICLES];

PARTICLE contour[MAXPARTICLES];
//int teste(const int );


/****************************************************************************************************/


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow) {

	static char szAppName[] = "OTOG";

	MSG          msg;
	WNDCLASS     wndclass;

	int b;


	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = 0;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = IDR_MENU;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, "Nao pode rodar!",
			szAppName, MB_ICONERROR);
		return 0;
	}

	ghwnd = CreateWindow(szAppName,                 // window class name
		"OTOGrowth: Otolith growth simulation",				  // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		CW_USEDEFAULT,              // initial x size
		CW_USEDEFAULT,              // initial y size
		NULL,                       // parent window handle
		hMenu,                       // window menu handle
		hInstance,                  // program instance handle
		NULL);                     // creation parameters

	ShowWindow(ghwnd, iCmdShow);
	UpdateWindow(ghwnd);

	SendMessage(ghwnd, WM_COMMAND, IDM_INIPARAM, 0);


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;

}

/*******************************************************************************************/

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) {


	case WM_COMMAND:

		switch (LOWORD(wParam)) {

		case IDM_LATSIZE:

			hDlgLat = CreateDialog(hDLinstance, IDD_DIALOGLATTICE, 0, DlgParamProceed); //DlgLatticeProceed troca param OK
			ShowWindow(hDlgLat, SW_SHOW);
			UpdateWindow(hDlgLat);
			return msg.wParam;
			break;

		case IDM_INIPARAM:

			yradius = xradius = 512;
			radius2 = xradius * xradius;
			XWINDOW_SIZE = xradius * 2;
			YWINDOW_SIZE = yradius * 2;
			circle = 1;
			AdjustWindowSize(ghwnd, XWINDOW_SIZE, YWINDOW_SIZE);

			hDlgParam = CreateDialog(hDLinstance, IDD_DLGPARAM, 0, DlgParamProceed);
			ShowWindow(hDlgParam, SW_SHOW);
			UpdateWindow(hDlgParam);
			return msg.wParam;
			break;

		}



	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

/*******************************************************************************************/


//VERIFICAR ja pode retirar
BOOL CALLBACK DlgLatticeProceed(HWND hDlgLat, UINT message, WPARAM wParam, LPARAM lParam) {

	static char enter[NUMBERLEN + 1];


	switch (message) {

	case WM_INITDIALOG:
		CheckDlgButton(hDlgLat, IDC_CIRCLE, 0);
		CheckDlgButton(hDlgLat, IDC_ELLIPSE, 0);
		CheckDlgButton(hDlgLat, IDC_TRIANGLE, 0);
		//MODIFICADO
		CheckDlgButton(hDlgLat, IDC_TRL, 0);
		//			CheckDlgButton(hDlgTeste,IDC_SQL,0);
		//			CheckDlgButton(hDlgTeste,IDC_OFL4,0);
		//			CheckDlgButton(hDlgTeste,IDC_OFL8,0);
		//ATE AQUI			
		SetDlgItemText(hDlgLat, IDC_xRE, "512");
		SetDlgItemText(hDlgLat, IDC_yRE, "256");
		SetDlgItemText(hDlgLat, IDC_RC, "512");

		return 1;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDC_CIRCLE:
			//GetDlgItemText(hDlgLat, IDC_RC,enter,NUMBERLEN);
			xradius = atof(enter);
			yradius = xradius;
			radius2 = xradius * xradius;
			XWINDOW_SIZE = xradius * 2;
			YWINDOW_SIZE = yradius * 2;
			circle = 1;
			AdjustWindowSize(ghwnd, XWINDOW_SIZE, YWINDOW_SIZE);
			EndDialog(hDlgLat, TRUE);
			return 1;

		case IDC_ELLIPSE:
			GetDlgItemText(hDlgLat, IDC_xRE, enter, NUMBERLEN);
			cx = xradius = atof(enter);
			GetDlgItemText(hDlgLat, IDC_yRE, enter, NUMBERLEN);
			cy = yradius = atof(enter);
			focus = sqrt(xradius * xradius - yradius * yradius); //calca c
			XWINDOW_SIZE = xradius * 2;
			YWINDOW_SIZE = yradius * 2;
			elipse = 1;
			AdjustWindowSize(ghwnd, XWINDOW_SIZE, YWINDOW_SIZE);
			EndDialog(hDlgLat, TRUE);
			return 1;

		case IDC_TRIANGLE:
			return 1;

		}

	}

	return 0;
}



/********************************************************************************************/

int AdjustWindowSize(HWND ghwnd, int cxDib, int cyDib)  //ajusta a janela
{
	RECT rwin, rcli;

	GetWindowRect(ghwnd, &rwin);
	GetClientRect(ghwnd, &rcli);
	SetWindowPos(ghwnd, HWND_NOTOPMOST, rwin.left, rwin.top, cxDib + Width(rwin) - Width(rcli),
		cyDib + Height(rwin) - Height(rcli), SWP_SHOWWINDOW);
}

/******************************************************************************************/

int Width(RECT rc)
{
	return rc.right - rc.left;
}
/**********************************************************/

int Height(RECT rc)
{
	return rc.bottom - rc.top;
}
/**********************************************************/

//PROCEDIMENTO PARA CAIXA DE DIALOGO
BOOL CALLBACK DlgParamProceed(HWND hDlgParam, UINT message, WPARAM wParam, LPARAM lParam) {

	static int dx, dy, xsize, ysize, ls, x, y;
	static char string[NUMBERLEN + 1];
	RECT rc;
	int run;
	int* Hbuf;
	int j, i;

	switch (message) {

	case WM_INITDIALOG:

		//			CheckDlgButton(hDlgParam,IDC_C,BST_CHECKED); //define o botao inicial COMENTOU
		//			CheckDlgButton(hDlgParam,IDC_E,0);
		CheckRadioButton(hDlgParam, IDC_C, IDC_E, IDC_C);


		//			CheckDlgButton(hDlgParam,IDC_TRL,BST_CHECKED); CRIOU NOVO
		CheckRadioButton(hDlgParam, IDC_TRL, IDC_OFL8, IDC_TRL);

		SetDlgItemText(hDlgParam, IDC_ALPHA, "1.0");
		SetDlgItemText(hDlgParam, IDC_MAX, "50000");
		SetDlgItemText(hDlgParam, IDC_RXMIN, "150");
		SetDlgItemText(hDlgParam, IDC_RYMIN, "50");
		SetDlgItemText(hDlgParam, IDC_DR, "10"); ///tamanho do raio
		SetDlgItemText(hDlgParam, IDC_AGE, "1000");

		dx = 2 * GetSystemMetrics(SM_CXFRAME);
		dy = GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION);
		xsize = GetSystemMetrics(SM_CXSCREEN);
		ysize = GetSystemMetrics(SM_CYSCREEN);

		hdc = GetDC(ghwnd);
		hdcMemory = CreateCompatibleDC(hdc);

		hLattice = CreateCompatibleBitmap(hdc, LATTICE_SIZE, LATTICE_SIZE);


		if (!hLattice) {
			MessageBox(0, "Could not create Bitmab!", "Error", MB_OK);
			SendMessage(ghwnd, WM_COMMAND, IDM_LATSIZE, 0L); //IDM_LATSIZE
		}

		break;


	case WM_COMMAND:

		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			bAbort = 1;
			EndDialog(hDlgParam, 1);
			PostQuitMessage(0);  /* send a WM_QUIT to the message queue */
			DestroyWindow(ghwnd);
			return 1;

		case IDC_C: //circular
			circle = 1; elipse = 0;
			EnableWindow(GetDlgItem(hDlgParam, IDC_RYMIN), 0);
			break;

		case IDC_E: //elipse
			circle = 0; elipse = 1;
			EnableWindow(GetDlgItem(hDlgParam, IDC_RYMIN), 1);
			break;
			//MODIFICADO
		case IDC_TRL:
			//					#define TRL
			LATT = 0;
			return 1;

		case IDC_SQL:
			//					#define SQL
			LATT = 2;
			return 1;
			//
		case IDC_OFL4:
			//					#define OFL4
			LATT = 1;
			return 1;

		case IDC_OFL8:
			//					#define OFL8
			LATT = 3;
			return 1;
			//ATE AQUI


		case IDC_AGE:
			if (HIWORD(wParam) == EN_CHANGE)
				return(TRUE);
			GetDlgItemText(hDlgParam, IDC_AGE, string, NUMBERLEN);
			step = atof(string);

			break;

		case IDC_CLEAR:

			GetWindowRect(hdc, (RECT FAR*) & rc);
			BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
				0, 0, 0, WHITENESS);

			break;

		case IDC_SHOW:
			show = IsDlgButtonChecked(hDlgParam, IDC_SHOW);
			break;

		case IDC_DISPLAY:
			display = IsDlgButtonChecked(hDlgParam, IDC_DISPLAY);
			break;

		case IDC_CE:

			closed = IsDlgButtonChecked(hDlgParam, IDC_CE);

			return 1;

		case IDC_START:

			if (running) {
				SetDlgItemText(hDlgParam, IDC_START, "&START");
				bAbort = 1;
				running = 0;
				break;
			}
			srand(time(NULL));
			time(&start_time);

			show = IsDlgButtonChecked(hDlgParam, IDC_SHOW);
			display = IsDlgButtonChecked(hDlgParam, IDC_DISPLAY);

			GetDlgItemText(hDlgParam, IDC_ALPHA, string, NUMBERLEN);
			alphap = atof(string);
			GetDlgItemText(hDlgParam, IDC_AGE, string, NUMBERLEN);
			step = atof(string);
			GetDlgItemText(hDlgParam, IDC_MAX, string, NUMBERLEN);
			maxsize = atoi(string);

			GetDlgItemText(hDlgParam, IDC_RXMIN, string, NUMBERLEN);
			xradius = atoi(string);
			if (circle)
				yradius = xradius;
			else
			{
				GetDlgItemText(hDlgParam, IDC_RYMIN, string, NUMBERLEN);
				yradius = atof(string);
			}
			if (circle) { radius2 = xradius * xradius; }
			else if (elipse) { radius2 = sqrt(xradius * xradius + yradius * yradius); }



			GetDlgItemText(hDlgParam, IDC_DR, string, NUMBERLEN);
			deltr = atoi(string);


			prob = (unsigned int)(alphap * RMAX);
			prob2 = (unsigned int)(alphap * alphap * RMAX);
			prob3 = (unsigned int)(alphap * alphap * alphap * RMAX);
			prob4 = (unsigned int)(alphap * alphap * alphap * alphap * RMAX);
			prob5 = (unsigned int)(alphap * alphap * alphap * alphap * alphap * RMAX);
			prob6 = (unsigned int)(alphap * alphap * alphap * alphap * alphap * alphap * RMAX);

			massmax = LATTICE_SIZE * LATTICE_SIZE;

			xpos = ypos = bAbort = 0;
			running = 1;
			SetDlgItemText(hDlgParam, IDC_START, "&STOP");
			equilib = 0;
			new_damage = 1;
			StartThingsGoing(hDlgParam);

			return 1;

			//		case IDC_STARTCUDA:

					//CRIAR CAIXA DE AVISO AINDA EM PROGRAMAÇAO



		case IDC_SAVE:
			/*
			run=0;
			DibSaveImage(pstrFileName, bmih, run);
			*/
			strcpy_s(filename, sizeof(filename), "bitmap.bmp");
			SaveBitmapFile(hdcMemory, hLattice, filename);

			return 1;

		}



	}

	return 0;
}






void StartThingsGoing(HWND hDlg) {

	WORD lbytes, pbytes, pwords, lwords;
	int wsize, RowLength, way;
	int i, j, ii, jj, viz;
	int x0, y0, xsize, ysize;
	int boja = 255, bojar = 255, bojag = 255, bojab = 255, collor = 2, xstart = 0, ystart = 0;
	HPEN hpenOld, hpenBlue;
	static char string[40];
	int hour, min, sec;
	int rwidth, rheigth, sizebytes;
	RECT rw, rh;
	float Q1, Q2, Q3, P1, P2, P3, P4;
	int xc, yc, stop;
	FILE* h;
	char fname[256];

	sprintf_s(fname, sizeof(fname), "DLA_%.3f%s.txt", alphap, closed ? "_CLOSED" : "");
	h = fopen_s(&filepoint, fname, "w+");

	RMAX14 = RMAX / 4.0;
	RMAX24 = 2 * (RMAX / 4.0);
	RMAX34 = 3 * (RMAX / 4.0);

	RMAX16 = RMAX / 6.0;
	RMAX26 = 2 * (RMAX / 6.0);
	RMAX36 = 3 * (RMAX / 6.0);
	RMAX46 = 4 * (RMAX / 6.0);
	RMAX56 = 5 * (RMAX / 6.0);
	P1 = P2 = P3 = P4 = 0.25;
	//	P2=0.30;
	//	P3=0.35;
	way = 1;

	switch (way) {
	case 1:
		Q1 = P1;
		Q2 = Q1 + (1 - P1) / 3;
		Q3 = Q2 + (1 - P1) / 3;
		break;
	case 2:
		Q1 = (1 - P2) / 3;
		Q2 = Q1 + P2;
		Q3 = Q2 + (1 - P2) / 3;
		break;
	case 3:
		Q1 = (1 - P3) / 3;
		Q2 = Q1 + (1 - P3) / 3;
		Q3 = Q2 + P3;
		break;
	case 4:
		Q1 = (1 - P4) / 3;
		Q2 = Q1 + (1 - P4) / 3;
		Q3 = Q2 + (1 - P4) / 3;
		break;
	case 5:
		Q1 = P1;
		Q2 = Q1 + P2;
		Q3 = Q2 + (1 - (P1 + P2)) / 2;
		break;
	case 6:
		Q1 = P1;
		Q2 = Q1 + (1 - (P1 + P3)) / 2;
		Q3 = Q2 + P3;
		break;
	case 7:
		Q1 = P1;
		Q2 = Q1 + (1 - (P1 + P4)) / 2;
		Q3 = Q2 + (1 - (P1 + P4)) / 2;
		break;
	case 8:
		Q1 = (1 - (P2 + P3)) / 2;
		Q2 = Q1 + P2;
		Q3 = Q2 + P3;
		break;
	case 9:
		Q1 = (1 - (P2 + P4)) / 2;
		Q2 = Q1 + P2;
		Q3 = Q2 + (1 - (P2 + P3)) / 2;
		break;
	case 10:
		Q1 = (1 - (P3 + P4)) / 2;
		Q2 = Q1 + (1 - (P3 + P4)) / 2;
		Q3 = Q2 + P3;
		break;
	}
	//	RMAX4=Q1*RMAX;
	//	RMAX2=Q2*RMAX;
	//	RMAX34=Q3*RMAX;

	GetObject(hLattice, sizeof(BITMAP), &bm);

	pbytes = lbytes = bm.bmWidth;
	size = bm.bmHeight * bm.bmWidth;
	wsize = size >> 1;


	pwords = pbytes >> 1;
	lwords = lbytes >> 1;

	hcold = SetCursor(LoadCursor((HINSTANCE)NULL, (LPCSTR)IDC_WAIT));

	total = 0;


	//	RowLength =4*((bm.bmHeight*bm.bmBitsPixel + 31)/32);
	pixel = malloc(bm.bmHeight * sizeof(PIXEL*));

	buffer = malloc(bm.bmHeight * bm.bmWidth * sizeof(PIXEL));
	for (j = 0;j < bm.bmHeight;j++)
		pixel[j] = ((PIXEL*)buffer) + bm.bmWidth * j;


	for (j = 0;j < bm.bmHeight;j++) {
		for (i = 0;i < bm.bmWidth;i++) {
			pixel[j][i].red = 0;
			pixel[j][i].blue = 0;
			pixel[j][i].green = 0;
		}
	}

	for (i = 0;i < LATTICE_SIZE;i++)
	{
		im[i] = i - 1; ip[i] = i + 1;
	}
	im[0] = LATTICE_SIZE - 1;
	ip[LATTICE_SIZE - 1] = 0;

	radius = 5;
	radius2 = radius * radius;

	//	cx=i=XLATTICE_SIZE/2;
	//	cy=j=YLATTICE_SIZE/2;
	tscale = sqrt(3.0) / 2;
	cx = cy = i = j = LATTICE_SIZE / 2;
	cxt = cx - cy / 2;
	cyt = cy * tscale;
	pixel[j][i].red = bojar;
	pixel[j][i].green = bojag;
	pixel[j][i].blue = bojab;

	mass = 1;
	part[0].x = i;
	part[0].y = j;
	part[0].t = 1;



	sizebytes = bm.bmWidthBytes * bm.bmHeight;

	bmih.biSize = sizeof(BITMAPINFOHEADER);

	bmih.biBitCount = 24;

	bmih.biClrImportant = 0;

	bmih.biClrUsed = 0;

	bmih.biCompression = BI_RGB;

	bmih.biHeight = bm.bmHeight;

	bmih.biWidth = bm.bmWidth;

	bmih.biPlanes = 1;

	bmih.biSizeImage = sizebytes;

	bmih.biXPelsPerMeter = 0;

	bmih.biYPelsPerMeter = 0;



	bmi.bmiHeader = bmih;



	SetDIBits(hdcMemory, hLattice, 0, bm.bmHeight, buffer, &bmi, DIB_RGB_COLORS);
	SelectObject(hdcMemory, hLattice);
	BitBlt(hdc, xstart, ystart, XWINDOW_SIZE, YWINDOW_SIZE, hdcMemory, 0, 0, SRCCOPY);



	SetCursor(hcold);
	/***************************************************************/

	CheckRadius(j, i);
	NewPoint(&j, &i); //novo ponto 1° em diante

	disobs = (double)sqrt((double)((ox - cx) * (ox - cx)) + ((oy - cy) * (oy - cy)));  // circulo
	//disobs = (double) (((oy-cx)*(oy-cx) )/(radius*radius)) + (((ox-cy)*(ox-cy))/(0.19*radius*radius)); //elipesi

	pospart1 = 5 * (double)(disobs / radius);
	//pospart1 = 5*disobs; //elipse

	ciratu = (int)pospart1;

	if (ox > cx && oy > cy) Quadrante = 1;
	if (ox >= cx && oy <= cy) Quadrante = 2;
	if (ox <= cx && oy <= cy) Quadrante = 3;
	if (ox<cx && oy>cy) Quadrante = 4;



	pixel[j][i].red = bojar;
	pixel[j][i].green = bojag;
	pixel[j][i].blue = bojab;

	hpenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
	hpenOld = SelectObject(hdc, hpenBlue);

	count = 0;

	while (!bAbort)
	{
		while (PeekMessage((MSG FAR*) & msg, hDlgParam, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}//WHILE 2 FECHADO

		while (PeekMessage((MSG FAR*) & msg, ghwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}//WHILE 3 FECHADO

		while (1) {
			if (bAbort)
				break;

			total++;
			if (total > 5100)
				ii = 1;
			ii = i;jj = j;
			oox = ox;ooy = oy;
			r = UNI;
			if (LATT == 2) //latt SQL
			{
				if (r > RMAX34) i = im[i];
				else if (r > RMAX24) i = ip[i];
				else if (r > RMAX14) j = im[j];
				else j = ip[j];
			}
			else if (LATT == 0)  //latt TRL
			{
				ostep(&j, &i);
				//	i=ip[i];
				//	j=ip[j];
					/*if(r>RMAX56)
						{i=im[i];j=im[j];}
					else if(r>RMAX46)
						{i=ip[i];j=ip[j];} //walk triangular lattice
					else if(r>RMAX36)
						i=ip[i];
					else if(r>RMAX26)
						i=im[i];
					else if(r>RMAX16)
						j=im[j];
					else
						j=ip[j];*/
			}
			else if ((LATT == 1) || (LATT == 3))
				ostep(&j, &i);    //walk offlattice


			disobs = (double)sqrt((double)((ox - cx) * (ox - cx)) + ((oy - cy) * (oy - cy)));
			//disobs = (double) (((oy-cx)*(oy-cx) )/(radius*radius)) + (((ox-cy)*(ox-cy))/(0.19*radius*radius)); //elipesi


			pospart1 = 5 * ((double)disobs / radius);
			//pospart1 = 5 * disobs; //elipse


			ciratu = (int)pospart1;

			if (ox > cx && oy > cy) Quadrante = 1;
			if (ox >= cx && oy <= cy) Quadrante = 2;
			if (ox <= cx && oy <= cy) Quadrante = 3;
			if (ox<cx && oy>cy) Quadrante = 4;


			//ATÉ AQUI PAROU OS DEF


			if (((pixel[j][i].red)) != 0) {
				ox = oox;oy = ooy;i = ii;j = jj;continue;
			}
			else if (((pixel[j][i].green)) != 0) {
				ox = oox;oy = ooy;i = ii;j = jj;continue;
			}
			else if (((pixel[j][i].blue)) != 0) {
				ox = oox;oy = ooy;i = ii;j = jj;continue;
			}

			pixel[jj][ii].red = 0;
			pixel[jj][ii].green = 0;
			pixel[jj][ii].blue = 0;

			if (OutofRange(j, i))
			{
				if (closed)
				{
					do {
						r = UNI;

						i = ii;j = jj;
						if (LATT == 2) //latt SQL
						{
							if (r > RMAX34) i = im[i];
							else if (r > RMAX24) i = ip[i];
							else if (r > RMAX14) j = im[j];
							else j = ip[j];
						}
						else if (LATT == 0) //latt TRL

						{

							ostep(&j, &i);
							//i=ip[i];
							//j=ip[j];
						/*
						if(r>RMAX56)
							{i=im[i];j=im[j];}
						else if(r>RMAX46)
							{i=ip[i];j=ip[j];} //walk triangular lattice
						else if(r>RMAX36)
							i=ip[i];
						else if(r>RMAX26)
							i=im[i];
						else if(r>RMAX16)
							j=im[j];
						else
							j=ip[j];
							*/
						}

						else if ((LATT == 1) || (LATT == 3))								ostep(&j, &i);    //walk offlattice



						if ((pixel[j][i].red != 0) && (pixel[j][i].green != 0) && (pixel[j][i].blue != 0))
						{
							ox = oox;oy = ooy;i = ii;j = jj;continue;
						}
					} while (OutofRange(j, i));
				}
				else
				{
					NewPoint(&j, &i);  //o ponto fora gera novo


					disobs = (double)sqrt((double)((ox - cx) * (ox - cx)) + ((oy - cy) * (oy - cy)));
					//disobs = (double) (((oy-cx)*(oy-cx) )/(radius*radius)) + (((ox-cy)*(ox-cy))/(0.19*radius*radius)); //ellipesi
					pospart1 = 5 * ((double)disobs / radius);
					// pospart1 = 5 * disobs; //elipse

					ciratu = (int)pospart1;

					if (ox > cx && oy > cy) Quadrante = 1;
					if (ox >= cx && oy <= cy) Quadrante = 2;
					if (ox <= cx && oy <= cy) Quadrante = 3;
					if (ox<cx && oy>cy) Quadrante = 4;


					if ((pixel[j][i].red != 0) && (pixel[j][i].green != 0) && (pixel[j][i].blue != 0))
					{
						ox = oox;oy = ooy;i = ii;j = jj;continue;
					}
				}
			}

			pixel[j][i].red = bojar;
			pixel[j][i].green = bojag;
			pixel[j][i].blue = bojab;

			if (LATT == 0) //latt TRL
				//seis vizinhos
			{
				viz = (((pixel[j][im[i]].red) != 0) && ((pixel[j][im[i]].green) != 0) && ((pixel[j][im[i]].blue) != 0)) +
					(((pixel[j][ip[i]].red) != 0) & ((pixel[j][ip[i]].green) != 0) & ((pixel[j][ip[i]].blue) != 0)) +
					(((pixel[im[j]][i].red) != 0) & ((pixel[im[j]][i].green) != 0) & ((pixel[im[j]][i].blue) != 0)) +
					(((pixel[ip[j]][i].red) != 0) && ((pixel[ip[j]][i].green) != 0) && ((pixel[ip[j]][i].blue) != 0)) +
					(((pixel[im[j]][im[i]].red) != 0) && ((pixel[im[j]][im[i]].green) != 0) && ((pixel[im[j]][im[i]].blue) != 0)) +
					(((pixel[ip[j]][ip[i]].red) != 0) && ((pixel[ip[j]][ip[i]].green) != 0) && ((pixel[ip[j]][ip[i]].blue) != 0));

			}

			else if (LATT == 3) //latt OFL8
				//8 vizinhos
			{
				viz = (((pixel[j][im[i]].red) != 0) && ((pixel[j][im[i]].green) != 0) && ((pixel[j][im[i]].blue) != 0)) +
					(((pixel[j][ip[i]].red) != 0) & ((pixel[j][ip[i]].green) != 0) & ((pixel[j][ip[i]].blue) != 0)) +
					(((pixel[im[j]][i].red) != 0) & ((pixel[im[j]][i].green) != 0) & ((pixel[im[j]][i].blue) != 0)) +
					(((pixel[ip[j]][i].red) != 0) && ((pixel[ip[j]][i].green) != 0) && ((pixel[ip[j]][i].blue) != 0)) +
					(((pixel[im[j]][im[i]].red) != 0) && ((pixel[im[j]][im[i]].green) != 0) && ((pixel[im[j]][im[i]].blue) != 0)) +
					(((pixel[ip[j]][ip[i]].red) != 0) && ((pixel[ip[j]][ip[i]].green) != 0) && ((pixel[ip[j]][ip[i]].blue) != 0)) +
					(((pixel[im[j]][ip[i]].red) != 0) && ((pixel[im[j]][ip[i]].green) != 0) && ((pixel[im[j]][ip[i]].blue) != 0)) +
					(((pixel[ip[j]][im[i]].red) != 0) && ((pixel[ip[j]][im[i]].green) != 0) && ((pixel[ip[j]][im[i]].blue) != 0));
			}

			else if ((LATT == 1) || (LATT == 2))

			{
				viz = (((pixel[j][im[i]].red) != 0) && ((pixel[j][im[i]].green) != 0) && ((pixel[j][im[i]].blue) != 0)) +
					(((pixel[j][ip[i]].red) != 0) & ((pixel[j][ip[i]].green) != 0) & ((pixel[j][ip[i]].blue) != 0)) +
					(((pixel[im[j]][i].red) != 0) & ((pixel[im[j]][i].green) != 0) & ((pixel[im[j]][i].blue) != 0)) +
					(((pixel[ip[j]][i].red) != 0) && ((pixel[ip[j]][i].green) != 0) && ((pixel[ip[j]][i].blue) != 0));

			}



			if (viz)
			{
				r = UNI;
				if (LATT == 0) //latt TRL
				{
					if ((viz == 1 && r > prob4) || (viz == 2 && r > prob3) || (viz == 3 && r > prob2) || (viz == 4 && r > prob))
						continue;
				}
				else if (LATT == 3) //latt OFL 8
				{
					if ((viz == 1 && r > prob6) || (viz == 2 && r > prob5) || (viz == 3 && r > prob4) || (viz == 4 && r > prob3) || (viz == 5 && r > prob2) || (viz == 6 && r > prob))
						continue;
				}

				//PAROU AQUI ??????? else if

				else if ((LATT == 1) || (LATT == 2))
				{
					if ((viz == 1 && r > prob2) || (viz == 2 && r > prob))
						continue;

				}
				//ATE AQUI EM MUDANCA



				part[mass].x = i;
				part[mass].y = j;
				part[mass].t = total;
				mass++;
				if (mass == 432) {
					mass = 432;
				}
				fprintf(h, "%d\t%d\t%d\n", i, j, mass);
				fflush(h);

				CheckRadius(j, i);
				do {
					NewPoint(&j, &i);
				} while (OutofRange(j, i));
				pixel[j][i].red = bojar;
				pixel[j][i].green = bojag;
				pixel[j][i].blue = bojab;

				if (mass == maxsize /*|| CheckRadius(j,i)==1*/) {
					/*
										GetContour(mass,bm.bmHeight,bm.bmWidth,pixel,&contour);
										for(i=1;i<contour[0].t;i++){
											contour[i].t=0;
										}
										xc=yc=0;
										for(i=0;i<contour[0].t;i++){
											xc+=contour[i].x;
											yc+=contour[i].y;
										}
										xc/=contour[0].t;
										yc/=contour[0].t;
										for(i=0;i<contour[0].t;i++){
											contour[i].x-=xc;
											contour[i].y-=yc;
										}
										MFdfa(contour);
										*/
					sprintf_s(filename, sizeof(filename), "%fbitmap.BMP", alphap);
					SaveBitmapFile(hdcMemory, hLattice, filename);
					bAbort = 1;
				}

				if (count >= step) {
					count = 0;
					boja--;
					if (collor > 3) collor = 1;
					if (collor == 1) {
						bojar = bojar - 15;
						bojag = bojag - 15;
						bojab = bojab - 15;
						collor++;
					}
					else if (collor == 2) {
						bojar = 255;
						bojag = 255;
						bojab = 255;
						collor++;
					}
					else if (collor == 3) {
						bojar = 255;
						bojag = 255;
						bojab = 255;
						collor++;
					}
					if (collor == 2) boja -= 5;
					if (boja < 5)	boja = 255;
					if (bojar < 5) bojar = 255;
					if (bojag < 5) bojag = 255;
					if (bojab < 5) bojab = 255;

				}

				break;
			}
			else if (show)
				break;

		}//WHILE !aBOLT FECHADO


		if (mass == 5200)
		{
			stop = 1;
		}
		wsprintf(string, "%i", total);
		SetDlgItemText(hDlgParam, IDC_MSC, string);

		sprintf_s(string, sizeof(string), "%i", mass);
		SetDlgItemText(hDlg, IDC_TOTAL, string);


		//xradius e yradius 150 radius linha azul

		disobs = (double)sqrt((double)((ox - cx) * (ox - cx)) + ((oy - cy) * (oy - cy)));
		//disobs = (double) (((oy-cx)*(oy-cx) )/(radius*radius)) + (((ox-cy)*(ox-cy))/(0.19*radius*radius)); //ellipesi

		pospart1 = 5 * ((double)disobs / radius);
		// pospart1 = 5 * disobs; //elipse

		ciratu = (int)pospart1;

		sprintf_s(string, sizeof(string), "%d", ciratu);
		SetDlgItemText(hDlg, IDC_PP, string);

		if (ox > cx && oy > cy) Quadrante = 1;
		if (ox >= cx && oy <= cy) Quadrante = 2;
		if (ox <= cx && oy <= cy) Quadrante = 3;
		if (ox<cx && oy>cy) Quadrante = 4;

		sprintf_s(string, sizeof(string), "%d", Quadrante);
		SetDlgItemText(hDlg, IDC_QUA, string);


		//parou	

		time(&end_time);
		sec = end_time - start_time;
		hour = sec / 3600; sec -= hour * 3600;
		min = sec / 60; sec -= min * 60;
		sprintf_s(string, sizeof(string), "%02i:%02i:%02i", hour, min, sec);

		SetDlgItemText(hDlgParam, IDC_TIME, string);

		if (mass >= maxsize)
			break;

		if (!display && (mass % step))
			continue;

		SetDIBits(hdcMemory, hLattice, 0, bm.bmHeight, buffer, &bmi, DIB_RGB_COLORS);
		SelectObject(hdcMemory, hLattice);
		xsize = (radius - deltr) * 2;
		//		ysize=xsize;
		ysize = (radius - deltr) * 2;//definimos aqui o raio de laçamento utlizado nas funções
		if (xsize > XWINDOW_SIZE)
		{
			x0 = cx - xsize / 2;
			y0 = cy - xsize / 2;
			StretchBlt(hdc, xstart, ystart, XWINDOW_SIZE, YWINDOW_SIZE, hdcMemory,
				x0, y0, xsize, ysize, SRCCOPY);
		}
		else
		{
			x0 = cx - XWINDOW_SIZE / 2;
			y0 = cy - YWINDOW_SIZE / 2;
			BitBlt(hdc, xstart, ystart, XWINDOW_SIZE, YWINDOW_SIZE, hdcMemory, x0, y0, SRCCOPY);
			if (show)
			{
				x0 = XWINDOW_SIZE / 2;
				y0 = YWINDOW_SIZE / 2;
				if (circle) {
					Arc(hdc, x0 - radius, y0 - radius, x0 + radius, y0 + radius, 0, 0, 0, 0);
				}
				else if (elipse)
				{
					Arc(hdc, x0 - xradius, y0 - yradius, x0 + xradius, y0 + yradius, 0, 0, 0, 0);
				}
			}
		}

	}

	SelectObject(hdc, hpenOld);
	DeleteObject(hpenBlue);
	bAbort = 0;

	fclose(h);

}


/************************************************************************/

void vonmise(double* random) {

	int n, i, j, sign;
	double k, a, b, r;
	double u[3], z, f, c, c1, x, y, angle; //acres pas


	//	n=100;
	//	mi=PI/2; calculado no ostep

	k = 3; // ((radius - disobs)/radius) * 3; //10.0; //((radius - disobs)/radius) * 10; // (2/disobs); //0=normal

	//	random = malloc(n*sizeof( double ) );

	a = 1 + sqrt((1 + 4 * k * k));
	b = (a - sqrt(2 * a)) / (2 * k);
	r = (1 + b * b) / (2 * b);

	*random = 0;
	while (1) {
		for (i = 0;i < 3;i++) {
			u[i] = (double)UNI / RMAX;
		}
		z = cos(PI * u[0]);
		f = (1 + r * z) / (r + z);
		c = k * (r - f);
		if (u[1] < (c * (2 - c)) || !(log(c) - log(u[1]) + 1 - c < 0))break;
	}
	if ((u[2] - 0.5) > 0)sign = 1;
	else if ((u[2] - 0.5) < 0)sign = -1;
	else sign = 0;
	*random = mi + sign * acos(f);
	//y=sin(*random);
	//x=cos(*random);
	//*random=atan2(y,x);
	return random;
}

/************************************************************************/

void NewPoint(int* xpos, int* ypos)
{
	double fi, t;
	double x, y;
	int ix, jy, aleat;

	//vonmise(&fi);
	fi = (double)UNI * TWOPI / RMAX;

	//ponto inicial mudar??

	if (LATT == 0) //latt TRL

	{
		if (circle)
		{

			ox = x = cx + radius * sin(fi);
			oy = y = cy + radius * cos(fi);

			//x=cxt+radius*sin(fi);
			//y=cyt+radius*cos(fi);
			//y=y/tscale;
			//x=x+y/2;
		}
		else if (elipse)
		{
			t = tan(fi);
			x = 1 / sqrt(1.0 / (xradius * xradius) + t * t / (yradius * yradius));
			y = x * t;
			y = y / tscale;
			x = x + y / 2;
			aleat = UNI;
			if (aleat > RMAX / 2)
			{
				x = cxt - x;
				y = cyt - y;
			}
			else
			{
				x += cxt;
				y += cyt;
			}
		}
	}

	else if ((LATT == 3) || (LATT == 2) || (LATT == 1))

	{
		if (circle)
		{
			ox = x = cx + radius * sin(fi);
			oy = y = cy + radius * cos(fi);
		}
		else if (elipse)
		{
			t = tan(fi);
			y = 1 / sqrt(1.0 / (xradius * xradius) + t * t / (yradius * yradius));
			x = y * t;
			aleat = UNI;

			if (aleat > RMAX / 2)
			{
				ox = x = cx - x;
				oy = y = cy - y;
			}
			else
			{
				ox = x += cx;
				oy = y += cy;
			}
		}

	}


	//modificar 

	ix = x + (((x - (int)x) > 0.5) ? 1 : 0);
	jy = y + (((y - (int)y) > 0.5) ? 1 : 0);
	*xpos = ix;
	*ypos = jy;

	//ox=ix;oy=jy;

}

/**********************************************************************/

int CheckRadius(int i, int j)
{
	double dx, dy, yp, xp, c, x, y, t, rnovo;
	int m1, r;

	m1 = mass - 1;
	rnovo = 0.0;

	if (LATT == 0)  //latt TRL
	{
		xp = i;
		yp = j;
		dx = xp - cx;
		dy = yp - cy;


		//xp=i-j/2;
		//		yp=j*tscale;
		//		dx=xp-cxt;
		//		dy=yp-cyt;
	}
	else if ((LATT == 1) || (LATT == 2) || (LATT == 3))
	{
		xp = i;
		yp = j;
		dx = xp - cx;
		dy = yp - cy;
	}

	//ate aqui

	if (circle)
	{

		//  normal com uma face
		r = deltr + sqrt(dx * dx + dy * dy);
		//rnovo=r; //CIRCULO
		//rnovo=r*1.414213;//QUADRADO
		rnovo = r / 0.7141;//Elipse exe 0.9=0.4358 // 0.7=0.7141 // 0.5 = 0.866 mudar no ostep tb

		if (rnovo < rmin)
			rnovo = rmin;
		if (rnovo > radius)
		{
			radius = rnovo; radius2 = rnovo * rnovo;
		}
	}

	/*
			if (Quadrante==1 || Quadrante==2) {
	//para 2 formas

			r=deltr+sqrt(dx*dx+dy*dy); //CIRCULO
			rnovo=r;
			//rnovo=r*1.414213;//QUADRADO
			//rnovo=r/0.4358;//Elipse ex 0.9=0.4358 // 0.7=0.7141 // 0.5 = 0.866 mudar no ostep tb

			if(rnovo<rmin)
				rnovo=rmin;
			if(rnovo>radius)
				{radius=rnovo; radius2=rnovo*rnovo;}

			}

			else {
				r=deltr+sqrt(dx*dx+dy*dy); //CIRCULO
			//rnovo=r;
			rnovo=r*1.414213;//QUADRADO
			//rnovo=r/0.4358;//Elipse ex 0.9=0.4358 // 0.7=0.7141 // 0.5 = 0.866 mudar no ostep tb

			if(rnovo<rmin)
				rnovo=rmin;
			if(rnovo>radius)
				{radius=rnovo; radius2=rnovo*rnovo;}
			}

		}
		*/
	else
	{
		if (dy == 0)
			return;
		r = deltr + sqrt(dx * dx + dy * dy);
		t = (double)dx / (double)dy;
		y = 1 / sqrt(1.0 / (xradius * xradius) + t * t / (yradius * yradius));
		x = y * t;
		radius = sqrt(x * x + y * y);
		if (r > radius)
		{
			c = (double)r / (double)radius;
			xradius *= c;
			yradius *= c;
		}

	}
}



/***********************************************************************/

int OutofRange(int i, int j)
{
	int dx1, dx2, dy1, dy2, dfocus1, dfocus2;
	int dx, dy;
	int r2;
	double yp, xp;

	if (LATT == 0) //latt TRL
	{

		xp = i;
		yp = j;
		dx = j - cy;
		dy = i - cx;
		//xp=i-j/2;
		//yp=j*tscale;
		//dx=xp-cxt;
		//dy=yp-cyt;
	}

	else if ((LATT == 3) || (LATT == 2) || (LATT == 1))
	{
		xp = i;
		yp = j;
		dx = j - cy;
		dy = i - cx;
	}

	if (elipse) {
		r2 = ((dx * dx) / (double)(xradius * xradius)) + ((dy * dy) / (double)(yradius * yradius));
		if (r2 >= 1)
			return(1);
	}

	if (circle) {
		r2 = dx * dx + dy * dy;
		if (r2 > radius2)
			return(1);
	}


	return(0);
}

/*************************************************************************/
/*
BOOL DibSaveImage (PTSTR pstrFileName, BITMAPFILEHEADER * pbmfh, int run)
{
	 BOOL   bSuccess ;
	 DWORD  dwBytesWritten ;
	 HANDLE hFile ;

	 if(run==0){
		if (!DibFileSaveDlg (ghwnd, szFileName, szTitleName))
		return 0;
	 }
	 hFile = CreateFile (pstrFileName, GENERIC_WRITE, 0, NULL,
						 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

	 if (hFile == INVALID_HANDLE_VALUE)
		  return FALSE ;

	 bSuccess = WriteFile (hFile, pbmfh, pbmfh->bfSize, &dwBytesWritten, NULL) ;
	 CloseHandle (hFile) ;

	 if (!bSuccess || (dwBytesWritten != pbmfh->bfSize))
	 {
		  DeleteFile (pstrFileName) ;
		  return FALSE ;
	 }
	 return TRUE ;
}
*/
/****************************************************************************/

BOOL SaveBitmapFile(HDC hDC, HBITMAP hBmp, PSTR pszFileName)
{
	int         hFile;
	OFSTRUCT    ofReOpenBuff;
	HBITMAP     hTmpBmp, hBmpOld;
	BOOL        bSuccess;
	BITMAPFILEHEADER    bfh;
	PBITMAPINFO pbmi;
	PBYTE       pBits;
	BITMAPINFO  bmi;
	PBYTE pjTmp, pjTmpBmi;
	ULONG sizBMI;


	bSuccess = TRUE;

	if (!hBmp) {
		MessageBox(0, "There's no Bitmap to save!", "Error", MB_OK);
		return FALSE;
	}

	SelectObject(hDC, hBmp);

	bmi.bmiHeader = bmih;
	bmi.bmiHeader.biSize = 0x28;              // GDI need this to work
	// bmi.bmiHeader.biBitCount = 0;             // don't get the color table
	if ((GetDIBits(hDC, hBmp, 0, 0, (LPSTR)NULL, &bmi, DIB_RGB_COLORS)) == 0) {
		MessageBox(0, "GetDIBits failed!", "Error", MB_OK);
		return FALSE;
	}

	if ((pBits = (PBYTE)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		bmi.bmiHeader.biSizeImage)) == NULL) {
		MessageBox(0, "Failed in Memory Allocation for pBits!", "Error", MB_OK);
		return FALSE;
	}

	pbmi = &bmi;                                      // assume no color table

	if (bmi.bmiHeader.biBitCount != 24) {             // has color table
		sizBMI = sizeof(BITMAPINFO) + sizeof(RGBQUAD) * (1 << bmi.bmiHeader.biBitCount);
		if ((pbmi = (PBITMAPINFO)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, sizBMI)) == NULL) {
			MessageBox(0, "Failed in Memory Allocation for pbmi!", "Error", MB_OK);
			bSuccess = FALSE;

		}

		pbmi->bmiHeader.biSize = bmi.bmiHeader.biSize;
		pbmi->bmiHeader.biWidth = bmi.bmiHeader.biWidth;
		pbmi->bmiHeader.biHeight = bmi.bmiHeader.biHeight;
		pbmi->bmiHeader.biPlanes = bmi.bmiHeader.biPlanes;
		pbmi->bmiHeader.biBitCount = bmi.bmiHeader.biBitCount;
		pbmi->bmiHeader.biCompression = bmi.bmiHeader.biCompression;
		pbmi->bmiHeader.biSizeImage = bmi.bmiHeader.biSizeImage;
		pbmi->bmiHeader.biXPelsPerMeter = bmi.bmiHeader.biXPelsPerMeter;
		pbmi->bmiHeader.biYPelsPerMeter = bmi.bmiHeader.biYPelsPerMeter;
		pbmi->bmiHeader.biClrUsed = bmi.bmiHeader.biClrUsed;
		pbmi->bmiHeader.biClrImportant = bmi.bmiHeader.biClrImportant;

		pjTmp = (PBYTE)pbmi;
		pjTmpBmi = (PBYTE)&bmi;
		sizBMI = sizeof(BITMAPINFOHEADER);

		while (sizBMI--)
		{
			*(((PBYTE)pjTmp)++) = *((pjTmpBmi)++);
		}

	}

	if ((hFile = OpenFile(pszFileName, (LPOFSTRUCT)&ofReOpenBuff,
		OF_CREATE | OF_WRITE)) == -1) {
		MessageBox(0, "Failed in OpenFile!", "Error", MB_OK);
	}

	bfh.bfType = 0x4d42;                            // 'BM'

	bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) *
		((pbmi->bmiHeader.biBitCount == 24) ? 0 : (1 << pbmi->bmiHeader.biBitCount)) +
		pbmi->bmiHeader.biSizeImage;
	bfh.bfReserved1 =
		bfh.bfReserved2 = 0;
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
		sizeof(RGBQUAD) * ((pbmi->bmiHeader.biBitCount == 24) ? 0 : (1 << pbmi->bmiHeader.biBitCount));

	if (_lwrite(hFile, (LPSTR)&bfh, sizeof(BITMAPFILEHEADER)) == -1) {
		MessageBox(0, "Failed in WriteFile!", "Error", MB_OK);
		bSuccess = FALSE;

	}


	if (hTmpBmp = CreateCompatibleBitmap(hDC, pbmi->bmiHeader.biWidth, pbmi->bmiHeader.biHeight)) {
		hBmpOld = SelectObject(hDC, hTmpBmp);
		if ((GetDIBits(hDC, hBmp, 0, pbmi->bmiHeader.biHeight, (LPSTR)pBits, pbmi, DIB_RGB_COLORS)) == 0) {
			MessageBox(0, "Failed in GetDIBits!", "Error", MB_OK);
			bSuccess = FALSE;

		}
	}
	else {
		MessageBox(0, "Failed in creating bitmap!", "Error", MB_OK);
		bSuccess = FALSE;

	}

	if (_lwrite(hFile, (LPSTR)pbmi, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) *
		((pbmi->bmiHeader.biBitCount == 24) ? 0 : (1 << pbmi->bmiHeader.biBitCount))) == -1) {
		MessageBox(0, "Failed in WriteFile!", "Error", MB_OK);
		bSuccess = FALSE;

	}

	if (_lwrite(hFile, (LPSTR)pBits, pbmi->bmiHeader.biSizeImage) == -1) {
		MessageBox(0, "Failed in WriteFile!", "Error", MB_OK);
		bSuccess = FALSE;

	}



}

/******************************************************************/

//verificar a função fluxo ver vom

int ostep(int* i, int* j)
{
	double fi;
	double x, y, fx, fy, fxx, fyy;
	int ix, jy;

	x = ox;
	y = oy;

	fx = oy - 2048;
	fy = ox - 2048;




	if (Quadrante == 1) {


		mi = (3.2 * PI) / 4;
		vonmise(&fi);
		oy += step_size * cos(fi);
		ox += step_size * sin(fi);
	}



	else if (Quadrante == 2) {


		mi = (5.2 * PI) / 4;
		vonmise(&fi);
		oy += step_size * cos(fi);
		ox += step_size * sin(fi);
	}



	else if (Quadrante == 3) {



		mi = 7.2 * PI / 4;
		vonmise(&fi);
		oy += step_size * cos(fi);
		ox += step_size * sin(fi);
	}



	else {  //quadrante 4



		mi = 1.2 * PI / 4;
		vonmise(&fi);
		oy += step_size * cos(fi);
		ox += step_size * sin(fi);
	}





	if (closed)
		reflect(x, y);

	*i = ox + (((ox - (int)ox) > 0.5) ? 1 : 0);
	*j = oy + (((oy - (int)oy) > 0.5) ? 1 : 0);


	return 1;

}

/*****************************************************************************/

int reflect(double x0, double y0)
{
	double x1, y1, dx, dy, r2;
	double a, b, r, d, xr, yr, a2, b2;
	double xr1, xr2, dx1, dx2, alfa;

	dx = ox - cx;
	dy = oy - cy;
	r2 = dx * dx + dy * dy;
	if (r2 < radius2)
		return(0);
	r = radius;
	x0 -= cx; y0 -= cy;
	x1 = ox - cx; y1 = oy - cy;		// endpoint

	a = (y1 - y0) / (x1 - x0);	// slope of the line connecting the two points
	b = y0 - a * x0;			// intercept

	a2 = a * a; b2 = b * b;
	d = 4 * a2 * b2 - 4 * (1 + a2) * (b2 - r * r);
	if (d < 0)
		return (-1);
	d = sqrt(d);

	xr1 = (-2 * a * b + d) / (2 * (1 + a * a));
	xr2 = (-2 * a * b - d) / (2 * (1 + a * a));
	dx1 = fabs(xr1 - x0);
	dx2 = fabs(xr2 - x0);

	xr = dx1 < dx2 ? xr1 : xr2;
	yr = a * xr + b;

	alfa = 2 * ((x1 - xr) * xr + (y1 - yr) * yr) / (r * r);

	ox = x1 - alfa * xr;
	oy = y1 - alfa * yr;

	ox += cx; oy += cy;

	return 1;
}

/*****************************************************************************/