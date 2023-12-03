#include "instanceobj.h"
#include "qttestclass.h"

#include <qsciscintilla.h>
#include <QWidget>
#include <QMenu>
#include <QMessageBox>


InstanceObj::InstanceObj(QWidget* pNotepad) :QObject(pNotepad)
{
	m_pNotepad = pNotepad;
}

InstanceObj::~InstanceObj()
{

}


void InstanceObj::doMainWork()
{
	//��һ���򵥵�ת��д�Ĳ���
	if (m_pMainToolWin.isNull())
	{
		m_pMainToolWin = new QtTestClass(m_pNotepad);
		m_pMainToolWin->setWindowFlag(Qt::Window);
		m_pMainToolWin->setAttribute(Qt::WA_DeleteOnClose);
	}
	m_pMainToolWin->show();
}