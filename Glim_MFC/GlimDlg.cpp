
// GlimDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Glim.h"
#include "GlimDlg.h"
#include "afxdialogex.h"

#define IMAGE_SIZE_W 512
#define IMAGE_SIZE_H 512

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGlimDlg 대화 상자



CGlimDlg::CGlimDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_DIALOG, pParent)
	, m_nStartX(0)
	, m_nStartY(0)
	, m_nEndX(0)
	, m_nEndY(0)
	, m_nEllipseStartX(0)
	, m_nEllipseStartY(0)
	, m_nEllipseEndX(0)
	, m_nEllipseEndY(0)
	, m_bThreadStatus(FALSE)
	, m_hThread(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pImageData = NULL;
}

CGlimDlg::~CGlimDlg()
{
	DeleteObject(m_cBitMap);

	if (m_pImageData != NULL)
	{
		delete[] m_pImageData;
		m_pImageData = NULL;
	}
}

void CGlimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_picture_Image);
	DDX_Text(pDX, IDC_EDIT_START_X, m_nStartX);
	DDX_Text(pDX, IDC_EDIT_START_Y, m_nStartY);
	DDX_Text(pDX, IDC_EDIT_END_X, m_nEndX);
	DDX_Text(pDX, IDC_EDIT_END_Y, m_nEndY);
}

BEGIN_MESSAGE_MAP(CGlimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CGlimDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CGlimDlg::OnBnClickedButtonDraw)
	ON_BN_CLICKED(IDC_BUTTON_ACTION, &CGlimDlg::OnBnClickedButtonAction)
END_MESSAGE_MAP()


// CGlimDlg 메시지 처리기

BOOL CGlimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_pImageData = new unsigned char[IMAGE_SIZE_W * IMAGE_SIZE_H];

	m_cBitMap.CreateBitmap(IMAGE_SIZE_W, IMAGE_SIZE_H, 1, 32, NULL);

	m_hThread = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CGlimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CGlimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		OnDraw(&dc);

		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CGlimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGlimDlg::OnDraw(CDC* pDC)
{

	CDC* cDCPicture = m_picture_Image.GetDC();
	CDC cDC;
	cDC.CreateCompatibleDC(cDCPicture);
	cDC.SelectObject(&m_cBitMap);

	CRect rect;
	m_picture_Image.GetClientRect(&rect);

	cDCPicture->StretchBlt(0, 0, rect.Width(), rect.Height(), &cDC, 0, 0, IMAGE_SIZE_W, IMAGE_SIZE_H, SRCCOPY);
}

void CGlimDlg::OnBnClickedButtonOpen()
{
	CString fileName;
	CString strFilter;
	strFilter = _T("(*.raw)|*.raw||");

	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.raw"),												//LPCTSTR lpszDefExt = NULL,
		NULL,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);

	if (dlgFile.DoModal() != IDOK) return;

	SetCurrentDirectoryA(path);
	CString sFilePathName = dlgFile.GetPathName();

	CFile pFile;
	// 이미지 파일 오픈
	if (pFile.Open(sFilePathName, CFile::modeRead, NULL) == FALSE)
	{
		AfxMessageBox(_T("Image Open Fail"));
		return;
	}
	// 이미지 파일 읽기
	int nFileLength = pFile.GetLength();
	pFile.Read(m_pImageData, nFileLength);


	unsigned char* pImage32 = new unsigned char[nFileLength * 4];

	// 8비트 이미지 32비트 이미지로 변환
	for (int i = 0; i < IMAGE_SIZE_W * IMAGE_SIZE_H; i++)
	{
		int j = i * 4;
		pImage32[j++] = m_pImageData[i];
		pImage32[j++] = m_pImageData[i];
		pImage32[j++] = m_pImageData[i];
		pImage32[j++] = 0xFF;
	}
	pFile.Close();

	// 읽은 이미지 세팅
	SetBitmapBits(m_cBitMap, nFileLength * 4, pImage32);
	
	CRect rect;
	m_picture_Image.GetClientRect(&rect);

	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = rect.right; bmih.biHeight = rect.bottom;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	LPBYTE lpBits = new BYTE[IMAGE_SIZE_W * IMAGE_SIZE_H];
	memcpy(lpBits, pImage32, IMAGE_SIZE_W * IMAGE_SIZE_H);
	
	SetDIBits(m_picture_Image.GetDC()->m_hDC, m_cBitMap, 0, rect.bottom, lpBits, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	
	if (lpBits != NULL)
	{
		delete[] lpBits;
		lpBits = NULL;
	}

	if (pImage32 != NULL)
	{
		delete[] pImage32;
		pImage32 = NULL;
	}

	// 화면 갱신
	Invalidate();

	if (m_nStartX != 0 && m_nEndX != 0)
	{
		RedrawWindow();
		AfxMessageBox(_T("그려진 원을 삭제합니다.\n다시 그려주세요."));
		m_nStartX = 0;
		m_nEndX = 0;
		return;
	}

}


