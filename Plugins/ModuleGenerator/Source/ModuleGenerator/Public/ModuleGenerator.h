// Copyright 2021 Sam Carey. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FModuleGeneratorModule : public IModuleInterface
{
public:

	/**
	 * 模块启动
	 */
	virtual void StartupModule() override;
	
	/**
	 * 模块卸载
	 */
	virtual void ShutdownModule() override;
	
	/**
	 * ICommandList实例
	 */
	TSharedPtr<class FUICommandList> PluginCommands;
	
	/**
	 * 窗口是否已经创建
	 */
	bool WindowCreated = false;
	
	/**
	 * 按钮点击事件
	 */
	void OnOpenTool();
	
	/**
	 * 在编辑器中注册UI按钮显示
	 */
	void RegisterMenus();
	
	/**
	 * 关闭窗口
	 */
	void ToolWindowClosed(const TSharedRef<SWindow>&);
};
