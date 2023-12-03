﻿#include <qobject.h>
#include <qstring.h>
#include <pluginGl.h>
#include <functional>
#include <qsciscintilla.h>
#include "qttestclass.h"
#include "instanceobj.h"
#ifdef WIN32
#include <Windows.h>
#endif

#define NDD_EXPORTDLL

#if defined(Q_OS_WIN)
	#if defined(NDD_EXPORTDLL)
		#define NDD_EXPORT __declspec(dllexport)
	#else
		#define NDD_EXPORT __declspec(dllimport)
	#endif
#else
	#define NDD_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
	extern "C" {
#endif

	NDD_EXPORT bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData);
	NDD_EXPORT int NDD_PROC_MAIN(QWidget* pNotepad, const QString& strFileName, std::function<QsciScintilla* (QWidget*)>getCurEdit, std::function<bool(QWidget*, int, void*)> pluginCallBack, NDD_PROC_DATA* procData);

#ifdef __cplusplus
	}
#endif

static NDD_PROC_DATA s_procData;
static QWidget* s_pMainNotepad = nullptr;

std::function<QsciScintilla* (QWidget*)> s_getCurEdit;
std::function<bool(QWidget*, int, void*)> s_invokeMainFun;

bool NDD_PROC_IDENTIFY(NDD_PROC_DATA* pProcData)
{
	if(pProcData == NULL)
	{
		return false;
	}
	pProcData->m_strPlugName = QObject::tr("Hello World Plug");
	pProcData->m_strComment = QObject::tr(u8"不需要创建二级菜单的插件例子");

	pProcData->m_version = QString("v1.0");
	pProcData->m_auther = QString("zuowei.yin");

	pProcData->m_menuType = 0;

	return true;
}

//则点击菜单栏按钮时，会自动调用到该插件的入口点函数。
//pNotepad:就是CCNotepad的主界面指针
//strFileName:当前插件DLL的全路径，如果不关心，则可以不使用
//getCurEdit:从NDD主程序传递过来的仿函数，通过该函数获取当前编辑框操作对象QsciScintilla
//s_invokeMainFun: 可以回调NDD主程序中的功能函数，比如创建新文件功能等，根据需要可实时扩展。
//pProcData:如果pProcData->m_menuType = 0 ,则该指针为空；如果pProcData->m_menuType = 1，则该指针有值。目前需要关心s_procData.m_rootMenu
//开发者可以在该菜单下面，自行创建二级菜单
int NDD_PROC_MAIN(QWidget* pNotepad, const QString &strFileName, std::function<QsciScintilla*(QWidget*)>getCurEdit, std::function<bool(QWidget*, int, void*)> pluginCallBack, NDD_PROC_DATA* pProcData)
{
	InstanceObj* pInstanse = nullptr;

	if (pProcData != nullptr)
	{
		pInstanse = new InstanceObj(pNotepad);
		pInstanse->setObjectName("nddplg");
	}
	else
	{
		return -1;
	}

	s_getCurEdit = getCurEdit;
	s_invokeMainFun = pluginCallBack;

	//务必拷贝一份pProcData，在外面会释放。
	if (pProcData != nullptr)
	{
		s_procData = *pProcData;
	}

	s_pMainNotepad = pNotepad;

	QObject::connect(pProcData->m_pAction, &QAction::triggered, pInstanse, &InstanceObj::doMainWork, Qt::UniqueConnection);

	return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		if (NULL == lpvReserved) {
			//做全局回收工作
		}
		break;
	}
	return TRUE;
}
#else
void onDllUnload(void)
{
	//做全局回收工作
}
#endif