void CGlimDlg::OnBnClickedButtonDraw()
{
	UpdateData(TRUE);

	int nRand = rand() % 100;
	nRand = (nRand < 5) ? 5 : nRand;

	m_nEllipseStartX = m_nStartX - nRand;
	m_nEllipseStartY = m_nStartY - nRand;
	m_nEllipseEndX = m_nStartX + nRand;
	m_nEllipseEndY = m_nStartY + nRand;

	CRect rect;
	m_picture_Image.GetClientRect(&rect);

	if (m_nEllipseStartX < rect.left || m_nEllipseStartY < rect.top
		|| m_nEllipseEndX > rect.right || m_nEllipseEndY > rect.bottom)
	{
		AfxMessageBox(_T("이미지 상자 범위 밖입니다.\nStart X,Y를 다시 입력해주세요"));
		return;
	}

	RedrawWindow();

	CDC* cDCPicture = m_picture_Image.GetDC();
	cDCPicture->Ellipse(m_nEllipseStartX, m_nEllipseStartY, m_nEllipseEndX, m_nEllipseEndY);

}


void CGlimDlg::OnBnClickedButtonAction()
{
	if (m_bThreadStatus == TRUE)
	{
		AfxMessageBox(_T("이미 진행중입니다."));
		return;
	}
	UpdateData(TRUE);

	CRect rect;
	m_picture_Image.GetClientRect(&rect);

	int nEndArea = (m_nEllipseEndX - m_nEllipseStartX) / 2;
	if (m_nEndX - nEndArea < rect.left || m_nEndY - nEndArea < rect.top
		|| m_nEndX + nEndArea > rect.right || m_nEndY + nEndArea > rect.bottom)
	{
		AfxMessageBox(_T("이미지 상자 범위 밖입니다.\nEnd X,Y를 다시 입력해주세요"));
		return;
	}

	m_bThreadStatus = TRUE;
	m_hThread = AfxBeginThread(ThreadFunc, this);
}


UINT CGlimDlg::ThreadFunc(LPVOID pParam)
{
	CGlimDlg* pGlim = (CGlimDlg*)pParam;

	pGlim->Thread();

	return 0;
}

void CGlimDlg::Thread()
{
	// TRUE이면 End 좌표가 더 큼
	bool bX = (m_nStartX <= m_nEndX) ? TRUE : FALSE;
	bool bY = (m_nStartY <= m_nEndY) ? TRUE : FALSE;
	
	
	int nMoveNumX = abs(m_nEndX - ((m_nEllipseStartX + m_nEllipseEndX) / 2));
	int nMoveNumY = abs(m_nEndY - ((m_nEllipseStartY + m_nEllipseEndY) / 2));

	int nPixcel = 10;

	CDC* cDCPicture = m_picture_Image.GetDC();

	int nSaveCount = 0;

	while (true)
	{
		// 이동
		if (nMoveNumX > 0)
		{
			if (nMoveNumX < nPixcel)
			{
				Move(cDCPicture, nMoveNumX, bX, TRUE);
				nMoveNumX = nMoveNumX - nMoveNumX;
			}
			else
			{
				Move(cDCPicture, nPixcel, bX, TRUE);
				nMoveNumX = nMoveNumX - nPixcel;
			}
		}
		else if (nMoveNumY > 0)
		{
			if (nMoveNumY < nPixcel)
			{
				Move(cDCPicture, nMoveNumY, bY, FALSE);
				nMoveNumY = nMoveNumY - nMoveNumY;
			}
			else
			{
				Move(cDCPicture, nPixcel, bY, FALSE);
				nMoveNumY = nMoveNumY - nPixcel;
			}
		}
		
		// 원 좌표 표현
		int nCenterX = (m_nEllipseStartX + m_nEllipseEndX) / 2;
		int nCenterY = (m_nEllipseStartY + m_nEllipseEndY) / 2;

		cDCPicture->MoveTo(nCenterX - 5, nCenterY - 5);
		cDCPicture->LineTo(nCenterX + 5, nCenterY + 5);
		cDCPicture->MoveTo(nCenterX + 5, nCenterY - 5);
		cDCPicture->LineTo(nCenterX - 5, nCenterY + 5);

		CString str;
		str.Format(_T("%d, %d"), ((m_nEllipseStartX + m_nEllipseEndX) / 2), ((m_nEllipseStartY + m_nEllipseEndY) / 2));

		CRect rect;
		rect.left	= nCenterX + 7;
		rect.top	= nCenterY + 7;
		rect.right	= nCenterX + 500;
		rect.bottom = nCenterY + 500;

		cDCPicture->DrawTextW(str, rect, DT_SINGLELINE);

		// 저장
		Save(cDCPicture, nSaveCount);
		nSaveCount++;

		// 종료
		if (nMoveNumX == 0 && nMoveNumY == 0)
			break;
	}
	m_bThreadStatus = FALSE;
}

