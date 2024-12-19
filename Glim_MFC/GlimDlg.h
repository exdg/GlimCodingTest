
// GlimDlg.h: 헤더 파일
//

#pragma once

// CGlimDlg 대화 상자
class CGlimDlg : public CDialogEx
{
// 생성입니다.
public:
	CGlimDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CGlimDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	unsigned char* m_pImageData;
	
	CBitmap m_cBitMap;
	CStatic m_picture_Image;

	int m_nStartX;
	int m_nStartY;
	int m_nEndX;
	int m_nEndY;

	int m_nEllipseStartX;
	int m_nEllipseStartY;
	int m_nEllipseEndX;
	int m_nEllipseEndY;

	void OnDraw(CDC* pDC);

	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonDraw();
	afx_msg void OnBnClickedButtonAction();


	BOOL m_bThreadStatus;
	HANDLE m_hThread;
	
	static UINT ThreadFunc(LPVOID pParam);
	void Thread();	

	void Move(CDC* cDCPicture, int nMovePixcel, BOOL bRight, BOOL bX);
	void Save(CDC* cDCPicture, int nSaveCount);
};
