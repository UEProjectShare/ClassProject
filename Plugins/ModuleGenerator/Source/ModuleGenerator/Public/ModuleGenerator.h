// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FModuleGeneratorModule : public IModuleInterface
{
public:

	/**
	 * ģ������
	 */
	virtual void StartupModule() override;
	
	/**
	 * ģ��ж��
	 */
	virtual void ShutdownModule() override;
	
	/**
	 * ICommandListʵ��
	 */
	TSharedPtr<class FUICommandList> PluginCommands;
	
	/**
	 * �����Ƿ��Ѿ�����
	 */
	bool WindowCreated = false;
	
	/**
	 * ��ť����¼�
	 */
	void OnOpenTool();
	
	/**
	 * �ڱ༭����ע��UI��ť��ʾ
	 */
	void RegisterMenus();
	
	/**
	 * �رմ���
	 */
	void ToolWindowClosed(const TSharedRef<SWindow>&);
};
