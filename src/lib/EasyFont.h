// EasyFont.h: interface for the EasyFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EASYFONT_H__EC443BC3_2692_40E3_AF63_21022BC24BE4__INCLUDED_)
#define AFX_EASYFONT_H__EC443BC3_2692_40E3_AF63_21022BC24BE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EasyFont  
{
public:
	int fontID;
	float red,green,blue;
	float size;
	float xkern;
	float topkern;
	float bottomkern;

	void LoadFont(int id);
	void SetColor(float r,float g,float b);
	void SetSize(float size);
	void SetKern(float xk,float tk,float bk);
	void PlotText(char text[]);
	void CenterText(char text[]);

	void SetMaterial();

	EasyFont();
	virtual ~EasyFont();

};

#endif // !defined(AFX_EASYFONT_H__EC443BC3_2692_40E3_AF63_21022BC24BE4__INCLUDED_)
