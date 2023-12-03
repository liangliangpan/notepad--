#pragma once
#include <QObject>
#include <QWidget>
#include <QPointer>
#include "qttestclass.h"

class QMenu;

class InstanceObj :public QObject
{
public:
	//����Ndd�ͷ�ʱ�����Զ��ͷŸö���
	InstanceObj(QWidget* pNotepad);
	~InstanceObj();

public slots:
	void doMainWork();

public:
	QWidget* m_pNotepad;
	QPointer<QtTestClass> m_pMainToolWin;
private:
	InstanceObj(const InstanceObj& other) = delete;
	InstanceObj& operator=(const InstanceObj& other) = delete;
};