void CGlimDlg::Move(CDC* cDCPicture, int nMovePixcel, BOOL bRight, BOOL bX)
{
	RedrawWindow();
	
	if (bX == TRUE)
	{
		// X축 이동
		if (bRight == TRUE)
		{
			m_nEllipseStartX = m_nEllipseStartX + nMovePixcel;
			m_nEllipseEndX = m_nEllipseEndX + nMovePixcel;
		}
		else
		{
			m_nEllipseStartX = m_nEllipseStartX - nMovePixcel;
			m_nEllipseEndX = m_nEllipseEndX - nMovePixcel;
		}
	}
	else
	{
		// Y축 이동
		if (bRight == TRUE)
		{
			m_nEllipseStartY = m_nEllipseStartY + nMovePixcel;
			m_nEllipseEndY = m_nEllipseEndY + nMovePixcel;
		}
		else
		{
			m_nEllipseStartY = m_nEllipseStartY - nMovePixcel;
			m_nEllipseEndY = m_nEllipseEndY - nMovePixcel;
		}
	}
	cDCPicture->Ellipse(m_nEllipseStartX, m_nEllipseStartY, m_nEllipseEndX, m_nEllipseEndY);
}

void CGlimDlg::Save(CDC* cDCPicture, int nSaveCount)
{
	TCHAR path[256] = { 0, };
	GetModuleFileName(NULL, path, 256);

	CString folderPath(path);
	folderPath = folderPath.Left(folderPath.ReverseFind('\\'));

	folderPath = folderPath + (L"\\Image");

	CreateDirectory(folderPath, NULL);

	CString strSavePath;
	strSavePath.Format(_T("%d"), nSaveCount);
	strSavePath = folderPath + L"\\" + strSavePath + L".bmp";

	CRect rect;
	m_picture_Image.GetClientRect(&rect);

	HDC hDC = CreateCompatibleDC(cDCPicture->m_hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(cDCPicture->m_hDC, rect.right, rect.bottom);
	HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, hBitmap);
	BitBlt(hDC, 0, 0, rect.right, rect.bottom, cDCPicture->m_hDC, 0, 0, SRCCOPY);
	SelectObject(hDC, hBmpOld);
	DeleteDC(hDC);

	//비트맵사양설정 
	BITMAPINFOHEADER bmih;
	ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
	bmih.biSize = sizeof(BITMAPINFOHEADER);
	bmih.biWidth = rect.right; 
	bmih.biHeight = rect.bottom;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = BI_RGB;

	// 비트맵(DIB) 데이터 추출 
	GetDIBits(cDCPicture->m_hDC, hBitmap, 0, rect.bottom, NULL, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
	LPBYTE lpBits = new BYTE[bmih.biSizeImage];
	GetDIBits(cDCPicture->m_hDC, hBitmap, 0, rect.bottom, lpBits, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS); ReleaseDC(cDCPicture);
	DeleteObject(hBitmap);

	// 비트맵 파일 헤더 설정
	BITMAPFILEHEADER bmfh;
	bmfh.bfType = 'MB';
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmih.biSizeImage;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 비트맵 파일 생성 및 데이터 저장 
	DWORD dwWritten;
	HANDLE hFile = CreateFile(strSavePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(hFile, &bmfh, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	WriteFile(hFile, &bmih, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
	WriteFile(hFile, lpBits, bmih.biSizeImage, &dwWritten, NULL);
	CloseHandle(hFile);
	delete[] lpBits;

}
